// COM10 in lap COM5 in PC
/////update reset 26/12/2023
#include "main.h"
#include "get_power.h"
#include "reset_button.h"
// change in laptop

//change in pc

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(LED_PIN1, OUTPUT);  // Initialize the LED_BUILTIN pin as an output
  pinMode(RsBtt, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledRS, OUTPUT);
  pinMode(ledRSPre, OUTPUT);

  delay(100);
  Serial.println("ESP:Welcome!!!");
  Serial.println("ESP:Receiver data");
  Serial.println();
  Serial.print("First time Btt Reset:");
  delay(100);
  Serial.println(digitalRead(RsBtt));
  Serial.println();
  delay(100);

  // get btt reset status >> meet requirment : return 1 ; not meet requirment : return 0
  int buttonFlag = readRsBtt(5000,0);
  
  Serial.print("ButtonFlag:");
  Serial.println(buttonFlag);
  
  if (buttonFlag == 1) {
    digitalWrite(ledRSPre, 0);
    nhayLedReset();
  } else {
    Serial.println("ESP: Break !!!!!!");
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to Wi-Fi: ");
    for (int j = 0; j <= 15; j++) {
      Serial.print(".");
      delay(300);
    }
    Serial.println("");
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

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
