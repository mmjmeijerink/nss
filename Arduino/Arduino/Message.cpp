//
// Message.cpp 
// Class library C++ code
// ----------------------------------
// Developed with embedXcode 
// http://embedXcode.weebly.com
//
// Project Arduino
//
// Created by Mart Meijerink, 18-05-13 01:33
// Mart Meijerink
//	
//
// Copyright © Mart Meijerink, 2013
// Licence CC = BY NC SA
//
// See Message.h and ReadMe.txt for references
//

#include "Message.h"

Message::Message(int nodeID, unsigned long broadcastTime, bool isLastBroadcast) {
	_nodeID = nodeID;
	_broadcastTime = broadcastTime;
	_isLastBroadcast = isLastBroadcast;
}

int Message::getNodeID() {
	return _nodeID;
}

unsigned long Message::getBroadcastTime() {
	return _broadcastTime;
}

bool Message::isLastBroadcast() {
	return _isLastBroadcast;
}