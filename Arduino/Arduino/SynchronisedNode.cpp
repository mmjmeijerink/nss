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

SynchronisedNode::SynchronisedNode(int NodeID, uint8_t ledPin) {
    _nodeID = NodeID;
	_state = BROADCASTING;
	counter = 0;
	broadcastTime = random(FREQUENCY);
	broadcastsSend = 0;
	broadcastDone = false;
	
	pinMode(ledPin, OUTPUT);
	_ledPin = ledPin;
	blinkTime = 1000;
}

// Getters & Setters
int SynchronisedNode::getNodeID() {
	return _nodeID;
}

state SynchronisedNode::getState() {
    return _state;
}

void SynchronisedNode::setState(state state) {
	_state = state;
}

unsigned long SynchronisedNode::getCounter() {
	return counter;
}

void SynchronisedNode::raiseCounter(unsigned long value) {
	unsigned long oldCounter = counter;
	counter += value;
	counter = counter % FREQUENCY;
	
	if ((oldCounter > counter && oldCounter != FREQUENCY) || counter == FREQUENCY) {
		blinkLed();
		setBroadcastDone(false);
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

void SynchronisedNode::setBroadcastDone(bool done) {
	broadcastDone = done;
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
		Message broadcast(_nodeID, broadcastTime, broadcastsSend == BROADCASTS);
	}
	
	setBroadcastDone(true);
	
	if (broadcastsSend == BROADCASTS) {
		_state = QUIET;
		broadcastsSend = 0;
	}
}


void SynchronisedNode::handleBroadcastMessage(Message *msg) {
	if (_state == BROADCASTING) {
		if (msg->getNodeID() < _nodeID) {
			_state == LISTENING;
		}
	} else if (_state == QUIET) {
		counter += 0.1 * (counter - msg->getBroadcastTime());
	} else if (_state == LISTENING) {
		counter += 0.1 * (counter - msg->getBroadcastTime());
		
		if (msg->getNodeID() > _nodeID) {
			_state == LISTENING;
		}
	}
}
