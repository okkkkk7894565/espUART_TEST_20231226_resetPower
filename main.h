///Do not edit
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
// wifi config
#include <WiFiManager.h>

// Insert your network credentials
#define WIFI_SSID "iphoneminh"
#define WIFI_PASSWORD "12345678"
// Insert Firebase project API Key
#define API_KEY "AIzaSyDO7QsBc5frdD6xl96FbLiLecF1lrucbY4"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://demometter-default-rtdb.asia-southeast1.firebasedatabase.app/"

//define pin using
#define RsBtt D0
#define RsBttForWiFi D1
#define ledRSPre D3
#define ledRS D2
#define LED_PIN1 D8



// declare variable
float volt, ampe, PF, wat, Frequency, Energy;
int firstStatus, ledss;
unsigned long sendDataPrevMillis = 0, getButtonData = 0;
float startTime, dur, starTimeReadBtt;
bool signupOK = false;
int buttonFlag;
String id = "minhndhe163891";
String Path;

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;

//define for WiFiManager object
WiFiManager wifiManager;
