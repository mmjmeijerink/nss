#include <SPI.h>
#include "printf.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "SynchronisedNode.h"
#include "Broadcast.h"

#define LEDPIN	11

///
/// Instantiations
///

RF24				radio(3, 9);
SynchronisedNode	node(random(10), &radio, LEDPIN);


///
/// Variables
///

// enums
typedef enum {
	ALL = 0,
	BROADCAST,
	TIMEOUT,
	LEDSTATUS
} interval;

// Broadcast address
const uint64_t broadcastPipe = 0xABCD4567EFLL;

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
	
	// BROADCAST
	if (node.getState() == BROADCASTING && !node.getBroadcastDone() && node.getCounter() >= node.getBroadcastTime()) {
		node.sendBroadcast();
	}
	
	if (node.getRadio()->available()) {
		Broadcast *broadcastMessage = 0;
		bool done = false;
		while (!done) {
			done = node.getRadio()->read(broadcastMessage, sizeof(Broadcast));
		}
		node.handleBroadcast(broadcastMessage);
	}
	
	// TIMEOUT
	if (node.getState() == LISTENING && millis() - lastBroadcastReceived >= node.getTimeoutTime()) {
		node.setState(BROADCASTING);
	} else if (node.getState() == QUIET && millis() - lastBroadcastReceived >= 2 * node.getTimeoutTime()) {
		node.setState(BROADCASTING);
	}
	
	// LEDSTATUS
	node.checkLedStatus();
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
			
			if (node.getRadio()->available()) {
				Broadcast *broadcastMessage = 0;
				bool done = false;
				while (!done) {
					done = node.getRadio()->read(broadcastMessage, sizeof(Broadcast));
				}
				node.handleBroadcast(broadcastMessage);
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
			check();
			break;
	}
}


///
/// setup() & loop()
///

void setup(void) {
	//Serial.begin(57600);
	pinMode(LEDPIN, OUTPUT);
	
	// Prepare the radio
	node.getRadio()->begin();
	node.getRadio()->setChannel(107);
	node.getRadio()->setPALevel(RF24_PA_MAX);
	//node.getRadio()->setDataRate(RF24_2MBPS);
	node.getRadio()->setRetries(5,0);
	node.getRadio()->setPayloadSize(8);
	
	node.getRadio()->openWritingPipe(broadcastPipe);
	node.getRadio()->openReadingPipe(1, broadcastPipe);
	node.getRadio()->startListening();
	
	adjustCounter();
}

void loop(void) {
	check();
}
