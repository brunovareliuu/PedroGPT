#include <iostream>    
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

// DEFINIMOS LO QUE VIENE SIENDO LA STRUCT PARA PODER TENER LA BITACORA COMO UN ARRAY
struct Bitacora{
    string mes;
    int dia;
    int hora;
    int minuto;
    int segundo;
    string ip;
    int puerto;
    string motivo;
};


struct NodoIP {
    string ip;       
    string motivo; // CAMBIO CLAVEEEEEEE
    NodoIP* next;
};

struct verticePuerto {
    int numeroPuerto;      
    NodoIP* listaDeIPs;   
    int peso; 
};


const int puertosPosibles = 65536;

verticePuerto* grafoPuertos[puertosPosibles];

void AgregarAlGrafo(int puerto, string ip, string motivo){
    // VALIDAR RANGO
    if (puerto < 0 || puerto >= puertosPosibles){
        return;
    }
    
    if (grafoPuertos[puerto] == NULL){
        grafoPuertos[puerto] = new verticePuerto; 
        grafoPuertos[puerto]->numeroPuerto = puerto;
        grafoPuertos[puerto]->listaDeIPs = NULL; 
        grafoPuertos[puerto]->peso = 0; 
    }
    
    NodoIP* NodoIp = new NodoIP(); 
    NodoIp->ip = ip; 
    NodoIp->motivo = motivo; // GUARDAMOS MOTIVO EN LA LISTA

    NodoIp->next = grafoPuertos[puerto]->listaDeIPs; 
    grafoPuertos[puerto]->listaDeIPs = NodoIp; 

    grafoPuertos[puerto]->peso++;
}

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

    while(getline(archivo, linea) && contador < tamBitacora){
        Bitacora entrada;
        stringstream ss(linea);
        string hora_completa;
        string direccion_completa;

        ss >> entrada.mes;
        ss >> entrada.dia;
        ss >> hora_completa;
        ss >> direccion_completa;
        getline(ss, entrada.motivo);

        stringstream hora_ss(hora_completa);
        hora_ss >> entrada.hora;
        hora_ss.ignore(); 
        hora_ss >> entrada.minuto;
        hora_ss.ignore(); 
        hora_ss >> entrada.segundo;

        stringstream direccion_ss(direccion_completa);
        getline(direccion_ss, entrada.ip, ':'); 
        direccion_ss >> entrada.puerto;

        // MANDAMOS EL MOTIVO AL GRAFO TAMBIEN
        AgregarAlGrafo(entrada.puerto, entrada.ip, entrada.motivo);
        
        arrBitacora[contador] = entrada;
        contador ++;
    }
    archivo.close();

    const int datosBitacora = contador;

    cout << "Cargando analisis..." << endl;
    cout << endl;
    ofstream archivo_salida_fan_out_individual("FanOutIndividuales.txt");

    int contador_puertos = 0;

    int max_fan_out = 0;
    int segundo = 0, tercero = 0, cuarto = 0, quinto = 0;
    int puerto_ganador = -1;
    int puerto_segundo = 0, puerto_tercero = 0, puerto_cuarto = 0, puerto_quinto = 0;
    
    if(archivo_salida_fan_out_individual.is_open()){
        for(int i = 0; i < puertosPosibles; i++){
            
            if(grafoPuertos[i] != NULL){
                archivo_salida_fan_out_individual << "Puerto: " <<
                grafoPuertos[i]->numeroPuerto <<
                " -> Fan Out -> " << grafoPuertos[i]->peso << endl;
                
                contador_puertos++;

                int pesoActual = grafoPuertos[i]->peso;
                int puertoActual = grafoPuertos[i]->numeroPuerto;

                if(pesoActual > max_fan_out){
                    quinto = cuarto; puerto_quinto = puerto_cuarto;
                    cuarto = tercero; puerto_cuarto = puerto_tercero;
                    tercero = segundo; puerto_tercero = puerto_segundo;
                    segundo = max_fan_out; puerto_segundo = puerto_ganador;
                    
                    max_fan_out = pesoActual;
                    puerto_ganador = puertoActual;
                }
                else if(pesoActual > segundo){
                    quinto = cuarto; puerto_quinto = puerto_cuarto;
                    cuarto = tercero; puerto_cuarto = puerto_tercero;
                    tercero = segundo; puerto_tercero = puerto_segundo;
                    
                    segundo = pesoActual;
                    puerto_segundo = puertoActual;
                }
                else if(pesoActual > tercero){
                    quinto = cuarto; puerto_quinto = puerto_cuarto;
                    cuarto = tercero; puerto_cuarto = puerto_tercero;
                    
                    tercero = pesoActual;
                    puerto_tercero = puertoActual;
                }
                else if(pesoActual > cuarto){
                    quinto = cuarto; puerto_quinto = puerto_cuarto;
                    cuarto = pesoActual;
                    puerto_cuarto = puertoActual;
                }
                else if(pesoActual > quinto){
                    quinto = pesoActual;
                    puerto_quinto = puertoActual;
                }
            }
        }
        archivo_salida_fan_out_individual.close();
    }

    cout << "Determine el fan-out de cada nodo" << endl;
    cout << "Respuesta: Checar el archivo FanOutIndividuales.txt" << endl;
    cout << endl;

    cout << "Que nodos tienen el mayor fan-out (considerando por puertos)?" << endl;
    cout << "Puerto Top 1: " << puerto_ganador << " (" << max_fan_out << ")" << endl;
    cout << "Puerto Top 2: " << puerto_segundo << " (" << segundo << ")" << endl;
    cout << "Puerto Top 3: " << puerto_tercero << " (" << tercero << ")" << endl;
    cout << "Puerto Top 4: " << puerto_cuarto << " (" << cuarto << ")" << endl;
    cout << "Puerto Top 5: " << puerto_quinto << " (" << quinto << ")" << endl;

    cout << endl;
    cout << "Puertos totales: " << contador_puertos << endl;

    cout << endl;
    cout << "Deduce las conexiones de un usuario invitado, root y admin" << endl;
    cout << "(Buscando dentro de las listas enlazadas...)" << endl;

    int encontrados = 0;
    ofstream archivo_hackers("ReporteHackers.txt");

    if(archivo_hackers.is_open()){
        
        // RECORREMOS EL GRAFO PUERTO POR PUERTO
        for(int i = 0; i < puertosPosibles; i++){
            // SI EL PUERTO TIENE CONEXIONES
            if(grafoPuertos[i] != NULL){
                
                // OBTENEMOS LA CABEZA DE LA LISTA
                NodoIP* actual = grafoPuertos[i]->listaDeIPs;
                
                // RECORREMOS LA LISTA ENLAZADA DE ESTE PUERTO
                while(actual != NULL){
                    
                    string m = actual->motivo; 
                    bool esSospechoso = false;
                    int largo = m.length();

                    // BUSQUEDA MANUAL LETRA POR LETRA
                    for(int j = 0; j < largo; j++) {
                        // BUSCAMOS ROOT
                        if(j + 3 < largo) {
                            if(m[j] == 'r' && m[j+1] == 'o' && m[j+2] == 'o' && m[j+3] == 't'){
                                esSospechoso = true;
                                break; 
                            }
                        }
                        // BUSCAMOS ADMIN
                        if(j + 4 < largo) {
                            if(m[j] == 'a' && m[j+1] == 'd' && m[j+2] == 'm' && m[j+3] == 'i' && m[j+4] == 'n'){
                                esSospechoso = true;
                                break; 
                            }
                        }
                    }

                    if(esSospechoso){
                        archivo_hackers << "ALERTA (Puerto " << grafoPuertos[i]->numeroPuerto 
                                        << ") IP [" << actual->ip << "]: " << m << endl;
                        encontrados++;
                    }

                    // AVANZAMOS AL SIGUIENTE NODO DE LA LISTA
                    actual = actual->next;
                }
            }
        }
        
        cout << "Respuesta: Checar el archivo ReporteHackers.txt" << endl;
        cout << "Total de sospechosos detectados: " << encontrados << endl;
        
        archivo_hackers.close();
    }

    cout << endl;
    cout << "Gracias por ver :)" << endl;

    return 0;
}