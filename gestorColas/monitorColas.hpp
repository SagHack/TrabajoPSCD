#include <mutex>
#include <condition_variable>
#include <Semaphore_V4.hpp>
#include "BoundedQueue.hpp"
#include <string>
#include <cstdlib>

int MAX = 50;

class ControlColas{
    private:
        mutex mtx;
        BoundedQueue<string> colaTareas(MAX); //Como las ponemos en el constructor?
        BoundedQueue<string> colaQos(MAX);
        BoundedQueue<string> colaTags(MAX);
        condition_variable esperandoTareas, esperandoQos,esperandoTags;
    public:
        ControlColas();
        void ControlColas::getQueues();
        void ControlColas::publish();
        void ControlColas::read();
};






/*
#include <mutex>
#include <condition_variable>
#include <Semaphore_V4.hpp>
#include <string>
#include <cstdlib>


const int N_USER = 20; //num de usuarios 
const int N_TIMES_USER = 30; //num de veces un usuario utiliza el sistema
const int N_TIMES_CLEANING = 5; //num de limpiezas a ejecutar
const int PER_CLEANING = 100; //tiempo de espera antes de cada limpieza
const int N_CAB = 4; //num de cabinas en el locutorio


class ControlCabinas{
private:
    mutex mtx;
    bool cabinaOcup[N_CAB],permLimpiar;
    int cabinasLibres;
    double tiempo_total;
    condition_variable esperandoUsuarios, esperandoLimpieza;
public:
    ControlCabinas();
    //~ControlCabinas();
    // Pre: 1 <= i <= N_USER AND 1 <= j <= N.TIMES_USER
    // Post: "cab" tomar� el valor de la cabina ocupada por el usuario "i"
    // en su "j"-�sima ejecuci�n. Y la cabina queda ocupada
    void entraUsuario(int& cab);

    // Pre: 1<= i <= N_USER AND 1 <= j <= N_TIMES_USER AND 0 <= cab < N_CAB
    // Post: la cabina "cab" ha sido liberada
    void saleUsuario(int cab);

    void entraLimpieza();

    void saleLimpieza();
};
*/