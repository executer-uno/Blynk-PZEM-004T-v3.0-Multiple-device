// Thanks to Andreas Spiess
// https://www.youtube.com/watch?v=1pwqS_NUG7Q&t=8s


#ifdef ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#endif


#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//#include <TelnetStream.h>

#if defined(ESP32_RTOS) && defined(ESP32)
void taskOne( void * parameter )
{
  ArduinoOTA.handle();
  delay(3500);
}
#endif

void setupOTA(const char* nameprefix);

