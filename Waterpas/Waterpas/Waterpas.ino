/// 
/// @mainpage	Waterpas 
///
/// @details	<#details#>
/// @n 
/// @n 
/// @n @a	Developed with [embedXcode](http://embedXcode.weebly.com)
/// 
/// @author	Mart Meijerink and Max Kerkers
/// @author	Mart Meijerink and Max Kerkers
/// @date	25-04-13 14:54
/// @version	<#version#>
/// 
/// @copyright	© Mart Meijerink, 2013
/// @copyright	CC = BY NC SA
///
/// @see	ReadMe.txt for references
///


///
/// @file	Waterpas.ino 
/// @brief	Main sketch
///
/// @details	<#details#>
/// @n @a	Developed with [embedXcode](http://embedXcode.weebly.com)
/// 
/// @author	Mart Meijerink and Max Kerkers
/// @author	Mart Meijerink and Max Kerkers
/// @date	25-04-13 14:54
/// @version	<#version#>
/// 
/// @copyright	© Mart Meijerink and Max Kerkers, 2013
/// @copyright	CC = BY NC SA
///
/// @see	ReadMe.txt for references
/// @n
///


// Core library for code-sense
#if defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"   
#elif defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(DIGISPARK) // Digispark specific
#include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
#include "Energia.h"
#elif defined(CORE_TEENSY) // Teensy specific
#include "WProgram.h"
#elif defined(ARDUINO) && (ARDUINO >= 100) // Arduino 1.0 and 1.5 specific
#include "Arduino.h"
#elif defined(ARDUINO) && (ARDUINO < 100) // Arduino 23 specific
#include "WProgram.h"
#else // error
#error Platform not defined
#endif

// Include application, user and local libraries
#include "LocalLibrary.h"

// Define variables and constants
///
/// @brief	Name of the LED
/// @details	Each board has a LED but connected to a different pin
///
uint8_t greenLED, lightSensor;


///
/// @brief	Setup
/// @details	Define the pin the LED is connected to
///
// Add setup code
void setup() {
	greenLED = 11;
	Serial.begin(9600);
	
	pinMode(greenLED, OUTPUT);
}

///
/// @brief	Loop
/// @details	Call blink
///
// Add loop code 
void loop() {
	int sensorValue = analogRead(A0);
	analogWrite(greenLED, map(sensorValue, 0, 1023, 0, 255));
}
