//******************************************************************************
// File:   gestorDeColas.cpp
// Author: Equipo 11:
//         818058 Héctor Arcega Vela          820417 Cecilia Monteagudo Redrado
//         815935 Berta Olano Guillén         820949 Carlos Palomera Oliva
//         815767 Alonso Lucas Juberías       821038 Simón Alonso Gutiérrez       
// Date:   enero 2022
// Coms:   Compilar mediante el " make -f Makefile_gestor " asociado
//*******************************************************************************
#include <iostream>
#include <thread>
#include <vector>
#include "monitorColas.hpp"
#include "Socket/Socket.hpp"
using namespace std;

const int MESSAGE_SIZE = 4001; //mensajes de no más 4000 caracteres
const string SEPARADOR="\n";

/*CON EL MENSAJE "ENVIAR" PIDEN QUE SE LES ENVIE INFORMACIÓN, CON EL MENSAJE "RECIBIR" PIDEN QUE SE LES ACEPTE 
PARA QUE EL SERVIDOR RECIBA SU INFORMACION, CON "END" SE CIERRA EL CANAL*/


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


void servmaster(ControlColas& monitorTareas,Socket socket_master,int client_master_fd){
    //extraer de getqueue la palabra clave relacinandola con el nombre de la cola
    int recv_bytes = 0;
    bool end = false;
    string bloque = "";
    string tweet = "";
    int contador = 0;
    while(!end){
        recv_bytes = socket_master.Recv(client_master_fd,tweet,MESSAGE_SIZE);
        if(recv_bytes == -1){
            cerr << "Error al recibir datos de la cola Tareas: " << strerror(errno) << endl;
            // Cerramos el socket
            socket_master.Close(client_master_fd);
            exit(1);
        }
        if(tweet == "-END+"){
            end = true;
            socket_master.Close(client_master_fd);
            printf("fin de conexión master");
        }else{
            bloque += tweet;
            contador++;
            if(contador == 5){
				        cout << "Bloque recibido del master\n";
                monitorTareas.publish(bloque); //Encolando el bloque de tweets en la cola Tareas
                contador = 0;
                bloque = "";
            }           
        }
    }
}


void servworker(ControlColas& monitorTareas,ControlColas& monitorQos,ControlColas& monitorTags,Socket socket_worker,int client_worker_fd){
    int send_bytes = 0;
    int recv_bytes = 0;
    bool end = false;
    string tweets[5] = {""};
    string bloque = "";
    string solicitud = "";
    string tags = "";
    string qos = "";
    while(!end){
        recv_bytes = socket_worker.Recv(client_worker_fd,solicitud,MESSAGE_SIZE); //Recibimos si quieren que enviemos el contenido o lo recibamos. 
        if(recv_bytes == -1){
            cerr << "Error al recibir mensaje del Worker: " << strerror(errno) << endl;
            // Cerramos el socket
            socket_worker.Close(client_worker_fd);
            exit(1);
        }
        if(solicitud == "-END+"){
            end = true;
            socket_worker.Close(client_worker_fd);
            printf("fin de conexión worker");
        }
        else if(solicitud == "ENVIAR"){
            
            monitorTareas.read(bloque);
            separarcadena(bloque,SEPARADOR,tweets);
            for(int i = 0; i < 5; i++){
                send_bytes = socket_worker.Send(client_worker_fd, tweets[i]);
                if(send_bytes == -1){
                    cerr << "Error al enviar datos de la cola Tareas: " << strerror(errno) << endl;
                    // Cerramos el socket
                    socket_worker.Close(client_worker_fd);
                    exit(1);
                }
                tweets[i] = "";
            }
			      cout << "Bloque enviado al worker\n";
            bloque = "";
            
        }
        else if(solicitud == "RECIBIR"){
            recv_bytes = socket_worker.Recv(client_worker_fd,tags,MESSAGE_SIZE);/*Recibimos los tags*/
            if(recv_bytes == -1){
                cerr << "Error al recibir datos de la cola Tareas: " << strerror(errno) << endl;
                // Cerramos el socket
                socket_worker.Close(client_worker_fd);
                exit(1);
            }
			      cout << "Tags recibidas del worker\n";
            monitorTags.publish(tags);/*Encolamos las tags en la cola Tags*/

            recv_bytes = socket_worker.Recv(client_worker_fd,qos,MESSAGE_SIZE); /*Recibimos los qos*/
            if(recv_bytes == -1){
                cerr << "Error al recibir datos de la cola Tareas: " << strerror(errno) << endl;
                // Cerramos el socket
                socket_worker.Close(client_worker_fd);
                exit(1);
            }
			      cout << "Qos recibidas del worker\n";
            monitorQos.publish(qos);/*Encolamos los qos en la cola qos*/
        }
    }
}



void servqos(ControlColas& monitorQos,Socket socket_qos,int client_qos_fd){
    int send_bytes = 0;
    int recv_bytes = 0;
    bool end = false;
    string qos = "";
    string solicitud = "";
    while(!end){
        recv_bytes = socket_qos.Recv(client_qos_fd,solicitud,MESSAGE_SIZE);
        if(recv_bytes == -1){
            cerr << "Error al recibir mensaje del Analizador QoS: " << strerror(errno) << endl;
            // Cerramos el socket
            socket_qos.Close(client_qos_fd);
            exit(1);
        }
        if(solicitud == "-END+"){
            end = true;
            socket_qos.Close(client_qos_fd);
        }
        else if(solicitud == "PETICION"){
            monitorQos.read(qos);
            send_bytes = socket_qos.Send(client_qos_fd, qos);
            if(send_bytes == -1){
                cerr << "Error al enviar datos de la cola Qos: " << strerror(errno) << endl;
                // Cerramos el socket
                socket_qos.Close(client_qos_fd);
                exit(1);
            }
			      cout << "Qos enviadas\n";
        }
    }
}


void servtags(ControlColas& monitorTags,Socket socket_tags,int client_tags_fd){
    int send_bytes = 0;
    int recv_bytes = 0;
    bool end = false;
    string tags = "";
    string solicitud = "";
    while(!end){
        recv_bytes = socket_tags.Recv(client_tags_fd,solicitud,MESSAGE_SIZE); /*Nos tienen que enviar una solicitud de que quieren que les enviemos los datos*/
        if(recv_bytes == -1){
            cerr << "Error al recibir mensaje del Analizador QoS: " << strerror(errno) << endl;
            // Cerramos el socket
            socket_tags.Close(client_tags_fd);
            exit(1);
        }
        if(solicitud == "-END+"){ /*Si la solicitud es -END+ cerramos el canal, si es PETICION entonces les enviamos el dato que quieren*/
            end = true;
            socket_tags.Close(client_tags_fd);
        }
        else if(solicitud == "PETICION"){
            monitorTags.read(tags);
            send_bytes = socket_tags.Send(client_tags_fd, tags);
            if(send_bytes == -1){
                cerr << "Error al enviar datos de la cola Tags: " << strerror(errno) << endl;
                // Cerramos el socket
                socket_tags.Close(client_tags_fd);
                exit(1);
            }
			      cout << "Tags enviadas\n";
        }
    }
}



//DECLARAREMOS 3 MONITORES TODOS ControlColas YA QUE CADA UNO AL TENER DIFERENTES NOMBRES TENDRA SUS FUNCIONES SIN BLOQUEAR AL RESTO DE MONITORES
    //ControlColas MONITORTAREAS    //ControlColas MONITORQOS   //ControlColas MONITORTAGS

int main(int argc,char* argv[]) {
    if (argc != 6){
        cout<<"Ejecutar como puerto_master puerto_worker puerto_qos puerto_tags N_WORKER"<<endl;
    }
    int N_WORKER = 5;
    //Puertos: 1)Master 2)Worker 3)QoS 4)Tags 
    int puerto_master = atoi(argv[1]);
    int puerto_worker = atoi(argv[2]);
    int puerto_qos = atoi(argv[3]);
    int puerto_tags = atoi(argv[4]);
    //Numero de workers: 5)
    N_WORKER = atoi(argv[5]);
    //Canales asociados 
    Socket socket_master(puerto_master);
    Socket socket_worker(puerto_worker);
    Socket socket_qos(puerto_qos);
    Socket socket_tags(puerto_tags);
    
    int error_code = 0, max_connections = 1;
    
    int socket_master_fd, socket_worker_fd, socket_qos_fd, socket_tags_fd;

    int client_master_fd, client_worker_fd[N_WORKER],client_qos_fd,client_tags_fd;
    
    //Monitores asociados al gestor de colas
    ControlColas monitorTareas,monitorQos,monitorTags;
    //Los diferentes threads asociados a cada proceso
    thread master,worker[N_WORKER],qos,tags;

    /*****************************PROCESO MASTER*****************************/
    //Bind Master
    socket_master_fd = socket_master.Bind();
  	if (socket_master_fd == -1) {
  		cerr << "Error en el bind del master: " << strerror(errno) << endl;
  		exit(1);
  	}
  
    //Listen Master
    max_connections = 1;
  	error_code = socket_master.Listen(max_connections);
  	if(error_code == -1) {
  		cerr << "Error en el listen del master: " << strerror(errno) << endl;
  		// Cerramos el socket
  		socket_master.Close(socket_master_fd);
  		exit(1);
  	}
      
    printf("Listo para atender peticiones del master\n");
    client_master_fd = socket_master.Accept();
    if(client_master_fd == -1) {
        cerr << "Error en el accept del master: " + string(strerror(errno)) + "\n";
        // Cerramos el socket del Master
        socket_master.Close(socket_master_fd);
        exit(1);
    }
    master = thread(&servmaster,ref(monitorTareas),socket_master, client_master_fd);
    cout << "Master aceptado\n";



    /*****************************PROCESO WORKER*****************************/
    //Bind Worker
    socket_worker_fd = socket_worker.Bind();
  	if (socket_worker_fd == -1) {
  		cerr << "Error en el bind del worker: " << strerror(errno) << endl;
  		exit(1);
  	}
  
    //Listen Worker
    max_connections = 2*N_WORKER;
  	error_code = socket_worker.Listen(max_connections);
  	if(error_code == -1) {
  		cerr << "Error en el listen del worker: " << strerror(errno) << endl;
  		// Cerramos el socket
  		socket_worker.Close(socket_worker_fd);
  		exit(1);
  	}
      
    
    //Accept Worker
    printf("Listo para atender peticiones de los worker\n");
    for(int i = 0;i<N_WORKER;i++){
        client_worker_fd[i] = socket_worker.Accept();
        if(client_worker_fd[i] == -1) {
            cerr << "Error en el accept worker: " << strerror(errno) << endl;
            // Cerramos el socket
            socket_worker.Close(socket_worker_fd);
            exit(1);
        }
        worker[i] = thread(&servworker,ref(monitorTareas),ref(monitorQos),ref(monitorTags),socket_worker,client_worker_fd[i]);
        cout << "Nuevo worker " + to_string(i) + " aceptado" + "\n";
    }



    /*****************************PROCESO QoS*****************************/
    //Bind QoS
    socket_qos_fd = socket_qos.Bind();
  	if (socket_qos_fd == -1) {
  		cerr << "Error en el bind del QoS: " << strerror(errno) << endl;
  		exit(1);
  	}
  
    //Listen QoS
    max_connections = 1;
  	error_code = socket_qos.Listen(max_connections);
  	if(error_code == -1) {
  		cerr << "Error en el listen del QoS: " << strerror(errno) << endl;
  		// Cerramos el socket
  		socket_qos.Close(socket_qos_fd);
  		exit(1);
  	}

    //Accept QoS 
    printf("Listo para atender peticiones de QoS\n");
    client_qos_fd = socket_qos.Accept();
    if(client_qos_fd == -1) {
        cerr << "Error en el accept del QoS: " + string(strerror(errno)) + "\n";
        // Cerramos el socket del QoS
        socket_qos.Close(socket_qos_fd);
        exit(1);
    }
    qos = thread(&servqos,ref(monitorQos),socket_qos, client_qos_fd);
    cout << "QoS aceptado\n";



    /*****************************PROCESO TAGS*****************************/
    //Bind Tags
    socket_tags_fd = socket_tags.Bind();
  	if (socket_tags_fd == -1) {
  		cerr << "Error en el bind de Tags: " << strerror(errno) << endl;
  		exit(1);
  	}
  
    //Listen Tags
    max_connections = 1;
  	error_code = socket_tags.Listen(max_connections);
  	if(error_code == -1) {
  		cerr << "Error en el listen de Tags: " << strerror(errno) << endl;
  		// Cerramos el socket
  		socket_tags.Close(socket_tags_fd);
  		exit(1);
  	}
      
    //Accept Tags
    printf("Listo para atender peticiones de Tags\n");
    client_tags_fd = socket_tags.Accept();
    if(client_tags_fd == -1) {
        cerr << "Error en el accept de Tags: " + string(strerror(errno)) + "\n";
        // Cerramos el socket del Tags
        socket_tags.Close(socket_tags_fd);
        exit(1);
    }
    tags = thread(&servtags,ref(monitorTags),socket_tags, client_tags_fd);
    cout << "Tags aceptado\n";


    /*****************************JOIN*****************************/
    master.join();
    for(int i=0;i<N_WORKER;i++){
        worker[i].join();
    }
    qos.join();
    tags.join();

    /*****************************Cerrar Sockets*****************************/
    //Cerrar socket master
    error_code = socket_master.Close(socket_master_fd);
    if (error_code == -1) {
        cerr << "Error cerrando el socket del master: " + string(strerror(errno)) + " aceptado" + "\n";
    }
    //Cerrar socket worker
    error_code = socket_worker.Close(socket_worker_fd);
    if (error_code == -1) {
        cerr << "Error cerrando el socket del worker: " + string(strerror(errno)) + " aceptado" + "\n";
    }
    //Cerrar socket qos
    error_code = socket_qos.Close(socket_qos_fd);
    if (error_code == -1) {
        cerr << "Error cerrando el socket del QoS: " + string(strerror(errno)) + " aceptado" + "\n";
    }
    //Cerrar socket tags
    error_code = socket_tags.Close(socket_tags_fd);
    if (error_code == -1) {
        cerr << "Error cerrando el socket del Tags: " + string(strerror(errno)) + " aceptado" + "\n";
    }

    //Despedida
    cout << "Bye bye" << endl;
    return error_code;
}
