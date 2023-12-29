// COM10 in lap COM5 in PC
/////update reset 26/12/2023 >> in clone git >> can update
#include <Arduino.h>
#include "main.h"
#include "get_power.h"
#include "reset_button.h"

// //th1
// int RsBttForWiFi = D0;
// int RsBttForEner = D2;
// // >>>>>> RsBttForWiFi==0 ; RsBttForEner==1 >> rs ener ;


// //th2
// int RsBttForWiFi = D2;
// int RsBttForEner = D0;
// // >>>>>> RsBttForWiFi==1 >>> rs wifi ; RsBttForEner==0;


// //th3
// int RsBttForWiFi = D2;
// int RsBttForEner = D3;
// // >>>>>> RsBttForWiFi==1 >>> rs wifi ; RsBttForEner==1;

// //th3
// int RsBttForWiFi = D3;
// int RsBttForEner = D2;
// // >>>>>> RsBttForWiFi==1 >>> rs wifi ; RsBttForEner==1;

// ko gắn trở khi reset bị đẩy lên mức cao >>> vấn đề vi xử lý ????
// khai báo chân D0 và D2 >> chân Đằng sau bị đẩy lên cao(D2); D2 và D3 >>> D3 bị đẩy lên cao


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(RsBttForWiFi, INPUT);
  pinMode(RsBttForEner, INPUT);

  pinMode(ledRS, OUTPUT);
  pinMode(ledRSPre, OUTPUT);

  delay(300);


  Serial.println("ESP:Welcome!!!");
  Serial.println("ESP:Receiver data");
  Serial.println();

  //-------------- Reset Power----------------------
  Serial.print("RsBttForEner:");
  Serial.println(digitalRead(RsBttForEner));
  delay(300);

  // get btt reset status >> meet requirment : return 1 ; not meet requirment : return 0
  int flagForRsPower = readRsBtt(5000, 1);
  delay(300);

  Serial.print("flagForRsPower:");
  Serial.println(flagForRsPower);
  delay(100);

  if (flagForRsPower == 1) {
    digitalWrite(ledRSPre, 0);
    resetEneryByBtt();
  } else {
    Serial.println("ESP: Break Reset Power !!!!!!");
  }
  Serial.println();

  //-------------- Reset Wifi----------------------
  Serial.print("RsBttForWiFi:");
  Serial.println(digitalRead(RsBttForWiFi));
  delay(300);

  int flagForRsWifi = readRsBttForWifi(4000, 1);

  Serial.print("flagForRsWifi:");
  Serial.println(flagForRsWifi);

  if (flagForRsWifi == 1) {
    digitalWrite(ledRSPre, 0);
    resetWifiByBtt();
  } else {
    Serial.println("ESP: Break Reset Wifi !!!!!!");
  }
  Serial.println();
  //---------------------------------------------------

  //show wifi infor
  wifiManager.autoConnect("AutoConnectAP", "12345678");
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // Serial.println(WIFI_SSID);
  // while (WiFi.status() != WL_CONNECTED) {
  //   Serial.print("Connecting to Wi-Fi: ");
  //   for (int j = 0; j <= 15; j++) {
  //     Serial.print(".");
  //     delay(300);
  //   }
  //   Serial.println("");
  // }
  // Serial.println();
  // Serial.print("Connected with IP: ");
  // Serial.println(WiFi.localIP());



  // /* Assign the api key (required) */
  config.api_key = API_KEY;
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Conected to fibase Successfull");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - getButtonData > 1000 || getButtonData == 0)) {
    getButtonData = millis();

    if (Firebase.RTDB.getInt(&fbdo, "/METTER/Change")) {
      if (fbdo.dataType() == "int") {
        ledss = fbdo.intData();
        //chang led status
        digitalWrite(LED_PIN1, ledss);
      } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }

    // if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 3000 || sendDataPrevMillis == 0)) {
    if (millis() - sendDataPrevMillis > 3000) {
      sendDataPrevMillis = millis();
      //lấy giá trị sau khi đọc xong

      volt = getVol();
      ampe = getAmp();
      PF = getPF();
      wat = getWat();
      Frequency = getFre();
      Energy = getEnergy();

      showData(volt, ampe, PF, wat, Frequency, Energy);

      json.set("Vol", volt / 1.0);
      json.set("ampe", ampe);
      json.set("PF", PF);
      json.set("wat", wat);
      json.set("Frequency", Frequency);
      json.set("Energy", Energy);
      Path = "METTER/" + id + "/Data";

      if (Firebase.RTDB.setJSON(&fbdo, Path, &json)) {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }
  }
}


//----------------------------------- Reset Function -------------------------------------

void resetEneryByBtt() {
  int count = 0;
  while (count <= 5) {
    digitalWrite(ledRS, 1);
    delay(200);
    digitalWrite(ledRS, LOW);
    delay(200);
    count++;
  }
  if (pzem.resetEnergy()) {
    count = 0;
    while (count <= 5) {
      digitalWrite(ledRS, 1);
      delay(200);
      digitalWrite(ledRS, LOW);
      delay(200);
      count++;
    }
     Serial.println("PZEM:Reset Power Succesfull !!!");
  } else {
    count = 0;
    while (count <= 2) {
      digitalWrite(ledRS, 1);
      delay(1000);
      digitalWrite(ledRS, LOW);
      delay(800);
      count++;
    }
    Serial.println("PZEM:Can not reset power");
    delay(300);
  }
}

void resetWifiByBtt() {
  int count = 0;
  while (count <= 5) {
    digitalWrite(ledRS, 1);
    delay(200);
    digitalWrite(ledRS, LOW);
    delay(200);
    count++;
  }
  Serial.println("ESP:Starting reset WiFi");
  //reset settings - for testing
  wifiManager.resetSettings();
  delay(200);
  wifiManager.autoConnect("AutoConnectAP", "12345678");

  count = 0;
  while (count <= 5) {
    digitalWrite(ledRS, 1);
    delay(200);
    digitalWrite(ledRS, LOW);
    delay(200);
    count++;
  }
  //exit after config instead of connecting
  wifiManager.setBreakAfterConfig(true);
}
