//MASTER
#include <iostream>
#include "Socket/Socket.hpp"
#include <fstream>
#include <sstream>
#include <iterator>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

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

int main(int argc, char *argv[]){
    int STREAMING_MASTER_PORT = atoi(argv[1]);
    Socket chanstreaming(STREAMING_MASTER_PORT);
    Socket chancolas(STREAMING_MASTER_PORT);
    int length=8000;

    // Bind
    int socketstreaming_fd = chanstreaming.Bind();
    if (socketstreaming_fd == -1) {
        cerr << "Error en el bind: " + string(strerror(errno)) + "\n";
        exit(1);
    }

    // Listen
    int max_connections = 1;
    int error_code = chanstreaming.Listen(max_connections);
    if (error_code == -1) {
        cerr << "Error en el listen: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        chanstreaming.Close(socketstreaming_fd);
        exit(1);
    }

    // Accept
    socketstreaming_fd = chanstreaming.Accept();

    if(socketstreaming_fd == -1) {
        cerr << "Error en el accept: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        chanstreaming.Close(socketstreaming_fd);
        exit(1);
    }

    //Enviamos peticion
    string peticion="PETICION";
    int send_bytes = chanstreaming.Send(socketstreaming_fd,peticion);
    if(send_bytes == -1) {
        cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        chanstreaming.Close(socketstreaming_fd);
        exit(1);
    }
    
    //Recibimos 25 tweets en un string (buffer)
    string buffer;
    string bloques[5]={};
    string tweets[25]={}; 

    int rcv_bytes = chanstreaming.Recv(socketstreaming_fd,buffer,length);
    if (rcv_bytes == -1) {
        cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        chanstreaming.Close(socketstreaming_fd);
    }
    separarcadena(buffer,SEPARADOR,tweets);
    for(unsigned int i=0;i<5;i++){
        for(unsigned int j=0;j<5;j++){
            bloques[i]=bloques[i]+tweets[5*i+j]+SEPARADOR;
        }
    }

    // Cerramos el socket del master
    error_code = chanstreaming.Close(socketstreaming_fd);
    if (error_code == -1) {
        cerr << "Error cerrando el socket del master: " + string(strerror(errno)) + " aceptado" + "\n";
    }
    return error_code;
}