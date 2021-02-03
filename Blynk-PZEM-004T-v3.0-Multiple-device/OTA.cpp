/*
 * OTA.cpp
 *
 *  Created on: Aug 9, 2020
 *      Author: E_CAD
 */

#include <Ticker.h>  				//Ticker Library
extern	Ticker fetchCycle;

#include "OTA.h"
#include "my_secret.h"
#include "Definitions.h"

void setupOTA(const char* nameprefix) {
  const int maxlen = 40;
  char fullhostname[maxlen];
  uint8_t mac[6];
  WiFi.macAddress(mac);
  snprintf(fullhostname, maxlen, "%s-%02x%02x%02x", nameprefix, mac[3], mac[4], mac[5]);
  //ArduinoOTA.setHostname(fullhostname);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
	Serial.println("");
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 3232
  // Port 8266 for OTA download from Sloeber
  ArduinoOTA.setPort(8266);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");


  ArduinoOTA.onStart([]() {


	fetchCycle.detach();
    Serial.println("Cycle interrupt disabled for OTA");


    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA: %u%%\r\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");

    delay(1000);
    Serial.println("Reboot by error");
    ESP.reset();

  });

  ArduinoOTA.begin();
  //TelnetStream.begin();

  debug_out(F("OTA Initialized: "), 				DEBUG_ALWAYS, 1);
  debug_out(F("IP address: "), 						DEBUG_ALWAYS, 0);
  debug_out(WiFi.localIP().toString(), 				DEBUG_ALWAYS, 1);
  debug_out(F(""), 									DEBUG_ALWAYS, 1);


#if defined(ESP32_RTOS) && defined(ESP32)
  xTaskCreate(
    ota_handle,          /* Task function. */
    "OTA_HANDLE",        /* String with name of task. */
    10000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    1,                /* Priority of the task. */
    NULL);            /* Task handle. */
#endif
}
