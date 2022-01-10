#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include "Socket/Socket.hpp"
#include "Monitor.hpp"

using namespace std;
const string SERVER_ADDRESS = "localhost";
const int TAMMANO_PAQUETES_REDUCIDOS = 5;
const string DELIMITADOR = ";";
const string SEPARADOR = "\n";
int NumMostrados;
const int length = 10000;
Control controlCont;

struct Fecha{

    int elDia;
    int elMes;
    int elAnyo;
    int laHora;

};

    void igualar(Fecha& f, Fecha& g){
        f.elAnyo = g.elAnyo;
        f.elMes = g.elMes;
        f.elDia = g.elDia;
        f.laHora = g.laHora;
    };

   void crear(int dia, int mes, int anyo, int hora, Fecha& f) {

       f.elDia = dia;
       f.elMes = mes;
       f.elAnyo = anyo;
       f.laHora = hora;

     

   };

    int dia(const Fecha& f) {
       return f.elDia;
   };
   
   int hora(const Fecha& f){
       return f.laHora;
   }

   int mes(const Fecha& f) {
       return f.elMes;
   };

   int anyo(const Fecha& f) {
       return f.elAnyo;
   };


struct tag{
    string nombre;
    int num_veces;
    Fecha fecha;
};

std::vector<tag> vectorTags;

//tag vectorTags[NUM_MAXIMOS];
int cuentaTotal = 0, tiempoTotal = 0;


void mostrarDatosTags(){
    ofstream g;
    g.open("AnalisisTAGs.txt");

    tag tagaux[NumMostrados];
    int vectorHoras[24] = {0};
    int auxvector1[NumMostrados] = {0};
    int auxvector2[NumMostrados];
    for(unsigned int i = 0; i < NumMostrados; i++){
        tagaux[i].num_veces = 0;
    }
    controlCont.entraTags();//pide permiso para entrar en el semáforo
    for(unsigned int i = 0; i < vectorTags.size(); i++){


        vectorHoras[hora(vectorTags[i].fecha)/1000]++; //esta aberracion va contando los tweets y sus horas

        /*
        for(unsigned int j= 0; j < NumMostrados; j++){
            cout << "Entra al for1 iteracion " << j << endl;
            if(vectorTags[i].num_veces > tagaux[j].num_veces){
                for(unsigned int l = NumMostrados; l>j; l--){
                    cout << "entra al for2 iteracion " << 10-l+j << endl; 
                    tagaux[l].nombre = tagaux[l-1].nombre;
                    tagaux[l].num_veces = tagaux[l-1].num_veces;
                }
                //aqui
                cout << "Prueba1" << endl;
                tagaux[j].nombre=vectorTags[i].nombre;
                cout << "Prueba2" << endl;
                tagaux[j].num_veces = vectorTags[i].num_veces;
                cout << "Prueba3" << endl;                
                //j = NumMostrados;
            }
        }
        */
       for(unsigned int j = 0; j<NumMostrados; j++){
            if(vectorTags[i].num_veces > tagaux[j].num_veces){ // miramos solo los tags a la derecha de este
                for(unsigned int l = NumMostrados; l > j + 1; l--){ //desplazamos todos los tags de tagaux a la derecha
                    //cout << "control pre asignacion del bulce for2 " << 10-l+j << endl;
                    tagaux[l-1].nombre = tagaux[l-2].nombre;
                    tagaux[l-1].num_veces = tagaux[l-2].num_veces;
                    //cout << "control post asignacion del bulce for2 " << 10-l+j << endl;

                }

                //cout << "control salida del bucle for2 iteracion de for1 " << j << endl;
                tagaux[j].nombre = vectorTags[i].nombre;
                tagaux[j].num_veces = vectorTags[i].num_veces;
                j = NumMostrados; //salimos del bucle pues ya hemos colocado el actual y no hace falta mas
            } 
       }
    }
    controlCont.saleTags();// sale del semáforo

    cout << "-----------------------------------" << endl;
    g << "-----------------------------------" << endl;
    cout << "Los " << NumMostrados << " tags mas utilizados han sido:" << endl;
    g << "Los " << NumMostrados << " tags mas utilizados han sido:" << endl;
    for(unsigned int i = 0; i< NumMostrados; i++){
        cout << "#" << i + 1 << ": " << tagaux[i].nombre << " con " << tagaux[i].num_veces << " veces utilizado" << endl; 
        g << "#" << i + 1 << ": " << tagaux[i].nombre << " con " << tagaux[i].num_veces << " veces utilizado" << endl; 
    }
    int NumMostradosSeguridad = NumMostrados;

    //cout << "llega a fechas" << endl;
    if(NumMostrados > 24){
        
        NumMostrados = 24;
    }

    for(unsigned int i = 0; i < NumMostrados; i++){
        //cout << "for1 " << i << endl;
        for(unsigned int j = 0; j < 24; j++){
            //cout << "for2" << j << endl;
            if(vectorHoras[j]>auxvector1[i]){
                auxvector1[i] = vectorHoras[j];
                auxvector2[i] = j;
            }

        }
        vectorHoras[auxvector2[i]] = 0;
    }

    cout << "-----------------------------------" << endl;
    g << "-----------------------------------" << endl;
    cout << "Las " << NumMostrados << " horas en las que mas tags se han usado son:" << endl;
    g << "Las " << NumMostrados << " horas en las que mas tags se han usado son:" << endl;
    for(unsigned int i = 0; i < NumMostrados; i++){
        cout << "#" << i + 1 << ": " << auxvector2[i]  << " con " << auxvector1[i] << " veces utilizado" << endl; 
        g << "#" << i + 1 << ": " << auxvector2[i]  << " con " << auxvector1[i] << " veces utilizado" << endl; 
    }
    NumMostrados = NumMostradosSeguridad;
}

void mostrarDatosQOS(){
    ofstream g;
    g.open("AnalisisQOS.txt");
    controlCont.entraQos();
    controlCont.saleQos();
    cout << "-----------------------------------" << endl;
    g << "-----------------------------------" << endl;
    cout << "Se han extraido un total de: " << cuentaTotal << " tags, en tiempo total de: " << tiempoTotal/1000 << " milisegundos." << endl;
    g << "Se han extraido un total de: " << cuentaTotal << " tags, en tiempo total de: " << tiempoTotal/1000 << " milisegundos." << endl;
    cout << "Eso hace una media de " << cuentaTotal/(tiempoTotal/1000) << " tags por milisegundo." << endl;
    g << "Eso hace una media de " << cuentaTotal/(tiempoTotal/1000) << " tags por milisegundo." << endl;

    
}

void canalTags(const int &puerto){
    string buffer;
    tag tagaux;
    Socket chanTags(SERVER_ADDRESS, puerto);
    // Conectamos con el servidor. Probamos varias conexiones
    const int MAX_ATTEMPS = 10;
    int count = 0;
    int socketTags_fd;
    do {
        // Conexión con el servidor
        socketTags_fd = chanTags.Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(socketTags_fd == -1) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while(socketTags_fd == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if(socketTags_fd == -1) {
        exit(1);
    }
    //Enviamos la peticion
    int send_bytes, rcv_bytes;

    while(1){
        buffer = "";
        //cout << "Envio peticion de Tags" << endl;
        send_bytes = chanTags.Send(socketTags_fd,"PETICION");
        if(send_bytes == -1) {
        cerr << "Error al enviar datos: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        chanTags.Close(socketTags_fd);
        exit(1);
        }

        //cout << "Recibo los tags"<<endl;
        rcv_bytes = chanTags.Recv(socketTags_fd, buffer, length);
        if(rcv_bytes == -1) {
            string mensError(strerror(errno));
            cerr << "Error al recibir datos: " + mensError + "\n";
            // Cerramos los sockets
            chanTags.Close(socketTags_fd);
        }
        //cout << "Hemos recibido los tags" << endl;
        //cout << buffer << endl;
        //FECHA;#TAG1;#TAG2;#TAG3...

        size_t posSEPARDAOR = 0;
        size_t posFechaInicio;


        for(unsigned int it = 0;it < TAMMANO_PAQUETES_REDUCIDOS; it++){
            //cout << "iteracion " << it << " del bule for" << endl;
            if(posSEPARDAOR == 0){
                posFechaInicio = posSEPARDAOR;
            }
            else{
                posFechaInicio = posSEPARDAOR + 1;
            }

            size_t posMesInicio = buffer.find("-", posFechaInicio + 1);
            size_t posDiaInicio = buffer.find("-", posMesInicio + 1);
            size_t posHoraInicio = buffer.find(" ", posDiaInicio + 1);
            size_t posTagsInicio = buffer.find(DELIMITADOR, posHoraInicio + 1);
            string anyoaux = "";
            string mesaux = "";
            string diaaux = "";
            string horaaux = "";
            Fecha Fechaaux;
   
            
            
            for(unsigned int i = posFechaInicio + 1; i < posMesInicio - 1; i++){
                anyoaux += buffer[i];
            }
            
            for(unsigned int i = posMesInicio + 1; i < posDiaInicio - 1; i++){
                mesaux += buffer[i];
            }
            for(unsigned int i = posDiaInicio + 1; i < posHoraInicio - 1; i++){
                diaaux += buffer[i];
            }
            for(unsigned int i = posHoraInicio + 1; i < posTagsInicio - 1; i++){
                if(buffer[i]!=':'){
                    horaaux += buffer[i];
                }
                
            }
            crear(stoi(anyoaux), stoi(mesaux), stoi(diaaux), stoi(horaaux), Fechaaux); //con esto tenemos las fechas creadas.
            posSEPARDAOR = buffer.find(SEPARADOR);

            size_t posFinTag = buffer.find(DELIMITADOR, posTagsInicio + 1);
            while(posFinTag < posSEPARDAOR && posFinTag != string::npos){
                bool yaestaba = false;
                tagaux.nombre = "";
                tagaux.num_veces = 1;
                for(unsigned int i = posTagsInicio + 1; i < posFinTag; i++){
                    tagaux.nombre += buffer[i];
                }
                tagaux.fecha = Fechaaux;
                controlCont.entraTags();// pide permiso para entrar al semáforo
                for(unsigned int j = 0; j < vectorTags.size(); j++){
                    if(tagaux.nombre == vectorTags[j].nombre){
                        vectorTags[j].num_veces++;
                        yaestaba = true;
                        j = vectorTags.size();
                    }
                }
                if(!yaestaba){
                    vectorTags.resize(vectorTags.size()+1);
                    vectorTags[vectorTags.size() -1].nombre = tagaux.nombre; 
                    vectorTags[vectorTags.size() -1].num_veces = 1; 
                    igualar(vectorTags[vectorTags.size() -1 ].fecha, tagaux.fecha); 

                }
                controlCont.saleTags();// sale del semáforo
                posTagsInicio = posFinTag;
                posFinTag = buffer.find(DELIMITADOR, posTagsInicio + 1);
            }
        }

    }
    
}


void canalQOS(const int &puerto){
    string bufferQOS;
    Socket chanQOS(SERVER_ADDRESS, puerto);
    // Conectamos con el servidor. Probamos varias conexiones
    const int MAX_ATTEMPS = 10;
    int count = 0;
    int socketQOS_fd;
    do {
        // Conexión con el servidor
        socketQOS_fd = chanQOS.Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(socketQOS_fd == -1) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while(socketQOS_fd == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if(socketQOS_fd == -1) {
        exit(1);
    }
    int send_bytes, rcv_bytes;
    //Mandamos peticion
    while(1){
        bufferQOS = "";
        //cout << "Envio peticion de QOS" << endl;
        send_bytes = chanQOS.Send(socketQOS_fd,"PETICION");
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " + string(strerror(errno)) + "\n";
            // Cerramos los sockets
            chanQOS.Close(socketQOS_fd);
            exit(1);
        }

        //cout << "Recibo los QOS" << endl;
        rcv_bytes = chanQOS.Recv(socketQOS_fd, bufferQOS, length);
        if(rcv_bytes == -1) {
            string mensError(strerror(errno));
            cerr << "Error al recibir datos: " + mensError + "\n";
            // Cerramos los sockets
            chanQOS.Close(socketQOS_fd);
        }
        //cout << "Hemos recibido los QOS" << endl;
        //cout << bufferQOS << endl;
        //nos pasan un string con los resultados de analizar un paquete de 5 tweets. Esta organizado de la siguiente manera:
        //empieza por el numero de tags que habia en el tweet, el DELIMITADOR ";" , el tiempo que le ha costado encontrarlo,
        // el DELIMITADOR ";" y el SEPARADOR "\n" entre los tweets
        size_t posCuenta;
        size_t posTiempo;
        size_t inicio = 0;
        for(unsigned int k = 0;k < TAMMANO_PAQUETES_REDUCIDOS; k++){

            //cout << bufferQOS << endl;
            if(k==0){
                posCuenta = 0;
            }
            else{
                posCuenta = bufferQOS.find(DELIMITADOR, posTiempo + 1) + 1;
            }

            posTiempo = bufferQOS.find(DELIMITADOR, posCuenta + 1);
            size_t finTweet = bufferQOS.find(SEPARADOR, posTiempo + 1);

            string cuenta = "", tiempo = "";
            for(unsigned int i = posCuenta; i < posTiempo; i++){
                cuenta += bufferQOS[i];
                //cout << bufferQOS[i] << " " << i << " " << endl;
                
            }
            //cout << cuenta << endl;
            for(unsigned int i = posTiempo + 1; i < finTweet- 1; i++){
                tiempo += bufferQOS[i];
                //cout << bufferQOS[i];
            }
            //cout << endl;
            controlCont.entraQos();//entra en el monitor
            //cout << tiempo << endl;
            cuentaTotal += atoi(cuenta.c_str());
            tiempoTotal += atoi(tiempo.c_str());
            controlCont.saleQos();//sale del monitor

            inicio = bufferQOS.find(SEPARADOR);//para que la siguiente iteración busque el siguiente tweet
        }
    }
}

void cliente(){
    while(true){
        this_thread::sleep_for(chrono::seconds(5));
        cout << "aqui" << endl;

        //cout << "se va a escibir por pantalla " << endl;
        mostrarDatosTags();
        mostrarDatosQOS();
    }
}

int main(int argc, char* argv[]){


    
    cout << "se inician los analizadores" << endl;
    int PUERTO_ANALIZADORES_TAGS = atoi(argv[2]);
    int PUERTO_ANALIZADORES_QOS = atoi(argv[1]);
    NumMostrados = atoi(argv[3]);
    thread threadcliente(&cliente);
    thread channelTAGS(&canalTags, ref(PUERTO_ANALIZADORES_TAGS));
    thread channelQOS(&canalQOS, ref(PUERTO_ANALIZADORES_QOS));
    threadcliente.join();
    channelQOS.join();
    channelTAGS.join();
    return 0;
}
