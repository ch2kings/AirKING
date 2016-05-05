/**************************************************************************/
/*!
    @file     PComm.h
	
	*/
/**************************************************************************/

#ifndef _PCOMM_H_
#define _PCOMM_H_

#include <string.h>
#include <Arduino.h>
#include <SPI.h>

#include "Pump.h"
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

/* -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         1
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/


class PComm {
protected:
	String lastCommand;
	float CommandVal;
	int Command;
	Pump thisPump;  //Pump Object
	Adafruit_BluefruitLE_SPI ble;
	
public:
	PComm(void);
	void newBLECommand(String);
	void newBLECommand(char *);
	bool readBLE(void);
	bool writeBLE(String);
	bool replyBLE(void);
	bool initBLE(void);
	void interpreter(void);
	int getCommand(void);
	float getCommandVal(void);
};


#endif /* _PPARSER_H_ */