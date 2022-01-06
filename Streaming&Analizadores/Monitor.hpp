#include <mutex>
#include <condition_variable>

using namespace std;
const int NUM_MAXIMOS= 10000;

class Control{  
    public:
        Control();   //constructor
        void entra();
        void sale();
    private:
        bool puedoEntrar;
        mutex mtxMonitor; //FUNDAMENTAL: mutex usarán las funcs
        condition_variable esperando;      
};
