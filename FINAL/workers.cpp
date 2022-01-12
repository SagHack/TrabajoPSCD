//******************************************************************************
// File:   workers.cpp
// Author: Equipo 11:
//         818058 Héctor Arcega Vela          820417 Cecilia Monteagudo Redrado
//         815935 Berta Olano Guillén         820949 Carlos Palomera Oliva
//         815767 Alonso Lucas Juberías       821038 Simón Alonso Gutiérrez       
// Date:   enero 2022
// Coms:   Compilar mediante el " make -f Makefile_MW " asociado
//*******************************************************************************
#include <iostream>
#include "Socket/Socket.hpp"
#include <fstream>
#include <sstream>
#include <iterator>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>

using namespace std;

const string SEPARADOR="\n";
const string DELIMITADOR=";";
const string ESPACIO=" ";
const int N_PALABRAS_POR_TWEET=150;
const int N_LETRAS_POR_PALABRA=280;
const int TAMANO_MAXIMO = 10000;

//separa el buffer en tweets por el separador
void separarcadena(string buffer,const string SEPARADOR, string tweets[]){
    vector<string> words;
    size_t pos = 0;
    while ((pos = buffer.find(SEPARADOR)) != string::npos) {
        words.push_back(buffer.substr(0, pos));
        buffer.erase(0, pos + SEPARADOR.length());
    }
    int i=0;
    for (const auto &str : words) {
        tweets[i]=str;
        i++;
    }
}


//Extrae los tags de cada tweet y los separa por DELIMITADOR
void extraertags(string tweets,string &tags, int &cuenta){
    string palabras[N_PALABRAS_POR_TWEET]={};
    vector<string> words;
    separarcadena(tweets,ESPACIO,palabras);
    cuenta=0;
    size_t pos = 0;
    if((pos = tweets.find(DELIMITADOR)) != string::npos) {
        words.push_back(tweets.substr(0, pos));
        tweets.erase(0, pos + SEPARADOR.length());
    }
    int i=0;
    for (const auto &str : words) {
        tags += str;
        tags += DELIMITADOR;
        i++;
    }

    string enlace="https://";
    for(int i=0;i<N_PALABRAS_POR_TWEET;i++){ 
        if((palabras[i].front()=='#')||(palabras[i].front()=='@')||palabras[i].find(enlace)==0){
            tags+=palabras[i]+DELIMITADOR;
            cuenta++;
        }
    }
}

//Recibe los tweets del gestor de colas y envía de nuevo al gestor de colas los tags y los qos
void worker(int socketworker_fd, Socket chancolas){
    int cuenta;
    int tiempo[5]={};
    string tweets[5];
    string qos[5];
    string tags[5];
    string enviartags;

    chrono::time_point<chrono::steady_clock> start,end;

    while (1){

        cout << "Enviamos peticion" << endl;
        int send_bytes = chancolas.Send(socketworker_fd,"ENVIAR");
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " + string(strerror(errno)) + "\n";
            // Cerramos los sockets
            chancolas.Close(socketworker_fd);
            exit(1);
        }
        
        for(unsigned int i = 0; i<5; i++){
            tweets[i]="";
        }
        
        for(int j=0;j<5;j++){
          int rcv_bytes = chancolas.Recv(socketworker_fd,tweets[j],TAMANO_MAXIMO);
          if (rcv_bytes == -1) {
              cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
              // Cerramos los sockets
              chancolas.Close(socketworker_fd);
          }
          cout << "Recibimos tweets"<<endl;
        }

        start=chrono::steady_clock::now();

        for(unsigned int i=0;i<5;i++){
            tags[i]="";
            extraertags(tweets[i],tags[i],cuenta);
            end=chrono::steady_clock::now();
            tiempo[i]=chrono::duration_cast<chrono::microseconds>(end-start).count();
            qos[i]+=to_string(cuenta)+DELIMITADOR+to_string(tiempo[i])+DELIMITADOR;
            tiempo[i]=0;
        }
        
        cout << "Enviamos Recibir" << endl;
        int send_bytes2 = chancolas.Send(socketworker_fd,"RECIBIR");

        if(send_bytes2 == -1) {
            cerr << "Error al enviar peticion: " + string(strerror(errno)) + "\n";
            // Cerramos los sockets
            chancolas.Close(socketworker_fd);
            exit(1);
        }

        enviartags="";
        for(unsigned int i=0;i<5;i++){
            enviartags += tags[i]+SEPARADOR;
        }

        int send_bytes3 = chancolas.Send(socketworker_fd,enviartags);
        if(send_bytes3 == -1) {
            cerr << "Error al enviar tags: " + string(strerror(errno)) + "\n";
            // Cerramos los sockets
            chancolas.Close(socketworker_fd);
            exit(1);
        }
        cout << "Envia tags " <<endl;
        
        string enviarqos="";
        for(unsigned int i=0;i<5;i++){
            enviarqos += qos[i]+SEPARADOR;
        }
        int send_bytes4 = chancolas.Send(socketworker_fd,enviarqos);
        if(send_bytes4 == -1) {
            cerr << "Error al enviar qos: " + string(strerror(errno)) + "\n";
            // Cerramos los sockets
            chancolas.Close(socketworker_fd);
            exit(1);
        }
        cout << "Envia qos "<<endl;
        for(unsigned int i = 0;i<5;i++){
            qos[i]="";
        }
    }
}

int main(int argc, char *argv[]){
  if (argc != 4){
    cout<<"Ejecutar como workers puertoColas numeroDeWorkers hostColas"<<endl;
  }
    int WORKER_COLAS_PORT = atoi(argv[1]);
    int N_WORKERS = atoi(argv[2]);
    string SERVER_ADDRESS = argv[3];
    Socket chancolas(SERVER_ADDRESS,WORKER_COLAS_PORT);
    thread W[N_WORKERS];

    int socketworker_fd[N_WORKERS];

    const int MAX_ATTEMPS= 10;
    int count=0;

    for (int i=0; i<N_WORKERS; i++) {
        count =0;
        do{
            //conexión con el servidor
            socketworker_fd[i]=chancolas.Connect();
            count++;

            //Si error -> esperamos 1 segundo para reconectar
            if(socketworker_fd[i]==-1){
                this_thread::sleep_for(chrono::seconds(1));
            }
        }while(socketworker_fd[i]==-1 && count<MAX_ATTEMPS);
        
        W[i] = thread(&worker,socketworker_fd[i],chancolas);
        cout << "Nuevo worker " + to_string(i) + " aceptado" + "\n";
    }

    //Lanzamos N_WORKERS threads que se encargaran de recibir tweets y extraer tags y qos
    for (int i=0; i<N_WORKERS; i++) {
        W[i].join();
        
        // Cerramos el socket del worker
        int error_code = chancolas.Close(socketworker_fd[i]);
        if (error_code == -1) {
        cerr << "Error cerrando el socket del worker: " + string(strerror(errno)) + " aceptado" + "\n";
        }
    }
    return 0;
}
