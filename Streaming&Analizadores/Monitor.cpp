#include <mutex>
#include <condition_variable>
#include "Monitor.hpp"


Control::Control() { //constructor
    puedoEntrar = true;
};

void Control::entra(){
    unique_lock<mutex> lck(mtxMonitor);
    while(!puedoEntrar){
        esperando.wait(lck);
    }
    puedoEntrar = false;
};

void Control::sale(){  
    unique_lock<mutex> lck(mtxMonitor);
    puedoEntrar = true;
    esperando.notify_one();  //avisa a uno de que se va
};
