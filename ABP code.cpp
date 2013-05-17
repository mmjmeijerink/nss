///
/// Constants
///

RF24 radio(3, 9);

const int ping_pin = 7, pong_pin = 8;
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[4] = {0x3210FEDCBALL, 0x6789ABCDEFLL, 0xABCDEF0123LL, 0x3210FEDC03LL};
//const uint64_t pipes[3] = {0xABCD012301LL , 0xABCD0123ABLL, 0xABCD0123EFLL};

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_forward, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = {"invalid", "Node A", "Hub", "Node B"};

// The role of the current running sketch
role_e role;

// Alternating Bit
unsigned long alternatingBit = 0;

int total, packetSent, received;

void printLog(int total, int packetSent, int received) {
	printf("Tried to send %d packets\n\r", total);
	printf("ACKed %d packets\n\r", packetSent);
	printf("Received %d responses\n\r", received);
}

///
/// Setup
///

//
// Role
//

// set up the role pin
pinMode(ping_pin, INPUT);
digitalWrite(ping_pin, HIGH);
pinMode(pong_pin, INPUT);
digitalWrite(pong_pin, HIGH);
delay(20); // Just to get a solid reading on the role pin

// read the address pin, establish our role
if (!digitalRead(ping_pin))
role = role_ping_out;
else if (!digitalRead(pong_pin))
role = role_pong_back;
else
role = role_forward;

//
// Print preamble
//

Serial.begin(57600);
printf_begin();
printf("ROLE: %s\n\r",role_friendly_name[role]);

//
// Setup and configure rf radio
//

radio.begin();
radio.setChannel(107);
radio.setPALevel(RF24_PA_MAX);
//radio.setDataRate(RF24_2MBPS);

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

if (role == role_ping_out)
{
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1,pipes[1]);
}
else if (role == role_pong_back)
{
	radio.openWritingPipe(pipes[3]);
	radio.openReadingPipe(1,pipes[2]);
}
else {
	radio.openWritingPipe(pipes[2]);
	radio.openReadingPipe(1, pipes[0]);
	radio.openReadingPipe(2, pipes[3]);
}

radio.startListening(); // Start listening
radio.printDetails(); // Dump the configuration of the rf unit for debugging

total = 0;
packetSent = 0;
received = 0;

///
/// Loop
///

// Ping out role. Send packet till confirmation comes in, than send next
if (role == role_ping_out and total < 1000)
{
	// First, stop listening so we can talk.
	radio.stopListening();
	
	// Set the bit to send
	total++;
	//printf("Total %d \n\rSending %lu ", total, alternatingBit);
	bool ok = radio.write(&alternatingBit, sizeof(unsigned long));
	
	if (ok) {
		packetSent++;
		//printf("ok...");
	}
	else
		printf("\n\rfailed.\n\r");
		
		// Now, continue listening
		radio.startListening();
		
		// Wait here until we get a response, or timeout (250ms)
		unsigned long started_waiting_at = millis();
		bool timeout = false;
		while (!radio.available() && !timeout)
			if (millis() - started_waiting_at > 200)
				timeout = true;
				
				if (timeout)
				{
					printf("Failed, response timed out.\n\r");
				}
				else
				{
					unsigned long receivedBit;
					bool receivedOk = radio.read(&receivedBit, sizeof(unsigned long));
					if (receivedOk) received++;
					
					if (receivedBit == alternatingBit) {
						alternatingBit++;
					}
					//printf("Received %lu, \n\r Sending %lu next.", receivedBit, alternatingBit);
				}
	
	delay(50);
} else if (role == role_ping_out and total == 1000) {
	printLog(total, packetSent, received);
	total++;
}

// Pong back role.  Receive each packet, dump it out, and send it back
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

// Forward role. Receive packet and send it to the other