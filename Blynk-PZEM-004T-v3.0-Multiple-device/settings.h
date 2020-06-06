                           /////////////////////////////////////////////////////////////////
                          //                           Settings                          //
                         /////////////////////////////////////////////////////////////////
 



/***************************************************
 *        NodeMCU Pin Assignment
 **************************************************/

#define RX1_PIN_NODEMCU     D5        // Nodemcu pin, used has RX pin
#define TX1_PIN_NODEMCU     D6        // Nodemcu pin, used has TX pin
#define RX2_PIN_NODEMCU     D1        // Nodemcu pin, used has RX pin
#define TX2_PIN_NODEMCU     D2        // Nodemcu pin, used has TX pin
/***************************************************
 *        PZEM address value assignment 
 **************************************************/

 #define PZEM_SLAVE_1_ADDRESS     0x02    // Make sure you assign address to pzem first before you use
 #define PZEM_SLAVE_2_ADDRESS     0x03
 #define PZEM_SLAVE_3_ADDRESS     0x04
 #define PZEM_SLAVE_4_ADDRESS     0x05
 
/***************************************************
 *        Blynk Virtual Pin Assignment
 **************************************************/
 
#define vPIN_VOLTAGE_1                V11
#define vPIN_CURRENT_USAGE_1          V12
#define vPIN_ACTIVE_POWER_1           V13
#define vPIN_ACTIVE_ENERGY_1          V14
#define vPIN_FREQUENCY_1              V15
#define vPIN_POWER_FACTOR_1           V16
#define vPIN_OVER_POWER_ALARM_1       V17

#define vPIN_VOLTAGE_2                V21
#define vPIN_CURRENT_USAGE_2          V22
#define vPIN_ACTIVE_POWER_2           V23
#define vPIN_ACTIVE_ENERGY_2          V24
#define vPIN_FREQUENCY_2              V25
#define vPIN_POWER_FACTOR_2           V26
#define vPIN_OVER_POWER_ALARM_2       V27

/***************************************************
 *        Server Settings
 **************************************************/
      
#define OTA_HOSTNAME "PZEM-004T v3.0 Multiple Slave"

 
