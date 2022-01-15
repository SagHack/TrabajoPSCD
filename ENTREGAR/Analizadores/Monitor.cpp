#include <mutex>
#include <condition_variable>
#include "Monitor.hpp"


Control::Control() { //constructor
    puedoEntrarTags = true;
    puedoEntrarQos = true;
};


void Control::entraTags(){
    unique_lock<mutex> lck(mtxMonitor);
    while(!puedoEntrarTags){
        esperandoTags.wait(lck);
    }
    puedoEntrarTags = false;
};


void Control::saleTags(){  
    unique_lock<mutex> lck(mtxMonitor);
    puedoEntrarTags = true;
    esperandoTags.notify_one();  //avisa a uno de que se va
};

void Control::entraQos(){
    unique_lock<mutex> lck(mtxMonitor);
    while(!puedoEntrarQos){
        esperandoQos.wait(lck);
    }
    puedoEntrarQos = false;
};


void Control::saleQos(){  
    unique_lock<mutex> lck(mtxMonitor);
    puedoEntrarQos = true;
    esperandoQos.notify_one();  //avisa a uno de que se va
};
