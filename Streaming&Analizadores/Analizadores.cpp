#include <chrono>
#include <iostream>
#include <thread>
#include "Socket/Socket.hpp"

using namespace std;
const string SERVER_ADDRESS = "localhost";
const int NUM_MAXIMOS= 10000;
const int TAMMANO_PAQUETES_REDUCIDOS = 5;
const string DELIMITADOR = ";";
const string SEPARADOR = "\n";
int NumMostrados;
int coordenadasUsadasTAGS;
const int length = 1000;

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
    string usuario;
    Fecha fecha;
};
tag vectorTags[NUM_MAXIMOS];







void mostrarDatosTags(){
    tag tagaux[NumMostrados];
    int vectorHoras[24] = {0};
    int auxvector1[NumMostrados] = {0};
    int auxvector2[NumMostrados];
    for(unsigned int i = 0; i < NumMostrados; i++){
        tagaux[i].num_veces = 0;
    }
    
    for(unsigned int i = 0; i < coordenadasUsadasTAGS; i++){


        vectorHoras[hora(vectorTags[i].fecha)/1000]++; //esta aberracion va contando los tweets y sus horas


        for(unsigned int j= 0; j < NumMostrados; j++){
            if(vectorTags[i].num_veces > tagaux[j].num_veces){
                for(unsigned int l = NumMostrados; l>j; l--){
                    tagaux[l] = tagaux[l-1];

                }
                tagaux[j]=vectorTags[i];
                j = NumMostrados;
            }
        }
    }


    cout << "-----------------------------------" << endl;
    cout << "Los " << NumMostrados << " tags mas utilizados han sido:" << endl;
    for(unsigned int i = 0; i< NumMostrados; i++){
        cout << "#" << i << ": " << tagaux[i].nombre << " con " << tagaux[i].num_veces << " veces utilizado" << endl; 
    }
    int NumMostradosSeguridad = NumMostrados;
    if(NumMostrados > 24){
        
        NumMostrados = 24;
    }

    for(unsigned int i = 0; i < NumMostrados; i++){
        for(unsigned int j = 0; j < 24; j++){
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
/*
void mostrarDatosQOS(){
    
}
*/
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

    int rcv_bytes = chanTags.Recv(socketTags_fd, buffer, length);
    if(rcv_bytes == -1) {
        string mensError(strerror(errno));
        cerr << "Error al recibir datos: " + mensError + "\n";
        // Cerramos los sockets
        chanTags.Close(socketTags_fd);
    }
    //TAGS:FECHA;#TAG1;#TAG2;#TAG3...

    size_t posSEPARDAOR = 0;


    for(unsigned int it = 0;it < TAMMANO_PAQUETES_REDUCIDOS; it++){
        size_t posFechaInicio = buffer.find(":", posSEPARDAOR);
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
            for(unsigned int i = posTagsInicio + 1; i < posFinTag - 1; i++){
                tagaux.nombre += buffer[i];
            }
            tagaux.fecha = Fechaaux;
            for(unsigned int j = 0; j < coordenadasUsadasTAGS + 1; j++){
                if(tagaux.nombre == vectorTags[j].nombre){ //Falta concurrencia
                    vectorTags[j].num_veces++;
                    yaestaba = true;
                    j = coordenadasUsadasTAGS;
                }
            }
            if(!yaestaba){ //Falta concurrencia
                vectorTags[coordenadasUsadasTAGS] = tagaux; 
                coordenadasUsadasTAGS++;
            }
            
            posTagsInicio = posFinTag;
            posFinTag = buffer.find(DELIMITADOR, posTagsInicio + 1);
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

    int rcv_bytes = chanQOS.Recv(socketQOS_fd, bufferQOS, length);
    if(rcv_bytes == -1) {
        string mensError(strerror(errno));
        cerr << "Error al recibir datos: " + mensError + "\n";
        // Cerramos los sockets
        chanQOS.Close(socketQOS_fd);
    }


}
void cliente(){


    while(true){
        this_thread::sleep_for(chrono::seconds(20));

        mostrarDatosTags();
        //mostrarDatosQos();
        coordenadasUsadasTAGS = 0; //Falta concurrencia
        
    }

}

int main(int argc, char* argv[]){
    coordenadasUsadasTAGS = 0;
    int PUERTO_ANALIZADORES_TAGS = atoi(argv[1]);
    int PUERTO_ANALIZADORES_QOS = atoi(argv[2]);
    NumMostrados = atoi(argv[3]);
    thread threadcliente(&cliente);
    thread channelTAGS(&canalTags, PUERTO_ANALIZADORES_TAGS);
    thread channelQOS(&canalQOS, PUERTO_ANALIZADORES_QOS);
    threadcliente.join();
    channelQOS.join();
    channelTAGS.join();
    return 0;
}