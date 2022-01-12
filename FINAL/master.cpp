//******************************************************************************
// File:   master.cpp
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
using namespace std;

const string SEPARADOR="\n";
const int LONG_TWEETS=10000;
const string ESPACIO=" ";

void conectar(Socket chanstreaming, string tweets[], string bloques[], int &error_code_streaming,Socket chancolas, int &error_code_colas){
  // ------------------------------CONEXIÓN CON STREAMING----------------------------------------------------
  const int MAX_ATTEMPS= 10;
  int count=0;
  int socketstreaming_fd;
  do{
      //conexión con el servidor
      socketstreaming_fd=chanstreaming.Connect();
      count++;

      //Si error -> esperamos 1 segundo para reconectar
      if(socketstreaming_fd==-1){
          this_thread::sleep_for(chrono::seconds(1));
      }
  }while(socketstreaming_fd==-1 && count<MAX_ATTEMPS);

  //---------------------------------------CONEXION CON GESTOR DE COLAS------------------------------------
 
  count=0;
  int socketcolas_fd;
  do{
      //conexión con el servidor
      socketcolas_fd=chancolas.Connect();
      count++;

      //Si error -> esperamos 1 segundo para reconectar
      if(socketcolas_fd==-1){
          this_thread::sleep_for(chrono::seconds(1));
      }
  }while(socketcolas_fd==-1 && count<MAX_ATTEMPS);

  //-------------------------------------------------------------------------------------------------------

  while(1){
    //Enviamos peticion al streaming
    string peticion="PETICION";
    int send_bytes = chanstreaming.Send(socketstreaming_fd,peticion);
    if(send_bytes == -1) {
        cerr << "Error al enviar datos: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        chanstreaming.Close(socketstreaming_fd);
        exit(1);
    }
    cout << "Peticion enviada al Streaming"<<endl;
    
    //Recibimos 25 tweets en un string cada uno

    for(int j=0;j<25;j++){
      int rcv_bytes = chanstreaming.Recv(socketstreaming_fd,tweets[j],LONG_TWEETS);
      if (rcv_bytes == -1) {
          cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
          // Cerramos los sockets
          chanstreaming.Close(socketstreaming_fd);
      }
    }
    cout << "Tweets recibidos"<<endl;

    //Enviamos los tweets al gestor de colas
    for(int k=0;k<25;k++){
      int send_bytes = chancolas.Send(socketcolas_fd,tweets[k]+ESPACIO+SEPARADOR);
    
      if(send_bytes == -1) {
        cerr << "Error al enviar datos: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        chancolas.Close(socketcolas_fd);
        exit(1);
      }
    cout << "Bloques enviados a gestor de colas"<<endl;
    }
  }

  // Cerramos el socket del master-streaming
  error_code_streaming = chanstreaming.Close(socketstreaming_fd);
  if (error_code_streaming == -1) {
      cerr << "Error cerrando el socket del master: " + string(strerror(errno)) + " aceptado" + "\n";
  }
  //-----------------------------------FIN DE CONEXION CON STREAMING---------------------------------------

   // Cerramos el socket del master-colas
    error_code_colas = chancolas.Close(socketcolas_fd);
    if (error_code_colas == -1) {
        cerr << "Error cerrando el socket del master: " + string(strerror(errno)) + " aceptado" + "\n";
    }
  //-----------------------------------FIN DE CONEXION CON COLAS---------------------------------------
}


int main(int argc, char *argv[]){
  if (argc != 5){
    cout<<"Ejecutar como master puertoStreaming puertoColas hostStreaming hostColas"<<endl;
  }
  else{
    int STREAMING_MASTER_PORT = atoi(argv[1]);
    int COLAS_MASTER_PORT = atoi(argv[2]);
    string SERVER_ADDRESS = argv[3];
    string SERVER_ADDRESS_2 = argv[4];
    Socket chanstreaming(SERVER_ADDRESS,STREAMING_MASTER_PORT);
    Socket chancolas(SERVER_ADDRESS_2,COLAS_MASTER_PORT);
    thread C;
    int error_code_streaming;
    int error_code_colas;
    string bloques[5]={};
    string tweets[25]={};

    C=thread(&conectar, chanstreaming, tweets,bloques, ref(error_code_streaming),chancolas,ref(error_code_colas));

    C.join();

    if(error_code_streaming==-1||error_code_colas==-1){
      return -1;
    }
    else{
      return 0;
    }
  }
}