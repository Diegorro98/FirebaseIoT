# Firebase IoT Library

This is a library to create easily devices that can be connected to the Cloud, or, in this case, to Firebase.

At this time, it works with Firebase's Real Time Database.

# Tested Devices
 * ESP32

## Library dependencies
This libraries were tested with the mentioned version, but fell free to test newer versions if exist and post me the result.
 * [Firebase Arduino Client Library for ESP8266 and ESP32 (v2.0.3)](https://github.com/mobizt/Firebase-ESP-Client)
 * [ezTime (v0.8.3)](https://github.com/ropg/ezTime)

## Should defines
This are `#define`s that you should define in your code to have everything working
 * **DEVICE_ID**: Device ID that will be on Firebase.
 * **FIREBASE_HOST**
 * **API_KEY**
 * **USER_EMAIL**
 * **USER_PASSWORD**
