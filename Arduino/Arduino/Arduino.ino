#include <SPI.h>
#include "printf.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "SynchronisedNode.h"

#define LEDPIN 11

///
/// Instantiations
///

SynchronisedNode	node(random(10), LEDPIN);


///
/// Variables
///

// enums
typedef enum {NOTHING = -1, ALL, BROADCAST}	interval;

// Interval variables
unsigned long	lastTime;


///
/// Userdefined functions
///

void check(interval interval) {
	// Raise the counter with the time that has past since last raise
	unsigned long currentTime = millis();
	node.raiseCounter(currentTime - lastTime);
	lastTime = currentTime;
	
	switch (interval) {
		case ALL:
			node.checkLedStatus();
			
			if (!node.getBroadcastDone() && node.getCounter() >= node.getBroadcastTime()) {
				node.sendBroadcast();
			}
			break;
		case BROADCAST:
			if (!node.getBroadcastDone() && node.getCounter() >= node.getBroadcastTime()) {
				node.sendBroadcast();
			}
			break;
		
		default:
			break;
	}
}


///
/// setup() & loop()
///

void setup(void) {
	//Serial.begin(57600);
	pinMode(LEDPIN, OUTPUT);
	
	check(NOTHING);
}

void loop(void) {
	check(ALL);
}
