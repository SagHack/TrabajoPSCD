#include <chrono>
#include <iostream>
#include <thread>
#include "Socket/Socket.hpp"
#include "Monitor.hpp"

using namespace std;
const string SERVER_ADDRESS = "localhost";
const int TAMMANO_PAQUETES_REDUCIDOS = 5;
const string DELIMITADOR = ";";
const string SEPARADOR = "\n";
int NumMostrados;
int coordenadasUsadasTAGS;
int coordenadasUsadasQOS;
const int length = 10000;
Control controlCont;

struct Fecha{

    int elDia;
    int elMes;
    int elAnyo;
    int laHora;

};


   bool esBisiesto(int a) {
     return (a % 4 == 0 && !(a % 100 == 0 && a % 400 != 0));
   }



  bool esFechaValida(int d, int m, int a) {
     bool valida = 1583 <= a && 1 <= m && m <= 12;
     if (valida) {
       switch (m) {
         case 4: case 6: case 9: case 11: 
           valida = 1 <= d && d <= 30; 
           break;
         case 1: case 3: case 5: case 7: case 8: case 10: case 12:
           valida = 1 <= d && d <= 31;
           break;
         case 2: 
           if (!esBisiesto(a)) {
             valida = 1 <= d && d <= 28;
           }
           else
             valida = 1 <= d && d <= 29;
           break;
       }         
     }
     return valida;
   }



   void crear(int dia, int mes, int anyo, int hora, Fecha& f, bool& error) {
     if (esFechaValida(dia, mes, anyo)) {
       f.elDia = dia;
       f.elMes = mes;
       f.elAnyo = anyo;
       f.laHora = hora;
       error = false;
     }
     else
       error = true;
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

tag vectorTags[NUM_MAXIMOS];
int vectorQos[NUM_MAXIMOS];


void mostrarDatosTags(){

    tag tagaux[NumMostrados];
    int vectorHoras[24] = {0};
    int auxvector1[NumMostrados] = {0};
    int auxvector2[NumMostrados];
    for(unsigned int i = 0; i < NumMostrados; i++){
        tagaux[i].num_veces = 0;
    }
    controlCont.entraTags();//pide permiso para entrar en el semáforo
    for(unsigned int i = 0; i < coordenadasUsadasTAGS; i++){


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
    coordenadasUsadasTAGS = 0;
    controlCont.saleTags();// sale del semáforo

    cout << "-----------------------------------" << endl;
    cout << "Los " << NumMostrados << " tags mas utilizados han sido:" << endl;
    for(unsigned int i = 0; i< NumMostrados; i++){
        cout << "#" << i + 1 << ": " << tagaux[i].nombre << " con " << tagaux[i].num_veces << " veces utilizado" << endl; 
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
    cout << "Las " << NumMostrados << " horas en las que mas tags se han usado son:" << endl;
    for(unsigned int i = 0; i < NumMostrados; i++){
        cout << "#" << i << ": " << auxvector2[i]  << " con " << auxvector1[i] << " veces utilizado" << endl; 
    }
    NumMostrados = NumMostradosSeguridad;
}

void mostrarDatosQOS(){
    int num = 0, tiempo = 0;
    unsigned int i = 0;
    controlCont.entraQos();
    while(i < coordenadasUsadasQOS){
        num += vectorQos[i];
        tiempo += vectorQos[i+1];
        i += 2;      
    }
    coordenadasUsadasQOS = 0;
    controlCont.saleQos();
    cout << "-----------------------------------" << endl;
    cout << "Se han extraido un total de: " << num << " tags, en tiempo total de: " << tiempo/1000 << " milisegundos." << endl;
    cout << "Eso hace una media de " << num/(tiempo/1000) << " tags por milisegundo." << endl;
    
}

void canalTags(const int puerto){
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
            bool boolaux;
            
            
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
            crear(stoi(anyoaux), stoi(mesaux), stoi(diaaux), stoi(horaaux), Fechaaux, boolaux); //con esto tenemos las fechas creadas.
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
                for(unsigned int j = 0; j < coordenadasUsadasTAGS + 1; j++){
                    if(tagaux.nombre == vectorTags[j].nombre){
                        vectorTags[j].num_veces++;
                        yaestaba = true;
                        j = coordenadasUsadasTAGS;
                    }
                }
                if(!yaestaba){
                    vectorTags[coordenadasUsadasTAGS] = tagaux; 
                    coordenadasUsadasTAGS++;
                }
                controlCont.saleTags();// sale del semáforo
                posTagsInicio = posFinTag;
                posFinTag = buffer.find(DELIMITADOR, posTagsInicio + 1);
            }
        }

    }
    
}


void canalQOS(const int puerto){
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
    int cuentaTotal = 0, tiempoTotal = 0;
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
                cuentaTotal = 0;
                tiempoTotal = 0;
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
            //en el vector estan el numero de Tags en las componentes pares y en las impares el tiempo
            vectorQos[coordenadasUsadasQOS] = atoi(cuenta.c_str());;
            vectorQos[coordenadasUsadasQOS + 1] = atoi(tiempo.c_str());
            coordenadasUsadasQOS += 2;
            controlCont.saleQos();//sale del monitor

            inicio = bufferQOS.find(SEPARADOR);//para que la siguiente iteración busque el siguiente tweet
        }
    }
}

void cliente(){
    while(true){
        this_thread::sleep_for(chrono::seconds(5));
        //cout << "se va a escibir por pantalla " << endl;
        mostrarDatosTags();
        mostrarDatosQOS();
    }
}

int main(int argc, char* argv[]){
    cout << "se inician los analizadores" << endl;
    coordenadasUsadasTAGS = 0;
    coordenadasUsadasQOS = 0;
    int PUERTO_ANALIZADORES_TAGS = atoi(argv[2]);
    int PUERTO_ANALIZADORES_QOS = atoi(argv[1]);
    NumMostrados = atoi(argv[3]);
    thread threadcliente(&cliente);
    thread channelTAGS(&canalTags, PUERTO_ANALIZADORES_TAGS);
    thread channelQOS(&canalQOS, PUERTO_ANALIZADORES_QOS);
    threadcliente.join();
    channelQOS.join();
    channelTAGS.join();
    return 0;
}
