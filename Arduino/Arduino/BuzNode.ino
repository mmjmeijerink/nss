#include <SPI.h>
#include "printf.h"
#include "nRF24L01.h"
#include "RF24.h"

#define GRIDNODES	9
#define AVGOVER	10

RF24	radio(3, 9); // Pin 3, 9, 11, 12, 13 worden door de radio gebruikt

///
/// Constants
///

typedef struct {
	unsigned short	counter;
	unsigned int	nodeId;
	unsigned int	distance;
} Message;

typedef struct {
	int				nodeId;
	int				currentDistance;
	int				lastDistance;
} GridNode;

///
/// Variables
///

bool isGridNode = true;

// Moving node vars
int				buzPin = 10;
unsigned short	messageCounter = 0;

GridNode*		gridNodes[GRIDNODES];
int				gridNodesInArray = 0;

Message*		distances[AVGOVER][GRIDNODES];

// Pipes
const uint64_t pipes[2] = {0xABCD4567EFLL, 0xFE7654DCBALL};


///
/// Userdefined functions
///

// Moving node methods
void sendLocSig() {
	// Radio Broadcast - Received by all grid-nodes at the same time
	Message broadcast;
	broadcast.counter = messageCounter;
	
	radio.stopListening();
	radio.write(&broadcast, sizeof(broadcast));
	
	// Buzzer Signal - Received with a delay, delay between Radio and Buzzer -> Distance
	tone(buzPin, 2750, 15);
	delay(15);
	tone(buzPin, 2750, 15);
	
	messageCounter++;
	radio.startListening();
}

GridNode* getGridNode(int nodeId) {
	GridNode *result;
	
	bool found = false;
	for (int i = 0; i < gridNodesInArray && !found; i++) {
		if (gridNodes[i]->nodeId == nodeId) {
			result = gridNodes[i];
			found = true;
		}
	}
	
	// GridNode not in array (not seen before), so add to array
	if (!found) {
		GridNode *node;
		node->nodeId = nodeId;
		node->currentDistance = 0;
		node->lastDistance = 0;
		
		gridNodes[gridNodesInArray] = node;
		gridNodesInArray++;
		result = gridNodes[gridNodesInArray - 1];
	}
	
	return result;
}

void newCounterReceived(Message msg) {
	for (int i = 0; i < AVGOVER; i++) {
		for (int j = 0; j < GRIDNODES; i++) {
			distances[i][j] = distances[i+1][j];
		}
	}
	
	distances[AVGOVER - 1][msg.nodeId] = &msg;
}

void newDistanceReceived(Message msg) {
	distances[AVGOVER - 1][msg.nodeId] = &msg;
}

void distanceReceived(Message msg) {
	bool isLastCounter = false, newCounter = false;
	for (int i = 0; i < GRIDNODES && !isLastCounter && !newCounter; i++) {
		if (distances[AVGOVER - 1][i]->counter == msg.counter) {
			isLastCounter = true;
		} else if (distances[AVGOVER - 1][i]->counter < msg.counter) {
			newCounter = true;
		}
	}
	
	if (newCounter) {
		newCounterReceived(msg);
	} else if (isLastCounter) {
		newDistanceReceived(msg);
	}
}

void calculateDistance() {
	// distances array gebruiken om afstand tot nodes te bereken en daarmee eigen afgelegde afstand
}

///
/// setup() & loop()
///

void setup(void) {
	Serial.begin(19200);
	pinMode(buzPin, OUTPUT);
	
	// Prepare the radio
	radio.begin();
	radio.setChannel(107);
	radio.setPALevel(RF24_PA_MAX);
	//radio.setDataRate(RF24_2MBPS);
	radio.setRetries(5,0);
	radio.setPayloadSize(8);
	//radio.setAutoAck(false);
	
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1, pipes[1]);
	radio.startListening();
	
	printf_begin();
	radio.printDetails();
}

void loop(void) {
	sendLocSig();
	
	if (radio.available()) {
		//unsigned long rcvTime = micros();
		Message msg;
		radio.read(&msg, sizeof(Message));
		
		distanceReceived(msg);
		calculateDistance();
	}
	
	delay(500);
}

