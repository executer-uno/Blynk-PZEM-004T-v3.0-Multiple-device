/*
 * Definitions.cpp
 *
 *  Created on: Mar 23, 2020
 *      Author: E_CAD
 */

#include <Arduino.h>
#include <TelnetStream.h>


#include "Definitions.h"



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
void measurement::NewCycle(){

	if(this->Meas_2_Store.count){					// data have not been sent yet, concatenate measurements

		// AVG
		float t_sum = 0.0;
		t_sum += this->Meas_2_Store.avg * this->Meas_2_Store.count;
		t_sum += this->sum;

		this->Meas_2_Store.count += this->Measurements.count;
		this->Meas_2_Store.avg = t_sum / this->Meas_2_Store.count;

		// MAX
		this->Meas_2_Store.max = max(this->Meas_2_Store.max, this->Measurements.max);

		// MIN
		this->Meas_2_Store.min = min(this->Meas_2_Store.min, this->Measurements.min);

	}
	else{											// store fresh values to outbox
		this->Meas_2_Store = this->Measurements;
	}

	this->Clear();
	this->first_ms = millis();

}
void measurement::NewMeas(float Measure, float treshold){
	float t_max = 0.0;
	float t_min = 0.0;
	bool  new_cycle = false;


	t_max = max(this->Measurements.max, Measure);
	t_min = min(this->Measurements.min, Measure);

	// check if new accumulation cycle should be started
	new_cycle |= (millis() - this->first_ms) > this->Tmax_ms;
	new_cycle |= (t_max - t_min) > treshold;
	new_cycle &= (millis() - this->first_ms) > this->Tmin_ms;
	new_cycle &= this->Measurements.count > 0;

	if(new_cycle){

		this->NewCycle();

		t_max = Measure;
		t_min = Measure;
	}

	this->Measurements.count++;
	this->sum += Measure;

	this->Measurements.avg = this->sum / this->Measurements.count;
	this->Measurements.max = t_max;
	this->Measurements.min = t_min;
}
void measurement::AddMeas(float Measure){

	this->Measurements.count++;
	this->sum += Measure;

	this->Measurements.avg = this->sum;
	this->Measurements.max = this->sum;
	this->Measurements.min = this->sum;

	this->Meas_2_Store = this->Measurements;
	this->first_ms = millis();	// for AddMeas() it will be 'last_ms' in fact
}
void measurement::Clear(){

	// prepare for next measurements
	this->first_ms = 0;

	this->sum 	 =0;

	this->Measurements.count  	=0;
	this->Measurements.avg		= NAN;
	this->Measurements.max 		= -99999999999.9;
	this->Measurements.min 		= +99999999999.9;
}
void measurement::Stored(){
	// clear send buffer
	this->Meas_2_Store.count = 0;
	this->Meas_2_Store.avg = NAN;
	this->Meas_2_Store.max = NAN;
	this->Meas_2_Store.min = NAN;
}
measurement::measurement(){
	this->Clear();
	this->Stored();
}
bool measurement::setCycles(unsigned int Tmin_sec, unsigned int Tmax_sec){
	// Store internal parameters
	this->Tmax_ms = Tmax_sec * 1000;
	this->Tmin_ms = Tmin_sec * 1000;

	return (this->Tmax_ms >= this->Tmin_ms);
}
String measurement::DebugAvg(){
	String strDebug = "";
	if(this->Measurements.count > 0){
		strDebug = Float2String(this->Measurements.avg,1 , 7);
	}
	return strDebug;
}
String measurement::DebugRange(){
	String strDebug = "";
	if(this->Measurements.count > 0){
		strDebug  = Float2String(this->Measurements.min,1 , 7) + " : ";
		strDebug += Float2String(this->Measurements.max,1 , 7);
	}
	return strDebug;
}
String measurement::GetJson(){

	String data = this->Meas_2_Store.min + String(":") + Meas_2_Store.avg + String(":") + Meas_2_Store.max;

	return data;
}
uint32_t measurement::GetCount_2_Store(){

	return this->Meas_2_Store.count;
}

/*
Constructor.

Creates class object; initialize it using Meter::begin().
*/
Meter::Meter(){
	this->CRCerr = 0;
	this->Divisor = 1.0;
	this->PREV_active_energy = -1.0;
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

	data += Var2Json(F("VOLT"),		this->VOLTAGE.GetJson()			);
	data += Var2Json(F("CURR"),		this->CURRENT_USAGE.GetJson()	);
	data += Var2Json(F("POWR"),		this->ACTIVE_POWER.GetJson()	);
	data += Var2Json(F("ENRG"),		this->ACTIVE_ENERGY.GetJson()	);
	data += Var2Json(F("FREQ"),		this->FREQUENCY.GetJson()		);
	data += Var2Json(F("POWF"),		this->POWER_FACTOR.GetJson()	);

	data += Var2Json(F("MCNT"),		(double)this->VOLTAGE.GetCount_2_Store());

	interrupts();

	data += "}";

	return data;
}
void Meter::Stored(){
	this->VOLTAGE.Stored();
	this->CURRENT_USAGE.Stored();
	this->ACTIVE_POWER.Stored();
	this->ACTIVE_ENERGY.Stored();
	this->FREQUENCY.Stored();
	this->POWER_FACTOR.Stored();
}
bool Meter::Check_2_Store(){
	bool ToStore = false;

	ToStore |= (this->VOLTAGE.GetCount_2_Store());

	return ToStore;
}
void Meter::ResetEnergy() {
  //The command to reset the slave's energy is (total 4 bytes):
  //Slave address + 0x42 + CRC check high byte + CRC check low byte.
  static uint16_t resetCommand = 0x0042;

  debug_out(F("Resetting Energy"), 															DEBUG_MIN_INFO, 1);

  ESP.wdtDisable();     //disable watchdog during modbus read or else ESP crashes when no slave connected
  this->MBNode.send(resetCommand);
  ESP.wdtEnable(1);    	//enable watchdog during modbus read

  this->PREV_active_energy = 0.0;
}

void Meter::GetData(){

	  // PZEM Device data fetching
	  debug_out(F("Now reading Modbus "), 													DEBUG_MAX_INFO, 0);
	  debug_out(String(this->MBNode.getSlaveID()), 											DEBUG_MAX_INFO, 1);

	  uint8_t result1;

	  ESP.wdtDisable();     //disable watchdog during modbus read or else ESP crashes when no slave connected
	  result1 = this->MBNode.readInputRegisters(0x0000, 10);
	  ESP.wdtEnable(1);    	//enable watchdog during modbus read

	  if (result1 == this->MBNode.ku8MBSuccess)
	  {
		double voltage_usage      = (this->MBNode.getResponseBuffer(0x00) / 10.0f);
		double current_usage      = (this->MBNode.getResponseBuffer(0x01) / (this->Divisor * 1000.000f));
		double active_power       = (this->MBNode.getResponseBuffer(0x03) / (this->Divisor * 10.0f));
		double active_energy      = (this->MBNode.getResponseBuffer(0x05) / (this->Divisor * 1000.0f));
		double frequency          = (this->MBNode.getResponseBuffer(0x07) / 10.0f);
		double power_factor       = (this->MBNode.getResponseBuffer(0x08) / 100.0f);
		double over_power_alarm   = (this->MBNode.getResponseBuffer(0x09));

		if(this->PREV_active_energy < 0.0){ this->PREV_active_energy = active_energy;}		// Initialize PREV value

		this->VOLTAGE.NewMeas(			voltage_usage,	6.0);
		this->CURRENT_USAGE.NewMeas(	current_usage,	1.0);
		this->ACTIVE_POWER.NewMeas(		active_power,	200.0);
		this->ACTIVE_ENERGY.AddMeas(	active_energy - this->PREV_active_energy);
		this->FREQUENCY.NewMeas(		frequency,		1.0);
		this->POWER_FACTOR.NewMeas(		power_factor,	0.2);

		this->PREV_active_energy = active_energy;		// Store for previous
	  }
	  else {
		debug_out(F("Failed to read modbus slave "), 										DEBUG_ERROR, 0);
		debug_out(String(this->MBNode.getSlaveID()), 										DEBUG_ERROR, 1);

	    this->CRCError();
	  }

	  if(this->VOLTAGE.GetCount_2_Store() || this->CURRENT_USAGE.GetCount_2_Store() || this->ACTIVE_POWER.GetCount_2_Store()){
		  if(!this->VOLTAGE.GetCount_2_Store())			{	this->VOLTAGE.NewCycle();	}
		  if(!this->CURRENT_USAGE.GetCount_2_Store())	{	this->CURRENT_USAGE.NewCycle();	}
		  if(!this->ACTIVE_POWER.GetCount_2_Store())	{	this->ACTIVE_POWER.NewCycle();	}
		  if(!this->ACTIVE_ENERGY.GetCount_2_Store())	{	this->ACTIVE_ENERGY.NewCycle();	}
		  if(!this->FREQUENCY.GetCount_2_Store())		{	this->FREQUENCY.NewCycle();	}
		  if(!this->POWER_FACTOR.GetCount_2_Store())	{	this->POWER_FACTOR.NewCycle();	}
	  }


}

double Meter::GetLastEnergy(){
	return this->PREV_active_energy;
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
 * Debug output																									*
 *****************************************************************/
void debug_out(const String& text, const int level, const bool linebreak) {
	if (level <= cfg::debug) {

		if (linebreak) {
			Serial.println(text);
			TelnetStream.println(text);
		} else {
			Serial.print(text);
			TelnetStream.print(text);
		}
	}
}

/*****************************************************************
 * check display values, return '-' if undefined								 *
 *****************************************************************/
String check_display_value(double value, double undef, uint8_t digits, uint8_t str_len) {
	String s = (value != undef ? Float2String(value, digits, str_len) : "-");
	return s;
}

/*****************************************************************
 * convert value to json string																	*
 *****************************************************************/
String Value2Json(const String& type, const String& value) {
	String s = F("{\"value_type\":\"{t}\",\"value\":\"{v}\"},");
	s.replace("{t}", type);
	s.replace("{v}", value);
	return s;
}


/*****************************************************************
 * convert value to json string with timestamp and location																	*
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
 * convert string value to json string													 *
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
 * convert boolean value to json string													*
 *****************************************************************/
String Var2Json(const String& name, const bool value) {
	String s = F("\"{n}\":\"{v}\",");
	s.replace("{n}", name);
	s.replace("{v}", (value ? "true" : "false"));
	return s;
}

/*****************************************************************
 * convert integer value to json string													*
 *****************************************************************/
String Var2Json(const String& name, const int value) {
	String s = F("\"{n}\":\"{v}\",");
	s.replace("{n}", name);
	s.replace("{v}", String(value));
	return s;
}

/*****************************************************************
 * convert double value to json string													*
 *****************************************************************/
String Var2Json(const String& name, const double value) {
	String s = F("\"{n}\":\"{v}\",");
	s.replace("{n}", name);
	s.replace("{v}", String(value));
	return s;
}
