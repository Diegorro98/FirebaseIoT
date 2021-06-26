#ifndef ACTUATOR_H
#define ACTUATOR_H
#include <Arduino.h>
#include "FbIot_Utils.h"
#ifndef PATH_DEVICE
#define PATH_DEVICE "iot_devices/" DEVICE_ID
#endif
#define PATH_ACTUATORS PATH_DEVICE "/actuators"
template <typename T>
class Actuator{
private:
public:
    String path;
    T action;
    Actuator(String path){
        this->path = path;
        action = T();
        if (xSemaphoreTake(fbdoMutex, portMAX_DELAY)){log_d("Taking fbdoMutex from %s to update path %s", __FUNCTION__, path.c_str());

            if(Firebase.RTDB.set(&fbdo, (PATH_ACTUATORS+path).c_str(), action)){
                log_v(" path \"%s\" setted to default value", path.c_str());
            }else{
                log_e("Could not update state in path %s.\n\tREASON: %s", path.c_str(), fbdo.errorReason().c_str());
            }

            log_d("Freeing fbdoMutex from %s to update path %s", __FUNCTION__, path.c_str());
            xSemaphoreGive(fbdoMutex);
        }else{log_e("Not possible to obtain fbdoMutex");}
    };
    Actuator(){};
    ~Actuator(){};
};

#endif