#ifndef FBDO_H
#define FBDO_H
#include <FreeRTOS.h>
#include <Firebase_ESP_Client.h>
static SemaphoreHandle_t fbdoMutex;
static FirebaseData fbdo;
#endif