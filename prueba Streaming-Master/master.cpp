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

void separarcadena(string buffer,const string SEPARADOR, string tweets[]){
    vector<string> words;
    size_t pos = 0;
    while ((pos = buffer.find(SEPARADOR)) != string::npos) {
        words.push_back(buffer.substr(0, pos));
        buffer.erase(0, pos + SEPARADOR.length());
    }
    int i=0;
    for (const auto &str : words) {
        cout << str << endl;
        tweets[i]=str;
        i++;
    }
}

void streaming(Socket chanstreaming, string tweets[], string bloques[], int &error_code_streaming){
  // ------------------------------CONEXIÓN CON STREAMING----------------------------------------------------
  cout<<"entra"<<endl;
  const int MAX_ATTEMPS= 10;
  int count=0;
  int socketstreaming_fd;
  string caca;
  const string peticion="PETICION";
  int send_bytes;



  do{
      //conexión con el servidor
      socketstreaming_fd=chanstreaming.Connect();
      count++;

      //Si error -> esperamos 1 segundo para reconectar
      if(socketstreaming_fd==-1){
          this_thread::sleep_for(chrono::seconds(1));
      }
  }while(socketstreaming_fd==-1 && count<MAX_ATTEMPS);

  while(1){
    //Enviamos peticion al streaming


    cout << "Mandamos peticion al streaming" << endl;
    cin >> caca;
    send_bytes = chanstreaming.Send(socketstreaming_fd, "PETICION");
    if(send_bytes == -1) {
       
        cerr << "Error al mandar datos: " + string(strerror(errno)) + peticion + "\n" ;
        // Cerramos los sockets
        chanstreaming.Close(socketstreaming_fd);
        exit(1);
    }
    
    //Recibimos 25 tweets en un string (buffer)
    string buffer; 

    int rcv_bytes = chanstreaming.Recv(socketstreaming_fd,buffer,sizeof(string));
    if (rcv_bytes == -1) {
        cout << buffer << endl;
        cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets

        chanstreaming.Close(socketstreaming_fd);
    }
    cout << "Separamos la cadena" << endl;
    separarcadena(buffer,SEPARADOR,tweets);
    for(unsigned int i=0;i<5;i++){
        for(unsigned int j=0;j<5;j++){
            bloques[i]=bloques[i]+tweets[5*i+j]+SEPARADOR;
        }
    }
  }

  // Cerramos el socket del master-streaming
  error_code_streaming = chanstreaming.Close(socketstreaming_fd);
  if (error_code_streaming == -1) {
      cerr << "Error cerrando el socket del master: " + string(strerror(errno)) + " aceptado" + "\n";
  }
// --------------------------------------FIN DE CONEXION CON STREAMING--------------------------------------

}

void colas(Socket chancolas, string tweets[], string bloques[], int &error_code_colas){
  //---------------------------------------CONEXION CON GESTOR DE COLAS------------------------------------
  
  const int MAX_ATTEMPS= 10;
  int count=0;
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

  
  //Enviamos una tarea (bloque de 5 tweets) al gestor de colas
  for(int j=0;j<5;j++){
    int send_bytes = chancolas.Send(socketcolas_fd,bloques[j]);
    
    if(send_bytes == -1) {
      cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
      // Cerramos los sockets
      chancolas.Close(socketcolas_fd);
      exit(1);
    }
  }

  // Cerramos el socket del master-colas
  error_code_colas = chancolas.Close(socketcolas_fd);
  if (error_code_colas == -1) {
      cerr << "Error cerrando el socket del master: " + string(strerror(errno)) + " aceptado" + "\n";
  }

//-----------------------------------FIN DE CONEXION CON STREAMING---------------------------------------

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
    thread S;
    thread C;
    int error_code_streaming;
    int error_code_colas;
    string bloques[5]={};
    string tweets[25]={};

    S=thread(&streaming, chanstreaming, tweets,bloques, ref(error_code_streaming));
    //C=thread(&colas, chancolas, tweets,bloques,ref(error_code_colas));

    S.join();
    //C.join();


    if(error_code_streaming==-1||error_code_colas==-1){
      return -1;
    }
    else{
      return 0;
    }
  }
}