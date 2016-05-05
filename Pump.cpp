/**************************************************************************/
/*!
    @file     Pump.cpp
	
*/
/**************************************************************************/
#include "Pump.h"
#include <EEPROM.h>

/******************************************************************************/
/*!
    @brief  Constructor
*/
/******************************************************************************/
Pump::Pump(void)
{
	pressureHysteresis = DEFAULT_HYSTERESIS;
	eeAddress = DEFAULT_EEADDRESS;
	_mode = DEFAULT_MODE;
	loadTargetPressure();
	actualPressure = 0.1;
	actualTemperature = 0.1;
	valvePin = PIN_VALVE;
	pumpPin = PIN_PUMP;
	pinMode(valvePin, OUTPUT);  
	pinMode(pumpPin, OUTPUT); 
	pumpstarttime = 0;
	valvestarttime = 0;
	totalpumptime = 0;
	totalvalvetime = 0;
}

float Pump::getTargetPressure(void){
	return targetPressure;
}

void Pump::storeTargetPressure(float f){
	EEPROM.put(eeAddress, f);
}

void Pump::loadTargetPressure(void){
	float f = 0.00f;   //Variable to store data read from EEPROM.
	EEPROM.get(eeAddress, f);
	
	if (f < 1) {
		targetPressure = DEFAULT_PRESSURE;  
	}
	else {
		targetPressure = f;
	}
}
float Pump::readSensor(bool b) {
	if ( b ) { // pressure psi
		return 15;
	} else { // temperature
		return 25;
	}
	return 0;
}

// messure Pressure or Temp
float Pump::getActualPressure(void){
	if (_mode == 0) {  // valve and pump disabled
	// connect to I2C sensor and get values
		actualPressure = readSensor (true);
	}
	if (_mode == 1) 
	{
	// halt pump
		actualPressure = readSensor (true);
	}
	if (_mode == 2) // valve open
	{
	// halt valve
		closeValve();
		delay(50);
		actualPressure = readSensor (true);
		delay(50);
		openValve();
	}
	return actualPressure;
};
float Pump::getPressure(void){
	return actualPressure;
};
float Pump::getTemperature(void) {
	return actualTemperature;
}
void Pump::setTargetPressure(float f){
	targetPressure = f;
	storeTargetPressure(f);
}

void Pump::setMode(uint8_t mode){
	_mode = mode;
}
/******************************************************************************/
/*!
    ACTION...
*/
/******************************************************************************/
void Pump::openValve(void){
	valvestarttime = millis();
	digitalWrite(valvePin, HIGH); 
	_mode = 2;
}
void Pump::startPump(void){
	pumpstarttime = millis();
	digitalWrite(pumpPin, HIGH); 
	_mode = 1;
}
unsigned long Pump::closeValve(void){
	totalvalvetime = totalvalvetime + millis() - valvestarttime; // calculate total used valve time
	digitalWrite(valvePin, LOW);
	_mode = 0;
	return millis() - valvestarttime;
}
unsigned long Pump::stopPump(void){
	totalpumptime = totalpumptime + millis() - valvestarttime;
	digitalWrite(pumpPin, LOW); 
	_mode = 0;
	return millis()- pumpstarttime;
}
unsigned long Pump::adjustPressure(uint8_t mode){
	float pact = getActualPressure();
	float pdiff = pact - targetPressure;
	if (abs(pdiff) > pressureHysteresis && _mode == 0) {	// pump and valve inactive, target value outside boundaries
		if (pdiff > 0) { // overpressure	
			openValve(); 
			return 0;
		} else			// underpressure
		{
			startPump();
			return 0; 
		}
	} 
	else if (abs(pdiff) <= pressureHysteresis && _mode == 1 ) { // target pressure value reached
		stopPump();
		return 0;
	}
	else if (abs(pdiff) <= pressureHysteresis && _mode == 2 ) { // target pressure value reached
		closeValve();
		return 0;
	}
	return 0;
}
