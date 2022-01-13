#**********************************************************************************
# File:   monitorColas.cpp
# Author: Equipo 11:
#         818058 Héctor Arcega Vela          820417 Cecilia Monteagudo Redrado
#         815935 Berta Olano Guillén         820949 Carlos Palomera Oliva
#         815767 Alonso Lucas Juberías       821038 Simón Alonso Gutiérrez       
# Date:   enero 2022
# Coms:   Compilar mediante el " make -f Makefile_gestor " asociado
#**********************************************************************************

#include "monitorColas.hpp"


void ControlColas::publish(string& cadena){/*Añadir en la cola */ //cadena = bloque_twets
    unique_lock<mutex> lck(mtx);
    while(cola.length() >= cola.size()){ //Si esta llena, bloqueamos para que no pueda añadir
        esperandoPublish.wait(lck);
    }
    cola.enqueue(cadena);
    if(cola.length() > 0){  //Si hay más de 0 elementos notificamos a los read para que puedan leer
        esperandoRead.notify_one();
    }
}

void ControlColas::read(string& cadena){ /*Parametro de salida que envia el elemento más arriba de la cola que piden*/
    unique_lock<mutex> lck(mtx);
    while(cola.length() == 0){   /*Si la cola esta vacia que se espere ya que no podrá leer*/
        esperandoRead.wait(lck);
    }
    cadena = cola.first();
    cola.dequeue();
    if(cola.length() < cola.size() - 1){ /*Si el numero de elementos actual de la cola es menor que el máximo notificamos de que se pueden añadir nuevos elementos*/
        esperandoPublish.notify_one();
    }
}
