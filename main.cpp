#include <iostream>    
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

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

// Funciones auxiliares

// Funcion para ordernar los meses, donde en si solo ponemos por orden y ya, not big deal
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
    return 0; // En caso de que no sea un mes valido
}
// FUNCIÓN AUXILIAR: Compara dos registros de Bitacora.
// Devuelve 'true' si el registro 'a' debe ir antes que el 'b'. esto sera para el merge y que este sea mas sencillo
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
    if (a.segundo <= b.segundo) return true;
    if (a.segundo > b.segundo) return false;

    return true; 
}

// Funcion para guardar las fechas de inicio con lo que nos diga el usuario
Bitacora pedir_fecha(const string& mensaje){
    Bitacora fecha;
    string mes_str;
    int dia_int;

    cout << mensaje << endl;
    cout << "Ingresa el mes de inicio en tres letras y la primera en mayuscula" << endl;
    cout << "Jan, Feb, Mar, Abr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec" << endl;
    cout << "Mes: ";
    cin >> mes_str;

    cout << "Ingresa el dia de inicio en numero de dos digitos (01-31): ";
    cin >> dia_int;
    fecha.mes = mes_str;
    fecha.dia = dia_int;

    // Inicializamos lo demas en 0 o vacio
    fecha.hora = 0;
    fecha.minuto = 0;
    fecha.segundo = 0;
    fecha.ip = "";
    fecha.puerto = 0;
    fecha.motivo = "";
    return fecha;

}

// Para lo que viene siendo estas funciones binarias queremos devolver el indice para que sea mas facil al momentod e ponerlo en el ofstream
// Funcion de busqueda BINARIA para el inicio
int busquedaBinariaInicio(Bitacora arrBitacora[], int n, Bitacora fechaBuscada) {
    int start = 0;
    int end = n - 1;
    int resultado = -1;

    while (start <= end) {
        int mid = start + (end - start) / 2;
        // Esta condición es como preguntar: "fechaBuscada es <= arrBitacora[mid]?"
        // Es decir, ¿la fecha en mid ya es la que buscamos o una posterior?
        if (compararFechas(fechaBuscada, arrBitacora[mid])) { 
            resultado = mid; // Es un resultado válido, lo guardamos.
            end = mid - 1;   // Ahora buscamos si hay otro resultado válido más a la izquierda (repetidos).
        } else {
            // Si la fecha en 'mid' es muy temprana, debemos buscar a la derecha.
            start = mid + 1;
        }
    }
    return resultado;
}

// Funcion de busqueda BINARIA para el inicio
int busquedaBinariaFin(Bitacora arrBitacora[], int n, Bitacora fechaBuscada) {
    int start = 0;
    int end = n - 1;
    int resultado = -1;

    while (start <= end) {
        int mid = start + (end - start) / 2;

        if (compararFechas(arrBitacora[mid], fechaBuscada)) {
            resultado = mid;  // Este es un posible resultado
            start = mid + 1;  // Pero intentamos encontrar uno después (más a la derecha)
        } else {
            // Si la fecha en mid es mayor, buscamos a la izquierda
            end = mid - 1;
        }
    }
    return resultado;
}

// Funcion para hacer el merge
void merge(Bitacora arrBitacora[], int start, int mid, int end) {
    int n1 = mid - start + 1;
    int n2 = end - mid;

    // crear arreglos temporales del tipo bitacora con el new
    // Los apuntadores es porque los arreglos en funciones actuan de una forma diferente
    Bitacora* arregloIzq = new Bitacora[n1];
    Bitacora* arregloDer = new Bitacora[n2];

    // Aqui separamos el arreglo
    for (int i = 0; i < n1; i++) {
        arregloIzq[i] = arrBitacora[start + i];
    }
    for (int j = 0; j < n2; j++) {
        arregloDer[j] = arrBitacora[mid + 1 + j];
    }

    int i = 0;
    int j = 0; 
    int k = start;

    // Bucle principal para fusionar de vuelta en el arreglo original
    while (i < n1 && j < n2) {
        // usamos la funcion de compararFechas para no tener un codigo tan robusto aqui
        if (compararFechas(arregloIzq[i], arregloDer[j])) {
            arrBitacora[k] = arregloIzq[i];
            i++;
        } else {
            arrBitacora[k] = arregloDer[j];
            j++;
        }
        k++;
    }

    // copia los elementos restantes de cada arreglo, ya que siempre quedara
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
    
    // liberamos la memoria y asi, para que no pete el sistema con tantos arreglos temporales
    delete[] arregloIzq;
    delete[] arregloDer;
}

// Ahora si haremos la funcion para usar la del merge, para que haga el sort

void mergeSort(Bitacora arrBitacora[], int start, int end){
    if (start >= end){
        return; // Esto se refiere a que si el start es igual o mayor que el end, pues ya esta todo bien, asi que no hace falta intentar el codigo
    }
    // vamos a encontrar el punto medio aqui dle arreglo
    int mid = start + (end - start) / 2;
    // Volvemos a llamar a la funcion pero pues ahora lo haremos con lo que viene siendo los dos bloques que tenemos
    mergeSort(arrBitacora, start, mid);
    mergeSort(arrBitacora, mid + 1, end);
    
    // como todo esto es recursivo, pues siempre lo ira acomodando y siempre se ira haciendo mas y mas pequeño y asi. y terminamos
    merge(arrBitacora, start, mid, end);
}


int main(void) {
    //Size de la bitacora como constante para que nadie lo pueda cambiar despues y asi
    const int tamBitacora = 16807;

    Bitacora arrBitacora[tamBitacora];
    int contador = 0;

    ifstream archivo;
    archivo.open("bitacora.txt");
    string linea;

    while(getline(archivo, linea) && contador < tamBitacora){
        // Hacemos un objeto de tipo Bitacora para poder guardar los datos ahi
        Bitacora entrada;
        // Hacemos un stringstream, que es como una linea string que le puedes meter de todo para poder separar los datos que vienen en la linea
        stringstream ss(linea);
        
        string hora_completa;
        string direccion_completa;

        ss >> entrada.mes;
        ss >> entrada.dia;
        ss >> hora_completa;
        ss >> direccion_completa;
        // El getline lo que hace es leer hasta el final de la linea, por lo que lo que queda es el motivo
        getline(ss, entrada.motivo);

        // Ahora tenemos que separar la hora completa en hora, minuto y segundo, lo que se hizo fue en si usar otros stringstreams
        // pero con lo que se guardo en la hora completa, es como lo anterior pero anteriormente lo hicimos con lo que se habia guardado
        // en la Bitacora
        stringstream hora_ss(hora_completa);
        hora_ss >> entrada.hora;
        hora_ss.ignore(); // Ignoramos los dos puntos
        hora_ss >> entrada.minuto;
        hora_ss.ignore(); // Ignoramos los dos puntos
        hora_ss >> entrada.segundo;

        // Ahora tenemos que separar la direccion completa en ip y puerto, donde hacemos lo mismo que con lo que viene siendo lo de la hora completa
        stringstream direccion_ss(direccion_completa);
        // Aqui en vez de muchas veces le decimos a get line donde buscar, despues donde lo guardara y al final hasta donde terminara
        getline(direccion_ss, entrada.ip, ':'); // Leemos hasta el caracter ':'
        // Ahora lo que queda es el puerto, el cual lo guardamos como antes lo habiamos estado haciendo
        direccion_ss >> entrada.puerto;

        // Aqui ponemos toda nuestra entrada en nuestro arreglo, que entrada es como nuestro objeto de tipo bitacora
        arrBitacora[contador] = entrada;

        // Sumamos con el contador
        contador ++;
    }

    archivo.close(); // Cerramos
    
    // Vamos a acomodar todo ahora, vamos a ello

    mergeSort(arrBitacora, 0, contador - 1);
    // Ahora pues tenemos que ver que show con lo que viene siendo la busqueda
    // Para esto primero vamos a pedir la fecha, y con la fecha vamos a buscar la fecha y ver que show con los duplicados
    
    cout << "###### Bienvenido al Programa ######" << endl;
    cout << endl;

    cout << "Porfavor elige una fecha de inicio" << endl;
    Bitacora fecha_inicio = pedir_fecha("------Fecha de inicio------"); 
    
    cout << endl;
    
    cout << "Porfavor elige una fecha de fin" << endl;
    Bitacora fecha_fin = pedir_fecha("------Fecha de fin------");
    
    // Esto para que en el ultimo dia tambien se vea y asi
    fecha_fin.hora = 23;
    fecha_fin.minuto = 59;
    fecha_fin.segundo = 59;

    // Empezamos la busqueda con nuestras funciones
    int indiceInicial = busquedaBinariaInicio(arrBitacora, contador, fecha_inicio);
    int indiceFinal = busquedaBinariaFin(arrBitacora, contador, fecha_fin);
    
    // Pues ya tenemos todo, ahora lo que haremos es ponerlo en un archivo
    ofstream archivo_salida("bitacora_ordenada_al_gusto.txt");
    
    int contador_impresos = 0;
    // si el archivo no se a cerrado
    if(archivo_salida.is_open()){
        // vamos de inicio a fin
        for(int i = indiceInicial; i <= indiceFinal; i++){
            archivo_salida << arrBitacora[i].mes << " " << arrBitacora[i].dia << " " 
                    << arrBitacora[i].hora << ":" << arrBitacora[i].minuto << ":" << arrBitacora[i].segundo << " "
                    << arrBitacora[i].ip << ":" << arrBitacora[i].puerto
                    << arrBitacora[i].motivo << endl;
            contador_impresos ++;
        }
        archivo_salida.close();
    }
    cout << endl;
    cout << "Total de registros encontrados: " << contador_impresos << endl;
    
    cout << endl;
    cout << "Una lista con esos datos ya fue mostrada en un archivo llamada bitacora_ordenada_al_gusto.txt" << endl;
    
    cout << endl;
    cout << "###### Se cerro este Programa ######" << endl;
    
    
    return 0;
}
