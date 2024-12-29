#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// اطلاعات WiFi
const char* ssid = "Targol";
const char* password = "Targol@110";

// آدرس سرور
const String serverUrl = "http://devhelper.ir";

// متغیرها
int soilMoisture = 0;    // مقدار رطوبت دریافتی از Arduino
String pumpStatus = "off"; // وضعیت فعلی پمپ

WiFiClient wifiClient;

void setup() {
  Serial.begin(115200); // برای ارتباط با Arduino
  delay(1000);

  // اتصال به WiFi
  connectToWiFi();
}

void loop() {
  // دریافت داده از Arduino
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    processIncomingData(data);
  }

  // ارسال داده رطوبت خاک به سرور و دریافت وضعیت پمپ
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= 5000) { // هر 5 ثانیه
    lastUpdate = millis();
    sendSoilMoisture();
    updatePumpStatus();
  }
}

// اتصال به WiFi
void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
}

// پردازش داده دریافتی از Arduino
void processIncomingData(String data) {
  if (data.startsWith("moisture:")) {
    soilMoisture = data.substring(9).toInt();
    Serial.println("Soil moisture: " + String(soilMoisture));
  }
}

// ارسال مقدار رطوبت خاک به سرور
void sendSoilMoisture() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(wifiClient, serverUrl + "/Soil/soil-data");
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"moisture\": " + String(soilMoisture) + "}";
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode == 201) {
      String response = http.getString();
      Serial.println("Moisture data sent successfully: " + response);
    } else {
      Serial.println("Failed to send moisture data. Error code: " + String(httpResponseCode));
    }

    http.end();
  } else {
    connectToWiFi(); // در صورت قطع WiFi، اتصال دوباره
  }
}

// دریافت وضعیت پمپ از سرور و ارسال به Arduino
void updatePumpStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(wifiClient, serverUrl + "/Soil/soil-status");

    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
      String response = http.getString();
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, response);

      if (!error) {
        pumpStatus = doc["pump_status"].as<String>();
        Serial.println("Pump status: " + pumpStatus);
        Serial.println("pump:" + pumpStatus); // ارسال به Arduino
        delay(500);
      } else {
        Serial.println("JSON Parsing failed!");
      }
    } else {
      Serial.println("Failed to get pump status. Error code: " + String(httpResponseCode));
    }

    http.end();
  } else {
    connectToWiFi(); // در صورت قطع WiFi، اتصال دوباره
  }
}
