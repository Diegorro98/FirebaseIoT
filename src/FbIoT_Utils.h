#ifndef FBDO_H
#define FBDO_H
#include <FreeRTOS.h>
#include <Firebase_ESP_Client.h>
#include <ezTime.h>
static SemaphoreHandle_t fbdoMutex;
static FirebaseData fbdo;
Timezone timezone;
#endif