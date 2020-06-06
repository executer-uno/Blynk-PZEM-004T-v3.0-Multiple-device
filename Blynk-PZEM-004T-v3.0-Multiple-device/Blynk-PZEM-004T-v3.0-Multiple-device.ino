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

//#define BLYNK_PRINT Serial        // Uncomment for debugging 

#include "Definitions.h"
//#include "secret.h"               // <<--- UNCOMMENT this before you use and change values on config.h tab
#include "my_secret.h"              // <<--- COMMENT-OUT or REMOVE this line before you use. This is my personal settings.

#include "MyModbusMaster.h"

#include <ArduinoOTA.h>
//#include <BlynkSimpleEsp8266.h>
//#include <SimpleTimer.h>
//#include <ModbusMaster.h>
#include <ESP8266WiFi.h>


#include <SoftwareSerial.h>  //  ( NODEMCU ESP8266 )
SoftwareSerial pzem1Serial(RX1_PIN_NODEMCU, TX1_PIN_NODEMCU); // (RX,TX) NodeMCU connect to (TX,RX) of PZEM
//SoftwareSerial pzem2Serial(RX2_PIN_NODEMCU, TX2_PIN_NODEMCU); // (RX,TX) NodeMCU connect to (TX,RX) of PZEM


#include "lib/HTTPSRedirect.h"


namespace cfg {
	int	debug 			= DEBUG;
}
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

MyModbusMaster node1;
MyModbusMaster node2;
MyModbusMaster node3;
MyModbusMaster node4;


Meter	Meter1;
Meter	Meter2;
Meter	Meter3;
Meter	Meter4;



const char *GScriptId = GSHEET_ID;
const char* host = "script.google.com";
const int 	httpsPort = 443;

String url_read  = String("/macros/s/") + GScriptId + "/exec?read";		// Write to Google Spreadsheet
String url_write = String("/macros/s/") + GScriptId + "/exec?write";				// Read from Google Spreadsheet
String payload_base =	"{\"command\": \"appendRow\", \"sheet_name\": \"DATA\", \"values\": ";
HTTPSRedirect* client = nullptr;
const char data_first_part[] PROGMEM = "{\"sensordatavalues\":{";


void setup() {
  Serial.begin(76800);

// initialize digital LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED ON by making the voltage HIGH

  pzem1Serial.begin(9600);
  //pzem2Serial.begin(9600);

  // start Modbus/RS-485 serial communication
  node1.begin(pzemSlave1Addr, pzem1Serial);
  node2.begin(pzemSlave2Addr, pzem1Serial);
  node3.begin(pzemSlave3Addr, pzem1Serial);
  node4.begin(pzemSlave4Addr, pzem1Serial);

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
   
  //changeAddress(0x04, 0x14);  //uncomment to set pzem address. You can press reset button on nodemcu if this function is not called
  //changeAddress(0x16, 0x04);  //uncomment to set pzem address. You can press reset button on nodemcu if this function is not called


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

void pzemdevice(MyModbusMaster *node, Meter *Meter1)
{
  // PZEM Device data fetching
  Serial.println("====================================================");
  Serial.print("Now checking Modbus "); Serial.println(node->getSlaveID());

  uint8_t result1;

  ESP.wdtDisable();     //disable watchdog during modbus read or else ESP crashes when no slave connected                                               
  result1 = node->readInputRegisters(0x0000, 10);
  ESP.wdtEnable(1);    //enable watchdog during modbus read  
  
  if (result1 == node->ku8MBSuccess)
  {
	double voltage_usage      = (node->getResponseBuffer(0x00) / 10.0f);
	double current_usage      = (node->getResponseBuffer(0x01) / 1000.000f);
	double active_power       = (node->getResponseBuffer(0x03) / 10.0f);
	double active_energy      = (node->getResponseBuffer(0x05) / 1000.0f);
	double frequency          = (node->getResponseBuffer(0x07) / 10.0f);
	double power_factor       = (node->getResponseBuffer(0x08) / 100.0f);
	double over_power_alarm   = (node->getResponseBuffer(0x09));
/*
    Serial.print("Modbus "); Serial.print(node->getSlaveID()); Serial.println(" slave");
    Serial.print("VOLTAGE:           ");   Serial.println(voltage_usage_1);   	// V
    Serial.print("CURRENT_USAGE:     ");   Serial.println(current_usage_1, 3);  // A
    Serial.print("ACTIVE_POWER:      ");   Serial.println(active_power_1);   	// W
    Serial.print("ACTIVE_ENERGY:     ");   Serial.println(active_energy_1, 3);  // kWh
    Serial.print("FREQUENCY:         ");   Serial.println(frequency_1);    		// Hz
    Serial.print("POWER_FACTOR:      ");   Serial.println(power_factor_1);
    Serial.print("OVER_POWER_ALARM:  ");   Serial.println(over_power_alarm_1, 0);
    Serial.println("====================================================");
*/
    Meter1->VOLTAGE.NewMeas(		voltage_usage);
    Meter1->CURRENT_USAGE.NewMeas(	current_usage);
    Meter1->ACTIVE_POWER.NewMeas(	active_power);
    Meter1->ACTIVE_ENERGY.NewMeas(	active_energy);
    Meter1->FREQUENCY.NewMeas(		frequency);
    Meter1->POWER_FACTOR.NewMeas(	power_factor);
  }
  else {
    Serial.print("Failed to read modbus "); Serial.println(node->getSlaveID());
    Meter1->CRCError();
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
  pzem1Serial.write(slaveAddr);
  pzem1Serial.write(resetCommand);
  pzem1Serial.write(lowByte(u16CRC));
  pzem1Serial.write(highByte(u16CRC));
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

  pzem1Serial.write(OldslaveAddr);
  pzem1Serial.write(SlaveParameter);
  pzem1Serial.write(highByte(registerAddress));
  pzem1Serial.write(lowByte(registerAddress));
  pzem1Serial.write(highByte(NewslaveAddr));
  pzem1Serial.write(lowByte(NewslaveAddr));
  pzem1Serial.write(lowByte(u16CRC));
  pzem1Serial.write(highByte(u16CRC));
  delay(1000);
}

void loop() {

  digitalWrite(LED_BUILTIN, LOW);
  delay(10);
  digitalWrite(LED_BUILTIN, HIGH);

  ArduinoOTA.handle();

  pzemdevice(&node1, &Meter1);
  pzemdevice(&node2, &Meter2);
  pzemdevice(&node3, &Meter3);
  pzemdevice(&node4, &Meter4);

  Send2GSheets();

  delay(10000);
  yield();
}


void Send2GSheets(){
	// Connect to spreadsheet

	client = new HTTPSRedirect(httpsPort);
	client->setPrintResponseBody(false);
	client->setContentTypeHeader("application/json");

	delay(50);  // one tick delay (1ms) in between reads for stability
	debug_out(F("WWW: Client object created"), 											DEBUG_MED_INFO, 1);

	if (client != nullptr){
		if (!client->connected()){

			// Try to connect for a maximum of 1 times
			for (int i=0; i<1; i++){

				debug_out(F("WWW: Calling Client->connect"), 							DEBUG_MED_INFO, 1);
				delay(50);

				int retval = client->connect(host, httpsPort);
				if (retval == 1) {
					 break;
				}
				else {
					debug_out(F("Connection failed. Retrying..."), 						DEBUG_WARNING, 1);
					delay(50);
					Serial.println(client->getResponseBody() );
				}
			}
		}
	}
	else{
		debug_out(F("Error creating client object!"), 									DEBUG_ERROR, 1);
	}
	if (!client->connected()){
		debug_out(F("Connection failed. Stand by till next period"), 					DEBUG_ERROR, 1);
	}
	else
	{
		debug_out(F("WWW: Client object requests to Spreadsheet"), 						DEBUG_MED_INFO, 1);

		String  data			= "";

			// GPS data
			debug_out("WWW: Prepare JSON.",												DEBUG_MED_INFO, 1);

			data = FPSTR(data_first_part);

			data += Var2Json(F("M1"),		Meter1.GetJson());
			data += Var2Json(F("M2"),		Meter2.GetJson());
			data += Var2Json(F("M3"),		Meter3.GetJson());
			data += Var2Json(F("M4"),		Meter4.GetJson());

			data += "}";

			// prepare fo gscript
			data.remove(0, 1);

			data.replace(",}","}");

			data = payload_base + data;

			debug_out(F("WWW: Send from buffer to spreadsheet. Payload prepared:"), 				DEBUG_MED_INFO, 1);
			debug_out(data, 																		DEBUG_MED_INFO, 1);


		if(client->POST(url_write, host, data)){
			debug_out(F("Spreadsheet updated"), DEBUG_MIN_INFO, 1);
		}
		else{
			debug_out(F("Spreadsheet update fails: "), DEBUG_MIN_INFO, 1);
		}
	}

	// delete HTTPSRedirect object
	delete client;
	client = nullptr;

	debug_out(F("WWW: Client object deleted"), 											DEBUG_MED_INFO, 1);

}


