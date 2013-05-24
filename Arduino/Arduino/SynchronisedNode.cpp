//
// SynchronisedNode.cpp 
// Class library C++ code
// ----------------------------------
// Developed with embedXcode 
// http://embedXcode.weebly.com
//
// Project Arduino
//
// Created by Mart Meijerink, 15-05-13 14:26
// Mart Meijerink
//	
//
// Copyright © Mart Meijerink, 2013
// Licence CC = BY NC SA
//
// See SynchronisedNode.h and ReadMe.txt for references
//

#include "SynchronisedNode.h"

SynchronisedNode::SynchronisedNode(int NodeID, RF24 *radio, uint8_t ledPin) {
    _nodeID = NodeID;
	_state = BROADCASTING;
	counter = 0;
	broadcastTime = random(FREQUENCY);
	broadcastsSend = 0;
	broadcastDone = false;
	
	// Radio setup
	_radio = radio;
	
	// LED setup
	//pinMode(ledPin, OUTPUT);
	_ledPin = ledPin;
	blinkTime = FREQUENCY / 10;
}

// Getters & Setters
int SynchronisedNode::getNodeID() {
	return _nodeID;
}

state SynchronisedNode::getState() {
    return _state;
}

void SynchronisedNode::setState(state state) {
	if (_state == BROADCASTING) {
		broadcastsSend = 0;
		broadcastDone = false;
	}
	
	_state = state;
}

unsigned long SynchronisedNode::getCounter() {
	return counter;
}

void SynchronisedNode::raiseCounter(unsigned long value) {
	unsigned long oldCounter = counter;
	counter += value;
	counter = counter % FREQUENCY;
	
	if ((oldCounter > counter && oldCounter != 0) || counter == 0) {
		blinkLed();
		broadcastDone = false;
	}
}

unsigned long SynchronisedNode::getFrequency() {
	return FREQUENCY;
}

unsigned long SynchronisedNode::getTimeoutTime() {
	return TIMEOUTTIME;
}

unsigned long SynchronisedNode::getBroadcastTime() {
	return broadcastTime;
}

int SynchronisedNode::getBroadcastsSend() {
	return broadcastsSend;
}

bool SynchronisedNode::getBroadcastDone() {
	return broadcastDone;
}

RF24* SynchronisedNode::getRadio() {
	return _radio;
}


// LED functions
void SynchronisedNode::blinkLed() {
	digitalWrite(_ledPin, HIGH);
	ledTurnedOn = millis();
	//if(counter % 100 == 0) printf("Led aangezet om: %u \n\r", ledTurnedOn);
}

void SynchronisedNode::checkLedStatus() {
	//if(counter % 100 == 0) printf("Led status check om: %u \n\rTijd aan: %u \n\r", millis(), millis() - ledTurnedOn);
	if (millis() - ledTurnedOn >= blinkTime) {
		digitalWrite(_ledPin, LOW);
	}
}


// Broadcast functions
void SynchronisedNode::sendBroadcast() {
	if (_state == BROADCASTING && !broadcastDone && counter >= broadcastTime) {
		broadcastsSend++;
		Broadcast broadcast(_nodeID, broadcastTime, broadcastsSend == BROADCASTS);
		_radio->stopListening();
		_radio->write(&broadcast, sizeof(broadcast));
		_radio->startListening();
	}
	
	broadcastDone = true;
	printf("Broadcast #%d sent\n\r", broadcastsSend);
	printf("BroadcastsSend == BROADCAST => %d == %d \n\r\n", broadcastsSend, BROADCASTS);
	
	if (broadcastsSend == BROADCASTS) {
		setState(QUIET);
		broadcastsSend = 0;
		broadcastDone = false;
	}
}

void SynchronisedNode::handleBroadcast(Broadcast *msg) {
	printf("Broadcast Received: \n\rmsg->getNodeID(): %d\n\r msg->getBroadcastTime(): %u \n\r msg->isLastBroadcast(): %d", msg->getNodeID(), msg->getBroadcastTime(), msg->isLastBroadcast());
	
	if (_state == BROADCASTING) {
		if (msg->getNodeID() < _nodeID) {
			setState(LISTENING);		}
	} else if (_state == QUIET) {
		counter += 0.1 * (counter - msg->getBroadcastTime());
		
		if (msg->getNodeID() < _nodeID) {
			setState(LISTENING);
		}
	} else if (_state == LISTENING) {
		counter += 0.1 * (counter - msg->getBroadcastTime());
		
		if ((msg->getNodeID() == (_nodeID - 1)%16 && msg->isLastBroadcast()) || msg->getNodeID() > _nodeID) {
			setState(BROADCASTING);
		}
	}
}
