/*
 * Definitions.cpp
 *
 *  Created on: Mar 23, 2020
 *      Author: E_CAD
 */

#include <Arduino.h>


#include "Definitions.h"

extern SoftwareSerial pzem1Serial;

namespace cfg {
	extern int	debug;
}

inline float min(float a, float b) {
    if (a > b)
        return b;
    return a;
}
inline float max(float a, float b) {
    if (a < b)
        return b;
    return a;
}


void measurement::NewMeas(float Measure, float treshold){
	float t_max = 0.0;
	float t_min = 0.0;
	bool  new_cycle[3] 	= {false};
	bool  cycle 		= false;

	t_max = max(this->Meas_02_Accum.max, Measure);
	t_min = min(this->Meas_02_Accum.min, Measure);
	t_max = max(this->Meas_01_Check.max, t_max);		// Meas_01_Check can contain several measurements,
	t_min = min(this->Meas_01_Check.min, t_min);		// so better to check previous values also

	this->Meas_01_Check.sum 	+= Measure;
	this->Meas_01_Check.max		 = t_max;
	this->Meas_01_Check.min		 = t_min;
	this->Meas_01_Check.count++;


	// check if new accumulation cycle should be started
	cycle |=	new_cycle[0] = (millis() - this->first_ms) > this->Tmax_ms;
	cycle |=	new_cycle[1] = (this->Meas_01_Check.max - this->Meas_01_Check.min) >= treshold;
	cycle &=	new_cycle[2] = (millis() - this->first_ms) > this->Tmin_ms;

	if(new_cycle[0]){						debug_out(F("New cycle: maximum period. "), 			DEBUG_MAX_INFO, 0);}
	if(new_cycle[1]){						debug_out(F("New cycle: threshold exceed."), 			DEBUG_MAX_INFO, 0);}
	if(new_cycle[1]){						debug_out(" Max: " + String(this->Meas_01_Check.max),	DEBUG_MAX_INFO, 0);}
	if(new_cycle[1]){						debug_out(" Min: " + String(this->Meas_01_Check.min),	DEBUG_MAX_INFO, 0);}

	if(new_cycle[0] || new_cycle[1]){
		if(!new_cycle[2]){					debug_out(F(" But no minimum period yet "), 			DEBUG_MAX_INFO, 0);}
		if(!new_cycle[2]){					debug_out(String(millis() - this->first_ms), 			DEBUG_MAX_INFO, 0);}
											debug_out(F(""), 										DEBUG_MAX_INFO, 1);
	}

	this->Need_to_Store |= cycle;
}
void measurement::Meas_to_Accum(){

	if(this->Meas_01_Check.count){									// Check if any data is in Meas_01_Check

		float t_max = 0.0;
		float t_min = 0.0;

		this->Need_to_Store 		= false;						// Reset flag that new cycle required
		if(!this->Meas_02_Accum.count){								// If its a first record after Accum cleared - store timestamp

			this->first_ms 	= millis();

			t_max	= this->Meas_01_Check.sum / this->Meas_01_Check.count;		// Check.max and .min can not be used - they are from previous cycle
			t_min	= this->Meas_01_Check.sum / this->Meas_01_Check.count;
		}
		else {
			t_max 	= this->Meas_01_Check.max;									// max and min were calculated in previous NewMeas() call
			t_min 	= this->Meas_01_Check.min;
		}

		this->Meas_02_Accum.sum 	+= this->Meas_01_Check.sum;		// Store temporary Meas_01_Check to accumulator
		this->Meas_02_Accum.count	+= this->Meas_01_Check.count;
		this->Meas_02_Accum.max		 = t_max;
		this->Meas_02_Accum.min		 = t_min;
	}

	// clear temporary Meas_01_Check structure
	this->Meas_01_Check.sum 	= 0.0;
	this->Meas_01_Check.count	= 0;
	this->Meas_01_Check.max		= -99999999999.9;
	this->Meas_01_Check.min		= +99999999999.9;
}
void measurement::Accum_to_Store(){

	float t_max = 0.0;
	float t_min = 0.0;

	t_max = max(this->Meas_02_Accum.max, this->Meas_03_Store.max);
	t_min = min(this->Meas_02_Accum.min, this->Meas_03_Store.min);

	this->Meas_03_Store.sum 	+= this->Meas_02_Accum.sum;		// Store temporary Meas_01_Check to accumulator
	this->Meas_03_Store.count	+= this->Meas_02_Accum.count;
	this->Meas_03_Store.max		 = t_max;
	this->Meas_03_Store.min		 = t_min;

	// clear accumulator Meas_02_Accum
	this->Meas_02_Accum.sum 	= 0.0;
	this->Meas_02_Accum.count	= 0;
	this->Meas_02_Accum.max		= -99999999999.9;
	this->Meas_02_Accum.min		= +99999999999.9;

	debug_out(F("Stored records count: "), 						DEBUG_MED_INFO, 0);
	debug_out(String(this->Meas_03_Store.count), 				DEBUG_MED_INFO, 1);
}
void measurement::AddMeas(float Measure){

	this->Meas_02_Accum.count++;
	this->Meas_02_Accum.sum 	+= Measure;
	this->Meas_02_Accum.max 	 = this->Meas_02_Accum.sum;
	this->Meas_02_Accum.min 	 = this->Meas_02_Accum.sum;
}
void measurement::Clear(){		// Clear all measurements in accumulation buffer to be ready for new cycle

	// prepare for next measurements
	this->first_ms = 0;

	this->Meas_01_Check.sum  	= 0.0;
	this->Meas_01_Check.count	= 0;
	this->Meas_01_Check.max 	= -99999999999.9;
	this->Meas_01_Check.min 	= +99999999999.9;

	this->Meas_02_Accum.sum		= 0.0;
	this->Meas_02_Accum.count 	= 0;
	this->Meas_02_Accum.max		= -99999999999.9;
	this->Meas_02_Accum.min		= +99999999999.9;

	this->Need_to_Store			= false;
}
void measurement::ClearStore(){
	// clear send buffer
	this->Meas_03_Store.sum 	= 0.0;
	this->Meas_03_Store.count 	= 0;
	this->Meas_03_Store.max 	= -99999999999.9;
	this->Meas_03_Store.min 	= +99999999999.9;
}
measurement::measurement(){
	this->Clear();
	this->ClearStore();
}
bool measurement::setCycles(unsigned int Tmin_sec, unsigned int Tmax_sec){
	// Store internal parameters
	this->Tmax_ms = Tmax_sec * 1000;
	this->Tmin_ms = Tmin_sec * 1000;

	return (this->Tmax_ms >= this->Tmin_ms);
}
bool measurement::Check_2_Store(){

	return this->Need_to_Store;
}
String measurement::DebugAvg(){
	String strDebug = "";
	if(this->Meas_02_Accum.count > 0){
		strDebug = Float2String(this->Meas_02_Accum.sum/this->Meas_02_Accum.count,1 , 7);
	}
	return strDebug;
}
String measurement::DebugRange(){
	String strDebug = "";
	if(this->Meas_02_Accum.count > 0){
		strDebug  = Float2String(this->Meas_02_Accum.min,1 , 7) + " : ";
		strDebug += Float2String(this->Meas_02_Accum.max,1 , 7);
	}
	return strDebug;
}
String measurement::GetJson(){

	String data = "";

	if(Meas_03_Store.count){
		data = this->Meas_03_Store.min + String(":") + Meas_03_Store.sum/Meas_03_Store.count + String(":") + Meas_03_Store.max;
	}
	else{
		data = "NAN:NAN:NAN";
	}

	return data;
}
uint32_t measurement::GetCount_2_Store(){

	return this->Meas_03_Store.count;
}

/*
Constructor.

Creates class object; initialize it using Meter::begin().
*/
Meter::Meter(){
	this->CRCerr = 0;
	this->Divisor = 1.0;
	this->PREV_active_energy = -1.0;
	this->NeedZeroing = false;
	this->ForceToStore = false;
}
void Meter::begin(uint8_t pzemSlaveAddr, SoftwareSerial *pzemSerial, unsigned int Tmin_sec, unsigned int Tmax_sec){
	this->MBNode.begin(pzemSlaveAddr, *pzemSerial);
	this->Clear();

	this->VOLTAGE.setCycles(		Tmin_sec, Tmax_sec);
	this->CURRENT_USAGE.setCycles(	Tmin_sec, Tmax_sec);
	this->ACTIVE_POWER.setCycles(	Tmin_sec, Tmax_sec);
	this->ACTIVE_ENERGY.setCycles(	Tmin_sec, Tmax_sec);
	this->FREQUENCY.setCycles(		Tmin_sec, Tmax_sec);
	this->POWER_FACTOR.setCycles(	Tmin_sec, Tmax_sec);
}
String Meter::DebugCRC(){
	String strDebug = "";
		strDebug  =	F("CRC errors: ");
		strDebug += Float2String(this->CRCerr);
	return strDebug;
}
void Meter::Clear(){

	// prepare for next measurements
	this->CRCerr  =0;

	this->VOLTAGE.Clear();
	this->CURRENT_USAGE.Clear();
	this->ACTIVE_POWER.Clear();
	this->ACTIVE_ENERGY.Clear();
	this->FREQUENCY.Clear();
	this->POWER_FACTOR.Clear();
}
void Meter::CRCError(){
	this->CRCerr++;
}
String Meter::GetJson(){
	String data = "{";

	noInterrupts();

	data += Var2Json(F("ID"),		this->ID						);
	data += Var2Json(F("VOLT"),		this->VOLTAGE.GetJson()			);
	data += Var2Json(F("CURR"),		this->CURRENT_USAGE.GetJson()	);
	data += Var2Json(F("POWR"),		this->ACTIVE_POWER.GetJson()	);
	data += Var2Json(F("ENRG"),		this->ACTIVE_ENERGY.GetJson()	);
	data += Var2Json(F("FREQ"),		this->FREQUENCY.GetJson()		);
	data += Var2Json(F("POWF"),		this->POWER_FACTOR.GetJson()	);

	data += Var2Json(F("MCNT"),		(double)this->VOLTAGE.GetCount_2_Store());

	data += Var2Json(F("ELST"),		(double)this->GetLastEnergy());

	interrupts();

	data += "}";

	return data;
}
void Meter::Stored(){
	this->VOLTAGE.ClearStore();
	this->CURRENT_USAGE.ClearStore();
	this->ACTIVE_POWER.ClearStore();
	this->ACTIVE_ENERGY.ClearStore();
	this->FREQUENCY.ClearStore();
	this->POWER_FACTOR.ClearStore();

	this->ForceToStore = false;
}
bool Meter::Check_2_Store(){
	bool ToStore = false;

	ToStore |= (this->VOLTAGE.GetCount_2_Store());

	return ToStore;
}
void Meter::SetToStore(){
	this->ForceToStore = true;
}
void Meter::ResetEnergy() {

  debug_out(F("Resetting Energy device ID="), 												DEBUG_MIN_INFO, 0);
  debug_out(String(this->ID), 																DEBUG_MIN_INFO, 1);

  //ESP.wdtDisable();     //disable watchdog during modbus read or else ESP crashes when no slave connected
  // Its not a modbus command

  //The command to reset the slave's energy is (total 4 bytes):
  //Slave address + 0x42 + CRC check high byte + CRC check low byte.
  uint16_t u16CRC = 0xFFFF;
  static uint8_t resetCommand = 0x42;
  u16CRC = crc16_update(u16CRC, this->MBNode.getSlaveID());
  u16CRC = crc16_update(u16CRC, resetCommand);
  pzem1Serial.write(this->MBNode.getSlaveID());
  pzem1Serial.write(resetCommand);
  pzem1Serial.write(lowByte(u16CRC));
  pzem1Serial.write(highByte(u16CRC));

  //ESP.wdtEnable(1);    	//enable watchdog during modbus read

  this->PREV_active_energy = 0.0;
  this->NeedZeroing		   = false;
}

void Meter::GetData(){

	  // PZEM Device data fetching
	  debug_out(F("Now reading Modbus "), 													DEBUG_MED_INFO, 0);
	  debug_out(String(this->MBNode.getSlaveID()), 											DEBUG_MED_INFO, 1);

	  uint8_t result1;

	  ESP.wdtDisable();     //disable watchdog during modbus read or else ESP crashes when no slave connected
	  result1 = this->MBNode.readInputRegisters(0x0000, 10);
	  ESP.wdtEnable(1);    	//enable watchdog during modbus read

	  if (result1 == this->MBNode.ku8MBSuccess)
	  {
		double voltage_usage      = (this->MBNode.getResponseBuffer(0x00) / 10.0f);
		double current_usage      = (this->MBNode.getResponseBuffer(0x01) / (this->Divisor * 1000.0f));		// to Amps
		double active_power       = (this->MBNode.getResponseBuffer(0x03) / (this->Divisor * 10.0f));		// to Watts
		double active_energy      = (this->MBNode.getResponseBuffer(0x05) / (this->Divisor * 1.0f));		// in W*h
		double frequency          = (this->MBNode.getResponseBuffer(0x07) / 10.0f);
		double power_factor       = (this->MBNode.getResponseBuffer(0x08) / 100.0f);
		double over_power_alarm   = (this->MBNode.getResponseBuffer(0x09));

		if(this->PREV_active_energy < 0.0){ this->PREV_active_energy = active_energy;}		// Initialize PREV value

		debug_out(F("VOLTAGE"), 															DEBUG_MAX_INFO, 1);
		this->VOLTAGE.NewMeas(			voltage_usage,	5.0);
		debug_out(F("CURRENT_USAGE"), 														DEBUG_MAX_INFO, 1);
		this->CURRENT_USAGE.NewMeas(	current_usage,	0.25);
		debug_out(F("ACTIVE_POWER"), 														DEBUG_MAX_INFO, 1);
		this->ACTIVE_POWER.NewMeas(		active_power,	50.0);
		debug_out(F("ACTIVE_ENERGY"), 														DEBUG_MAX_INFO, 1);
		this->ACTIVE_ENERGY.AddMeas(	active_energy - this->PREV_active_energy);
		debug_out(F("FREQUENCY"), 															DEBUG_MAX_INFO, 1);
		this->FREQUENCY.NewMeas(		frequency,		0.1);
		debug_out(F("POWER_FACTOR"), 														DEBUG_MAX_INFO, 1);
		this->POWER_FACTOR.NewMeas(		power_factor,	0.2);

		this->PREV_active_energy = active_energy;		// Store for previous
	  }
	  else {
		debug_out(F("Failed to read modbus slave "), 										DEBUG_ERROR, 0);
		debug_out(String(this->MBNode.getSlaveID()), 										DEBUG_ERROR, 1);

	    this->CRCError();
	  }

	  if(this->CURRENT_USAGE.Check_2_Store() || this->ACTIVE_POWER.Check_2_Store() || (this->ForceToStore)){		// New cycle only by current or power changes dramatically
		  this->VOLTAGE.Accum_to_Store();
		  this->CURRENT_USAGE.Accum_to_Store();
		  this->ACTIVE_POWER.Accum_to_Store();
		  this->ACTIVE_ENERGY.Accum_to_Store();
		  this->FREQUENCY.Accum_to_Store();
		  this->POWER_FACTOR.Accum_to_Store();
	  }

	  this->VOLTAGE.Meas_to_Accum();
	  this->CURRENT_USAGE.Meas_to_Accum();
	  this->ACTIVE_POWER.Meas_to_Accum();
	  //this->ACTIVE_ENERGY.Meas_to_Accum();			// .NewMeas not called for Active Energy. AddMeas() adds data directly to Accum
	  this->FREQUENCY.Meas_to_Accum();
	  this->POWER_FACTOR.Meas_to_Accum();
}

double Meter::GetLastEnergy(){
	return this->PREV_active_energy/1000.0;				// W*h to kW*h units conversion
}

/*****************************************************************
 * convert float to string with a      							 *
 * precision of two (or a given number of) decimal places		 *
 *****************************************************************/
String Float2String(const double value) {
	return Float2String(value, 2);
}

String Float2String(const double value, uint8_t digits) {
	// Convert a float to String with two decimals.
	char temp[15];

	dtostrf(value, 13, digits, temp);
	String s = temp;
	s.trim();
	return s;
}

String Float2String(const double value, uint8_t digits, uint8_t size) {

	String s = Float2String(value, digits);

	s = String("               ").substring(1, size - s.length() +1 ) + s;

	return s;
}

/*****************************************************************
 * Debug output													 *
 *****************************************************************/
void debug_out(const String& text, const int level, const bool linebreak) {
	if (level <= cfg::debug) {

		if(level==1){
			Serial.print("ERR ");
			//TelnetStream.print("ERR ");
		}
		if(level==2){
			Serial.print("WRN ");
			//TelnetStream.print("WRN ");
		}

		if (linebreak) {
			Serial.println(text);
			//TelnetStream.println(text);
		} else {
			Serial.print(text);
			//TelnetStream.print(text);
		}
	}
}

/*****************************************************************
 * check display values, return '-' if undefined				 *
 *****************************************************************/
String check_display_value(double value, double undef, uint8_t digits, uint8_t str_len) {
	String s = (value != undef ? Float2String(value, digits, str_len) : "-");
	return s;
}

/*****************************************************************
 * convert value to json string									 *
 *****************************************************************/
String Value2Json(const String& type, const String& value) {
	String s = F("{\"value_type\":\"{t}\",\"value\":\"{v}\"},");
	s.replace("{t}", type);
	s.replace("{v}", value);
	return s;
}


/*****************************************************************
 * convert value to json string with timestamp and location		 *
 *****************************************************************/
String ValueLocated2Json(const String& timestamp, const String& lat, const String& lng, const String& value) {
	String s = F("{\"value\":\"{v}\",\"createdAt\":\"{t}\",\"location\":[{lng},{lat}]}");

	//s = F("{\"value\":\"{v}\" , \"createdAt\":\"{t}\" }\r\n");

	s.replace("{t}" , timestamp);
	s.replace("{v}" , value);
	s.replace("{lng}", lng);
	s.replace("{lat}", lat);

	debug_out("ValueLocated2Json: " + s,																	DEBUG_ALWAYS, 1);

	return s;
}


/*****************************************************************
 * convert string value to json string							 *
 *****************************************************************/
String Var2Json(const String& name, const String& value) {
	String s = F("\"{n}\":\"{v}\",");
	String tmp = value;
	//tmp.replace("\\", "\\\\"); tmp.replace("\"", "\\\"");
	s.replace("{n}", name);
	s.replace("{v}", tmp);
	return s;
}

/*****************************************************************
 * convert boolean value to json string							 *
 *****************************************************************/
String Var2Json(const String& name, const bool value) {
	String s = F("\"{n}\":\"{v}\",");
	s.replace("{n}", name);
	s.replace("{v}", (value ? "true" : "false"));
	return s;
}

/*****************************************************************
 * convert integer value to json string							 *
 *****************************************************************/
String Var2Json(const String& name, const int value) {
	String s = F("\"{n}\":\"{v}\",");
	s.replace("{n}", name);
	s.replace("{v}", String(value));
	return s;
}

/*****************************************************************
 * convert double value to json string							 *
 *****************************************************************/
String Var2Json(const String& name, const double value) {
	String s = F("\"{n}\":\"{v}\",");
	s.replace("{n}", name);
	s.replace("{v}", String(value));
	return s;
}
