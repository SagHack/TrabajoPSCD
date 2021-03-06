#include <iostream>
#include "Socket/Socket.hpp"
#include <fstream>
#include <chrono>
#include <thread>


using namespace std;

const int TAMANNO_PAQUETE = 25;
const string SEPARADOR = "\n";



void filtrar(int &Puerto){


    Socket chanStreaming(Puerto);


    int socketStreaming_fd = chanStreaming.Bind();
    if (socketStreaming_fd == -1) {
        cerr << "Error en el bind: " + string(strerror(errno)) + "\n";
        exit(1);
    }

    // Listen
    int max_connections = 1;
    int error_code = chanStreaming.Listen(max_connections);
    if (error_code == -1) {
        cerr << "Error en el listen: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        chanStreaming.Close(socketStreaming_fd);
        exit(1);
    }

    // Accept
    int streaming_fd = chanStreaming.Accept();
    if(streaming_fd == -1) {
        cerr << "Error en el accept: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        chanStreaming.Close(socketStreaming_fd);
        exit(1);
    }




    string caca;
    string nombrefichero = "tweets-sinProcesar.csv";
    ifstream f;
    string basura;
    string fechaaux;
    string aplicacion;
    string autor;
    string tweetaux;
    string buffer;
    int length = 1000;
    bool vaBien;
    char charaux;
    char charaux2;
    string chariso;
    f.open(nombrefichero);




    
    
    getline(f,basura);

    while(true){


        cout << "A la espera de una peticion" << endl;
        cin >> caca;
        int rcv_bytes = chanStreaming.Recv(streaming_fd, buffer, length);
        if(rcv_bytes == -1) {
            cout << buffer << endl;
            string mensError(strerror(errno));

            cerr << "Error al recibir datos: " + mensError + "\n";
            // Cerramos los sockets
            chanStreaming.Close(socketStreaming_fd);
        }

        cout << "Mensaje recibido: '" + buffer + "'\n";

        if(buffer == "PETICION"){





                string mensaje = "";
                for(int i= 0; i<TAMANNO_PAQUETE; i++){


                    //getline(f, fechaaux, ';');
                    charaux = ' ';
                    vaBien = true;

                    fechaaux = "";  
                    while(charaux!=';' && charaux !='\n' && !f.eof()){

                        f.get(charaux);
                        fechaaux = fechaaux + charaux;
                    }
                    
                    vaBien = charaux !='\n';
                    

                    //getline(f, aplicacion, ';');
                    aplicacion = "";  
                    while(charaux!=';' && charaux !='\n'  && !f.eof() && vaBien){

                        f.get(charaux);
                        aplicacion = aplicacion + charaux;
                    }
                    
                    vaBien = charaux !='\n';
                    
                    //getline(f, basura, ';');
                    autor = "";  
                    while(charaux!=';' && charaux !='\n'  && !f.eof() && vaBien){

                        f.get(charaux);
                        autor = autor + charaux;
                    }
                    
                    vaBien = charaux !='\n';



                    if(vaBien && !f.eof()){
                        mensaje = mensaje + fechaaux + aplicacion + autor;
                
                        do{

                            f.get(charaux);
                            if((charaux >= 'a' && charaux <= 'z')||(charaux >= '@' && charaux <= 'Z')||(charaux >= ','&&charaux <= ';' )||
                            charaux == ' ' || charaux == '_' || charaux == '#' || charaux == -61){

                                if(charaux == -61){
                                    charaux2 = charaux;
                                    f.get(charaux);
                                    if(charaux == -95 || charaux == -87 || charaux == -83 || charaux == -79 || charaux == -111 || charaux == -77
                                    || charaux == -70 || charaux == -127 || charaux == -119 || charaux == -115 || charaux == -109 || charaux == -102){

                                        mensaje = mensaje + charaux2 + charaux;
                                    }
                                    
                                }
                                else{
                                    
                                    mensaje = mensaje + charaux;
                                }

                            }

                        }while(charaux!='\n' && !f.eof());
                        mensaje = mensaje + SEPARADOR;
                    
                    }
                    else if(!f.eof()){ // siempre y cuando se puedan seguir leyendo tweets
                        i--;//no se ha a??adido el tweet al string, dejamos hueco para uno mas
                    }
                    else{//identico al caso anterior pero reabre el fichero y elimina la primera linea que no es un tweet


                        f.close();
                        f.open(nombrefichero);
                        getline(f,basura);
                        i--;
                    }
                }


                //el paquete ahora siempre es completo
                int send_bytes = chanStreaming.Send(streaming_fd, mensaje);

                

                if(send_bytes == -1){
                    string mensError(strerror(errno));
                    cerr << "Error al enviar el mensaje: " + mensError + "\n";
                    chanStreaming.Close(socketStreaming_fd);
                    exit(1);
                }
                
                cout << "se ha enviado el paquete " << endl;
                buffer = "";

                
                

                
            
        }
        else{
            cout << "el programa no sabe que hacer con este mensaje: " << buffer << endl;
        }

        //vuelve al inicio del bucle de peticiones
    }

}

int main(int argc, char* argv[]){


    int STREAMING_MASTER_PORT = atoi(argv[1]);
    cout << "A la espera de conexi??n" << endl;




 

    thread Filtro=thread(&filtrar, ref(STREAMING_MASTER_PORT));

    Filtro.join();

 

    cout << "no deberia llegar aqui" << endl;
    return 0;


}