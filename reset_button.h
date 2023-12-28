int readRsBtt(float press_button_duration, int button_status_will_get) {
  if (digitalRead(RsBtt) == button_status_will_get) {
    delay(20);  //chong doi phim
    //kiểm tra trạng thái của btt ?= trạng thái muốn bắt ko
    if (digitalRead(RsBtt) == button_status_will_get) {
      // nếu trạng thái btt == trạng thái muốn bắt ko >>> đang bấm nút
      // đèn báo bấm nút sáng
      digitalWrite(ledRSPre, 1);
      //delay == press_button_duration (thời gian yêu cầu giữ nút)
      delay(press_button_duration);
      // giữ nút đủ thời gian (==press_button_duration) >>  return  1(thoả mãn điều kiện của nút bấm)
      if (digitalRead(RsBtt) == button_status_will_get) {
        Serial.print("Btt Reset:");
        Serial.println(digitalRead(RsBtt));
        return 1;
        Serial.println("Enough time on rsBTT!!!");
      } else {
        // bấm thời gian < (press_button_duration) >> return  0(ko thoả mãn điều kiện của nút bấm)
        digitalWrite(ledRSPre, 0);
        return 0;
      }
    }
  }
  return 0;
}

void demoReset() {
  digitalWrite(LED_BUILTIN, 1);
  delay(200);
  digitalWrite(ledRS, LOW);
  Serial.println("ESP:Break after reset power");
}