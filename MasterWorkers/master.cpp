//MASTER
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
const string SERVER_ADDRESS = "localhost";

const string SEPARADOR="\n";
const int LONG_25TWEETS=10000;

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
    
    //Recibimos 25 tweets en un string (buffer)
    string buffer; 

    int rcv_bytes = chanstreaming.Recv(socketstreaming_fd,buffer,LONG_25TWEETS);
    if (rcv_bytes == -1) {
        cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        chanstreaming.Close(socketstreaming_fd);
    }
    cout << "Tweets recibidos"<<endl;

    separarcadena(buffer,SEPARADOR,tweets);
    for(unsigned int i=0;i<5;i++){
        for(unsigned int j=0;j<5;j++){
            bloques[i]=bloques[i]+tweets[5*i+j]+SEPARADOR;
        }
    }
    cout << "Tweets separados en bloques"<<endl;

    //Enviamos una tarea (bloque de 5 tweets) al gestor de colas
    for(int j=0;j<5;j++){
      if (bloques[j]!=""){
        cout <<"envia colas"<<endl;
        int send_bytes = chancolas.Send(socketcolas_fd,bloques[j]);
      
        if(send_bytes == -1) {
          cerr << "Error al enviar datos: " + string(strerror(errno)) + "\n";
          // Cerramos los sockets
          chancolas.Close(socketcolas_fd);
          exit(1);
        }
        cout << "Bloques enviados a gestor de colas"<<endl;
      }
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
  if (argc != 3){
    cout<<"Ejecutar como master puertoStreaming puertoColas"<<endl;
  }
  else{
    int STREAMING_MASTER_PORT = atoi(argv[1]);
    int COLAS_MASTER_PORT = atoi(argv[2]);
    Socket chanstreaming(SERVER_ADDRESS,STREAMING_MASTER_PORT);
    Socket chancolas(SERVER_ADDRESS,COLAS_MASTER_PORT);
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