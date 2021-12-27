//gestorDeColas
#include <iostream>
#include <thread>
#include "monitorColas.hpp"
#include "Socket/Socket.hpp"


void servmaster(controlColas& monitorColas,Socket socket_master,int client_master_fd){
        
}

void servworker(controlColas& monitorColas,Socket socket_worker,int client_worker_fd){

}

void servqos(controlColas& monitorColas,Socket socket_qos,int client_qos_fd){

}

void servtags(controlColas& monitorColas,Socket socket_tags,int client_tags_fd){

}






using namespace std;
const int N_WORKER = 20;
int main(int argc,char* argv[]) {
    //Puertos: 1)Master 2)Worker 3)QoS 4)Tags
    int puerto_master = atoi(argv[1]);
    int puerto_worker = atoi(argv[2]);
    int puerto_qos = atoi(argv[3]);
    int puerto_tags = atoi(argv[4]);
    
    //Canales asociados 
    Socket socket_master(puerto_master);
    Socket socket_worker(puerto_worker);
    Socket socket_qos(puerto_qos);
    Socket socket_tags(puerto_tags);
    
    int error_code = 0, max_connections = 1;
    
    int socket_master_fd, socket_worker_fd, socket_qos_fd, socket_tags_fd;

    int client_master_fd, client_worker_fd[N_WORKER],client_qos_fd,client_tags_fd;
    
    //Monitor asociado al gestor de colas
    controlColas monitorColas;
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
    master = thread(&servmaster,ref(monitorColas),socket_master, client_master_fd);
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
        worker[i] = thread(&servworker,ref(monitorColas),socket_worker,client_worker_fd); //El que le pasamos aqui es el client_worker_fd
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
    qos = thread(&servqos,ref(monitorColas),socket_qos, client_qos_fd);
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
    tags = thread(&servtags,ref(monitorColas),socket_tags, client_tags_fd);
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









/*
//Servidor

#include <iostream>
#include <thread>
#include <ctime>
#include<chrono>
#include "ControlCabinas.hpp"
#include "Socket.hpp"


using namespace std;
using namespace chrono;

void guardarDatos(string buffer,string& t_texto, string& t_numero){
    t_texto = "";
    t_numero = "";
    bool tiene_numero = false;
    for(int i =0;i<buffer.length();i++){
        if(buffer[i-1]==','){
            tiene_numero = true;
        }
        if(tiene_numero){
            t_numero += buffer[i];
        }else if(buffer[i] != ','){
            t_texto +=buffer[i];
        }
    }
}




void servUsuario(ControlCabinas& monitorCab,Socket socket_user,int client_user_fd){
    int length = 4001;
    string buffer;
    string t_texto ="",t_numero="";  
    int cab =0;
    int recvbytes=0,sendbytes=0;
    bool out = false;
    steady_clock::time_point t1,t2;
    while(!out){
        recvbytes = socket_user.Recv(client_user_fd,buffer,length);
        if (recvbytes == -1) {
            cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
            // Cerramos los sockets
            socket_user.Close(client_user_fd);
        }
       
        guardarDatos(buffer,t_texto,t_numero);
        if(t_texto == "RESERVAR_CABINA"){ //RESERVAR_CABINA,i
            monitorCab.entraUsuario(cab);
            t1 = steady_clock::now();
            sendbytes = socket_user.Send(client_user_fd,"CONCEDIDA," + to_string(cab));
            if(sendbytes == -1) {
                cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
                // Cerramos los sockets
                socket_user.Close(client_user_fd);
                exit(1);
            }
        }else if (t_texto == "LIBERAR_CABINA"){ //LIBERAR_CABINA,c
            cab = stoi(t_numero);
            monitorCab.saleUsuario(cab);
            t2 = steady_clock::now();
        }else if(t_texto == "FIN"){ //FIN
            out = true;
            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
            double tiempo_total = time_span.count();
            //double time = (double(t_cab1-t_cab0)/CLOCKS_PER_SEC); 
            sendbytes = socket_user.Send(client_user_fd, "TIEMPO_TOTAL," + to_string(tiempo_total));
            if(sendbytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                socket_user.Close(client_user_fd);
                exit(1);
            }
        }
    }
}




//RESPUESTA DEL SERVIDOR AL PROCESO LIMPIEZA
void servLimpieza(ControlCabinas& monitorCab, Socket sosocket_limp, int limp_fd){
    int length = 100;
    string buffer;
    string t_texto ="",t_numero="";
    int sendbytes ,recvbytes;
    bool cabinaOcup[N_CAB] = {false};
    bool out = false;
    double tiempo_total =0;
    steady_clock::time_point t1,t2;
    while(!out){
        // Recibimos el mensaje del cliente
        recvbytes = sosocket_limp.Recv(limp_fd,buffer,length);
        if (recvbytes == -1) {
            cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
            // Cerramos los sockets
            sosocket_limp.Close(limp_fd);
        }

        guardarDatos(buffer,t_texto,t_numero);

        
        if(t_texto == "LIMPIEZA_ENTRAR"){
            monitorCab.entraLimpieza();
            t1 = steady_clock::now();
            //CUANDO RECIBE EL LIMPIEZA ENTRAR HACE LA COMPROBACIÓN CON EL MONITOR Y SI PUEDE ENVIA LOS DATOS DE OK_LIMPIEZA
            sendbytes = sosocket_limp.Send(limp_fd, "OK_LIMPIEZA");  
            if(sendbytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                sosocket_limp.Close(limp_fd);
                exit(1);
            }
        }else if(t_texto == "LIMPIEZA_SALIR"){
            monitorCab.saleLimpieza();
            t2 = steady_clock::now();
            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
            tiempo_total += time_span.count();
        }else if(t_texto == "FIN"){
            out = true;
            //CUANDO RECIBE EL FIN DE LIMPIEZA LO QUE TIENE QUE HACER ES ENVIAR EL TOTAL DE TIEMPO 
            sendbytes = sosocket_limp.Send(limp_fd, "TIEMPO_TOTAL," + to_string(tiempo_total));  
            if(sendbytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                sosocket_limp.Close(limp_fd);
                exit(1);
            }
        }
    }
}







int main(int argc,char* argv[]) {
    ControlCabinas monitorCab;
    int SERVER_PORT_usuario = atoi(argv[2]);
    int SERVER_PORT_limp = atoi(argv[1]);
    thread usuarios[N_USER];
    Socket socket_user(SERVER_PORT_usuario);
    Socket socket_limp(SERVER_PORT_limp);
    int socket_user_fd, socket_limp_fd;


    //LIMPIEZA
    //Bind limpieza
    socket_limp_fd =socket_limp.Bind();
  	if (socket_limp_fd == -1) {
  		cerr << "Error en el bind: " << strerror(errno) << endl;
  		exit(1);
  	}
  
      // Listen limpieza
    int max_connections = 1;    //REVISAR!!!!!!!!!!!!!!
  	int error_code = socket_limp.Listen(max_connections);
  	if(error_code == -1) {
  		cerr << "Error en el listen: " << strerror(errno) << endl;
  		// Cerramos el socket
  		socket_limp.Close(socket_limp_fd);
  		exit(1);
  	}
      
    
    int client_limp_fd = socket_limp.Accept();
    if(client_limp_fd == -1) {
            cerr << "Error en el accept: " + string(strerror(errno)) + "\n";
            // Cerramos el socket
            socket_limp.Close(client_limp_fd);
            exit(1);
        }
     thread limpieza(&servLimpieza,ref(monitorCab),socket_limp, client_limp_fd);
     cout << "Nueva limpieza aceptado\n";


    //USUARIOS
    //Bind
    socket_user_fd = socket_user.Bind();
    if (socket_user_fd == -1) {
        cerr << "Error en el bind: " << strerror(errno) << endl;
        exit(1);
    }

    //Listen
    max_connections = 2*N_USER;
    error_code = socket_user.Listen(max_connections);
    if(error_code == -1) {
        cerr << "Error en el listen: " << strerror(errno) << endl;
        // Cerramos el socket
        socket_user.Close(socket_user_fd);
        exit(1);
    }

    //Accept
    printf("Listo para atender peticiones de los usuarios: \n");
    for(int i = 0;i<N_USER;i++){
        int client_user_fd = socket_user.Accept();
        if(client_user_fd == -1) {
            cerr << "Error en el accept: " << strerror(errno) << endl;
            // Cerramos el socket
            socket_user.Close(socket_user_fd);
            exit(1);
        }
        usuarios[i] = thread(&servUsuario,ref(monitorCab),socket_user,client_user_fd);
        cout << "Nuevo cliente " + to_string(i) + " aceptado" + "\n";
    }
    limpieza.join();
    for (int i=0; i<N_USER; i++) {
       usuarios[i].join();
    }
    error_code = socket_user.Close(socket_user_fd);
    if (error_code == -1) {
        cerr << "Error cerrando el socket del servidor: " + string(strerror(errno)) + " aceptado" + "\n";
    }

    error_code = socket_limp.Close(socket_limp_fd);
    if (error_code == -1) {
        cerr << "Error cerrando el socket del servidor: " + string(strerror(errno)) + " aceptado" + "\n";
    }
    cout << "Bye bye" << endl;
    return error_code;
}
*/