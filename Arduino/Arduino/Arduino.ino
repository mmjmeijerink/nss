#include <SPI.h>
#include "printf.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "SynchronisedNode.h"
#include "Broadcast.h"

#define LEDPIN	2

///
/// Instantiations
///

RF24				radio(3, 9); // Pin 3, 9, 11, 12, 13 worden door de radio gebruikt
SynchronisedNode	*node = 0;


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
unsigned long	lastTime = 0;
unsigned long	lastBroadcastReceived = 0;


///
/// Userdefined functions
///

void adjustCounter() {
	// Raise the counter with the time that has past since last raise
	unsigned long currentTime = millis();
	node->raiseCounter(currentTime - lastTime);
	lastTime = currentTime;
}

void check() {
	adjustCounter();
	
	// BROADCAST
	if (node->getState() == BROADCASTING && !node->getBroadcastDone() && node->getCounter() >= node->getBroadcastTime()) {
		node->sendBroadcast();
		lastBroadcastReceived = millis();
	}
	
	if (node->getRadio()->available()) {
		Broadcast *broadcastMessage = 0;
		bool done = false;
		while (!done) {
			done = node->getRadio()->read(broadcastMessage, sizeof(Broadcast));
		}
		node->handleBroadcast(broadcastMessage);
		lastBroadcastReceived = millis();
	}
	
	// TIMEOUT
	if (node->getState() == LISTENING && millis() - lastBroadcastReceived >= node->getTimeoutTime()) {
		node->setState(BROADCASTING);
	} else if (node->getState() == QUIET && millis() - lastBroadcastReceived >= 2 * node->getTimeoutTime()) {
		node->setState(BROADCASTING);
	}
	
	// LEDSTATUS
	node->checkLedStatus();
}
/*
void check(interval interval) {
	adjustCounter();
	
	switch (interval) {
		case ALL:
			check();
			break;
		case BROADCAST:
			if (node->getState() == BROADCASTING && !node->getBroadcastDone() && node->getCounter() >= node->getBroadcastTime()) {
				node->sendBroadcast();
			}
			
			if (node->getRadio()->available()) {
				Broadcast *broadcastMessage = 0;
				bool done = false;
				while (!done) {
					done = node->getRadio()->read(broadcastMessage, sizeof(Broadcast));
				}
				node->handleBroadcast(broadcastMessage);
				lastBroadcastReceived = millis();
			}
			break;
		case TIMEOUT:
			if (node->getState() == LISTENING && millis() - lastBroadcastReceived >= node->getTimeoutTime()) {
				node->setState(BROADCASTING);
			} else if (node->getState() == QUIET && millis() - lastBroadcastReceived >= 2 * node->getTimeoutTime()) {
				node->setState(BROADCASTING);
			}
			break;
		case LEDSTATUS:
			node->checkLedStatus();
			break;
		
		default:
			check();
			break;
	}
}
*/

void printDebugInfo() {
	printf("NodeID: %d \n\r", node->getNodeID());
	printf("Counter value: %d \n\r", node->getCounter());
	printf("State: %d \n\r\n", node->getState());
	
	printf("BroadcastTime: %u \n\r", node->getBroadcastTime());
	printf("millis() - lastBroadcastReceived >= node->getTimeoutTime() == %lu - %lu >= %lu  \n\r\n", millis(), lastBroadcastReceived, node->getTimeoutTime());
}


///
/// setup() & loop()
///

void setup(void) {
	Serial.begin(57600);
	pinMode(LEDPIN, OUTPUT);
	digitalWrite(LEDPIN, LOW);
	
	// Setup NodeID
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);
	pinMode(5, OUTPUT);
	digitalWrite(5, HIGH);
	pinMode(6, OUTPUT);
	digitalWrite(6, HIGH);
	pinMode(7, OUTPUT);
	digitalWrite(7, HIGH);
	
	int ID = !digitalRead(4)*8 + !digitalRead(5)*4 + !digitalRead(6)*2 + !digitalRead(7)*1;
	node = new SynchronisedNode(ID, &radio, LEDPIN);
	
	// Prepare the radio
	node->getRadio()->begin();
	node->getRadio()->setChannel(107);
	node->getRadio()->setPALevel(RF24_PA_MAX);
	//node->getRadio()->setDataRate(RF24_2MBPS);
	node->getRadio()->setRetries(5,0);
	node->getRadio()->setPayloadSize(8);
	
	node->getRadio()->openWritingPipe(broadcastPipe);
	node->getRadio()->openReadingPipe(1, broadcastPipe);
	node->getRadio()->startListening();
	
	printf_begin();
	node->getRadio()->printDetails();
	printDebugInfo();
	
	adjustCounter();
}

void loop(void) {
	check();
	//if (node->getCounter() % 1000 == 0) printDebugInfo();
}
