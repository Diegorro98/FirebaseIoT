#ifndef ACTUATOR_H
#define ACTUATOR_H
#include <Arduino.h>
#include "FbIot_Utils.h"
#ifndef PATH_DEVICE
#define PATH_DEVICE "/iot_devices/" DEVICE_ID
#endif
#define PATH_ACTUATORS PATH_DEVICE "/actuators"
template <typename T>
class Actuator{
private:
    String absolutePath;
public:
    String path;
    T action;
    Actuator(String pPath): absolutePath{PATH_ACTUATORS+pPath}, path{pPath}{
        action = T();
        if(nullSetter()){
                log_d(" path \"%s\" setted to default value", path.c_str());
            }else{
                log_e("Could not update state in path %s.\n\tREASON: %s", path.c_str(), fbdo.errorReason().c_str());
            }

    };
    bool nullSetter(){
        if (xSemaphoreTake(fbdoMutex, portMAX_DELAY)){log_d("Taking fbdoMutex from %s", __FUNCTION__);

            bool ret=Firebase.RTDB.set(&fbdo, absolutePath.c_str(), T());

            log_d("Freeing fbdoMutex from %s", __FUNCTION__);
            xSemaphoreGive(fbdoMutex);
            return ret;
        }else{log_e("Not possible to obtain fbdoMutex");}
        return false;
    }
    Actuator() = default;
    ~Actuator(){};
};

#endif