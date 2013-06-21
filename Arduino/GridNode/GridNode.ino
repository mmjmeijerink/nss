#include <SPI.h>
#include "printf.h"
#include "nRF24L01.h"
#include "RF24.h"

#define NODEID 1

RF24	radio(3, 9); // Pin 3, 9, 11, 12, 13 worden door de radio gebruikt

///
/// Constants
///

typedef struct {
	unsigned short	counter;
	unsigned int	nodeId;
	unsigned int	distance;
} Message;

// Grid node states
typedef enum {WAITING = 0, RADIORCV, CONFIRM} state;

///
/// Variables
///

int ledPin = 10;
int nodeId = NODEID;
int micPort = A0;
int average;
state _state = WAITING;

unsigned long	radioRcvTime;
unsigned long	soundRcvTime;
unsigned int	lastCounter;

// Pipes
const uint64_t pipes[2] = {0xABCD4567EFLL, 0xFE7654DCBALL};


///
/// Userdefined functions
///

// Grid node methods
int distance(unsigned long travelTime) {
	return (travelTime/1000000) * 343;
}

void sendDistance(unsigned long travelTime) {
	Message msg;
	msg.counter = lastCounter;
	msg.nodeId = nodeId;
	msg.distance = distance(travelTime);
	
	radio.stopListening();
	radio.write(&msg, sizeof(Message));
	radio.startListening();
}

void checkSound() {
	unsigned long time = micros();
	int sound = analogRead(micPort);
	
	if (sound - average < -20 || sound - average > 20) {
		if (_state == CONFIRM && time - soundRcvTime > 25000 && time - soundRcvTime < 50000) {
			sendDistance(soundRcvTime - radioRcvTime);
			printf("TravelTime = %lu  \n\r", soundRcvTime - radioRcvTime);
			digitalWrite(ledPin, HIGH);
			delay(30);
			digitalWrite(ledPin, LOW);
			_state = WAITING;
		} else {
			soundRcvTime = time;
			_state = CONFIRM;
		}
	}
	
	average = 0.95 * average + 0.05 * sound;
	delayMicroseconds(100);
}


///
/// setup() & loop()
///

void setup(void) {
	Serial.begin(19200);
	pinMode(ledPin, OUTPUT);
	
	// Prepare the radio
	radio.begin();
	radio.setChannel(107);
	radio.setPALevel(RF24_PA_MAX);
	//radio.setDataRate(RF24_2MBPS);
	radio.setRetries(5,0);
	radio.setPayloadSize(8);
	//radio.setAutoAck(false);
	
	radio.openWritingPipe(pipes[1]);
	radio.openReadingPipe(1, pipes[0]);
	radio.startListening();
	
	//TODO: bereken/geef nodeId
	for (int i = 0; i < 100; i++) {
		int sound = analogRead(micPort);
		average = 0.95 * average + 0.05 * sound;
		delayMicroseconds(100);
	}
	
	printf_begin();
	radio.printDetails();
}

void loop(void) {
	if (_state == WAITING) {
		unsigned long time = micros();
		if (radio.available()) {
			Message msg;
			radio.read(&msg, sizeof(Message));
			
			lastCounter = msg.counter;
			radioRcvTime = time;
			_state = RADIORCV;
		}
	} else {
		checkSound();
		
		if (micros() - radioRcvTime > 100000) {
			_state == WAITING;
		}
	}
}

