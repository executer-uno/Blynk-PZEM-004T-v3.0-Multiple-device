/*
 * MyModbusMaster.h
 *
 *  Created on: Jun 5, 2020
 *      Author: E_CAD
 */

#ifndef MYMODBUSMASTER_H_
#define MYMODBUSMASTER_H_

#include <ModbusMaster.h>

class MyModbusMaster : public ModbusMaster{
public:

    void begin(uint8_t Slave, Stream &serial){
    	_u8MBSlave = Slave;
	    ModbusMaster::begin(Slave, serial);
    }

	uint8_t getSlaveID(){
		return _u8MBSlave;
	}

private:
	uint8_t _u8MBSlave;
};


#endif /* MYMODBUSMASTER_H_ */
