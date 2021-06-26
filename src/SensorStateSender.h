#ifndef SENSORSTATESENDER_H
#define SENSORSTATESENDER_H
#include "FbIot_Utils.h"
#ifndef PATH_DEVICE
#define PATH_DEVICE "/iot_devices/" DEVICE_ID
#endif
#define PATH_SENSORS PATH_DEVICE "/sensors"
#define PATH_STATES "/states"
template <typename T>
class SensorStateSender{
private:
    String path;
    TaskHandle_t sendStatesTask;
    T state;
    static void postStateTask(void *pvParameters){
        SensorStateSender<T>* stateToPost = reinterpret_cast<SensorStateSender<T>*>(pvParameters);
        if (xSemaphoreTake(fbdoMutex, portMAX_DELAY)){ log_d("fbdoMutex taken from %s", __FUNCTION__);

            if(Firebase.RTDB.set(&fbdo, (stateToPost->path+PATH_STATES+"/"+String(UTC.now())).c_str(), stateToPost->state)){
                log_d("State in path %s updated", fbdo.dataPath().c_str());
            }else{
                log_e("Could not update state in path %s.\n\tREASON: %s", fbdo.dataPath().c_str(), fbdo.errorReason().c_str());
            }

            log_d("Freeing fbdoMutex from %s", __FUNCTION__);
            xSemaphoreGive(fbdoMutex);
        }else{log_e("Not possible to obtain fbdoMutex");}
      vTaskDelete(NULL);
    };
    void initPostTask(){
        if(!path.isEmpty()){
            xTaskCreatePinnedToCore(postStateTask, "Send state to Firebase", 10000, (void*)this, 1, NULL, 0);
        }
    }
public:
    SensorStateSender(){
        this->path.clear();
        this->state = T();
    }
    void initFirebase(const char* path){ //Initialized this way in order to update the last state received
        this->path = (String(PATH_SENSORS)+path);
        initPostTask();
    };
    void deinit(){
        this->path.clear();
    };
    ~SensorStateSender(){};
    void postState(const T stateToPost){
        if (state != stateToPost){

            state = stateToPost;
            initPostTask();
        }
    };
};

#endif