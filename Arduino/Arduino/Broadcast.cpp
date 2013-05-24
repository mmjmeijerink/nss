//
// Broadcast.cpp 
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
// See Broadcast.h and ReadMe.txt for references
//

#include "Broadcast.h"

Broadcast::Broadcast(int nodeID, int broadcastTime, bool isLastBroadcast) {
	_nodeID = nodeID;
	_broadcastTime = broadcastTime;
	_isLastBroadcast = isLastBroadcast;
}

int Broadcast::getNodeID() {
	return _nodeID;
}

int Broadcast::getBroadcastTime() {
	return _broadcastTime;
}

bool Broadcast::isLastBroadcast() {
	return _isLastBroadcast;
}