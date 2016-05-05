/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include "PComm.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

/*=========================================================================
    APPLICATION SETTINGS

    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         1
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
	#define VBATPIN A9   // Pin with voltage devider from battery
	#define ENPIN 5   // Pin with EN
/*=========================================================================*/

  volatile byte blinkstate = LOW;
  bool sleepstate = false;
  PComm PumpComm;
  volatile bool watchdogActivated = false;
  int sleepIterations = 0;

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  while (!Serial);  // required for Flora & Micro
  delay(500);
  
  PumpComm.initBLE();
  pinMode(13, OUTPUT);
  blinkstate =0;
  attachInterrupt(7, testISR, CHANGE); 
}
void testISR(void) {
    // Echo received data
	digitalWrite(13, blinkstate);
	blinkstate = !blinkstate;
}
ISR(WDT_vect) // watchdog interrupt  routine
{
  // Set the watchdog activated flag.
  // Note that you shouldn't do much work inside an interrupt handler.
  watchdogActivated = true;
}

/**************************************************************************/
/*!
loop
*/
/**************************************************************************/
void loop(void)
{
  // Check for user input
    char n, inputs[BUFSIZE+1];
    if(!BattOK()){
		Serial.println("BAT not OK - go to sleep");
		BattLowSleep();
	}
	

  if (Serial.available())  // PC data available
  {
    n = Serial.readBytes(inputs, BUFSIZE); 
    inputs[n] = 0;
    // Send characters to Bluefruit
    Serial.print("Sending: ");
    Serial.println(inputs);

    // Send input data to host via Bluefruit
    //ble.print(inputs);
    PumpComm.writeBLE(inputs);
  }
  if(PumpComm.readBLE()) { //data via bluetooth to pump available
    PumpComm.replyBLE();
  }

}
bool BattOK(){
	int   BattValue = 0;
	BattValue= analogRead(BattCheckPin);
	float measuredvbat = analogRead(VBATPIN);
	measuredvbat *= 2; // we divided by 2, so multiply back
	measuredvbat *= 3.3; // Multiply by 3.3V, our reference voltage
	measuredvbat /= 1024; // convert to voltage
	Serial.println("VBat: " ); Serial.println(measuredvbat);
	if (measuredvbat < 3.3) 
		return false;
	else
		return true;
}
void BattLowSleep() {
	  
	  // Sleep Activation //
	  sleepstate = true;		// sleep flag
	  pinMode(ENPIN, OUTPUT);
	  Serial.print("going to sleep" )
	  pinMode(ENPIN, OUTPUT);	//Set EN to low to disable voltage regulator
	  digitalWrite(ENPIN, LOW) ; //connect EN pin to ENPIN output pin
	  
	  set_sleep_mode (SLEEP_MODE_PWR_DOWN);            //Sleep mode Selection//
	  sleep_enable();  
	  power_adc_disable();
	  power_twi_disable(); 
	  power_spi_disable();	  

	  sleep_cpu ();     //CPU is now sleeping
	  
	    // When awake, disable sleep mode and turn on all devices.
	  sleep_disable();
	  power_all_enable();
	  digitalWrite(ENPIN, HIGH) 
	  sleepstate = false;
	  Serial.println("wake up" )
  }
  void watchdogOn() {
  
// Clear the reset flag, the WDRF bit (bit 3) of MCUSR.
	MCUSR = MCUSR & B11110111;
	  
	// Set the WDCE bit (bit 4) and the WDE bit (bit 3) 
	// of WDTCSR. The WDCE bit must be set in order to 
	// change WDE or the watchdog prescalers. Setting the 
	// WDCE bit will allow updtaes to the prescalers and 
	// WDE for 4 clock cycles then it will be reset by 
	// hardware.
	WDTCSR = WDTCSR | B00011000; 

	// Set the watchdog timeout prescaler value to 1024 K 
	// which will yeild a time-out interval of about 8.0 s.
	WDTCSR = B00100001;

	// Enable the watchdog timer interupt.
	WDTCSR = WDTCSR | B01000000;
	MCUSR = MCUSR & B11110111;

}



