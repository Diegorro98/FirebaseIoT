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
/** 
 * Define this method in order to define how are the actuators are going to act. 
 * @param data Gives all the information about the data and the data itself
 * @return This function should return the Actuator::nullSetter method to notify who did the action that has it been received. 
 * Handle later or not even handle it return 2. (If handled later, execute it in the not-main-loop-executer core) 
 * Return UINT8_MAX is for the case that no actuator was found.
 * @code{c}
 * uint8_t act(FirebaseStream data){
 *  if(data.dataPath().compareTo(actuator.path) == 0){
 *    actuator.action = data.intData(); //if actuator is an instance of Actuator<int>
 *    //do whatever
 *    return actuator.nullSetter(); //or 2 if you will handle it later.
 *  }
 *  //if there are no coincidences
 *  return UINT8_MAX;
 * }
 * @endcode
*/
uint8_t act(FirebaseStream data);
void callbackActuators(FirebaseStream data){
  //log_i("New value(%s) aviable!", data.rawData().c_str());
  if(data.dataType().compareTo("int") == 0){
    log_i("New value(%d) aviable!", data.intData());
  }else if (data.dataType().compareTo("string") == 0) {
    log_i("New value(%s) aviable!", data.stringData().c_str());
  }else if (data.dataType().compareTo("json") == 0){
    log_i("New value(%s) aviable!", data.jsonString().c_str());
  }else{
    log_i("New value aviable!");
  }
  log_d("STREAM PATH: %s", data.streamPath().c_str());
  log_d("DATA PATH: %s", data.dataPath().c_str());
  log_d("DATA TYPE: %s", data.dataType().c_str());
  log_d("EVENT TYPE: %s",data.eventType().c_str());
  uint8_t result = act(data);
  if(result == true){
    log_d("Reseted actuator %s", data.dataPath().c_str());
  }else if(result == false){
    log_e("Could not reset actuator %s, %s", data.dataPath().c_str(), fbdo.errorReason().c_str());
  }else if( result == UINT8_MAX){
    log_i("No actuator \"%s\" is declared", data.dataPath().c_str());
  }
}
void streamTimeoutCallback(bool timeout){
  if (timeout)
    log_w("Stream Timeout!");
}

void FirebaseSetup(){
  log_v("Setting up Firebase...");
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
  
  Firebase.reconnectWiFi(true);
  Firebase.RTDB.setwriteSizeLimit(&fbdo, "tiny");
  log_v("Preparing Firebase streaming for actuators");
  if (!Firebase.RTDB.beginStream(&fbdoStream, PATH_ACTUATORS)){
    log_e("Could not begin the Firebase streaming ...\n\t(ruta: %s)\n\tREASON: %s", PATH_ACTUATORS, fbdoStream.errorReason());
  } else{
    log_d("Firebase Streaming ready! (Path: %s)", PATH_ACTUATORS);
  }
  Firebase.RTDB.setStreamCallback(&fbdoStream, callbackActuators, streamTimeoutCallback);
  if (xSemaphoreTake(fbdoMutex, portMAX_DELAY)){
    log_v("Wating for the token to be ready...");
    for (token_info_t info = Firebase.authTokenInfo(); info.status != token_status_ready; info = Firebase.authTokenInfo()){
      log_v("AuthToken not ready yet");  
    }
    log_i("AuthToken ready!");
    xSemaphoreGive(fbdoMutex);
  }
  
  initActuators();
  initSensorStateSenders();
}
#endif