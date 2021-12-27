#include "monitorColas.hpp"


ControlColas::ControlColas(){ //Como inicializamos las colas?
    //colaTareas<string>.BoundeQueue(MAX);
    //colaQos(MAX);
    //colaTags(MAX);
}


/*Tenemos el problema de exclusion mutua de poder acceder a las distintas cola, ya que de la forma en la que esta ahora
si accedemos a una cola entonces se activa el lock y ya no podemos acceder a las otras dos*/
/*Igual la solucion esta en hacer 3 monitores uno para cada cola, simplemente para que cada operacion este en exclusión mutua
entonces ya desde el gestorDeColas.cpp le indique que monitor tiene que hacer dependiendo del parametro que le pasen*/

void ControlColas::getQueues(){ /*Devuelve una lista con la palabra clave y el nombre de las colas disponibles para que otros programas puedan buscar el nombre de 
                                    la cola a la que quieren acceder y puedan extraer su clave para dirigirse a ella*/
    unique_lock<mutex> lck(mtx);

}

void ControlColas::publish(){
    unique_lock<mutex> lck(mtx);
    
}

void ControlColas::read(){ /*Parametro de salida que envia el elemento más arriba de la pila que piden*/
    unique_lock<mutex> lck(mtx);

}





/*
#include "ControlCabinas.hpp"

ControlCabinas::ControlCabinas(){
    for(int p =0;p<4;p++){
        cabinaOcup[p] = false;
    }
    permLimpiar = false;
    cabinasLibres = N_CAB;
}


int cabinaOcupada(bool cabinaOCUP[]){
    int w =0;
    while(w<N_CAB){
        if(!cabinaOCUP[w]){
            cabinaOCUP[w] = true;
            return w;
        }
        w++;
    }
    return -1;
}

// Pre: 1 <= i <= N_USER AND 1 <= j <= N.TIMES_USER
// Post: "cab" tomar� el valor de la cabina ocupada por el usuario "i"
// en su "j"-�sima ejecuci�n. Y la cabina queda ocupada
void ControlCabinas::entraUsuario(int& cab){
    unique_lock<mutex> lck(mtx);
    //Si hay permiso de limpiar todos los usuarios que quieran entrar se bloquean || Espera a que tenga al menos una cabina libre, si no tiene cabinas libres se bloquean.
    while(permLimpiar || cabinasLibres == 0){
        esperandoUsuarios.wait(lck);  //waitC(esperandoUsuarios);
    }
    cab = cabinaOcupada(cabinaOcup);
    cabinasLibres--;
}

// Pre: 1<= i <= N_USER AND 1 <= j <= N_TIMES_USER AND 0 <= cab < N_CAB
// Post: la cabina "cab" ha sido liberada
void ControlCabinas::saleUsuario(int cab){
    unique_lock<mutex> lck(mtx);
    cabinaOcup[cab] = false;
    cabinasLibres++;
    if(!permLimpiar && cabinasLibres >0){
        esperandoUsuarios.notify_one();
    }else if(cabinasLibres == N_CAB){
        esperandoLimpieza.notify_one();
    }
}

void ControlCabinas::entraLimpieza(){
    unique_lock<mutex> lck(mtx);
    permLimpiar = true;
    while(cabinasLibres != N_CAB){
        esperandoLimpieza.wait(lck);
    }    
}

void ControlCabinas::saleLimpieza(){
    unique_lock<mutex> lck(mtx);
    permLimpiar = false;
    if(cabinasLibres == N_CAB){
        esperandoUsuarios.notify_all();
    }
}







*/