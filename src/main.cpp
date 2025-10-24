// SmartCity - ESP8266 firmware
// Sensors: DHT11 (temp/humidity), MQ-2 (gas analog), MH-B (IR avoidance digital), HC-SR04 (ultrasonic)
// Serves a modern Bootstrap 5 based UI on port 80 and provides /api/sensors JSON

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
// OLED removed - U8g2 and Wire not used anymore

// --- Pin assignments (change as needed) ---
// Note: default I2C uses SDA=GPIO4 (D2) and SCL=GPIO5 (D1).
// Keep I2C on those pins; therefore move DHT and MH-B to other GPIOs.
const uint8_t PIN_DHT = 14;     // GPIO14 = D5
const uint8_t DHT_TYPE = DHT11; // DHT11
const uint8_t PIN_MQ2_A = A0;   // MQ-2 (analog) - A0 only on ESP8266
const uint8_t PIN_MHB = 2;      // GPIO2 = D4 - MH-B (digital input)
const uint8_t PIN_SR04_TRIG = 12; // GPIO12 = D6
const uint8_t PIN_SR04_ECHO = 13; // GPIO13 = D7

// WiFi AP credentials (softAP)
const char* AP_SSID = "SmartCity";
const char* AP_PASS = "SmartCity"; // min 8 chars

DHT dht(PIN_DHT, DHT_TYPE);
// webserver removed

// OLED support removed

// helper to read HC-SR04 distance in cm (with timeout)
long readUltrasonicCM(uint8_t trigPin, uint8_t echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // pulseIn returns microseconds - timeout 30000us = 30ms
  unsigned long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1; // no echo / out of range
  long cm = duration / 58; // approximate
  return cm;
}

// Read sensors and produce JSON payload
String getSensorJson() {
  float humidity = dht.readHumidity();
  float tempC = dht.readTemperature();
  if (isnan(humidity)) humidity = -999;
  if (isnan(tempC)) tempC = -999;

  int mqRaw = analogRead(PIN_MQ2_A); // 0..1023
  int mqPct = map(mqRaw, 0, 1023, 0, 100);

  int irState = digitalRead(PIN_MHB); // depending on sensor logic
  bool obstacle = (irState == LOW); // common for MH-B: LOW when obstacle

  long distance = readUltrasonicCM(PIN_SR04_TRIG, PIN_SR04_ECHO);

  String json = "{";
  json += "\"temperature_c\":" + String(tempC, 1) + ",";
  json += "\"humidity\":" + String(humidity, 1) + ",";
  json += "\"mq2_raw\":" + String(mqRaw) + ",";
  json += "\"mq2_pct\":" + String(mqPct) + ",";
  json += "\"mhb_obstacle\":" + String(obstacle ? "true" : "false") + ",";
  json += "\"hcsr04_cm\":" + String(distance);
  json += "}";
  return json;
}

// Read sensors into a struct for reuse
struct Sensors {
  float temperature_c;
  float humidity;
  int mq2_raw;
  int mq2_pct;
  bool mhb_obstacle;
  long hcsr04_cm;
};

Sensors readSensors() {
  Sensors s;
  float humidity = dht.readHumidity();
  float tempC = dht.readTemperature();
  s.humidity = isnan(humidity) ? -999 : humidity;
  s.temperature_c = isnan(tempC) ? -999 : tempC;
  s.mq2_raw = analogRead(PIN_MQ2_A);
  s.mq2_pct = map(s.mq2_raw, 0, 1023, 0, 100);
  int irState = digitalRead(PIN_MHB);
  s.mhb_obstacle = (irState == LOW);
  s.hcsr04_cm = readUltrasonicCM(PIN_SR04_TRIG, PIN_SR04_ECHO);
  return s;
}

// Webserver removed - no handlers

void setup() {
  Serial.begin(115200);
  // Disable SDK debug output to keep Serial clean (boot ROM messages at 74880 cannot be suppressed)
  Serial.setDebugOutput(false);
  delay(100);
  Serial.println("\nSmartCityFiw starting...");

  pinMode(PIN_MHB, INPUT);
  pinMode(PIN_SR04_TRIG, OUTPUT);
  pinMode(PIN_SR04_ECHO, INPUT);

  dht.begin();

  // LittleFS / webserver removed

  // Start WiFi AP
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("AP started. Connect to SSID: "); Serial.println(AP_SSID);
  Serial.print("AP IP: "); Serial.println(apIP);

  // No webserver configured

  // OLED removed — no display initialization
}

unsigned long lastPrint = 0;

void loop() {
  unsigned long now = millis();
  if (now - lastPrint >= 2000) {
    lastPrint = now;
    // Print sensor readings to Serial
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int mqRaw = analogRead(PIN_MQ2_A);
    int mqPct = map(mqRaw, 0, 1023, 0, 100);
    int irState = digitalRead(PIN_MHB);
    bool obstacle = (irState == LOW);
    long dist = readUltrasonicCM(PIN_SR04_TRIG, PIN_SR04_ECHO);

    Serial.print("Temp(C): "); Serial.print(isnan(t)?-999:t);
    Serial.print("  Hum(%): "); Serial.print(isnan(h)?-999:h);
    Serial.print("  MQ2: "); Serial.print(mqRaw); Serial.print(" ("); Serial.print(mqPct); Serial.print("%)");
    Serial.print("  MH-B obstacle: "); Serial.print(obstacle?"YES":"NO");
    Serial.print("  HC-SR04(cm): "); Serial.println(dist);
    // OLED removed — no display updates
  }
}
