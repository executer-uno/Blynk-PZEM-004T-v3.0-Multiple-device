/*
 * Definitions.cpp
 *
 *  Created on: Mar 23, 2020
 *      Author: E_CAD
 */

#include <Arduino.h>


#include "Definitions.h"



namespace cfg {
	extern int	debug;
}


void measurement::NewMeas(float Measure){

	this->count++;
	this->sum += Measure;

	this->Measurements.avg = this->sum / this->count;
	this->Measurements.max = (this->Measurements.max < Measure ? Measure : this->Measurements.max);
	this->Measurements.min = (this->Measurements.min > Measure ? Measure : this->Measurements.min);
}

void measurement::Clear(){

	// prepare for next measurements
	this->count  =0;
	this->sum 	 =0;

	this->Measurements.avg = NAN;
	this->Measurements.max = -99999999999.9;
	this->Measurements.min = +99999999999.9;
}
measurement::measurement(){

	this->Clear();
}
String measurement::DebugAvg(){
	String strDebug = "";
	if(this->count > 0){
		strDebug = Float2String(this->Measurements.avg,1 , 7);
	}
	return strDebug;
}
String measurement::DebugRange(){
	String strDebug = "";
	if(this->count > 0){
		strDebug  = Float2String(this->Measurements.min,1 , 7) + " : ";
		strDebug += Float2String(this->Measurements.max,1 , 7);
	}
	return strDebug;
}
String measurement::GetJson(){

	String data = this->Measurements.min + String(":") + this->Measurements.avg + String(":") + this->Measurements.max;

	return data;
}
uint32_t measurement::GetCount(){

	return this->count;
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
Meter::Meter(){
	this->Clear();
}
void Meter::CRCError(){
	this->CRCerr++;
}
String Meter::GetJson(){
	String data = "{";

	data += Var2Json(F("VOLT"),		this->VOLTAGE.GetJson()			);
	data += Var2Json(F("CURR"),		this->CURRENT_USAGE.GetJson()	);
	data += Var2Json(F("POWR"),		this->ACTIVE_POWER.GetJson()	);
	data += Var2Json(F("ENRG"),		this->ACTIVE_ENERGY.GetJson()	);
	data += Var2Json(F("FREQ"),		this->FREQUENCY.GetJson()		);
	data += Var2Json(F("POWF"),		this->POWER_FACTOR.GetJson()	);

	data += Var2Json(F("MCNT"),		(double)this->VOLTAGE.GetCount());

	data += "}";

	return data;
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
		} else {
			Serial.print(text);
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
