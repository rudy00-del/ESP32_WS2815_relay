// Created by Rūdolfs Rutkovskis December 2025

#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <Adafruit_NeoPixel.h>

// ===== USER CONFIGURATION =====
#define LED_PIN     5
#define LED_COUNT   300
#define BRIGHTNESS  255
// ==============================

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// =============================================
// MODE VARIABLES
// =============================================
int currentMode = 1;            // start with mode 1
unsigned long lastModeTime = 0;
const unsigned long modeTime = 120000;

// =============================================
// ===== MODE 1: FIRST SKETCH (BOUNCING COMETS)
// =============================================
#define COMET_COUNT_1 4

struct Comet1 {
  int pos;
  int dir;
  uint32_t color;
  int bounceCount;
  int speedOffset;
  unsigned long lastMoveTime;
};

Comet1 comets1[COMET_COUNT_1];
unsigned long lastFadeUpdate1 = 0;
const int fadeInterval1 = 10;

void mode1_setup() {
  for (int i = 0; i < COMET_COUNT_1; i++) {
    comets1[i].pos = random(0, LED_COUNT);
    comets1[i].dir = random(0, 2) == 0 ? 1 : -1;
    comets1[i].color = strip.ColorHSV(random(0, 65535), 255, 255);
    comets1[i].bounceCount = 0;
    comets1[i].speedOffset = random(0, 7);
    comets1[i].lastMoveTime = millis();
  }
}

void mode1() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastFadeUpdate1 >= fadeInterval1) {
    lastFadeUpdate1 = currentMillis;

    for (int i = 0; i < LED_COUNT; i++) {
      uint32_t c = strip.getPixelColor(i);
      uint8_t r = (c >> 16) & 0xFF;
      uint8_t g = (c >> 8) & 0xFF;
      uint8_t b = c & 0xFF;

      r = (uint8_t)((r * 230) >> 8);
      g = (uint8_t)((g * 230) >> 8);
      b = (uint8_t)((b * 230) >> 8);

      strip.setPixelColor(i, r, g, b);
    }
  }

  for (int i = 0; i < COMET_COUNT_1; i++) {
    int cometDelay = fadeInterval1 + comets1[i].speedOffset * 5;

    if (currentMillis - comets1[i].lastMoveTime >= cometDelay) {
      comets1[i].lastMoveTime = currentMillis;
      comets1[i].pos += comets1[i].dir;

      if (comets1[i].pos <= 0 || comets1[i].pos >= LED_COUNT - 1) {
        comets1[i].dir = -comets1[i].dir;
        comets1[i].bounceCount++;
        comets1[i].speedOffset = random(0, 7);

        if (comets1[i].bounceCount >= random(2, 5)) {
          comets1[i].bounceCount = 0;
          comets1[i].color = strip.ColorHSV(random(0, 65535), 255, 255);
        }
      }

      if (comets1[i].pos >= 0 && comets1[i].pos < LED_COUNT) {
        strip.setPixelColor(comets1[i].pos, comets1[i].color);
      }
    }
  }

  strip.show();
}

// =============================================
// ===== MODE 2: SECOND SKETCH (ONE-WAY COMETS)
// =============================================
#define COMET_COUNT_2 6

struct Comet2 {
  float pos;
  float speed;
  uint32_t color;
  unsigned long lastMoveTime;
  int delayOffset;
};

Comet2 comets2[COMET_COUNT_2];
unsigned long lastFadeUpdate2 = 0;
const int fadeInterval2 = 10;

void mode2_setup() {
  for (int i = 0; i < COMET_COUNT_2; i++) {
    comets2[i].pos = random(0, LED_COUNT);
    comets2[i].speed = 0.5 + (random(5, 20) / 10.0);
    comets2[i].color = strip.ColorHSV(random(0, 65535), 255, 255);
    comets2[i].lastMoveTime = millis() + random(0, 500);
    comets2[i].delayOffset = random(0, 10);
  }
}

void mode2() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastFadeUpdate2 >= fadeInterval2) {
    lastFadeUpdate2 = currentMillis;

    for (int i = 0; i < LED_COUNT; i++) {
      uint32_t c = strip.getPixelColor(i);
      uint8_t r = (c >> 16) & 0xFF;
      uint8_t g = (c >> 8) & 0xFF;
      uint8_t b = c & 0xFF;

      r = (uint8_t)((r * 230) >> 8);
      g = (uint8_t)((g * 230) >> 8);
      b = (uint8_t)((b * 230) >> 8);

      strip.setPixelColor(i, r, g, b);
    }
  }

  for (int i = 0; i < COMET_COUNT_2; i++) {
    int moveInterval = fadeInterval2 + comets2[i].delayOffset * 5;

    if (currentMillis - comets2[i].lastMoveTime >= moveInterval) {
      comets2[i].lastMoveTime = currentMillis;

      comets2[i].pos += comets2[i].speed;

      if (comets2[i].pos >= LED_COUNT) {
        comets2[i].pos = 0;
        comets2[i].speed = 0.5 + (random(5, 20) / 10.0);
        comets2[i].color = strip.ColorHSV(random(0, 65535), 255, 255);
      }

      int head = (int)comets2[i].pos;
      if (head >= 0 && head < LED_COUNT) {
        strip.setPixelColor(head, comets2[i].color);
      }
    }
  }

  strip.show();
}
//------------------------------------------------------------------------------------

// ===== Wi-Fi Credentials =====
const char* ssid = "Your WIFI";
const char* password = "Your WIFI";

// ===== HTTP Basic Auth =====
const char* www_username = "Your username";
const char* www_password = "Your password";

// ===== Relay Pin =====
const int relayPin = 14;

// ===== NTP Configuration =====
const char* ntpServer = "time.google.com";
const long  gmtOffset_sec = 3600 * 2;  // GMT+2 (Baltic States)
const int   daylightOffset_sec = 3600; // DST

// ===== Web Server =====
WebServer server(55647);

// ===== AUTO Mode Variables =====
bool autoMode = false;

int autoOnHour = 16;
int autoOnMinute = 0;

int autoOffHour = 22;
int autoOffMinute = 0;

// Relay state
bool relayOn = false;

// WiFi watchdog
unsigned long lastWiFiCheck = 0;
const unsigned long wifiInterval = 2000;

// ===== Function Prototypes =====
void handleRoot();
void handleSetTimes();
void reconnectWiFi();
void syncTime();

// ======================================================
//                       SETUP
// ======================================================
void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  // --- WiFi ---
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // --- NTP ---
  syncTime();

  // --- Web Server ---
  server.on("/", handleRoot);
  server.on("/settime", handleSetTimes);

  server.begin();
  Serial.println("Web server started.");

  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();
  randomSeed(analogRead(0));

  mode1_setup();
  mode2_setup();

}

// ======================================================
//                       LOOP
// ======================================================
void loop() {

  server.handleClient();

  // ===== WiFi watchdog =====
  unsigned long now = millis();
  if (now - lastWiFiCheck > wifiInterval) {
    lastWiFiCheck = now;
    reconnectWiFi();
  }

  // ===== AUTO MODE =====
  if (autoMode) {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      int h = timeinfo.tm_hour;
      int m = timeinfo.tm_min;

      // Turn ON
      if (h == autoOnHour && m == autoOnMinute && !relayOn) {
        relayOn = true;
        digitalWrite(relayPin, LOW);
        Serial.println("AUTO → Relay ON");
      }

      // Turn OFF
      if (h == autoOffHour && m == autoOffMinute && relayOn) {
        relayOn = false;
        digitalWrite(relayPin, HIGH);
        Serial.println("AUTO → Relay OFF");
      }
    }
  }

  // ===== MODE CHECK =====
  if (millis() - lastModeTime > modeTime) {
      lastModeTime = millis();
      currentMode = (currentMode == 1 ? 2 : 1);

      // Clear strip when switching modes
      strip.clear();
      strip.show();
  }

  // ===== RUN SELECTED MODE =====
  if (currentMode == 1) mode1();
  else mode2();
}

// ======================================================
//             WEB PAGE (Manual + Auto Control)
// ======================================================
void handleRoot() {

  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }

  // Handle manual ON/OFF
  if (server.hasArg("manual")) {
    String val = server.arg("manual");
    if (val == "on") {
      relayOn = true;
      autoMode = false;
      digitalWrite(relayPin, LOW);
    }
    if (val == "off") {
      relayOn = false;
      autoMode = false;
      digitalWrite(relayPin, HIGH);
    }
    if (val == "auto") {
      autoMode = true;
    }
  }

  struct tm timeinfo;
  getLocalTime(&timeinfo);

  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<title>ESP32 Relay Control</title></head><body>";

  html += "<h2>ESP32 Relay Control</h2>";

  html += "<p><strong>Current Time:</strong> ";
  html += String(timeinfo.tm_hour) + ":" +
          (timeinfo.tm_min < 10 ? "0" : "") + String(timeinfo.tm_min);
  html += "</p>";

  html += "<p><strong>Relay State:</strong> ";
  html += relayOn ? "ON" : "OFF";
  html += "</p>";

  html += "<p><strong>Mode:</strong> ";
  html += autoMode ? "AUTO" : "MANUAL";
  html += "</p><hr>";

  // ===== Manual Control =====
  html += "<h3>Manual Control</h3>";
  html += "<a href='/?manual=on'><button style='width:100px;height:40px;'>ON</button></a> ";
  html += "<a href='/?manual=off'><button style='width:100px;height:40px;'>OFF</button></a><br><br>";
  html += "<a href='/?manual=auto'><button style='width:100px;height:40px;'>AUTO MODE</button></a><br><hr>";

  // ===== AUTO TIME SETTINGS =====
  html += "<h3>Auto Mode Time Settings</h3>";
  html += "<form action='/settime' method='GET'>";

  // ON time
  html += "ON Time: <select name='onH'>";
  for (int h = 0; h < 24; h++) {
    html += "<option value='" + String(h) + "'";
    if (h == autoOnHour) html += " selected";
    html += ">" + String(h) + "</option>";
  }
  html += "</select> : ";

  html += "<select name='onM'>";
  for (int m = 0; m < 60; m++) {
    html += "<option value='" + String(m) + "'";
    if (m == autoOnMinute) html += " selected";
    html += ">" + String(m) + "</option>";
  }
  html += "</select><br><br>";

  // OFF time
  html += "OFF Time: <select name='offH'>";
  for (int h = 0; h < 24; h++) {
    html += "<option value='" + String(h) + "'";
    if (h == autoOffHour) html += " selected";
    html += ">" + String(h) + "</option>";
  }
  html += "</select> : ";

  html += "<select name='offM'>";
  for (int m = 0; m < 60; m++) {
    html += "<option value='" + String(m) + "'";
    if (m == autoOffMinute) html += " selected";
    html += ">" + String(m) + "</option>";
  }
  html += "</select><br><br>";

  html += "<button type='submit' style='width:150px;height:40px;'>Save Times</button>";
  html += "</form>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

// ======================================================
//          HANDLE AUTO TIME SETTINGS UPDATE
// ======================================================
void handleSetTimes() {

  if (!server.authenticate(www_username, www_password))
    return server.requestAuthentication();

  if (server.hasArg("onH")) autoOnHour = server.arg("onH").toInt();
  if (server.hasArg("onM")) autoOnMinute = server.arg("onM").toInt();
  if (server.hasArg("offH")) autoOffHour = server.arg("offH").toInt();
  if (server.hasArg("offM")) autoOffMinute = server.arg("offM").toInt();

  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Updated");
}

// ======================================================
//                WIFI RECONNECT (STABLE!)
// ======================================================
void reconnectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.println("WiFi lost → reconnecting...");

  WiFi.disconnect(true); // hard reset
  delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
    delay(200);
  }

  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi RECONNECTED!");
  else
    Serial.println("WiFi reconnect failed.");
}

// ======================================================
//                     TIME SYNC
// ======================================================
void syncTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;

  while (!getLocalTime(&timeinfo)) {
    Serial.println("Waiting for NTP time...");
    delay(1000);
  }

  Serial.println("Time synchronized.");
}
