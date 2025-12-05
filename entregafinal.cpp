#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <queue> // NECESARIO PARA EL BFS

using namespace std;

// STRUCTUS
struct Bitacora {
    string mes;
    int dia;
    string hora; //MEJOR COMO STRING, YA NO OCUPAREMOS QUE ESTE DE QUE SEPARADO
    string ip;
    int puerto;
    string motivo;
};

// DEFINIMOS LA ARISTA 
struct Arista {
    string motivo;
    int peso; // EL PESO QUE SE REPETIRA CADA MOTIVA
};

// DEFINIMOS EL VERTICE (PUERTO)
struct Vertice {
    int puerto;
    vector<Arista> conexiones; // LISTA DE MOTIVOS ARISTAS
    int pesoTotal; // SUMA DE TODOS LOS PESOS, EL FAN-OUT
};

// FUNCION BFS
// RECIBE EL PUERTO, Y EL VECTOR Y ASI, PARA PUES QUE SE BUSQUE Y HAGA EL RECORRIDO PARA SACAR EL RESULTADO
void BFS(int puertoInicio, vector<Vertice> &grafo) {
    // 1. CREAMOS LA COLA PARA GUARDAR NODOS QUE VAMOS A VISITAR
    queue<int> cola;
    
    // 2. METEMOS EL PUERTO INICIAL A LA COLA
    cola.push(puertoInicio);
    
    cout << "Iniciando Recorrido BFS el Puerto " << puertoInicio << endl;
    
    // 3. MIENTRAS LA COLA NO ESTE VACIA, SEGUIREMOS BUSCANDO
    while(!cola.empty()) {
        // SACAMOS EL PRIMER ELEMENTO DE LA COLA
        int puertoActual = cola.front();
        cola.pop();
        
        // IMPRIMIMOS EL PUERTO ACTUAL
        cout << "Analizando Puerto: " << puertoActual << endl;
        cout << "Peso Total (Fan-Out): " << grafo[puertoActual].pesoTotal << endl;
        cout << "Conexiones (Aristas):" << endl;
        
        // RECORREMOS TODAS LAS CONEXIONES (ARISTAS) DE ESTE PUERTO
        for(int i = 0; i < grafo[puertoActual].conexiones.size(); i++) {
            // OBTENEMOS LA ARISTA ACTUAL
            string motivo = grafo[puertoActual].conexiones[i].motivo;
            int peso = grafo[puertoActual].conexiones[i].peso;
            
            cout << "Motivo: " << motivo << " Peso: " << peso << endl;
        }
    }
    cout << "Fin del Recorrido BFS" << endl;
    cout << endl;
}

int main() {
    vector<Bitacora> registros;

    ifstream archivo("bitacora.txt");

    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        Bitacora entrada;
        string direccion_completa;

        ss >> entrada.mes;
        ss >> entrada.dia;
        ss >> entrada.hora;
        ss >> direccion_completa;
        getline(ss, entrada.motivo);

        stringstream direccion_ss(direccion_completa);
        getline(direccion_ss, entrada.ip, ':');
        direccion_ss >> entrada.puerto;

        registros.push_back(entrada);
    }

    archivo.close();

    // HACEMOS UN VECTOR DE SIZE 65536, LOS PUERTOS QUE PUEDEN EXISTIR
    vector<Vertice> grafo(65536);

    // INICIALIZAMOS EL GRAFO, DE QUE LE PONEMOS EN EL VECTOR EL PESO TOTAL DE CERO
    // Y DESPUES EL PUERTO EN LA ITERACION
    for(int i = 0; i < 65536; i++) {
        grafo[i].puerto = i;
        grafo[i].pesoTotal = 0;
    }

    // Poblar el grafo
    for (int i = 0; i < registros.size(); i++) {
        int p = registros[i].puerto;

        // RECORREMOS EL GRAFO
        bool encontrado = false;
        for (int j = 0; j < grafo[p].conexiones.size(); j++) {
            if (grafo[p].conexiones[j].motivo == registros[i].motivo) {
                grafo[p].conexiones[j].peso++; // SUMAMOS EL PESO
                encontrado = true;
                break;
            }
        }

        // SI NO ENCONTRAMOS EL MOTIVO, LO CREAMOS
        if (encontrado == false) {
            Arista nuevaArista;
            nuevaArista.motivo = registros[i].motivo;
            nuevaArista.peso = 1;
            grafo[p].conexiones.push_back(nuevaArista);
        }

        // SUMAMOS EL PESO TOTAL DEL NODO, OSEA EL FAN OUT
        grafo[p].pesoTotal++;
    }

    int puertoBuscado = 0;
    
    // CICLO PARA QUE EL USUARIO PUEDA BUSCAR EL PUERTO
    while(true) {
        cout << "Ingresa el puerto que quieres analizar (-1 para salir): ";
        cin >> puertoBuscado;

        if (puertoBuscado == -1) {
            break; // BYEEEE
        }

        if (puertoBuscado >= 0 && puertoBuscado < 65536) {
            BFS(puertoBuscado, grafo); // BFS
        } else {
            cout << "Numero de puerto no existe papu" << endl; // SI NO EXISTE EL PUERTO
            // EN BITACORA, ENTONCES TE DICE QUE NO EXISTE
        }
    }

    cout << "Gracias por ver:)" << endl;

    return 0;
}
