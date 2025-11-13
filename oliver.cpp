#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
using namespace std;


//DESCRIPCIÓN DEL PROGRAMA - Buscamos crear un programa que lea los registros de un archivo de texto que represente una bitácora. Luego los ordene usando Mergesort
// y permita buscar registros entre fechas que el usuario ingrese. Luego muestra las 5 Ip con mas accesos (catalogándolos como sospechosos).

// primero creamos la estructura para poder guardar los registros
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


// convertimos los meses en vzlores numéricos
int ordenar_meses(const string& mes) {
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
    
    int mesA = ordenar_meses(a.mes);
    int mesB = ordenar_meses(b.mes);
    if (mesA < mesB) return true;
    if (mesA > mesB) return false;
    if (a.dia < b.dia) return true;
    if (a.dia > b.dia) return false;
    if (a.hora < b.hora) return true;
    if (a.hora > b.hora) return false;
    if (a.minuto < b.minuto) return true;
    if (a.minuto > b.minuto) return false;
    return a.segundo <= b.segundo;
}

// le pedimos la fecha al usuario  para iniciar con la busqueda (iniciamos el rango)

Bitacora pedir_fecha(const string& mensaje) {
    Bitacora fecha;
    cout << mensaje << endl;
    cout << "Ingresa el mes (Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec): ";
    cin >> fecha.mes;
    cout << "Ingresa el dia en numero (01-31): ";
    cin >> fecha.dia;
    fecha.hora = 0;
    fecha.minuto = 0;
    fecha.segundo = 0;
    fecha.ip = "";
    fecha.puerto = 0;
    fecha.motivo = "";
    return fecha;
}

// realizamos una búsqueda binaria para encontrar el índice del primer registro

int busquedaBinariaInicio(Bitacora arr[], int n, Bitacora fechaBuscada) {
    int start = 0, end = n - 1, resultado = -1;
    while (start <= end) {
        int mid = start + (end - start) / 2;
        if (compararFechas(fechaBuscada, arr[mid])) {
            resultado = mid;
            end = mid - 1;
        } else start = mid + 1;
    }
    return resultado;
}

// realizamos una búsqueda binaria para encontrar el índice del último registro

int busquedaBinariaFin(Bitacora arr[], int n, Bitacora fechaBuscada) {
    int start = 0, end = n - 1, resultado = -1;
    while (start <= end) {
        int mid = start + (end - start) / 2;
        if (compararFechas(arr[mid], fechaBuscada)) {
            resultado = mid;
            start = mid + 1;
        } else end = mid - 1;
    }
    return resultado;
}



void merge(Bitacora arr[], int start, int mid, int end) {
    int n1 = mid - start + 1;
    int n2 = end - mid;
    Bitacora* izq = new Bitacora[n1];
    Bitacora* der = new Bitacora[n2];
    for (int i = 0; i < n1; i++) izq[i] = arr[start + i];
    for (int j = 0; j < n2; j++) der[j] = arr[mid + 1 + j];
    int i = 0, j = 0, k = start;
    while (i < n1 && j < n2) {
        if (compararFechas(izq[i], der[j])) arr[k++] = izq[i++];
        else arr[k++] = der[j++];
    }
    while (i < n1) arr[k++] = izq[i++];
    while (j < n2) arr[k++] = der[j++];
    delete[] izq;
    delete[] der;
}

void mergeSort(Bitacora arr[], int start, int end) {
    if (start >= end) return;
    int mid = start + (end - start) / 2;
    mergeSort(arr, start, mid);
    mergeSort(arr, mid + 1, end);
    merge(arr, start, mid, end);
}

// en el main se leen la bitácora, se ordenan lo registros y realiza una busqueda por rango, imprimiendo las 5 ip con mas registros
int main(void) {
    const int tamBitacora = 16807;
    Bitacora arrBitacora[tamBitacora];
    int contador = 0;

    
    // se hace la lectura del archivo bitacora en donde ordenamos los registros por fecha y hora
    
    ifstream archivo("/Users/oliver/Downloads/trabajos_c++/Ejemplos/cmasmas/avance2/avance2/bitacora.txt");
    string linea;
    while (getline(archivo, linea) && contador < tamBitacora) {
        Bitacora entrada;
        stringstream ss(linea);
        string hora_completa, direccion_completa;
        ss >> entrada.mes >> entrada.dia >> hora_completa >> direccion_completa;
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
        arrBitacora[contador++] = entrada;
    }
    archivo.close();

    
    // se ordena de forma cronológica
    
    mergeSort(arrBitacora, 0, contador - 1);

    cout << "¡Siéntete libre de Usar el Programa PedroGPT!" << endl;
    Bitacora fecha_inicio = pedir_fecha("-- Fecha de inicio --");
    Bitacora fecha_fin = pedir_fecha("------ Fecha de fin ------");
    fecha_fin.hora = 23; fecha_fin.minuto = 59; fecha_fin.segundo = 59;

    int indiceInicial = busquedaBinariaInicio(arrBitacora, contador, fecha_inicio);
    int indiceFinal = busquedaBinariaFin(arrBitacora, contador, fecha_fin);

    ofstream archivo_salida("bitacora_ordenada_al_gusto.txt");
    int contador_impresos = 0;
    if (archivo_salida.is_open()) {
        for (int i = indiceInicial; i <= indiceFinal; i++) {
            archivo_salida << arrBitacora[i].mes << " " << arrBitacora[i].dia << " "
                << arrBitacora[i].hora << ":" << arrBitacora[i].minuto << ":" << arrBitacora[i].segundo << " "
                << arrBitacora[i].ip << ":" << arrBitacora[i].puerto
                << arrBitacora[i].motivo << endl;
            contador_impresos++;
        }
        archivo_salida.close();
    }

    cout << "\nTotal de registros encontrados: " << contador_impresos << endl;
    cout << "Archivo generado: bitacora_ordenada_al_gusto.txt\n";

    const int tamBitacora2 = contador_impresos;
    Bitacora arrBitacora2[tamBitacora2];
    int contador2 = 0;
    ifstream archivo2("bitacora_ordenada_al_gusto.txt");
    string linea2;
    while (getline(archivo2, linea2) && contador2 < tamBitacora2) {
        Bitacora entrada2;
        stringstream ss(linea2);
        string hora_completa2, direccion_completa2;
        ss >> entrada2.mes >> entrada2.dia >> hora_completa2 >> direccion_completa2;
        getline(ss, entrada2.motivo);
        stringstream hora_ss2(hora_completa2);
        hora_ss2 >> entrada2.hora;
        hora_ss2.ignore();
        hora_ss2 >> entrada2.minuto;
        hora_ss2.ignore();
        hora_ss2 >> entrada2.segundo;
        stringstream direccion_ss2(direccion_completa2);
        getline(direccion_ss2, entrada2.ip, ':');
        direccion_ss2 >> entrada2.puerto;
        arrBitacora2[contador2++] = entrada2;
    }
    archivo2.close();

    map<string, int> contadorIP;
    for (int i = 0; i < contador2; i++) {
        contadorIP[arrBitacora2[i].ip]++;
    }

    vector<pair<string, int>> lista;
    for (auto& par : contadorIP) {
        lista.push_back({ par.first, par.second });
    }

    //Ordenamos de mayor a menor dependiendo de la cantidad de accesos de cada usuario
    
    sort(lista.begin(), lista.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });

    cout << "\n Top 5 IPs con más accesos sospechosos \n";
    for (int i = 0; i < 5 && i < lista.size(); i++) {
        cout << i + 1 << ". " << lista[i].first
            << " -> " << lista[i].second << " accesos\n";
    }

    cout << "\n ¡Gracias por usar PedroGPT! \n";
    return 0;
}