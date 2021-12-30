#include <mutex>
#include <condition_variable>
#include <Semaphore_V4.hpp>
#include <BoundedQueue.hpp>
#include <string>
#include <cstdlib>

const int TAM = 50;

/*Hemos pensado en hacer 3 monitores pero que tengan los recursos compartidos. Es decir, 3 cpp y un hpp*/
/*Queremos que los 3 monitores compartan este hpp para que todos tengan acceso a las 3 colas*/
class ControlColas{
    private:
        mutex mtx;
        BoundedQueue<string> cola = BoundedQueue<string>(TAM); //Como las ponemos en el constructor?--------------------------------------
        //BoundedQueue(TAM);
        //condition_variable esperandoTareas, esperandoQos,esperandoTags;
        condition_variable esperandoPublish, esperandoRead;
    public:
        ControlColas();
        void getQueues(string& cadena);
        void publish(string& cadena);
        void read(string& cadena);
};
