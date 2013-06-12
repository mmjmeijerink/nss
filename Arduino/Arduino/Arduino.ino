#include <SPI.h>
#include "printf.h"
#include "nRF24L01.h"
#include "RF24.h"

RF24	radio(3, 9); // Pin 3, 9, 11, 12, 13 worden door de radio gebruikt

///
/// Constants
///

typedef enum {POSSIGNAL = 0} type;

typedef struct {
	type	msgType;
} Broadcast;

// Grid node states
typedef enum {WAITING = 0, RADIORCV} state;

///
/// Variables
///

bool isGridNode = true;

// Moving node vars
int buzPin = 8;

// Grid node vars
state _state = WAITING;

int micPort = A0;
int average;

unsigned long radioRcvTime;
unsigned long soundRcvTime;

// Broadcast address
const uint64_t broadcastPipe = 0xABCD4567EFLL;


///
/// Userdefined functions
///

// Moving node methods
void sendLocSig() {
	// Radio Broadcast - Received by all grid-nodes at the same time
	Broadcast broadcast;
	broadcast.msgType = POSSIGNAL;
	radio.write(&broadcast, sizeof(broadcast));
	
	// Buzzer Signal - Received with a delay, delay between Radio and Buzzer -> Distance
	tone(buzPin, 2750, 20);
	delay(50);
	tone(buzPin, 2750, 20);
	delay(20);
	tone(buzPin, 2750, 20);
	delay(50);
	tone(buzPin, 2750, 20);
}

// Grid node methods
void checkSound() {
	unsigned long time = micros();
	int sound = analogRead(micPort);
	//Serial.println(map(sound - average, -128, 128, 0, 1024));
	
	if (sound - average < -17 || sound - average > 17) {
		soundRcvTime = time;
		printf("TravelTime = %lu  \n\r", radioRcvTime - soundRcvTime);
		digitalWrite(buzPin, HIGH);
		delay(30);
		digitalWrite(buzPin, LOW);
	}
	
	average = 0.95 * average + 0.05 * sound;
	delayMicroseconds(100);
	_state = WAITING;
}


///
/// setup() & loop()
///

void setup(void) {
	Serial.begin(19200);
	pinMode(buzPin, OUTPUT);
	
	// TODO: check Moving vs Grid node
	// Mogelijke implementatie met D2 verbonden met GND
	pinMode(2, INPUT);
	digitalWrite(2, HIGH);
	if (digitalRead(2)) {
		isGridNode = false;
	}
	
	// Prepare the radio
	radio.begin();
	radio.setChannel(107);
	radio.setPALevel(RF24_PA_MAX);
	//radio.setDataRate(RF24_2MBPS);
	radio.setRetries(5,0);
	radio.setPayloadSize(8);
	//radio.setAutoAck(false);
	
	if (!isGridNode) {
		radio.openWritingPipe(broadcastPipe);
	} else {
		radio.openReadingPipe(1, broadcastPipe);
		radio.startListening();
	}
	
	if (isGridNode) {
		for (int i = 0; i < 100; i++) {
			int sound = analogRead(micPort);
			average = 0.95 * average + 0.05 * sound;
			delayMicroseconds(100);
		}
	}
	
	printf_begin();
	radio.printDetails();
}

void loop(void) {
	if (!isGridNode) {
		sendLocSig();
	} else {
		if (radio.available()) {
			printf("Radio event processed \n\r");
			unsigned long time = micros();
			
			Broadcast msg;
			bool done = false;
			while (!done) {
				done = radio.read(&msg, sizeof(Broadcast));
			}
			if (msg.msgType == POSSIGNAL) {
				radioRcvTime = time;
				_state = RADIORCV;
			}
		}
		
		if (_state == RADIORCV && micros() - radioRcvTime > 750) {
			checkSound();
		}
	}
	
	delay(1000);
}
