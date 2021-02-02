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

/*  ************************** OTA PROBLEM FIXED HERE **********************************************
 * 	https://github.com/platformio/platformio-core/issues/911
 *  ************************************************************************************************/

#include <Ticker.h>  				//Ticker Library
#include "Definitions.h"
//#include "secret.h"               // <<--- UNCOMMENT this before you use and change values on config.h tab
#include "my_secret.h"              // <<--- COMMENT-OUT or REMOVE this line before you use. This is my personal settings.


//#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>


#include <SoftwareSerial.h>  //  ( NODEMCU ESP8266 )
SoftwareSerial pzem1Serial(RX1_PIN_NODEMCU, TX1_PIN_NODEMCU); // (RX,TX) NodeMCU connect to (TX,RX) of PZEM

#include "lib/HTTPSRedirect.h"

#include "OTA.h"					// OTA standard functions added

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Hash.h>
#include <FS.h>
#include "HTML.h"

AsyncWebServer server(80);

const char* PARAM_STRING = "inputString";
const char* PARAM_INT = "inputInt";
const char* PARAM_FLOAT = "inputFloat";

namespace cfg {
	int		debug 			= DEBUG;
	float 	cycle			= 2.0;		//0.5;		//Sensor read cycle
	int 	SendPeriod		= 300; 		// GSheets send period in seconds
	bool	OK				= false;	// Configuration initialized and can be used
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

String pzemSlave1Tag;
String pzemSlave2Tag;
String pzemSlave3Tag;
String pzemSlave4Tag;



Meter	PZEM_Meter[4];


Ticker fetchCycle;
uint8_t Mindex = 0;

const char *GScriptId = GSHEET_ID;
const char* host = "script.google.com";
const int 	httpsPort = 443;

String url_read  = String("/macros/s/") + GScriptId + "/exec?read";		// Write to Google Spreadsheet
String url_write = String("/macros/s/") + GScriptId + "/exec?write";				// Read from Google Spreadsheet
String payload_base =	"{\"command\": \"appendRow\", \"sheet_name\": \"DATA\", ";
HTTPSRedirect* client = nullptr;
const char data_first_part[] PROGMEM = "{\"sensordatavalues\":";

unsigned long	LastSend;
unsigned long	LastRead;

bool			TickFlag;		// Flag to tick from interrupt to main loop
unsigned long	MissedTicks;	// Missed ticks counter






void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}

// Replaces placeholder with stored values
String processor(const String& var){
  //Serial.println(var);
  if(var == "DevTag1"){
    return pzemSlave1Tag;
  }
  else if(var == "DevAdr1"){
    return pzemSlave1Addr;
  }
  else if(var == "DevGain1"){
    return PZEM_Meter[0].Divisor;
  }
  else if(var == "DevTag2"){
    return pzemSlave2Tag;
  }
  else if(var == "DevAdr2"){
    return pzemSlave2Addr;
  }
  else if(var == "DevGain2"){
    return PZEM_Meter[1].Divisor;
  }
  else if(var == "DevTag3"){
	return pzemSlave3Tag;
  }
  else if(var == "DevAdr3"){
	return pzemSlave3Addr;
  }
  else if(var == "DevGain3"){
	return PZEM_Meter[2].Divisor;
  }
  else if(var == "MaxSendPeriod"){
	return (cfg::SendPeriod / 60.0);
  }

  return String();
}













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


  ArduinoOTA.setHostname(OTA_HOSTNAME);
  setupOTA(OTA_HOSTNAME);

  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
  }

  // Get configuration from SPIFFS
  ReadConfig();

  Serial.print("AsyncWebServer setup begins.");
  // Send main web page index.html to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send configuration web page config.html to client
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });





  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam("Name") && request->hasParam("Value")) {
      inputMessage1 = request->getParam("Name")->value();
      inputMessage2 = request->getParam("Value")->value();
      digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
    Serial.print("GPIO: ");
    Serial.print(inputMessage1);
    Serial.print(" - Set to: ");
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK");
  });





  // Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
    if (request->hasParam(PARAM_STRING)) {
      inputMessage = request->getParam(PARAM_STRING)->value();
      writeFile(SPIFFS, "/inputString.txt", inputMessage.c_str());
    }
    // GET inputInt value on <ESP_IP>/get?inputInt=<inputMessage>
    else if (request->hasParam(PARAM_INT)) {
      inputMessage = request->getParam(PARAM_INT)->value();
      writeFile(SPIFFS, "/inputInt.txt", inputMessage.c_str());
    }
    // GET inputFloat value on <ESP_IP>/get?inputFloat=<inputMessage>
    else if (request->hasParam(PARAM_FLOAT)) {
      inputMessage = request->getParam(PARAM_FLOAT)->value();
      writeFile(SPIFFS, "/inputFloat.txt", inputMessage.c_str());
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/text", inputMessage);
  });
  server.onNotFound(notFound);
  server.begin();

  Serial.println(" Done.");

  // Return if no consistent configuration present
  if(!cfg::OK){
	  Serial.print("Configuration inconsistent - Modbus terminated.");
	  return;
  }


  // start Modbus/RS-485 serial communication
  Serial.print("Configure Modbus communication.");
  digitalWrite(LED_BUILTIN, LOW);    			// turn the LED ON by making the voltage HIGH

  PZEM_Meter[0].begin(pzemSlave1Addr, &pzem1Serial, 300, cfg::SendPeriod);
  PZEM_Meter[1].begin(pzemSlave2Addr, &pzem1Serial, 300, cfg::SendPeriod);
  PZEM_Meter[2].begin(pzemSlave3Addr, &pzem1Serial, 300, cfg::SendPeriod);
  PZEM_Meter[3].begin(pzemSlave4Addr, &pzem1Serial, 300, cfg::SendPeriod);

  PZEM_Meter[0].ID = 1;
  PZEM_Meter[1].ID = 2;
  PZEM_Meter[2].ID = 3;
  PZEM_Meter[3].ID = 4;

  digitalWrite(LED_BUILTIN, HIGH);    			// turn the LED ON by making the voltage HIGH
  Serial.println(" Done.");

  LastSend = millis();
  LastRead = millis();

  Serial.println("Start Modbus communication.");
  fetchCycle.attach(cfg::cycle, fetchCycleCall);			// Cyclic interrupt to call sensor data

  debug_out(F("Setup done."), 													DEBUG_ALWAYS, 1);
  debug_out(String("SendPeriod=") + String(cfg::SendPeriod),					DEBUG_ALWAYS, 1);
  debug_out(F("===================================================="), 			DEBUG_ALWAYS, 1);

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

  debug_out(F("Changing Slave Address"), 						DEBUG_ALWAYS, 1);

  pzem1Serial.write(OldslaveAddr);
  pzem1Serial.write(SlaveParameter);
  pzem1Serial.write(highByte(registerAddress));
  pzem1Serial.write(lowByte(registerAddress));
  pzem1Serial.write(highByte(NewslaveAddr));
  pzem1Serial.write(lowByte(NewslaveAddr));
  pzem1Serial.write(lowByte(u16CRC));
  pzem1Serial.write(highByte(u16CRC));
}

// Time interrupt cycle
void fetchCycleCall(){

	if(TickFlag) {MissedTicks++;}
	TickFlag = true;

	ArduinoOTA.handle();
}

// Free loop
void loop() {

  ArduinoOTA.handle();

  if(TickFlag){
	PZEM_Meter[Mindex].GetData();

	if(PZEM_Meter[Mindex].GetLastEnergy()>95.0 && PZEM_Meter[Mindex].GetLastEnergy()<1000.0 ){	// If more than 95 kW - rise reset totalizer flag (if more than 1000kW - likely wrong readings)
		//PZEM_Meter[Mindex].NeedZeroing = true;
	}

	Mindex++;

	if(Mindex>3){
		digitalWrite(LED_BUILTIN, LOW);    	// turn the LED ON by making the voltage LOW
		delay(10);
		digitalWrite(LED_BUILTIN, HIGH);    // turn the LED OFF by making the voltage HIGH
		Mindex = 0;
	}
	TickFlag = false;
  }
  else {



	  for(int i = 0; i<=3; i++){										// 3! 0 is for debug
		  if(PZEM_Meter[i].Check_2_Store()){

			  fetchCycle.detach();

			  debug_out(String("loop: FreeHeap=") + String(ESP.getFreeHeap()), 												DEBUG_MED_INFO, 1);

			  Send2GSheets(&PZEM_Meter[i]);
			  LastSend = millis();

			  fetchCycle.attach(cfg::cycle, fetchCycleCall);			// Cyclic interrupt to call sensor data
		  }

		  if(PZEM_Meter[i].NeedZeroing){								// If reset counter flag raised
			  fetchCycle.detach();
			  PZEM_Meter[i].ResetEnergy();
			  delay(1000);
			  fetchCycle.attach(cfg::cycle, fetchCycleCall);			// Cyclic interrupt to call sensor data
		  }

	  }

	  // check GSheets parameters update
	  if((millis() - LastRead)/1000 > (unsigned int)cfg::ReadPeriod){

		  fetchCycle.detach();

		  if(CheckGSheets()){
			  debug_out(String("loop: Prepare to reboot from GSheets"), 													DEBUG_MED_INFO, 1);
			  Send2GSheets(&PZEM_Meter[0]);
			  Send2GSheets(&PZEM_Meter[1]);
			  Send2GSheets(&PZEM_Meter[2]);
			  Send2GSheets(&PZEM_Meter[3]);
			  ESP.reset();
		  }
		  LastRead = millis();

		  cfg::debug = DEBUG;										// Reset debug to default level after each GSheets connection
		  debug_out(String("Debug level reset to default"),			 														DEBUG_ALWAYS, 1);

		  fetchCycle.attach(cfg::cycle, fetchCycleCall);			// Cyclic interrupt to call sensor data
	  }
  }

	  switch (Serial.read()) {
		case '0':
			cfg::debug = 0;
			break;
		case '1':
			cfg::debug = 1;
			break;
		case '2':
			cfg::debug = 2;
			break;
		case '3':
			cfg::debug = 3;
			break;
		case '4':
			cfg::debug = 4;
			break;
		case '5':
			cfg::debug = 5;
			break;
	  }

	  if(millis()> (unsigned long)(10 * 24 * 3600 * 1000)){
		  debug_out(String("Force restart after 10 days online"),			 										DEBUG_ALWAYS, 1);
		  delay(5000);
		  ESP.reset();
	  }

	  yield();
}

void Send2GSheets(Meter *PZMeter){
	// Connect to spreadsheet


		client = new HTTPSRedirect(httpsPort);
		client->setInsecure();											// Important row! Not works without (no connection establish)
		client->setPrintResponseBody(false);
		client->setContentTypeHeader("application/json");

		delay(50);  // one tick delay (1ms) in between reads for stability
		debug_out(F("Send2GSheets: Client object created"), 											DEBUG_MED_INFO, 1);

		if (client != nullptr){
			if (!client->connected()){

				// Try to connect for a maximum of 1 times
				for (int i=0; i<1; i++){

					debug_out(F("Send2GSheets: Calling Client->connect"), 								DEBUG_MED_INFO, 1);
					delay(50);

					int retval = client->connect(host, httpsPort);
					if (retval == 1) {
						 break;
					}
					else {
						debug_out(F("Send2GSheets: Connection failed. Retrying..."), 					DEBUG_WARNING, 1);
						delay(50);
						debug_out(client->getResponseBody(), 											DEBUG_WARNING, 1);
					}
				}
			}
		}
		else{
			debug_out(F("Send2GSheets: Error creating client object!"), 								DEBUG_ERROR, 1);
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

				//data += Var2Json("M"+String(PZMeter->ID),		PZMeter->GetJson());
				data += PZMeter->GetJson();

				data += "}";

				// prepare fo gscript
				data.remove(0, 1);

				data.replace(",}","}");
				data.replace("}\"","}");
				data.replace("\"{","{");

				data = payload_base + data;

				debug_out(F("Send2GSheets: Send from buffer to spreadsheet. Payload prepared:"), 		DEBUG_MED_INFO, 1);
				debug_out(data, 																		DEBUG_MED_INFO, 1);

			if(client->POST(url_write, host, data)){
				debug_out(F("Send2GSheets: Spreadsheet updated"), DEBUG_MIN_INFO, 1);

				PZMeter->Stored();
			}
			else{
				debug_out(F("Send2GSheets: Spreadsheet update fails: "), DEBUG_MIN_INFO, 1);
			}
		}
		// delete HTTPSRedirect object
		delete client;
		client = nullptr;

		debug_out(F("Send2GSheets Client object deleted"), 												DEBUG_MED_INFO, 1);
}

void ReadConfig(){

	debug_out(F("ReadConfig. Read device configuration from SPIFFS"), 												DEBUG_MED_INFO, 1);


	pzemSlave1Addr = readFile(SPIFFS, "/pzemSlave1Addr.txt").toInt();
	pzemSlave2Addr = readFile(SPIFFS, "/pzemSlave2Addr.txt").toInt();
	pzemSlave3Addr = readFile(SPIFFS, "/pzemSlave3Addr.txt").toInt();
	pzemSlave4Addr = readFile(SPIFFS, "/pzemSlave4Addr.txt").toInt();

	pzemSlave1Tag  = readFile(SPIFFS, "/pzemSlave1Tag.txt");
	pzemSlave2Tag  = readFile(SPIFFS, "/pzemSlave2Tag.txt");
	pzemSlave3Tag  = readFile(SPIFFS, "/pzemSlave3Tag.txt");
	pzemSlave4Tag  = readFile(SPIFFS, "/pzemSlave4Tag.txt");

	PZEM_Meter[0].Divisor = readFile(SPIFFS, "/PZEM_Meter0Div.txt").toFloat();
	PZEM_Meter[1].Divisor = readFile(SPIFFS, "/PZEM_Meter1Div.txt").toFloat();
	PZEM_Meter[2].Divisor = readFile(SPIFFS, "/PZEM_Meter2Div.txt").toFloat();
	PZEM_Meter[3].Divisor = readFile(SPIFFS, "/PZEM_Meter3Div.txt").toFloat();

	cfg::SendPeriod  = readFile(SPIFFS, "/SendPeriod.txt").toInt() * 60; // Minutes to seconds


	// Check read configuration consistent
	cfg::OK = true;

	cfg::OK &= pzemSlave1Addr;
	cfg::OK &= pzemSlave2Addr;
	cfg::OK &= pzemSlave3Addr;
	cfg::OK &= pzemSlave4Addr;

	cfg::OK &= pzemSlave1Tag.length();
	cfg::OK &= pzemSlave2Tag.length();
	cfg::OK &= pzemSlave3Tag.length();
	cfg::OK &= pzemSlave4Tag.length();

	cfg::OK &= PZEM_Meter[0].Divisor > 0.0;
	cfg::OK &= PZEM_Meter[1].Divisor > 0.0;
	cfg::OK &= PZEM_Meter[2].Divisor > 0.0;
	cfg::OK &= PZEM_Meter[3].Divisor > 0.0;

	cfg::OK &= cfg::SendPeriod > 120;

	if(!cfg::OK){
		debug_out(F("ReadConfig. SPIFFS configuration is not consistent."), 									DEBUG_ERROR, 1);
	}
	else{
		debug_out(F("ReadConfig. Configuration read sucesfull."), 												DEBUG_MED_INFO, 1);
	}

}



bool CheckGSheets(){

	bool Restart = false;

	// Connect to spreadsheet

	client = new HTTPSRedirect(httpsPort);
	client->setInsecure();											// Important row! Not works without (no connection establish)
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
					debug_out(client->getResponseBody(), 												DEBUG_WARNING, 1);
					delay(5000);
					yield();
					ArduinoOTA.handle();
				}
			}
		}
	}
	else{
		debug_out(F("SetupGSheets: Error creating client object! Reboot."), 							DEBUG_ERROR, 1);
		Serial.flush();
		delay(1000);
		ESP.reset();
	}
	if (!client->connected()){
		debug_out(F("SetupGSheets: Connection failed. Reboot."), 										DEBUG_ERROR, 1);
		Serial.flush();
		delay(1000);
		ESP.reset();
	}
	else
	{
		String RebootReq = GetGSheetsRange("reboot");
		RebootReq.toUpperCase();
		Restart = String("REBOOT").equals(RebootReq);
	}

	// delete HTTPSRedirect object
	delete client;
	client = nullptr;

	debug_out(F("SetupGSheets: Client object deleted"), 												DEBUG_MED_INFO, 1);


	if(!pzemSlave1Addr || !pzemSlave2Addr || !pzemSlave3Addr || !pzemSlave4Addr){
		debug_out(F("SetupGSheets: No device adresses. Reboot."), 										DEBUG_ERROR, 1);
		Serial.flush();
		delay(1000);
		ESP.reset();
	}

	return Restart;
}




String GetGSheetsRange(String Range){

	debug_out(F("GetGSheetsRange: Requests data from Spreadsheet"), 									DEBUG_MED_INFO, 1);

	delay(2000);
	yield();
	ArduinoOTA.handle();

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
