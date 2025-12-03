#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// ====================== ESTRUCTURAS ======================

struct Bitacora {
    string mes;
    int dia;
    int hora;
    int minuto;
    int segundo;
    string ip;
    int puerto;
    string motivo;
};

struct MotivoNode {
    string motivo;      // texto completo del motivo
    int peso;           // cantidad de accesos con ese motivo
    MotivoNode* next;   // siguiente motivo en la lista

    MotivoNode(string m) : motivo(m), peso(1), next(nullptr) {}
};

struct Puerto {
    int numero;                // número de puerto
    int totalAccesos;          // fan-out total (cantidad de registros)
    MotivoNode* motivos;       // lista enlazada de motivos (grafo ponderado)
    Puerto* next;              // siguiente puerto en lista global
    Puerto* nextEnFrecuencia;  // siguiente puerto en lista del nodo Frecuencia

    Puerto(int num = 0)
        : numero(num),
          totalAccesos(0),
          motivos(nullptr),
          next(nullptr),
          nextEnFrecuencia(nullptr) {}
};

struct Frecuencia {
    int peso;              // cantidad de accesos
    Puerto* listaPuertos;  // puertos con este mismo peso
    Frecuencia* left;      // menores
    Frecuencia* right;     // mayores

    Frecuencia(int p)
        : peso(p), listaPuertos(nullptr), left(nullptr), right(nullptr) {}
};

// ====================== FUNCIONES AUXILIARES ======================

int mesToInt(const string& mes) {
    if (mes == "Jan") return 1;
    if (mes == "Feb") return 2;
    if (mes == "Mar") return 3;
    if (mes == "Apr") return 4;
    if (mes == "May") return 5;
    if (mes == "Jun") return 6;
    if (mes == "Jul") return 7;
    if (mes == "Aug") return 8;
    if (mes == "Sep") return 9;
    if (mes == "Oct") return 10;
    if (mes == "Nov") return 11;
    if (mes == "Dec") return 12;
    return 0;
}

bool compararFechas(const Bitacora& a, const Bitacora& b) {
    int ma = mesToInt(a.mes);
    int mb = mesToInt(b.mes);
    if (ma != mb) return ma < mb;
    if (a.dia != b.dia) return a.dia < b.dia;
    if (a.hora != b.hora) return a.hora < b.hora;
    if (a.minuto != b.minuto) return a.minuto < b.minuto;
    return a.segundo <= b.segundo;
}

bool compararPuertos(const Bitacora& a, const Bitacora& b) {
    if (a.puerto != b.puerto)
        return a.puerto < b.puerto;   // ordenar por numero de puerto

    // desempate por fecha
    return compararFechas(a, b);
}

// ====================== MERGE SORT POR PUERTO ======================

void merge(Bitacora arrBitacora[], int start, int mid, int end) {
    int n1 = mid - start + 1;
    int n2 = end - mid;

    Bitacora* arregloIzq = new Bitacora[n1];
    Bitacora* arregloDer = new Bitacora[n2];

    for (int i = 0; i < n1; i++) {
        arregloIzq[i] = arrBitacora[start + i];
    }
    for (int j = 0; j < n2; j++) {
        arregloDer[j] = arrBitacora[mid + 1 + j];
    }

    int i = 0;
    int j = 0;
    int k = start;

    while (i < n1 && j < n2) {
        if (compararPuertos(arregloIzq[i], arregloDer[j])) {
            arrBitacora[k] = arregloIzq[i];
            i++;
        } else {
            arrBitacora[k] = arregloDer[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arrBitacora[k] = arregloIzq[i];
        i++;
        k++;
    }
    while (j < n2) {
        arrBitacora[k] = arregloDer[j];
        j++;
        k++;
    }

    delete[] arregloIzq;
    delete[] arregloDer;
}

void mergeSort(Bitacora arrBitacora[], int start, int end) {
    if (start >= end) return;
    int mid = (start + end) / 2;
    mergeSort(arrBitacora, start, mid);
    mergeSort(arrBitacora, mid + 1, end);
    merge(arrBitacora, start, mid, end);
}

// ====================== LISTA DE PUERTOS Y MOTIVOS ======================

void agregarMotivo(Puerto* p, const string& m) {
    MotivoNode* act = p->motivos;

    // Buscar si ya existe el motivo
    while (act != nullptr) {
        if (act->motivo == m) {
            act->peso++;     // aumentar peso de la arista
            return;
        }
        act = act->next;
    }

    // Si no existe, crear nodo nuevo al inicio
    MotivoNode* nuevo = new MotivoNode(m);
    nuevo->next = p->motivos;
    p->motivos = nuevo;
}

Puerto* buscarOCrearPuerto(Puerto*& head, int numero) {
    Puerto* actual = head;
    while (actual != nullptr) {
        if (actual->numero == numero)
            return actual;
        actual = actual->next;
    }
    // no existe: crear al inicio de la lista
    Puerto* nuevo = new Puerto(numero);
    nuevo->next = head;
    head = nuevo;
    return nuevo;
}

Puerto* construirListaPuertos(Bitacora arrBitacora[], int n) {
    Puerto* listaPuertos = nullptr;

    for (int i = 0; i < n; i++) {
        Bitacora& reg = arrBitacora[i];
        Puerto* p = buscarOCrearPuerto(listaPuertos, reg.puerto);

        p->totalAccesos++;

        // Agregar/actualizar motivo como nodo en la lista
        agregarMotivo(p, reg.motivo);
    }

    return listaPuertos;
}

// ====================== BST DE FRECUENCIAS ======================

void insertarPuertoEnListaFrecuencia(Puerto*& lista, Puerto* p) {
    p->nextEnFrecuencia = lista;
    lista = p;
}

void insertarFrecuencia(Frecuencia*& root, Puerto* p) {
    if (root == nullptr) {
        root = new Frecuencia(p->totalAccesos);
        insertarPuertoEnListaFrecuencia(root->listaPuertos, p);
        return;
    }

    if (p->totalAccesos < root->peso) {
        insertarFrecuencia(root->left, p);
    } else if (p->totalAccesos > root->peso) {
        insertarFrecuencia(root->right, p);
    } else {
        // mismo peso
        insertarPuertoEnListaFrecuencia(root->listaPuertos, p);
    }
}

Frecuencia* construirArbolFrecuencias(Puerto* listaPuertos) {
    Frecuencia* root = nullptr;
    Puerto* actual = listaPuertos;
    while (actual != nullptr) {
        insertarFrecuencia(root, actual);
        actual = actual->next;
    }
    return root;
}

// ====================== TOP 5 POR FRECUENCIA ======================

void imprimirTopNRec(Frecuencia* node, int N, int& contadorPuertos, int& pesoQuinto) {
    if (node == nullptr) return;

    // primero los pesos mayores
    imprimirTopNRec(node->right, N, contadorPuertos, pesoQuinto);

    if (pesoQuinto != -1 && node->peso < pesoQuinto)
        return;

    Puerto* p = node->listaPuertos;
    while (p != nullptr) {
        contadorPuertos++;
        if (contadorPuertos == N) {
            pesoQuinto = node->peso;
        }

        if (contadorPuertos <= N || node->peso == pesoQuinto) {
            cout << "Puerto " << p->numero << " - accesos: " << node->peso << endl;
        }

        p = p->nextEnFrecuencia;
    }

    imprimirTopNRec(node->left, N, contadorPuertos, pesoQuinto);
}

void imprimirTop5(Frecuencia* root) {
    int contadorPuertos = 0;
    int pesoQuinto = -1;
    cout << "Top 5 Puertos con mas Fanouts\n";
    imprimirTopNRec(root, 5, contadorPuertos, pesoQuinto);
}

// ====================== BUSCAR PUERTO Y DESGLOSE ======================

Puerto* buscarPuerto(Puerto* head, int numero) {
    Puerto* actual = head;
    while (actual != nullptr) {
        if (actual->numero == numero)
            return actual;
        actual = actual->next;
    }
    return nullptr;
}

void imprimirDesglosePuerto(Puerto* p) {
    if (p == nullptr) return;

    cout << "\nPuerto " << p->numero << endl;
    cout << "Total de intentos de acceso (fan-out): "
         << p->totalAccesos << endl;

    cout << "\nDesglose de motivos:\n";

    MotivoNode* m = p->motivos;
    while (m != nullptr) {
        cout << "- \"" << m->motivo << "\" : "
             << m->peso << " fan-out" << endl;
        m = m->next;
    }
}

// ====================== LECTURA DE BITACORA ======================

bool leerBitacora(const string& nombreArchivo, vector<Bitacora>& registros) {
    ifstream fin(nombreArchivo);
    if (!fin.is_open()) {
        cerr << "No se pudo abrir el archivo " << nombreArchivo << endl;
        return false;
    }

    string linea;
    while (getline(fin, linea)) {
        if (linea.empty()) continue;

        istringstream iss(linea);
        Bitacora reg;
        string tiempo, ipPuerto;

        iss >> reg.mes >> reg.dia >> tiempo >> ipPuerto;
        string motivo;
        getline(iss, motivo);
        if (!motivo.empty() && motivo[0] == ' ')
            motivo.erase(0, 1);
        reg.motivo = motivo;

        // parsear tiempo hh:mm:ss
        if (tiempo.size() == 8 && tiempo[2] == ':' && tiempo[5] == ':') {
            reg.hora   = stoi(tiempo.substr(0, 2));
            reg.minuto = stoi(tiempo.substr(3, 2));
            reg.segundo= stoi(tiempo.substr(6, 2));
        } else {
            reg.hora = reg.minuto = reg.segundo = 0;
        }

        // parsear ip y puerto
        size_t pos = ipPuerto.rfind(':');
        if (pos != string::npos) {
            reg.ip = ipPuerto.substr(0, pos);
            reg.puerto = stoi(ipPuerto.substr(pos + 1));
        } else {
            reg.ip = ipPuerto;
            reg.puerto = 0;
        }

        registros.push_back(reg);
    }

    return true;
}

// ====================== MAIN ======================

int main() {
    vector<Bitacora> registros;

    if (!leerBitacora("bitacora.txt", registros)) {
        return 1;
    }

    if (registros.empty()) {
        cout << "La bitacora esta vacia.\n";
        return 0;
    }

    // Ordenar por puerto con MergeSort
    mergeSort(registros.data(), 0, (int)registros.size() - 1);

    // Construir lista de puertos con motivos (grafo ponderado)
    Puerto* listaPuertos = construirListaPuertos(registros.data(), (int)registros.size());

    // Construir arbol BST por frecuencia (peso = totalAccesos)
    Frecuencia* rootFreq = construirArbolFrecuencias(listaPuertos);

    // Imprimir Top 5 puertos (con empates en el quinto lugar)
    imprimirTop5(rootFreq);

    // Preguntar al usuario por un puerto y mostrar desglose
    int puertoBuscado;
    cout << "\nIngresa un puerto para ver sus accesos: ";
    cin >> puertoBuscado;

    Puerto* p = buscarPuerto(listaPuertos, puertoBuscado);
    if (p == nullptr) {
        cout << "Ese puerto no aparece en la bitacora.\n";
    } else {
        imprimirDesglosePuerto(p);
    }

    return 0;
}
