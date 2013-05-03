/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example RF Radio Ping Pair
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two different nodes,
 * connect the role_pin to ground on one.  The ping node sends the current time to the pong node,
 * which responds by sending the value back.  The ping node can then see how long the whole cycle
 * took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
// (MV:) Adapted to work with the configuration of the shield. Original: RF24 radio(9,10);

RF24 radio(3, 9);

// sets the role of this unit in hardware.  Connect to GND to be the 'pong' receiver
// Leave open to be the 'ping' transmitter
const int role_pin = 7;

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x3210FEDCBALL, 0xABCDEF0123LL };

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  The hardware itself specifies
// which node it is.
//
// This is done through the role_pin
//

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role;

int total, packetSent, received;

void printLog(int total, int packetSent, int received) {
	printf("Tried to send %d packets\n\r", total);
	printf("ACKed %d packets\n\r", packetSent);
	printf("Received %d responses\n\r", received);
}

void setup(void)
{
	//
	// Role
	//
	
	// set up the role pin
	pinMode(role_pin, INPUT);
	digitalWrite(role_pin,HIGH);
	delay(20); // Just to get a solid reading on the role pin
	
	// read the address pin, establish our role
	if ( ! digitalRead(role_pin) )
		role = role_ping_out;
	else
		role = role_pong_back;
	
	//
	// Print preamble
	//
	
	Serial.begin(57600);
	printf_begin();
	printf("\n\rRF24/examples/pingpair/\n\r");
	printf("ROLE: %s\n\r",role_friendly_name[role]);
	
	//
	// Setup and configure rf radio
	//
	
	radio.begin();
	radio.setChannel(62);
	radio.setPALevel(RF24_PA_MAX);
	
	// optionally, increase the delay between retries & # of retries
	radio.setRetries(5,0);
	
	// optionally, reduce the payload size.  seems to
	// improve reliability
	radio.setPayloadSize(8);
	
	//
	// Open pipes to other nodes for communication
	//
	
	// This simple sketch opens two pipes for these two nodes to communicate
	// back and forth.
	// Open 'our' pipe for writing
	// Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)
	
	if ( role == role_ping_out )
	{
		radio.openWritingPipe(pipes[0]);
		radio.openReadingPipe(1,pipes[1]);
	}
	else
	{
		radio.openWritingPipe(pipes[1]);
		radio.openReadingPipe(1,pipes[0]);
	}
	
	//
	// Start listening
	//
	
	radio.startListening();
	
	//
	// Dump the configuration of the rf unit for debugging
	//
	
	radio.printDetails();
	
	total = 0;
	packetSent = 0;
	received = 0;
}

void loop(void)
{
	//
	// Ping out role.  Repeatedly send the current time
	//
	
	if (role == role_ping_out and total < 1000)
	{
		// First, stop listening so we can talk.
		radio.stopListening();
		
		// Take the time, and send it.  This will block until complete
		unsigned long time = millis();
		total++;
		if(total % 100 == 0)
			printf("%d: Now sending %lu...", total, time);
		bool ok = radio.write( &time, sizeof(unsigned long) );
		
		if (ok) {
			packetSent++;
			if(total % 100 == 0)
				printf("ok...");
		}
		else
			printf("failed.\n\r");
		
		// Now, continue listening
		radio.startListening();
		
		// Wait here until we get a response, or timeout (250ms)
		unsigned long started_waiting_at = millis();
		bool timeout = false;
		while ( ! radio.available() && ! timeout )
			if (millis() - started_waiting_at > 200 )
				timeout = true;
		
		// Describe the results
		if ( timeout )
		{
			printf("Failed, response timed out.\n\r");
		}
		else
		{
			// Grab the response, compare, and send to debugging spew
			unsigned long got_time;
			bool receivedOk = radio.read( &got_time, sizeof(unsigned long) );
			
			if (receivedOk) received++;
			
			// Spew it
			if(total % 100 == 0)
				printf("Got response %d: %lu, round-trip delay: %lu\n\r", received, got_time,millis()-got_time);
		}
		
		// Try again 1s later
		delay(50);
	} else if (role == role_ping_out and total == 1000) {
		printLog(total, packetSent, received);
		total++;
	}
	
	//
	// Pong back role.  Receive each packet, dump it out, and send it back
	//
	
	if ( role == role_pong_back )
	{
		// if there is data ready
		if ( radio.available() )
		{
			// Dump the payloads until we've gotten everything
			unsigned long got_time;
			bool done = false;
			while (!done)
			{
				// Fetch the payload, and see if this was the last one.
				done = radio.read( &got_time, sizeof(unsigned long) );
				
				// Spew it
				//printf("Got payload %lu...",got_time);
				
				// Delay just a little bit to let the other unit
				// make the transition to receiver
				delay(20);
			}
			
			// First, stop listening so we can talk
			radio.stopListening();
			
			// Send the final one back.
			radio.write( &got_time, sizeof(unsigned long) );
			//printf("Sent response.\n\r");
			
			// Now, resume listening so we catch the next packets.
			radio.startListening();
		}
	}
}
// vim:cin:ai:sts=2 sw=2 ft=cpp