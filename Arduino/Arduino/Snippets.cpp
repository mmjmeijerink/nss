/*void check(interval interval) {
	adjustCounter();
	
	switch (interval) {
		case ALL:
			check();
			break;
		case BROADCAST:
			if (node->getState() == BROADCASTING && !node->getBroadcastDone() && node->getCounter() >= node->getBroadcastTime()) {
				node->sendBroadcast();
			}
			
			if (node->getRadio()->available()) {
				Broadcast *broadcastMessage = 0;
				bool done = false;
				while (!done) {
					done = node->getRadio()->read(broadcastMessage, sizeof(Broadcast));
				}
				node->handleBroadcast(broadcastMessage);
				lastBroadcastReceived = millis();
			}
			break;
		case TIMEOUT:
			if (node->getState() == LISTENING && millis() - lastBroadcastReceived >= node->getTimeoutTime()) {
				node->setState(BROADCASTING);
			} else if (node->getState() == QUIET && millis() - lastBroadcastReceived >= 2 * node->getTimeoutTime()) {
				node->setState(BROADCASTING);
			}
			break;
		case LEDSTATUS:
			node->checkLedStatus();
			break;
			
		default:
			check();
			break;
	}
}*/