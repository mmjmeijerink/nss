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

SynchronisedNode::SynchronisedNode(int nodeId, RF24 *radio, uint8_t ledPin) {
    _nodeId = nodeId;
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
int SynchronisedNode::getNodeId() {
	return _nodeId;
}

state SynchronisedNode::getState() {
    return _state;
}

void SynchronisedNode::setState(state state) {
	if (_state == BROADCASTING) {
		broadcastsSend = 0;
		broadcastDone = false;
		broadcastTime = random(FREQUENCY);
	}
	
	_state = state;
}

int SynchronisedNode::getCounter() {
	return counter;
}

void SynchronisedNode::raiseCounter(int value) {
	int oldCounter = counter;
	counter += value;
	counter = counter % FREQUENCY;
	
	if (counter < oldCounter) {
		blinkLed();
		broadcastDone = false;
	}
}

int SynchronisedNode::getFrequency() {
	return FREQUENCY;
}

int SynchronisedNode::getTimeoutTime() {
	return TIMEOUTTIME;
}

int SynchronisedNode::getBroadcastTime() {
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
}

void SynchronisedNode::checkLedStatus() {
	if (millis() - ledTurnedOn >= blinkTime) {
		digitalWrite(_ledPin, LOW);
	}
}


// Broadcast functions
void SynchronisedNode::sendBroadcast() {
	if (_state == BROADCASTING && !broadcastDone && counter >= broadcastTime) {
		broadcastsSend++;
		Broadcast broadcast;
		broadcast.nodeId = _nodeId;
		broadcast.broadcastTime = counter;
		broadcast.isLastBroadcast = (broadcastsSend == BROADCASTS);
		
		_radio->stopListening();
		_radio->write(&broadcast, sizeof(Broadcast));
		_radio->startListening();
		
		printf("BroadcastTime: %d\n\r", broadcastTime);
		printf("Counter: %d\n\r", counter);
	}
	
	broadcastDone = true;
	
	if (broadcastsSend == BROADCASTS) {
		setState(QUIET);
	}
}

void SynchronisedNode::handleBroadcast(Broadcast msg) {
	printf("Broadcast Received at %d \n\rmsg.nodeId: %d\n\rmsg.broadcastTime: %d \n\rmsg.isLastBroadcast.: %d \n\r\n", counter, msg.nodeId, msg.broadcastTime, msg.isLastBroadcast);
	
	raiseCounter(0.2 * (msg.broadcastTime - counter));
	
	if ((_state == BROADCASTING || _state == QUIET) && msg.nodeId < _nodeId) {
		setState(LISTENING);
	} else if (_state == LISTENING && (msg.nodeId > _nodeId || (msg.nodeId == (_nodeId - 1)%16 && msg.isLastBroadcast))) {
		setState(BROADCASTING);
	}
}
