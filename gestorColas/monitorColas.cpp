#include "monitorColas.hpp"


ControlColas::ControlColas(){ //Como inicializamos las colas?
    //cola<string>.BoundeQueue(MAX);
    //BoundedQueue(TAM);
    //colaQos(MAX);
    //colaTags(MAX);
}


/*Tenemos el problema de exclusion mutua de poder acceder a las distintas cola, ya que de la forma en la que esta ahora
si accedemos a una cola entonces se activa el lock y ya no podemos acceder a las otras dos*/
/*Igual la solucion esta en hacer 3 monitores uno para cada cola, simplemente para que cada operacion este en exclusión mutua
entonces ya desde el gestorDeColas.cpp le indique que monitor tiene que hacer dependiendo del parametro que le pasen*/

void ControlColas::getQueues(string& cadena){ /*Devuelve una lista con la palabra clave y el nombre de las colas disponibles para que otros programas puedan buscar el nombre de 
                                    la cola a la que quieren acceder y puedan extraer su clave para dirigirse a ella*/
    //unique_lock<mutex> lck(mtx);
    //while
      //  cadena = cadena + clave + nombre

}

void ControlColas::publish(string& cadena){/*Añadir en la cola */ //cadena = bloque_twets
    unique_lock<mutex> lck(mtx);
    if(cola.length() >= cola.size()){ //Si esta llena, bloqueamos para que no pueda añadir
        esperandoPublish.wait(lck);
    }
    cola.enqueue(cadena);
    if(cola.length() > 0){  //Si hay más de 0 elementos notificamos a los read para que puedan leer
        esperandoRead.notify_one();
    }
}

void ControlColas::read(string& cadena){ /*Parametro de salida que envia el elemento más arriba de la cola que piden*/
    unique_lock<mutex> lck(mtx);
    if(cola.length() == 0){   /*Si la cola esta vacia que se espere ya que no podrá leer*/
        esperandoRead.wait(lck);
    }
    cadena = cola.first();
    cola.dequeue();
    if(cola.length() < cola.size() - 1){ /*Si el numero de elementos actual de la cola es menor que el máximo notificamos de que se pueden añadir nuevos elementos*/
        esperandoPublish.notify_one();
    }
}
