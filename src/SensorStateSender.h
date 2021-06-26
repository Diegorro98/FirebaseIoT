#ifndef SENSORSTATESENDER_H
#define SENSORSTATESENDER_H
#include "FbIot_Utils.h"
#ifndef PATH_DEVICE
#define PATH_DEVICE "/iot_devices/" DEVICE_ID
#endif
#define PATH_STATES PATH_DEVICE "/sensors"
template <typename T>
class SensorStateSender{
private:
    const char* path;
    TaskHandle_t sendStatesTask;
    T state;
    SemaphoreHandle_t postStateMutex;
    static void postStateTask(void *pvParameters){
        if (xSemaphoreTake(fbdoMutex, portMAX_DELAY)){ log_d("fbdoMutex taken from %s", __FUNCTION__);
        
        if(Firebase.RTDB.set(&fbdo, stateToPost.path, stateToPost.state)){
          log_v("State in path %s updated", stateToPost.path);
        }else{
          log_e("Could not update state in path %s.\n\tREASON: %s", stateToPost.path, fbdo.errorReason().c_str());
        }

            log_d("Freeing fbdoMutex from %s", __FUNCTION__);
        xSemaphoreGive(fbdoMutex);
      }else{log_e("Not possible to obtain fbdoMutex");}
      vTaskDelete(NULL);
    };
public:
    SensorStateSender(){
        this->path = "";
        state = -1;
    }
    SensorStateSender(const char* path){
        this->path = path;
        state = -1;
    };
    void deinit(){
        this->path = "";
    };
    ~SensorStateSender(){};
    void postState(const T stateToPost){
        if (state != stateToPost){

            state = stateToPost;
            if(strcmp(path,"") != 0){
                xTaskCreatePinnedToCore(postStateTask, "Send state to Firebase", 10000, (void*) this, 1, &sendStatesTask, 0);
            }
        }
    };
};

#endif