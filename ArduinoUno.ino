#include <SoftwareSerial.h>

#define SOIL_SENSOR_PIN A0 // پایه حسگر رطوبت خاک
#define RELAY_PIN 3        // پایه رله

SoftwareSerial espSerial(10, 11); // RX, TX (پایه‌های ارتباط با ESP8266)

void setup() {
  Serial.begin(115200);      // برای دیباگ
  espSerial.begin(115200);   // برای ارتباط با ESP8266
  pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH); // پمپ خاموش در ابتدا
}

void loop() {
  // خواندن مقدار رطوبت خاک
  int soilMoisture = analogRead(SOIL_SENSOR_PIN);
  espSerial.println("moisture:" + String(soilMoisture));
  delay(500);

  // دریافت وضعیت پمپ از ESP8266
  if (espSerial.available() > 0) {
    String data = espSerial.readStringUntil('\n');
    if (data.startsWith("pump:")) {
      String pumpStatus = data.substring(5);
      Serial.println("pumpStatus =>" + pumpStatus);
      if (digitalRead(3)==HIGH){
        Serial.println("RELAY_PIN => HIGH");
      }else{
        Serial.println("RELAY_PIN => LOW");
      }
      
      if (pumpStatus == "on") {
        digitalWrite(3, HIGH); // روشن کردن پمپ
        delay(500);
      } else if (pumpStatus == "off") {
        digitalWrite(3, LOW); // خاموش کردن پمپ
                delay(500);
      }
    }
  }
}
