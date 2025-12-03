#include <iostream>    
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

struct Bitacora{
    string mes;
    int dia;
    string hora;
    int minuto;
    int segundo;
    string ip;
    int puerto;
    string motivo;
};

// NODO DE CONEXION (LA FLECHA)
struct NodoConexion {
    string ip;       
    int pesoArista;  // <--- CUANTAS VECES ESTA IP ATACÓ AQUI
    NodoConexion* next;
};

// VERTICE (EL NODO PRINCIPAL: PUERTO O ERROR)
struct Vertice {
    string nombre;       // Ej: "Puerto 80" o "Error Root"
    NodoConexion* listaIPs; 
    int pesoTotal;       // <--- SUMA DE TODOS LOS ATAQUES RECIBIDOS
};

// ==========================================
// VARIABLES GLOBALES
// ==========================================

// 1. GRAFO DE PUERTOS NUMERICOS (Del 0 al 65535)
const int MAX_PUERTOS = 65536;
Vertice* grafoPuertos[MAX_PUERTOS];

// 2. GRAFO DE ERRORES ESPECIALES (Los 5 que pediste)
// Indices: 0=Root, 1=Admin, 2=Guest, 3=Test, 4=IllegalUser
const int MAX_ERRORES = 5;
Vertice* grafoErrores[MAX_ERRORES]; 

// ==========================================
// FUNCIONES
// ==========================================

// FUNCION INTELIGENTE: BUSCA SI LA IP YA EXISTE, SI SI SUMA PESO, SI NO AGREGA
void AgregarOIncrementar(Vertice*& nodoPrincipal, string ip, string nombreSiEsNuevo) {
    
    // 1. Si el vértice principal no existe, lo creamos
    if (nodoPrincipal == NULL) {
        nodoPrincipal = new Vertice();
        nodoPrincipal->nombre = nombreSiEsNuevo;
        nodoPrincipal->listaIPs = NULL;
        nodoPrincipal->pesoTotal = 0;
    }

    // 2. Buscamos en la lista si esta IP ya atacó antes
    NodoConexion* actual = nodoPrincipal->listaIPs;
    bool encontrado = false;

    while (actual != NULL) {
        if (actual->ip == ip) {
            // ¡YA EXISTIA! Solo aumentamos el peso de la conexión
            actual->pesoArista++;
            encontrado = true;
            break; 
        }
        actual = actual->next;
    }

    // 3. Si no existía, creamos la conexión nueva
    if (!encontrado) {
        NodoConexion* nuevo = new NodoConexion();
        nuevo->ip = ip;
        nuevo->pesoArista = 1; // Primera vez
        
        // Insertamos al inicio
        nuevo->next = nodoPrincipal->listaIPs;
        nodoPrincipal->listaIPs = nuevo;
    }

    // 4. Aumentamos el peso total del Nodo Principal (Fan-Out Total)
    nodoPrincipal->pesoTotal++;
}

// FUNCION MANUAL PARA BUSCAR TEXTO (Reutilizamos la logica que ya teniamos)
bool contieneTexto(string texto, string busqueda) {
    int largoT = texto.length();
    int largoB = busqueda.length();
    
    for (int i = 0; i <= largoT - largoB; i++) {
        bool match = true;
        for (int j = 0; j < largoB; j++) {
            if (texto[i+j] != busqueda[j]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

// ==========================================
// MAIN
// ==========================================
int main(){
    const int tamBitacora = 16807;
    Bitacora arrBitacora[tamBitacora]; 
    int contador = 0;

    ifstream archivo;
    archivo.open("bitacora.txt");
    string linea;

    if (!archivo.is_open()) {
        cout << "Error: No se encontro bitacora.txt" << endl;
        return 1;
    }

    cout << "Leyendo archivo y construyendo grafos..." << endl;

    while(getline(archivo, linea) && contador < tamBitacora){
        Bitacora entrada;
        stringstream ss(linea);
        string hora_completa, direccion_completa;

        ss >> entrada.mes >> entrada.dia >> hora_completa >> direccion_completa;
        getline(ss, entrada.motivo);

        // Parsing manual rápido
        stringstream direccion_ss(direccion_completa);
        getline(direccion_ss, entrada.ip, ':'); 
        direccion_ss >> entrada.puerto;

        // ---------------------------------------------------------
        // AQUI LLENAMOS LOS GRAFOS CON LA NUEVA LOGICA DE PESOS
        // ---------------------------------------------------------

        // 1. LLENAR GRAFO DE PUERTO (Si es valido)
        if(entrada.puerto >= 0 && entrada.puerto < MAX_PUERTOS){
            string nombrePuerto = "Puerto " + to_string(entrada.puerto);
            AgregarOIncrementar(grafoPuertos[entrada.puerto], entrada.ip, nombrePuerto);
        }

        // 2. LLENAR GRAFO DE ERRORES (Los 5 Nodos Especiales)
        // Checamos el motivo y lo mandamos al nodo correspondiente
        
        // Nodo 0: Root
        if (contieneTexto(entrada.motivo, "root")) {
            AgregarOIncrementar(grafoErrores[0], entrada.ip, "Failed Password Root");
        }
        // Nodo 1: Admin
        else if (contieneTexto(entrada.motivo, "admin")) {
            AgregarOIncrementar(grafoErrores[1], entrada.ip, "Failed Password Admin");
        }
        // Nodo 2: Guest
        else if (contieneTexto(entrada.motivo, "guest")) {
            AgregarOIncrementar(grafoErrores[2], entrada.ip, "Failed Password Guest");
        }
        // Nodo 3: Test
        else if (contieneTexto(entrada.motivo, "test")) {
            AgregarOIncrementar(grafoErrores[3], entrada.ip, "Failed Password Test");
        }
        // Nodo 4: Illegal User (Username raro)
        else if (contieneTexto(entrada.motivo, "illegal user") || contieneTexto(entrada.motivo, "failed user")) {
            AgregarOIncrementar(grafoErrores[4], entrada.ip, "Illegal User Generic");
        }

        arrBitacora[contador] = entrada;
        contador++;
    }
    archivo.close();

    // ==========================================
    // RESULTADOS Y REPORTES
    // ==========================================
    
    ofstream archivoSalida("ReporteNodos.txt");

    if (archivoSalida.is_open()) {
        archivoSalida << "=== REPORTE DEL GRAFO DE ERRORES (NODOS ESPECIALES) ===" << endl;
        
        // IMPRIMIR LOS 5 NODOS DE ERROR
        for(int i=0; i<MAX_ERRORES; i++){
            if(grafoErrores[i] != NULL){
                archivoSalida << "NODO: " << grafoErrores[i]->nombre << endl;
                archivoSalida << "   PESO TOTAL (Ataques recibidos): " << grafoErrores[i]->pesoTotal << endl;
                archivoSalida << "   DETALLE DE CONEXIONES:" << endl;
                
                NodoConexion* aux = grafoErrores[i]->listaIPs;
                while(aux != NULL){
                    archivoSalida << "      -> IP: " << aux->ip << " (Peso Arista: " << aux->pesoArista << ")" << endl;
                    aux = aux->next;
                }
                archivoSalida << "--------------------------------------------" << endl;
            }
        }

        archivoSalida << "\n=== TOP PUERTOS MAS ATACADOS ===" << endl;
        // Buscamos el puerto mas atacado
        int maxPeso = 0;
        int puertoGanador = -1;

        for(int i=0; i<MAX_PUERTOS; i++){
            if(grafoPuertos[i] != NULL){
                if(grafoPuertos[i]->pesoTotal > maxPeso){
                    maxPeso = grafoPuertos[i]->pesoTotal;
                    puertoGanador = i;
                }
            }
        }
        archivoSalida << "El puerto con mayor peso es el " << puertoGanador << " con " << maxPeso << " eventos." << endl;
        
        archivoSalida.close();
    }

    // IMPRESION EN CONSOLA RESUMIDA
    cout << "Analisis completado." << endl;
    cout << "Se genero el archivo 'ReporteNodos.txt' con los pesos detallados." << endl;
    cout << endl;
    
    cout << "=== RESUMEN DE NODOS DE ERROR (SOSPECHOSOS) ===" << endl;
    for(int i=0; i<MAX_ERRORES; i++){
        if(grafoErrores[i] != NULL){
            cout << "Nodo [" << grafoErrores[i]->nombre << "] -> Peso Total: " << grafoErrores[i]->pesoTotal << endl;
        } else {
            // Si es NULL es que no hubo ataques de ese tipo
            string nombres[5] = {"Root", "Admin", "Guest", "Test", "Illegal User"};
            cout << "Nodo [" << nombres[i] << "] -> Peso Total: 0" << endl;
        }
    }

    cout << "\nGracias por ver :)" << endl;
    return 0;
}