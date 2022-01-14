//******************************************************************************
// File:   gestorDeColas.cpp
// Author: Equipo 11:
//         818058 Héctor Arcega Vela          820417 Cecilia Monteagudo Redrado
//         815935 Berta Olano Guillén         820949 Carlos Palomera Oliva
//         815767 Alonso Lucas Juberías       821038 Simón Alonso Gutiérrez       
// Date:   enero 2022
// Coms:   Compilar mediante el " make -f Makefile_gestor " asociado
//*******************************************************************************
#include <mutex>
#include <condition_variable>
#include <Semaphore_V4.hpp>
#include <BoundedQueue.hpp>
#include <string>
#include <cstdlib>

const int TAM = 50;

class ControlColas{
    private:
        mutex mtx;
        BoundedQueue<string> cola = BoundedQueue<string>(TAM);
        condition_variable esperandoPublish, esperandoRead;
    public:
        void publish(string cadena);
        void read(string& cadena);
};
