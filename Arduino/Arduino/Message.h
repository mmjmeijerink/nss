///
/// @file	Message.h
/// @brief	Class library header
/// @details	<#details#>
/// @n
/// @n @b	Project Arduino
/// @n @a	Developed with [embedXcode](http://embedXcode.weebly.com)
///
/// @author	Mart Meijerink
/// @author	Mart Meijerink
/// @date	18-05-13 01:33
/// @version	<#version#>
///
/// @copyright	© Mart Meijerink, 2013
/// @copyright	CC = BY NC SA
///
/// @see	ReadMe.txt for references
///


// Core library - IDE-based
#if defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(DIGISPARK) // Digispark specific
#include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
#include "Energia.h"
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#elif defined(CORE_TEENSY) // Teensy specific
#include "WProgram.h"
#elif defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(ARDUINO) && (ARDUINO >= 100) // Arduino 1.0x and 1.5x specific
#include "Arduino.h"
#elif defined(ARDUINO) && (ARDUINO < 100)  // Arduino 23 specific
#include "WProgram.h"
#endif // end IDE

#ifndef Message_h
#define Message_h

class Message {
private:
	int				_nodeID;
	unsigned long	_broadcastTime;
	bool			_isLastBroadcast;
  
public:
	Message(int nodeID, unsigned long broadcastTime, bool isLastBroadcast);
	int				getNodeID(void);
	unsigned long	getBroadcastTime(void);
	bool			isLastBroadcast(void);
};

#endif