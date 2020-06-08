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
	int SendPeriod		= 20; 		//GSheets send period in seconds
}
/*
   This is the address of Pzem devices on the network. Each pzem device has to set unique
   address when we are working with muliple pzem device (multiple modbus devices/multiple slaves)
   You can use the changeAddress(OldAddress, Newaddress) function below in the code to assign new
   address to each pzem device first time.
   
*/
static uint8_t pzemSlave1Addr;
static uint8_t pzemSlave2Addr;
static uint8_t pzemSlave3Addr;
static uint8_t pzemSlave4Addr;

static uint8_t pzemSlave1Gain = 1;
static uint8_t pzemSlave2Gain = 1;
static uint8_t pzemSlave3Gain = 1;
static uint8_t pzemSlave4Gain = 1;

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

unsigned long	LastSend;

void setup() {
  Serial.begin(76800);

// initialize digital LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);    			// turn the LED ON by making the voltage HIGH

  pzem1Serial.begin(9600);

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

  SetupGSheets();

  // start Modbus/RS-485 serial communication
  digitalWrite(LED_BUILTIN, LOW);    			// turn the LED ON by making the voltage HIGH
  node1.begin(pzemSlave1Addr, pzem1Serial);
  node2.begin(pzemSlave2Addr, pzem1Serial);
  node3.begin(pzemSlave3Addr, pzem1Serial);
  node4.begin(pzemSlave4Addr, pzem1Serial);
  digitalWrite(LED_BUILTIN, HIGH);    			// turn the LED ON by making the voltage HIGH

  LastSend = millis();
  Serial.print("====================================================\r\n\r\n\r\n\r\n\r\n");
}

void pzemdevice(MyModbusMaster *node, Meter *Meter1, uint8_t gain)
{
  // PZEM Device data fetching
  Serial.println("====================================================");
  Serial.print("Now checking Modbus "); Serial.println(node->getSlaveID());

  uint8_t result1;

  ESP.wdtDisable();     //disable watchdog during modbus read or else ESP crashes when no slave connected                                               
  result1 = node->readInputRegisters(0x0000, 10);
  ESP.wdtEnable(1);    	//enable watchdog during modbus read
  
  if (result1 == node->ku8MBSuccess)
  {
	double voltage_usage      = (node->getResponseBuffer(0x00) / 10.0f);
	double current_usage      = (node->getResponseBuffer(0x01) / (gain * 1000.000f));
	double active_power       = (node->getResponseBuffer(0x03) / (gain * 10.0f));
	double active_energy      = (node->getResponseBuffer(0x05) / (gain * 1000.0f));
	double frequency          = (node->getResponseBuffer(0x07) / 10.0f);
	double power_factor       = (node->getResponseBuffer(0x08) / 100.0f);
	double over_power_alarm   = (node->getResponseBuffer(0x09));

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

  pzemdevice(&node1, &Meter1, pzemSlave1Gain);
  delay(250);
  pzemdevice(&node2, &Meter2, pzemSlave2Gain);
  delay(250);
  pzemdevice(&node3, &Meter3, pzemSlave3Gain);
  delay(250);
  pzemdevice(&node4, &Meter4, pzemSlave4Gain);
  delay(250);

  if((millis() - LastSend)/1000 > (unsigned int)cfg::SendPeriod){
	  debug_out(String("loop: FreeHeap=") + String(ESP.getFreeHeap()), 												DEBUG_MED_INFO, 1);

	  Send2GSheets();
	  LastSend = millis();
  }

  yield();
}


void Send2GSheets(){
	// Connect to spreadsheet

	client = new HTTPSRedirect(httpsPort);
	client->setPrintResponseBody(false);
	client->setContentTypeHeader("application/json");

	delay(50);  // one tick delay (1ms) in between reads for stability
	debug_out(F("Send2GSheets: Client object created"), 											DEBUG_MED_INFO, 1);

	if (client != nullptr){
		if (!client->connected()){

			// Try to connect for a maximum of 1 times
			for (int i=0; i<1; i++){

				debug_out(F("Send2GSheets: Calling Client->connect"), 							DEBUG_MED_INFO, 1);
				delay(50);

				int retval = client->connect(host, httpsPort);
				if (retval == 1) {
					 break;
				}
				else {
					debug_out(F("Send2GSheets: Connection failed. Retrying..."), 						DEBUG_WARNING, 1);
					delay(50);
					Serial.println(client->getResponseBody() );
				}
			}
		}
	}
	else{
		debug_out(F("Send2GSheets: Error creating client object!"), 									DEBUG_ERROR, 1);
	}
	if (!client->connected()){
		debug_out(F("Send2GSheets: Connection failed. Stand by till next period"), 					DEBUG_ERROR, 1);
	}
	else
	{
		debug_out(F("Send2GSheets: Client object requests to Spreadsheet"), 						DEBUG_MED_INFO, 1);

		String  data			= "";

			// GPS data
			debug_out("Send2GSheets: Prepare JSON.",												DEBUG_MED_INFO, 1);

			data = FPSTR(data_first_part);

			data += Var2Json(F("M1"),		Meter1.GetJson());
			data += Var2Json(F("M2"),		Meter2.GetJson());
			data += Var2Json(F("M3"),		Meter3.GetJson());
			data += Var2Json(F("M4"),		Meter4.GetJson());

			data += "}";

			// prepare fo gscript
			data.remove(0, 1);

			data.replace(",}","}");
			data.replace("}\"","}");
			data.replace("\"{","{");

			data = payload_base + data;

			debug_out(F("Send2GSheets: Send from buffer to spreadsheet. Payload prepared:"), 				DEBUG_MED_INFO, 1);
			debug_out(data, 																		DEBUG_MED_INFO, 1);

		if(client->POST(url_write, host, data)){
			debug_out(F("Send2GSheets: Spreadsheet updated"), DEBUG_MIN_INFO, 1);

			Meter1.Clear();
			Meter2.Clear();
			Meter3.Clear();
			Meter4.Clear();
		}
		else{
			debug_out(F("Send2GSheets: Spreadsheet update fails: "), DEBUG_MIN_INFO, 1);
		}
	}

	// delete HTTPSRedirect object
	delete client;
	client = nullptr;

	debug_out(F("Send2GSheets Client object deleted"), 											DEBUG_MED_INFO, 1);

}

void SetupGSheets(){
	// Connect to spreadsheet

	client = new HTTPSRedirect(httpsPort);
	client->setPrintResponseBody(false);
	client->setContentTypeHeader("application/json");

	debug_out(F("SetupGSheets: Client object created"), 												DEBUG_MED_INFO, 1);

	if (client != nullptr){
		if (!client->connected()){

			// Try to connect for a maximum of 1 times
			for (int i=0; i<10; i++){

				debug_out(F("SetupGSheets: Calling Client->connect"), 									DEBUG_MED_INFO, 1);

				int retval = client->connect(host, httpsPort);
				if (retval == 1) {
					 break;
				}
				else {
					debug_out(F("SetupGSheets: Connection failed. Retrying..."), 						DEBUG_WARNING, 1);
					delay(5000);
					yield();
					Serial.println(client->getResponseBody() );
				}
			}
		}
	}
	else{
		debug_out(F("SetupGSheets: Error creating client object! Reboot."), 							DEBUG_ERROR, 1);
		Serial.flush();
		ESP.reset();
	}
	if (!client->connected()){
		debug_out(F("SetupGSheets: Connection failed. Reboot."), 										DEBUG_ERROR, 1);
		Serial.flush();
		ESP.reset();
	}
	else
	{
		pzemSlave1Addr = GetGSheetsRange("Addr01").toInt();
		pzemSlave2Addr = GetGSheetsRange("Addr02").toInt();
		pzemSlave3Addr = GetGSheetsRange("Addr03").toInt();
		pzemSlave4Addr = GetGSheetsRange("Addr04").toInt();

		pzemSlave1Gain = GetGSheetsRange("Gain01").toInt();
		pzemSlave2Gain = GetGSheetsRange("Gain02").toInt();
		pzemSlave3Gain = GetGSheetsRange("Gain03").toInt();
		pzemSlave4Gain = GetGSheetsRange("Gain04").toInt();
	}

	// delete HTTPSRedirect object
	delete client;
	client = nullptr;

	debug_out(F("SetupGSheets: Client object deleted"), 												DEBUG_MED_INFO, 1);


	if(!pzemSlave1Addr || !pzemSlave2Addr || !pzemSlave3Addr || !pzemSlave4Addr){
		debug_out(F("SetupGSheets: No device adresses. Reboot."), 										DEBUG_ERROR, 1);
		Serial.flush();
		ESP.reset();
	}
}


String GetGSheetsRange(String Range){

	debug_out(F("GetGSheetsRange: Requests data from Spreadsheet"), 									DEBUG_MED_INFO, 1);

	delay(2000);
	yield();

	if(client->GET(url_read + "=" + Range, host)){
		String str = client->getResponseBody();
		debug_out(F("GetGSheetsRange: getResponseBody:"), 												DEBUG_MED_INFO, 0);
		debug_out(str, 																					DEBUG_MED_INFO, 1);

		return str;
	}
	else{
		debug_out(F("GetGSheetsRange: GET data fails. "), DEBUG_MIN_INFO, 1);
	}
	return "";
}
