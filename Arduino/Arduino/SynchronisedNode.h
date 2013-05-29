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

#include "RF24.h"

typedef enum {BROADCASTING = 0, QUIET, LISTENING} state;

typedef struct {
	int		nodeId;
	int		broadcastTime;
	bool	isLastBroadcast;
} Broadcast;

const unsigned long	FREQUENCY	=	2000;
const int			BROADCASTS	=	5;
const int			TIMEOUTTIME	=	5 * FREQUENCY;

class SynchronisedNode {
private:
	int				_nodeId;
	state			_state;
	int				counter;
	int				broadcastTime;
	int				broadcastsSend;
	bool			broadcastDone;
	
	// Radio variables
	RF24*	_radio;
	
	// LED variables
	uint8_t			_ledPin;
	int				blinkTime;
	unsigned long	ledTurnedOn;
	
public:
	SynchronisedNode(int Id, RF24 *radio, uint8_t ledPin);
	
	// Getters & Setters
	int				getNodeId(void);
	state			getState(void);
	void			setState(state);
	int				getCounter(void);
	void			raiseCounter(int value);
	int				getFrequency(void);
	int				getTimeoutTime(void);
	int				getBroadcastTime(void);
	int				getBroadcastsSend(void);
	bool			getBroadcastDone(void);
	RF24*			getRadio(void);
	
	// LED functions
	void			blinkLed(void);
	void			checkLedStatus(void);
	
	// Broadcast functions
	void			sendBroadcast(void);
	void			handleBroadcast(Broadcast msg);
};

#endif
