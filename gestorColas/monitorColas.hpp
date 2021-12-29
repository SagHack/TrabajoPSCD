#include <mutex>
#include <condition_variable>
#include <Semaphore_V4.hpp>
#include "BoundedQueue.hpp"
#include <string>
#include <cstdlib>

const int MAX = 50;

/*Hemos pensado en hacer 3 monitores pero que tengan los recursos compartidos. Es decir, 3 cpp y un hpp*/
/*Queremos que los 3 monitores compartan este hpp para que todos tengan acceso a las 3 colas*/
class ControlColas{
    private:
        mutex mtx;
        BoundedQueue<string> cola(MAX); //Como las ponemos en el constructor?
        //condition_variable esperandoTareas, esperandoQos,esperandoTags;
        condition_variable esperandoPublish, esperandoRead;
    public:
        ControlColas();
        void ControlColas::getQueues(string& cadena);
        void ControlColas::publish(string& cadena);
        void ControlColas::read(string& cadena);
};





