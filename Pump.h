/**************************************************************************/
/*!
    @file     Pump.h
	
	*/
/**************************************************************************/

#ifndef _PUMP_H_
#define _PUMP_H_

#include <stdint.h>
#include <Arduino.h>

#define DEFAULT_PRESSURE 15 //psi default pressure if no value stored
#define DEFAULT_HYSTERESIS 1 // psi default hysteresis
#define DEFAULT_EEADDRESS 0
#define DEFAULT_MODE 0   /* 
mode 0: idle 
mode 1: pump 
mode 2: depressure 
mode 3: auto
*/
#define PIN_PUMP 13
#define PIN_VALVE 12 

class Pump
{
protected:
    uint8_t  _mode;
	uint8_t valvePin;
	uint8_t pumpPin;
	float targetPressure;
	float targetTemperature;
	float pressureHysteresis;
	float actualPressure;
	float actualTemperature;
	unsigned long pumpstarttime;
	unsigned long valvestarttime;
	unsigned long totalpumptime;
	unsigned long totalvalvetime;
	int eeAddress; //EEPROM address to start reading from
public:
    // Constructor
    Pump(void);	
	
	float getTargetPressure(void);
	void storeTargetPressure(float);
	void loadTargetPressure(void);
	float getActualPressure(void);
	float getPressure(void);
	float getTemperature(void);
	void setTargetPressure(float);
	void setMode(uint8_t);
	unsigned long adjustPressure(uint8_t);
	void openValve(void);
	void startPump(void);
	unsigned long closeValve(void);
	unsigned long stopPump(void);
	float readSensor(bool);
};


#endif /* _PUMP_H_ */