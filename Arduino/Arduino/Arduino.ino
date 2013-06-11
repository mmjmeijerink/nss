#include <SPI.h>
#include "printf.h"
#include "nRF24L01.h"
#include "RF24.h"

#define BUZPIN	2

///
/// Constants
///

const unsigned long	FREQUENCY = 500;


///
/// Variables
///

int				counter;
unsigned long	lastTime = 0;
unsigned long	buzTime = 50;
unsigned long	buzzerTurnedOn = 0;


///
/// Userdefined functions
///

// Buzzer functions
void buzBuzzer() {
	digitalWrite(BUZPIN, HIGH);
	buzzerTurnedOn = millis();
}

void checkBuzzerStatus() {
	if (millis() - buzzerTurnedOn >= buzTime) {
		tone(BUZPIN, 19, 1000);
	}
}

void adjustCounter() {
	// Raise the counter with the time that has past since last raise
	unsigned long currentTime = millis();
	int oldCounter = counter;
	counter += currentTime - lastTime;
	counter = counter % FREQUENCY;
	
	if (counter < oldCounter) {
		buzBuzzer();
	}
	
	lastTime = currentTime;
}


///
/// setup() & loop()
///

void setup(void) {
	//Serial.begin(57600);
	pinMode(BUZPIN, OUTPUT);
	digitalWrite(BUZPIN, LOW);
	
	/*/ Setup NodeId
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);
	pinMode(5, OUTPUT);
	digitalWrite(5, HIGH);
	pinMode(6, OUTPUT);
	digitalWrite(6, HIGH);
	pinMode(7, OUTPUT);
	digitalWrite(7, HIGH);
	
	byte Id = !digitalRead(4) * 8 + !digitalRead(5) * 4 + !digitalRead(6) * 2 + !digitalRead(7);
	node = new SynchronisedNode(Id, &radio, LEDPIN);
	
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
	*/
	
	adjustCounter();
}

void loop(void) {
	adjustCounter();
	checkBuzzerStatus();
}