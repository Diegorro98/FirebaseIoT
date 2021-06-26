#ifndef FIREBASEIOT_H
#define FIREBASEIOT_H
#include <Firebase_ESP_Client.h>
#include "Actuator.h"
#include "SensorStateSender.h"
static FirebaseData fbdoStream;
FirebaseAuth fbAuth;
FirebaseConfig fbConfig;

/** Define this method in order to init the actuators*/
void initActuators();
/** Define this method in order to init the Sensor senders*/
void initSensorStateSenders();
/** Define this method in order to define how are the actuators are going to act*/
void act(FirebaseStream);
void callbackActuators(FirebaseStream data){
  log_i("New value (%d) aviable!", data.intData());
  log_d("STREAM PATH: %s", data.streamPath().c_str());
  log_d("DATA PATH: %s", data.dataPath().c_str());
  log_d("DATA TYPE: %s", data.dataType().c_str());
  log_d("EVENT TYPE: %s",data.eventType().c_str());
  if(data.intData() != '\0'){
    act(data);
    if (xSemaphoreTake(fbdoMutex, portMAX_DELAY)){ log_d("fbdoMutex taken from %s", __FUNCTION__);

      log_v("Zeroing %s...", (data.streamPath()+data.dataPath()).c_str());
      if(!Firebase.RTDB.setInt(&fbdo, (data.streamPath()+data.dataPath()).c_str(), 0))
        log_e("REASON: %s", fbdo.errorReason().c_str());
      
      log_d("Freeing fbdoMutex from %s", __FUNCTION__);
      xSemaphoreGive(fbdoMutex);
    }else{log_e("Not possible to obtain fbdoMutex");}
    
  }else{
    log_d("0 obtained, discarded");
  }
}
void streamTimeoutCallback(bool timeout){
  if (timeout)
    log_w("Stream Timeout!");
}

void FirebaseSetup(){
  log_v("Inicializando Firebase...");
    /* Assign the project host and api key (required) */
  fbConfig.host = FIREBASE_HOST;
  fbConfig.api_key = API_KEY;

  /* Assign the user sign in credentials */
  fbAuth.user.email = USER_EMAIL;
  fbAuth.user.password = USER_PASSWORD;
  Firebase.begin(&fbConfig, &fbAuth);
  log_v("PATHS:\n\tPATH_DEVICE: %s\n\tPATH_ACTUATORS: %s\n\tPATH_STATES: %s", PATH_DEVICE, PATH_ACTUATORS, PATH_STATES);
  while (fbdoMutex == NULL){
    log_v("Creating fbdoMutex semaphore");
    fbdoMutex = xSemaphoreCreateMutex();
    delay(1);
  }
  initActuators();
  initSensorStateSenders();
  
  Firebase.reconnectWiFi(true);
  Firebase.RTDB.setwriteSizeLimit(&fbdo, "tiny");
  log_v("Preparing Firebase streaming for actuators");
  if (!Firebase.RTDB.beginStream(&fbdoStream, PATH_ACTUATORS)){
    log_e("Could not begin the Firebase streaming ...\n\t(ruta: %s)\n\tREASON: %s", PATH_ACTUATORS, fbdoStream.errorReason());
  } else{
    log_d("Firebase Streaming ready! (Path: %s)", PATH_ACTUATORS);
  }
  Firebase.RTDB.setStreamCallback(&fbdoStream, callbackActuators, streamTimeoutCallback);
}
#endif