#define SOIL_SENSOR_PIN A0 // پایه حسگر رطوبت خاک
#define RELAY_PIN 3        // پایه رله
#define AUTOMATIC 4

void setup() {
  Serial.begin(115200);      // برای دیباگ
  pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(AUTOMATIC, INPUT);
  digitalWrite(RELAY_PIN, LOW); // پمپ خاموش در ابتدا
}

void loop() {
  // خواندن مقدار رطوبت خاک
  int soilMoisture = analogRead(SOIL_SENSOR_PIN);
  Serial.println("Moisture:" + String(soilMoisture));
  delay(500);

  if (digitalRead(AUTOMATIC) == HIGH && soilMoisture>400){
    digitalWrite(RELAY_PIN, HIGH);
    delay(3000);
    digitalWrite(RELAY_PIN, LOW);
    delay(9500);
  }
  // دریافت وضعیت پمپ از ESP8266
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    Serial.println(data);
    data.trim();
    if (data.startsWith("pump:")) {
      
      String pumpStatus = data.substring(5);
      
      if (pumpStatus.startsWith("on")) {
        digitalWrite(RELAY_PIN, HIGH); // روشن کردن پمپ
      } else if (pumpStatus.startsWith("off")) {
        digitalWrite(RELAY_PIN, LOW); // خاموش کردن پمپ
      }
    }
  }
}
