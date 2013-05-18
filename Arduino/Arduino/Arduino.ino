#include <SPI.h>
#include "printf.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "SynchronisedNode.h"

#define LEDPIN	11

///
/// Instantiations
///

SynchronisedNode	node(random(10), LEDPIN);


///
/// Variables
///

// enums
typedef enum {ALL = 0, BROADCAST, TIMEOUT, LEDSTATUS}	interval;

// Interval variables
unsigned long	lastTime;
unsigned long	lastBroadcastReceived;


///
/// Userdefined functions
///

void adjustCounter() {
	// Raise the counter with the time that has past since last raise
	unsigned long currentTime = millis();
	node.raiseCounter(currentTime - lastTime);
	lastTime = currentTime;
}

void check() {
	adjustCounter();
	
	node.checkLedStatus();
	
	if (node.getState() == BROADCASTING && !node.getBroadcastDone() && node.getCounter() >= node.getBroadcastTime()) {
		node.sendBroadcast();
	}
}

void check(interval interval) {
	adjustCounter();
	
	switch (interval) {
		case ALL:
			check();
			break;
		case BROADCAST:
			if (node.getState() == BROADCASTING && !node.getBroadcastDone() && node.getCounter() >= node.getBroadcastTime()) {
				node.sendBroadcast();
			}
			break;
		case TIMEOUT:
			if (node.getState() == LISTENING && millis() - lastBroadcastReceived >= node.getTimeoutTime()) {
				node.setState(BROADCASTING);
			} else if (node.getState() == QUIET && millis() - lastBroadcastReceived >= 2 * node.getTimeoutTime()) {
				node.setState(BROADCASTING);
			}
			break;
		case LEDSTATUS:
			node.checkLedStatus();
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
	
	adjustCounter();
}

void loop(void) {
	check();
}
