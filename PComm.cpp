/**************************************************************************/
/*!
    @file     PComm.cpp
	
	*/
/**************************************************************************/

#include "PComm.h"


PComm::PComm(void) : ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST)  // member initialization
{
	lastCommand ="";
	CommandVal = 0.00;
	Command = 0; 
/*	
	0: idle
	1: set new target pressure
	2: get sensor pressure
	3: get sensor temp
	4: get sensor voltage
	5: get mode
*/
	// Create the bluefruit object
	/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
	

}
void PComm::newBLECommand(String str) {
	
}
void PComm::newBLECommand(char * str) {
	
}
// A small helper

int PComm::getCommand(void){
	return Command;
}
float PComm::getCommandVal(void){
	return CommandVal;
}
// configure BLE
bool PComm::initBLE(void) {
/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
	  Serial.begin(115200);
	  Serial.println(F("Adafruit Bluefruit Command <-> Data Mode Example"));
	  Serial.println(F("------------------------------------------------"));

	  /* Initialise the module */
	  Serial.print(F("Initialising the Bluefruit LE module: "));

	  if ( !ble.begin(VERBOSE_MODE) )
	  {
		   Serial.println(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
	  }
	  Serial.println( F("OK!") );

	  if ( FACTORYRESET_ENABLE )
	  {
  		/* Perform a factory reset to make sure everything is in a known state */
  		Serial.println(F("Performing a factory reset: "));
  		if ( ! ble.factoryReset() ){
  		  Serial.println("Couldn't factory reset");
		  }
	  }

	  /* Disable command echo from Bluefruit */
	  ble.echo(false);

	  Serial.println("Requesting Bluefruit info:");
	  /* Print Bluefruit information */
	  ble.info();

	  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
	  Serial.println(F("Then Enter characters to send to Bluefruit"));
	  Serial.println();

	  ble.verbose(false);  // debug info is a little annoying after this point!

	  /* Wait for connection */
	  while (! ble.isConnected()) {
		  delay(500);
	  }

	  Serial.println(F("******************************"));

	  // LED Activity command is only supported from 0.6.6
	  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
	  {
  		// Change Mode LED Activity
  		Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
  		ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
	  }

	  // Set module to DATA mode
	  Serial.println( F("Switching to DATA mode!") );
	  ble.setMode(BLUEFRUIT_MODE_DATA);

	  Serial.println(F("******************************"));
	
	return false;
}

void PComm::interpreter(void){
// SET<151>
// GET<2>

	String str, val;
  int istart, iend;
	if (lastCommand.startsWith("SET")){
  		Command = 1;
  		istart = lastCommand.indexOf('<');
  		iend = lastCommand.indexOf('>');
  		val = lastCommand.substring(istart+1, iend);
  		CommandVal = val.toFloat();	// target pressure
      str = "PComm::interpreter: SET: "+String(CommandVal, 2);
      Serial.println(str);
	} else if (lastCommand.startsWith("GET")){
  		istart = lastCommand.indexOf('<');
  		iend = lastCommand.indexOf('>');
  		val = lastCommand.substring(istart+1, iend);
  		Command = val.toInt();
      Serial.println("PComm::interpreter: GET: "+val);
	}
 
 return;
}

bool PComm::readBLE() {
	char n, inputs[BUFSIZE+1];
	String msg = ""; //the message starts empty
	byte ch; 		// the character that you use to construct the Message 
	byte d='#';	// the separating symbol 
	
  // Echo received data
	if (ble.available()) {
		while ( ble.available() )
		{
			int c = ble.read();

//			Serial.print((char)c);

			// Hex output too, helps w/debugging!
	//		Serial.print(" [0x");
	//		if (c <= 0xF) Serial.print(F("0"));
	//		Serial.print(c, HEX);
	//		Serial.print("] ");
			if (c == d) {
				lastCommand = msg;
				interpreter();
    //    Serial.println(msg);
				return true;
			} 
			else
				msg += (char)c;	//add the character to the message
			
		}
	} else
	return false; // return "false" if no message;
	

// SET<151>#
  /*
  if(Serial.available())// checks if there is a new message;
  {
    while(Serial.available() && Serial.peek()!=d)// while the message did not finish
    {
      ch = Serial.read();// get the character
      msg += (char)ch;	//add the character to the message
    }
  ch = Serial.read();	// pop the '#' from the buffer
  if(ch==d) { // id finished
	lastCommand = msg;
	interpreter();
	return true;
	}
  else
	return false;
  }
  else
	return false; // return "false" if no message;
	*/
}
bool PComm::writeBLE(String str) {
	Serial.println("PComm::writeBLE: " + str);
  ble.print("PComm::writeBLE: " + str);
	return false;
}
bool PComm::replyBLE() {
  Serial.println("PComm::replyBLE: ");
	String str;
	switch ( Command ) {
		case 1: // set pressure
			writeBLE(lastCommand); 
			thisPump.Pump::setTargetPressure(CommandVal);
			break;
		case 2: // get pressure
			str = String("GET PRESSURE"); 
			writeBLE(lastCommand); 
			writeBLE(str); 
			str = String(thisPump.getPressure());
			writeBLE(str);
			break;
		case 3: //get temp
			str= String("GET TEMPERATURE");
			writeBLE(lastCommand); 
			writeBLE(str); 
			str = String(thisPump.getTemperature());
			writeBLE(str);
			break;
		case 4: // get voltage
			#define VBATPIN A9
			float measuredvbat = analogRead(VBATPIN);
			measuredvbat *= 2; // we divided by 2, so multiply back
			measuredvbat *= 3.3; // Multiply by 3.3V, our reference voltage
			measuredvbat /= 1024; // convert to voltage
			Serial.print("VBat: " ); Serial.println(measuredvbat);
			str = String( measuredvbat);
			writeBLE(str); break;
		case 5: // get mode
			writeBLE(lastCommand); break;
		case 6: // turn off
			writeBLE(lastCommand); 
			
			break;
		default: break;
		}
		
	return false;
}

