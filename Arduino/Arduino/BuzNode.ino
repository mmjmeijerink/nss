#include <SPI.h>
#include "printf.h"
#include "nRF24L01.h"
#include "RF24.h"

RF24	radio(3, 9); // Pin 3, 9, 11, 12, 13 worden door de radio gebruikt

///
/// Constants
///

typedef enum {POSITION = 0, DISTANCE} type;

typedef struct {
	type	msgType;
	int		nodeId;
	int		distance;
} Broadcast;

typedef struct {
	int		nodeId;
	int		currentDistance;
	int		lastDistance;
} GridNode;

// Grid node states
typedef enum {WAITING = 0, RADIORCV, CONFIRM} state;

///
/// Variables
///

bool isGridNode = true;

// Moving node vars
int buzPin = 7;
GridNode *gridNodes[15];

// Grid node vars
int nodeId;
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
	broadcast.msgType = POSITION;
	radio.stopListening();
	radio.write(&broadcast, sizeof(broadcast));
	
	// Buzzer Signal - Received with a delay, delay between Radio and Buzzer -> Distance
	tone(buzPin, 2750, 15);
	delay(15);
	tone(buzPin, 2750, 15);
	
	radio.startListening();
}

GridNode* getGridNode(int nodeId) {
	GridNode *result;
	
	bool found = false;
	for (int i = 0; i < sizeof(gridNodes)/sizeof(GridNode) && !found; i++) {
		if (gridNodes[i]->nodeId == nodeId) {
			result = gridNodes[i];
			found = true;
		}
	}
	
	if (!found) {
		GridNode *node;
		node->nodeId = nodeId;
		node->currentDistance = 0;
		node->lastDistance = 0;
		
		gridNodes[sizeof(gridNodes)/sizeof(GridNode)] = node;
		result = gridNodes[sizeof(gridNodes)/sizeof(GridNode) - 1];
	}
	
	return result;
}

void distanceToNode(int nodeId, int distance, unsigned long rcvTime) {
	GridNode *node = getGridNode(nodeId);
	
}

// Grid node methods
int distance(unsigned long travelTime) {
	return (travelTime/1000000) * 343;
}

void sendDistance(unsigned long travelTime) {
	Broadcast broadcast;
	broadcast.msgType = DISTANCE;
	broadcast.nodeId = nodeId;
	broadcast.distance = distance(travelTime);
	
	radio.stopListening();
	radio.write(&broadcast, sizeof(Broadcast));
	radio.startListening();
}

void checkSound() {
	unsigned long time = micros();
	int sound = analogRead(micPort);
	//Serial.println(map(sound - average, -128, 128, 0, 1024));
	
	if (sound - average < -20 || sound - average > 20) {
		if (_state == CONFIRM && time - soundRcvTime > 25000 && time - soundRcvTime < 50000) {
			sendDistance(soundRcvTime - radioRcvTime);
			printf("TravelTime = %lu  \n\r", soundRcvTime - radioRcvTime);
			digitalWrite(buzPin, HIGH);
			delay(30);
			digitalWrite(buzPin, LOW);
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
	pinMode(buzPin, OUTPUT);
	
	pinMode(2, INPUT);
	digitalWrite(2, HIGH);
	if (!digitalRead(2)) {
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
		//TODO: bereken/geef nodeId
		
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
	sendLocSig();
	unsigned long rcvTime = micros();

	if (radio.available()) {
		Broadcast msg;
		radio.read(&msg, sizeof(Broadcast));

		distanceToNode(msg.nodeId, msg.distance, rcvTime);
	}

	delay(2000);
}

