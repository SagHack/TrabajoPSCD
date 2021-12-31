#include <mutex>
#include <condition_variable>

using namespace std;
const int NUM_MAXIMOS= 10000;

class Control{  
    public:
        Control();   //constructor
        void entraTags();
        void saleTags();
        void entraQos();
        void saleQos();

    private:
        bool puedoEntrarTags;
        bool puedoEntrarQos;
        mutex mtxMonitor; //FUNDAMENTAL: mutex usarán las funcs
        condition_variable esperandoTags;     
        condition_variable esperandoQos;  
};
