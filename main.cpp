#include <iostream> 
#include <vector>   
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// Hacemos un struct para lo que viene siendo nuestro .txt y que luego podamos manejar todo correcetamente y no tener que estar haciendo mil cosas raras
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

// Funcion para ordernar, donde vamos a comparar cual es el mas viejo comparando uno con el otro
// Esto es un bool, donde vamos a retornar como que uno es mas grande que el otro, donde si es falso, entonces sabemos que a es mas
// viejo que b. pero si es verdadero, sabremos que b es mas viejo que a
// Esto lo hacemos varias veces por si los meses, dias, horas, minutos o segundos son iguales, pero ya si los dos exactamente iguales pues
// sera falso como quiera y uno se iria arriba y otro abajo
bool comparar(const Bitacora& a, const Bitacora& b){
    if (a.mes != b.mes){
        return ordenar_meses(a.mes) < ordenar_meses(b.mes);
    }
    if (a.dia != b.dia){
        return a.dia < b.dia;
    }
    if (a.hora != b.hora){
        return a.hora < b.hora;
    }
    if (a.minuto != b.minuto){
        return a.minuto < b.minuto;
    }
    return a.segundo < b.segundo;
}

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

// Aqui en vez de lo que viene siendo una busqueda, lo hicimos pero de dirferente forma, donde hizimos una funcion que usamos comparar para ver si b es mayor.
// Lo que le cambiamos, es que si son iguales pues los dos son iguales, y son iguales y asi. Pero esto tiene mas sentido en el for del main, ahi se explica mas a fondo
bool anterior_igual(const Bitacora& a, const Bitacora& b){
    // Esto es como algo que vamos a ver usa la misma logica si una es mas grande que otra, donde si es mas grande pues devolvemos true si b es mas grande, pero si son del mismo tamaño
    if (comparar(a,b)){
        return true;
    }
    return (ordenar_meses(a.mes) == ordenar_meses(b.mes) && a.dia == b.dia && a.hora == b.hora && a.minuto == b.minuto && a.segundo == b.segundo);
}

int main(void) {
    cout << "Bienvenido a nuestro programa!" << endl;


    // Ponemos lo que viene siendo nuestra struct en un vector, el cual es vecBitacora
    vector<Bitacora> vecBitacora;

    // Esto lo que hace es crear un objeto el cual con input file stream vamos a abrir como el archivo ahi
    ifstream archivo;
    // Ahi literal ya tenemos el objeto que puede abrir objetos, por lo que solo abrimos la bitacora y asi
    archivo.open("bitacora.txt");

    // Hacemos esta variable para poder tener los datos en diferentes lineas de estas
    string linea;

    while(getline(archivo, linea)){
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

        // Ahora que ya tenemos todo lo que viene siendo la entrada, lo que hacemos es pushearla al vector, y ya tenemos todo en un vector
        vecBitacora.push_back(entrada);
    }
    archivo.close(); // Cerramos


    // Aquí con una funcion que se investigo llamada sort, podemos usar lo que viene siendo nuestra funcion de comparar entre los meses
    // para poder ordenar todo el vector y asi, lesssgoooo

    // Como trabajamos con vectores, lo que hace sort es que con dos rangos, osea begin y end ejecuta la funcion de comparar y asi
    sort(vecBitacora.begin(), vecBitacora.end(), comparar);

    cout << "Bienvenido a la App" << endl;
    cout << "Porfavor elige una fecha de inicio" << endl;
    Bitacora fecha_inicio = pedir_fecha("------Fecha de inicio------");
    cout << "Porfavor elige una fecha de fin" << endl;
    Bitacora fecha_fin = pedir_fecha("------Fecha de fin------");

    // Ponemos estos valores para usarlos despues para si uno es igual, pues comparar el tiempo tambien
    fecha_fin.hora = 23; 
    fecha_fin.minuto = 59;
    fecha_fin.segundo = 59; 

    cout << "Mostrando entradas entre " << fecha_inicio.mes << " " << fecha_inicio.dia << " a " << fecha_fin.mes << " " << fecha_fin.dia << endl;
    // Empezamos la cuenta!
    int count = 0;

    // Hacemos un vector sin nada pero que sea de tipo bitacora para guardar los que vamos a estar imprimiendo en el documento
    vector<Bitacora> resultados_busqueda;
    // Iniciamos un for para recorrer todo el vector, desde 0 al vector size
    for (int i = 0; i < vecBitacora.size(); ++i) {

        // Iniciamos lo que viene siendo nuestro objeto de tipo Bitacora, el cual sera el que vamos a estar comparando y lo llamaos registro
        Bitacora registro = vecBitacora[i];
        
        // Aqui queremos buscar que la fecha de inicio sea menor, ya que estamos buscando descartar lo que viene siendo que estaba antes de la fecha de inicio
        bool despues_de_inicio = anterior_igual(fecha_inicio, registro); 
        // Aqui buscamos lo contrario, que la fecha de fin sea mayor, para descartar lo que estaba despues de la fecha de fin
        bool antes_de_fin = anterior_igual(registro, fecha_fin);

        // Aqui si los dos son verdaderos, osea que la fecha de inicio es menor y la fecha de fin es mayor, entonces si lo imprimimos y asi
        if (despues_de_inicio && antes_de_fin) {
            resultados_busqueda.push_back(registro); 
            count++;
        }
    }

    // Ahora lo ponemos en lo que viene siendo el vector de resultados de busqueda

    ofstream archivo_salida("bitacora_ordenada_al_gusto.txt");
    if(archivo_salida.is_open()){
        for (int i = 0; i < resultados_busqueda.size(); ++i){
            Bitacora registro = resultados_busqueda[i];
            archivo_salida << registro.mes << " " << registro.dia << " " << registro.hora << ":" << registro.minuto << ":" << registro.segundo
                            << " " << registro.ip << ":" << registro.puerto << " " << registro.motivo << endl;
        }
        archivo_salida.close();
    }

    cout << "-----------------------------\n";
    cout << "Total de registros encontrados: " << resultados_busqueda.size() << endl;
    cout << "Una lista con esos datos ya fue mostrada en un archivo llamada bitacora_ordenada_al_gusto.txt" << endl;
    cout << "Adios!" << endl;
    return 0;
}
