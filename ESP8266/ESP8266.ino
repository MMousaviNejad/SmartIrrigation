#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// اطلاعات WiFi
const char* ssid = "Targol";
const char* password = "Targol@110";

// آدرس سرور (HTTPS)
const String serverUrl = "https://devhelper.ir";

// متغیرها
int soilMoisture = 0;    // مقدار رطوبت دریافتی از Arduino
String pumpStatus = "off"; // وضعیت فعلی پمپ

WiFiClientSecure wifiClient;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // اتصال به WiFi
  connectToWiFi();

  // تنظیم اعتبار SSL
  wifiClient.setInsecure();
}
String incomingData = "";
void loop() {
while (Serial.available()) {
    char receivedChar = Serial.read();  // خواندن یک کاراکتر
    incomingData += receivedChar;       // افزودن کاراکتر به بافر

    // بررسی وجود نشانگر پایانی
    if (receivedChar == '\n') {
      processIncomingData(incomingData); // پردازش داده کامل
      incomingData = "";                 // خالی کردن بافر برای داده‌های بعدی
    }
  }

    delay(500);
    sendSoilMoisture();
    updatePumpStatus();
}

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
  data.trim();
  if (data.startsWith("Moisture:")){
    soilMoisture = data.substring(9).toInt();
    Serial.println("Soil moisture: " + String(soilMoisture));
  }
}


// ارسال مقدار رطوبت خاک به سرور
void sendSoilMoisture() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient https;
    https.begin(wifiClient, serverUrl + "/Soil/soil-data");
    https.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"moisture\": " + String(soilMoisture) + "}";
    int httpResponseCode = https.POST(jsonPayload);

    if (httpResponseCode == 201) {
      String response = https.getString();
      Serial.println("Moisture data " + String(soilMoisture) + " sent successfully: " + response);
    } else {
      Serial.println("Failed to send moisture data. Error code: " + String(httpResponseCode));
    }

    https.end();
  } else {
    connectToWiFi(); // در صورت قطع WiFi، اتصال دوباره
  }
}

// دریافت وضعیت پمپ از سرور
void updatePumpStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient https;
    https.begin(wifiClient, serverUrl + "/Soil/soil-status");

    int httpResponseCode = https.GET();
    if (httpResponseCode == 200) {
      String response = https.getString();
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, response);

      if (!error) {
        pumpStatus = doc["pump_status"].as<String>();
        Serial.println("Pump status: " + pumpStatus);
        Serial.println("pump:" + pumpStatus);
      } else {
        Serial.println("JSON Parsing failed!");
      }
    } else {
      Serial.println("Failed to get pump status. Error code: " + String(httpResponseCode));
    }

    https.end();
  } else {
    connectToWiFi(); // در صورت قطع WiFi، اتصال دوباره
  }
}
