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

// ----------------- TODO PARA LA CREACION DE LA LISTA ADYACENCIA POR PUERTOS ----------------- 
// UN NODO DE UNA LISTA DE IPS, ESTO IRA CON EL VERTICE DE PUERTOOOO
struct NodoIP {
    string ip;       
    NodoIP* next;
};

struct verticePuerto {
    int numeroPuerto;      
    NodoIP* listaDeIPs;    // FILA DE IPS FORMADAS EN ESTE PUERTO
    int peso; // CUANTAS VECES SE USO ESTE PUERTO!
};


// CONST PARA EL SIZE DE PUERTOS POSIBLES, UN PUERTO NO PUEDE SER MAS QUE ESTE NUMERO
const int puertosPosibles = 65536;

verticePuerto* grafoPuertos[puertosPosibles];

void AgregarAlGrafo(int puerto, string ip){
    // VALIDAR QUE SI NO ES POSITIVO O QUE NO ENTRA EN NUESTRO RANGO, BYE BYEEEE
    if (puerto < 0 || puerto >= puertosPosibles){
        return;
    }
    // SI ESTA SOLO, LO AGREGAMOOOS
    // LO BONITO DE ESTO ES QUE NO TENDREMOS QUE TRABAJAR COMO grafoPuertos[i]
    // PODEMOS HACER grafoPuertos[puerto] PARA QUE EN VEZ DE i USAMOS LOS PUERTOS
    // ESTO GRACIAS A LO QUE VIENE SIENDO EL "verticePuerto* grafoPuertos[puertosPosibles];" 
    // CADA PUERTO "i" TENDRA YA SU LUGAR INDEPENDIENTE
    if (grafoPuertos[puerto] == NULL){
        grafoPuertos[puerto] = new verticePuerto; // CREAMOS EL VERTICE PRIMERO
        grafoPuertos[puerto]->numeroPuerto = puerto;
        grafoPuertos[puerto]->listaDeIPs = NULL; // LA INICIAMOS VACIA! PARA NO TENER QUE HACER UNA FUNCION PARA ESO LOL
        grafoPuertos[puerto]->peso = 0; // INICIAMOS EN CERO, LUEGO YA LO SUMAMOS, PERO ESTO ES PARA EL FAN OUT
    }
    // VEMOS LO QUE VIENE SIENDO LA IP Y SU LISTA!!
    NodoIP* NodoIp = new NodoIP(); // CREAMOS LO QUE VIENE SIENDO UN NUEVO NodoIP PARA ESTE PUERTO
    NodoIp->ip = ip; // EL IP CON EL IP

    // LINKEAMOS LO QUE VIENE SIENDO NUESTRA IP CON LA LISTA EN EL INICIO
    NodoIp->next = grafoPuertos[puerto]->listaDeIPs; // AQUI LO METEMOS AL PRINCIPIO, OSEA EL NODOIP NEXT VA A LA CABEZA
    // QUE ESTA EN EL PUERTO, QUE ES LA LISTA DEIPS, QUE ES LA QUE INICIO EN NULL
    grafoPuertos[puerto]->listaDeIPs = NodoIp; // AHORA ESE NUEVO ES LA CABEZA Y ASI

    // AUMENTAMOS UNOOOO AL PESO
    grafoPuertos[puerto]->peso++;
}



int main(){
    // ABRIMOS LA BITACORA, LITERAL EL CODE DE TODAS LAS ENTREGAS
    // LA HACEMOS UN ARRAY
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

        // -------------- UNICO CAMBIO ESTA ENTREGA --------------    
        // LLENAR CON LO QUE VIENE SIENDO LOS DATOS DE PUERTO Y DE IPS PARA PONER TODO EN NUESTROS GRAFOS
        AgregarAlGrafo(entrada.puerto, entrada.ip);
        // Aqui ponemos toda nuestra entrada en nuestro arreglo, que entrada es como nuestro objeto de tipo bitacora
        arrBitacora[contador] = entrada;

        // Sumamos con el contador
        contador ++;
    }

    archivo.close(); // Cerramos

    // EL CONTADOR ES EL NUMERO DE DATOS LEIDOS DESDE BITACORA
    const int datosBitacora = contador;
    // DETERMINAR EL FAN OUT (PESO) DE CADA NODO (VERTICE)
    // ESTO LO PONDREMOS EN UN ARCHIVO.TXT QUE SE LLAMARA FANOUTINIDIVIDUALES.TXT
    
    cout << "Cargando analisis" << endl;
    cout << endl;
    ofstream archivo_salida_fan_out_individual("FanOutIndividuales.txt");

    int contador_puertos = 0;

    int max_fan_out = 0;
    int segundo = 0;
    int tercero = 0;
    int cuarto = 0;
    int quinto = 0;
    
    int puerto_ganador = -1;
    int puerto_segundo = 0;
    int puerto_tercero = 0;
    int puerto_cuarto = 0;
    int puerto_quinto = 0;
    
    if(archivo_salida_fan_out_individual.is_open()){
        for(int i = 0; i < puertosPosibles; i++){
            // IMRPIMIR EN UN TXT
            if(grafoPuertos[i] != NULL){
                archivo_salida_fan_out_individual << "Puerto: " <<
                grafoPuertos[i]->numeroPuerto <<
                " -> Fan Out -> " << grafoPuertos[i]->peso << endl;
                
                contador_puertos++;
                // BUSCAR AL QUE TENGA MAS FAN OUT
                int pesoActual = grafoPuertos[i]->peso;
                int puertoActual = grafoPuertos[i]->numeroPuerto;
                // IR RECORRIENDO TODO YA QUE SI ESTAMOS ASI
                // 1.80 2.70 
                // LLEGA 85, SE SALTA AL DOS, Y QUEDA 2. 75, PERO EL 70 SE FUE, ASI QUE PUES RECORREMOS
                // TODO HACIA ABAJO
                if(pesoActual > max_fan_out){
                    quinto = cuarto; 
                    puerto_quinto = puerto_cuarto;

                    cuarto = tercero; 
                    puerto_cuarto = puerto_tercero;
                    
                    tercero = segundo; 
                    puerto_tercero = puerto_segundo;
                    
                    segundo = max_fan_out; 
                    puerto_segundo = puerto_ganador;
                    
                    max_fan_out = pesoActual;
                    puerto_ganador = puertoActual;
                }
                else if(pesoActual > segundo){
                    quinto = cuarto; 
                    puerto_quinto = puerto_cuarto;
                    
                    cuarto = tercero; 
                    puerto_cuarto = puerto_tercero;
                    
                    tercero = segundo; 
                    puerto_tercero = puerto_segundo;
                    
                    segundo = pesoActual;
                    puerto_segundo = puertoActual;
                }
                else if(pesoActual > tercero){
                    quinto = cuarto; 
                    puerto_quinto = puerto_cuarto;
                    
                    cuarto = tercero; 
                    puerto_cuarto = puerto_tercero;
                    
                    tercero = pesoActual;
                    puerto_tercero = puertoActual;
                }
                else if(pesoActual > cuarto){
                    quinto = cuarto; 
                    puerto_quinto = puerto_cuarto;
                    
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

    cout << "Que nodos tienen el mayor fan-out (considerando por puertos?" << endl;
    cout << "Puerto Top 1: " << puerto_ganador << " con un total de fan-outs de:  " << max_fan_out << endl;
    cout << "Puerto Top 2: " << puerto_segundo << " con un total de fan-outs de:  " << segundo << endl;
    cout << "Puerto Top 3: " << puerto_tercero << " con un total de fan-outs de:  " << tercero << endl;
    cout << "Puerto Top 4: " << puerto_cuarto << " con un total de fan-outs de:  " << cuarto << endl;
    cout << "Puerto Top 5: " << puerto_quinto << " con un total de fan-outs de:  " << quinto << endl;

    cout << endl;

    cout << "Puertos totales: " << contador_puertos << endl;

    
    cout << endl;
    cout << "Deduce las conexiones de un usuario invitado, root y admin" << endl;

    // CREAMOS UN NUEVO ARCHIVO PARA QUE SE VEA LIMPIO
    int encontrados = 0;

    ofstream archivo_hackers("ReporteHackers.txt");
    

    if(archivo_hackers.is_open()){

        for(int i = 0; i < datosBitacora; i++){
            
            // SACAMOS MENSAJEEEE        
            string m = arrBitacora[i].motivo; 
            
            bool esSospechoso = false;

            // ESTO CREO QUE FUE LO MAS ROLLOSO PERO ES BUSCAR COMO DE QUE LETRA POR LETRA Y ASI
            // LE AGREGUE UN "- 5" AL LENGTH PARA QUE NO MUERA SI LA PALABRA ESTA AL FINAL
            // SE IRA LETRA POR LETRA, POR LO QUE SI NO ES ROOT O ASI, PUES BYEEEE
            int largo = m.length();
            for(int j = 0; j < largo; j++) {
                
                // BUSCAMOS ROOT
                if(m[j] == 'r' && m[j+1] == 'o' && m[j+2] == 'o' && m[j+3] == 't'){
                    esSospechoso = true;
                    break; // SE ENCONTRO, FUIMONOS, YA NO VAMOS A ADMIN
                }
                // LO MISMO! BUSCAMOS ADMIN
                if(m[j] == 'a' && m[j+1] == 'd' && m[j+2] == 'm' && m[j+3] == 'i' && m[j+4] == 'n'){
                    esSospechoso = true;
                    break; // SE ENCONTRO ACA, FUIMONOS
                }
            }

            // SE DELATA, COMO ES TRUE, SE PONE EN EL ARCHIVO
            if(esSospechoso){
                // SE GUARDA EN EL TXT
                archivo_hackers << "ALERTA IP [" << arrBitacora[i].ip << "]: " << m << endl;
                encontrados++;
            }
        }
        cout << "Respuesta: Checar el archivo Reporteackers.txt" << endl;

        cout << "Total de sospechosos detectados: " << encontrados << endl;
        
        archivo_hackers.close(); // CERRAMOS EL ARCHIVO IMPORTANTE
    }
    cout << endl;
    cout << "Gracias por ver :)" << endl;

    return 0;
}