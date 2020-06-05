/*
   Blynk PZEM 004T v3.0 Multiple Device (Here 2 Pzem) Connection Program

   Source: https://github.com/pkarun/Blynk-PZEM-004T-v3.0-Multiple-device
   https://github.com/pkarun/Blynk-PZEM-004T-v3.0

   Reference:

    https://community.blynk.cc/t/pzem-004t-v3-0-and-nodemcu-wemos-mini-running-on-blynk-how-to-procedure/39338
    https://asndiy.wordpress.com/2019/03/02/pzem-016-nodemcu-thingspeak/
    http://evertdekker.com/?p=1307
    https://didactronica.com/medidas-electricas-en-corriente-alterna-con-arduino-y-un-solo-dispositivo-pzem-004t/
    http://solar4living.com/pzem-arduino-modbus.htm
    http://www.desert-home.com/2018/07/pzem-016-another-chinese-power-monitor.html
    https://www.youtube.com/watch?v=gJRhfs6A1SA

   Wiring:

   PZEM 004T v3.0 to NodeMCU
   5v to vin
   RX to D6 (TX Pin)
   TX to D5 (RX Pin)
   GND to GND

*/

// Build in LED
#define LED_BUILTIN 2


//#define BLYNK_PRINT Serial        // Uncomment for debugging 

#include "settings.h"           
//#include "secret.h"               // <<--- UNCOMMENT this before you use and change values on config.h tab
#include "my_secret.h"              // <<--- COMMENT-OUT or REMOVE this line before you use. This is my personal settings.

#include <ArduinoOTA.h>
//#include <BlynkSimpleEsp8266.h>
//#include <SimpleTimer.h>
//#include <ModbusMaster.h>
#include <ESP8266WiFi.h>


#include <SoftwareSerial.h>  //  ( NODEMCU ESP8266 )
SoftwareSerial pzemSerial(RX_PIN_NODEMCU, TX_PIN_NODEMCU); // (RX,TX) NodeMCU connect to (TX,RX) of PZEM
#include <ModbusMaster.h>

/*
   This is the address of Pzem devices on the network. Each pzem device has to set unique
   address when we are working with muliple pzem device (multiple modbus devices/multiple slaves)
   You can use the changeAddress(OldAddress, Newaddress) function below in the code to assign new
   address to each pzem device first time.
   
*/
static uint8_t pzemSlave1Addr = PZEM_SLAVE_1_ADDRESS; 
static uint8_t pzemSlave2Addr = PZEM_SLAVE_2_ADDRESS;
static uint8_t pzemSlave3Addr = PZEM_SLAVE_3_ADDRESS;
static uint8_t pzemSlave4Addr = PZEM_SLAVE_4_ADDRESS;


//ModbusMaster node;

ModbusMaster node1;
ModbusMaster node2;
ModbusMaster node3;
ModbusMaster node4;

//BlynkTimer timer;

double voltage_usage_1 = 0; 
double current_usage_1 = 0;
double active_power_1 = 0;
double active_energy_1 = 0;
double frequency_1 = 0;
double power_factor_1 = 0; 
double over_power_alarm_1 = 0;
 
double voltage_usage_2 = 0;
double current_usage_2 = 0;
double active_power_2 = 0;
double active_energy_2 = 0;
double frequency_2 = 0;
double power_factor_2 = 0; 
double over_power_alarm_2 = 0;


void setup() {
  Serial.begin(76800);

// initialize digital LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED ON by making the voltage HIGH

  pzemSerial.begin(9600);

  // start Modbus/RS-485 serial communication
  node1.begin(pzemSlave1Addr, pzemSerial);
  node2.begin(pzemSlave2Addr, pzemSerial);
  node3.begin(pzemSlave3Addr, pzemSerial);
  node4.begin(pzemSlave4Addr, pzemSerial);

  digitalWrite(LED_BUILTIN, HIGH);    // turn the LED ON by making the voltage HIGH


  /* 
     changeAddress(OldAddress, Newaddress)
     By Uncomment the function in the below line you can change the slave address from one of the nodes (pzem device),
     only need to be done ones. Preverable do this only with 1 slave in the network.
     If you forgot or don't know the new address anymore, you can use the broadcast address 0XF8 as OldAddress to change the slave address.
     Use this with one slave ONLY in the network.
     This is the first step you have to do when connecting muliple pzem devices. If you haven't set the pzem address, then this program won't
     works.
     1. Connect only one PZEM device to nodemcu and powerup your PZEM
     2. uncomment the changeAddress function below i.e., changeAddress(OldAddress, Newaddress)
     3. change the Newaddress value to some other value. Ex: 0x01, 0x02, 0x03 etc.,
     4. upload the program to nodemcu 
     5. if you see "Changing Slave Address" on serial monitor, then it successfully changed address 
     6. if you don't see that message, then click on RESET button on nodemcu

  */
   
  //changeAddress(0x01, 0x05);  //uncomment to set pzem address. You can press reset button on nodemcu if this function is not called


  //resetEnergy(0x01);
  /* By Uncomment the function in the above line you can reset the energy counter (Wh) back to zero from one of the slaves.
      //resetEnergy(pzemSlaveAddr);
  */

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // ArduinoOTA.setPort(8266);
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.begin();

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  delay(1000);

}






void pzemdevice(ModbusMaster *node)
{
  // PZEM Device data fetching
  Serial.println("===================================================="); 
  Serial.println("Now checking Modbus x");
  uint8_t result1;


  ESP.wdtDisable();     //disable watchdog during modbus read or else ESP crashes when no slave connected                                               
  result1 = node->readInputRegisters(0x0000, 10);
  ESP.wdtEnable(1);    //enable watchdog during modbus read  
  
  if (result1 == node->ku8MBSuccess)
  {
    voltage_usage_1      = (node->getResponseBuffer(0x00) / 10.0f);
    current_usage_1      = (node->getResponseBuffer(0x01) / 1000.000f);
    active_power_1       = (node->getResponseBuffer(0x03) / 10.0f);
    active_energy_1      = (node->getResponseBuffer(0x05) / 1000.0f);
    frequency_1          = (node->getResponseBuffer(0x07) / 10.0f);
    power_factor_1       = (node->getResponseBuffer(0x08) / 100.0f);
    over_power_alarm_1   = (node->getResponseBuffer(0x09));

    Serial.println("Modbus x Data");
    Serial.print("VOLTAGE:           ");   Serial.println(voltage_usage_1);   // V
    Serial.print("CURRENT_USAGE:     ");   Serial.println(current_usage_1, 3);  //  A
    Serial.print("ACTIVE_POWER:      ");   Serial.println(active_power_1);   //  W
    Serial.print("ACTIVE_ENERGY:     ");   Serial.println(active_energy_1, 3);  // kWh
    Serial.print("FREQUENCY:         ");   Serial.println(frequency_1);    // Hz
    Serial.print("POWER_FACTOR:      ");   Serial.println(power_factor_1);
    Serial.print("OVER_POWER_ALARM:  ");   Serial.println(over_power_alarm_1, 0);
    Serial.println("====================================================");
  }

  else {
    Serial.println("Failed to read modbus x");
   
  }
}



void resetEnergy(uint8_t slaveAddr) {
  //The command to reset the slave's energy is (total 4 bytes):
  //Slave address + 0x42 + CRC check high byte + CRC check low byte.
  uint16_t u16CRC = 0xFFFF;
  static uint8_t resetCommand = 0x42;
  u16CRC = crc16_update(u16CRC, slaveAddr);
  u16CRC = crc16_update(u16CRC, resetCommand);
  Serial.println("Resetting Energy");
  pzemSerial.write(slaveAddr);
  pzemSerial.write(resetCommand);
  pzemSerial.write(lowByte(u16CRC));
  pzemSerial.write(highByte(u16CRC));
  delay(1000);
}


//function to change/assign pzem address

void changeAddress(uint8_t OldslaveAddr, uint8_t NewslaveAddr)
{
  static uint8_t SlaveParameter = 0x06;
  static uint16_t registerAddress = 0x0002; // Register address to be changed
  uint16_t u16CRC = 0xFFFF;
  u16CRC = crc16_update(u16CRC, OldslaveAddr);
  u16CRC = crc16_update(u16CRC, SlaveParameter);
  u16CRC = crc16_update(u16CRC, highByte(registerAddress));
  u16CRC = crc16_update(u16CRC, lowByte(registerAddress));
  u16CRC = crc16_update(u16CRC, highByte(NewslaveAddr));
  u16CRC = crc16_update(u16CRC, lowByte(NewslaveAddr));

  Serial.println("Changing Slave Address");

  pzemSerial.write(OldslaveAddr);
  pzemSerial.write(SlaveParameter);
  pzemSerial.write(highByte(registerAddress));
  pzemSerial.write(lowByte(registerAddress));
  pzemSerial.write(highByte(NewslaveAddr));
  pzemSerial.write(lowByte(NewslaveAddr));
  pzemSerial.write(lowByte(u16CRC));
  pzemSerial.write(highByte(u16CRC));
  delay(1000);
}


void loop() {

  digitalWrite(LED_BUILTIN, LOW);
  ArduinoOTA.handle();
  delay(10);
  digitalWrite(LED_BUILTIN, HIGH);

  pzemdevice(&node1);
  delay(50);
  pzemdevice(&node2);
  delay(50);
  pzemdevice(&node3);
  delay(50);
  pzemdevice(&node4);
  delay(50);
}
