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

Broadcast::Broadcast(int nodeId, int broadcastTime, type broadcastType) {
	_nodeId = nodeId;
	_broadcastTime = broadcastTime;
	_broadcastType = broadcastType;
	_isLastBroadcast = false;
}

Broadcast::Broadcast(int nodeId, int broadcastTime, type broadcastType, bool isLastBroadcast) {
	_nodeId = nodeId;
	_broadcastTime = broadcastTime;
	_broadcastType = broadcastType;
	_isLastBroadcast = isLastBroadcast;
}

Broadcast::Broadcast(int nodeId, int broadcastTime, bool isLastBroadcast) {
	_nodeId = nodeId;
	_broadcastTime = broadcastTime;
	_broadcastType = SYNCHRONISE;
	_isLastBroadcast = isLastBroadcast;
}

int Broadcast::getNodeId() {
	return _nodeId;
}

int Broadcast::getBroadcastTime() {
	return _broadcastTime;
}

type Broadcast::getBroadcastType() {
	return _broadcastType;
}

bool Broadcast::isLastBroadcast() {
	return _isLastBroadcast;
}