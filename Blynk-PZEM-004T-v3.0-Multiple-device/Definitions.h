/*
 * Definitions.h
 *
 *  Created on: Mar 17, 2020
 *      Author: E_CAD
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_


	/***************************************************
	 *        NodeMCU Pin Assignment
	 **************************************************/
	#define RX1_PIN_NODEMCU     D5        // Nodemcu pin, used has RX pin
	#define TX1_PIN_NODEMCU     D6        // Nodemcu pin, used has TX pin

	/***************************************************
	 *        PZEM address value assignment
	 **************************************************/
	 #define PZEM_SLAVE_1_ADDRESS     0x02    // Make sure you assign address to pzem first before you use
	 #define PZEM_SLAVE_2_ADDRESS     0x03
	 #define PZEM_SLAVE_3_ADDRESS     0x04
	 #define PZEM_SLAVE_4_ADDRESS     0x05

	/***************************************************
	 *        Server Settings
	 **************************************************/
	#define OTA_HOSTNAME "PZEM-004T v3.0 Multiple Slave"


	// Build in LED
	#define LED_BUILTIN 2

	// Definition der Debuglevel
	#define DEBUG_ALWAYS 	0
	#define DEBUG_ERROR 	1
	#define DEBUG_WARNING 	2
	#define DEBUG_MIN_INFO 	3
	#define DEBUG_MED_INFO 	4
	#define DEBUG_MAX_INFO 	5
	// Wieviele Informationen sollenber die serielle Schnittstelle ausgegeben werden?
	#define DEBUG DEBUG_MED_INFO

	struct measure {
		float 	max;
		float 	min;
		float 	avg;
	};

	// Single sensor measurements object
	class measurement {
	private:
		uint32_t 		count; 		// number of measurements in accumulator
		float 			sum; 		// summ of measurements in accumulator
	public:
		struct measure	Measurements;
		void NewMeas(float Measure); 	// Add new measurement to accumulator
		void Clear();					// Push measured values to archive
		String DebugAvg();				// Returns actual average value for debug
		String DebugRange();			// Returns string in MIN:MAX format
		String GetJson();
		measurement();
	};

	// Electricity meter
	class Meter {
	private:
		uint32_t 		CRCerr;		// number of CRC errors
	public:
		float			CRCerrRate = 0.0;	// Rate of CRC errors (%)
		measurement		VOLTAGE;
		measurement		CURRENT_USAGE;
		measurement		ACTIVE_POWER;
		measurement		ACTIVE_ENERGY;
		measurement		FREQUENCY;
		measurement		POWER_FACTOR;

		void Clear();					// Push measured values to archive
		void CRCError();				// Increase CRC error counter
		String DebugCRC();				// Returns CRC errors rate and measurements count
		String GetJson();
		Meter();
	};




	String Float2String(const double value);
	String Float2String(const double value, uint8_t digits);
	String Float2String(const double value, uint8_t digits, uint8_t size);
	String check_display_value(double value, double undef, uint8_t len, uint8_t str_len);

	String Var2Json(const String& name, const String& value);
	String Var2Json(const String& name, const bool value);
	String Var2Json(const String& name, const int value);
	String Var2Json(const String& name, const double value);
	String ValueLocated2Json(const String& timestamp, const String& lat, const String& lng, const String& value);
	void debug_out(const String& text, const int level, const bool linebreak);

#endif /* DEFINITIONS_H_ */
