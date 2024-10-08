// Sketch used to deliver A0 readings from a wire connected to this pin
// https://github.com/invpe/Attracthor/tree/main/Findings#electromagnetic-noise
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include <vector>
#include "SPIFFS.h"

#define WIFI_SSID "xxxxx"
#define WIFI_PASS "zzzzz"
#define ANALOG_PIN 36

WebServer WWWServer(80);


void setup() {
  Serial.begin(115200);
  pinMode(ANALOG_PIN, INPUT);

  Serial.println("Mounting FS...");
  while (!SPIFFS.begin()) {
    SPIFFS.format();
    Serial.println("Failed to mount file system");
    delay(1000);
  }

  WiFi.hostname("ANTENOWIEC");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait for WIFI
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(1000);
  }

  // Allow OTA for easier uploads
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else  // U_SPIFFS
        type = "filesystem";
    })
    .onEnd([]() {

    })
    .onProgress([](unsigned int progress, unsigned int total) {
      yield();
    })
    .onError([](ota_error_t error) {
      ESP.restart();
    });
  ArduinoOTA.setHostname("ANTENOWIEC");
  ArduinoOTA.begin();

  // Return if busy or available
  WWWServer.on("/get", [&]() {
    String strSample = String(analogRead(ANALOG_PIN));
    WWWServer.send(200, "application/json", "{\"value\": " + strSample + "}");
  });


  WWWServer.begin();
  Serial.println("Starting analog capture...");
}

void loop() {

  // Handle OTA
  ArduinoOTA.handle();
  WWWServer.handleClient();
}
