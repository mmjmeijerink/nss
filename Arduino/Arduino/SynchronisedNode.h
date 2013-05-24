///
/// @file	SynchronisedNode.h
/// @brief	Class library header
/// @details	<#details#>
/// @n
/// @n @b	Project Arduino
/// @n @a	Developed with [embedXcode](http://embedXcode.weebly.com)
///
/// @author	Mart Meijerink
/// @author	Mart Meijerink
/// @date	15-05-13 14:26
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

#ifndef SynchronisedNode_h
#define SynchronisedNode_h

#include "Broadcast.h"
#include "RF24.h"

typedef enum {BROADCASTING = 0, QUIET, LISTENING} state;

const int	FREQUENCY	=	1000;
const int	BROADCASTS	=	5;
const int	TIMEOUTTIME	=	10 * FREQUENCY;

class SynchronisedNode {
private:
	int				_nodeID;
	state			_state;
	int				counter;
	int				broadcastTime;
	int				broadcastsSend;
	bool			broadcastDone;
	
	// Radio variables
	RF24*			_radio;
	
	// LED variables
	uint8_t			_ledPin;
	unsigned long	blinkTime;
	unsigned long	ledTurnedOn;
	
public:
	SynchronisedNode(int ID, RF24 *radio, uint8_t ledPin);
	
	// Getters & Setters
	int				getNodeID(void);
	state			getState(void);
	void			setState(state);
	unsigned long	getCounter(void);
	void			raiseCounter(unsigned long value);
	unsigned long	getFrequency(void);
	unsigned long	getTimeoutTime(void);
	unsigned long	getBroadcastTime(void);
	int				getBroadcastsSend(void);
	bool			getBroadcastDone(void);
	RF24*			getRadio(void);
	
	// LED functions
	void			blinkLed(void);
	void			checkLedStatus(void);
	
	// Broadcast functions
	void			sendBroadcast(void);
	void			handleBroadcast(Broadcast *msg);
};

#endif
