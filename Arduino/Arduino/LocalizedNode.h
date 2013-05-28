///
/// @file	LocalizedNode.h
/// @brief	Class library header
/// @details	<#details#>
/// @n
/// @n @b	Project Arduino
/// @n @a	Developed with [embedXcode](http://embedXcode.weebly.com)
///
/// @author	Mart Meijerink
/// @author	Mart Meijerink
/// @date	23-05-13 17:22
/// @version	<#version#>
///
/// @copyright	© Mart Meijerink, 2013
/// @copyright	CC = BY NC SA
///
/// @see	ReadMe.txt for references
///


// Core library - IDE-based
#if defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(DIGISPARK) // Digispark specific
#include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
#include "Energia.h"
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#elif defined(CORE_TEENSY) // Teensy specific
#include "WProgram.h"
#elif defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(ARDUINO) && (ARDUINO >= 100) // Arduino 1.0x and 1.5x specific
#include "Arduino.h"
#elif defined(ARDUINO) && (ARDUINO < 100)  // Arduino 23 specific
#include "WProgram.h"
#endif // end IDE

#ifndef LocalizedNode_h
#define LocalizedNode_h

#include "SynchronisedNode.h"
#include "Broadcast.h"

/*
 * Designkeuze broadcast voor geluidsignalen: 
 * - Nieuwe radio instantie? (mogelijk?)
 * - Synchronised node radio -> 2e pipe openen
 * - Broadcast aanpassen zodat en laten aangeven wat voor soort message die broadcast is
 *
 * Geluidsnelheid bij kamertemperatuur (20 celcius): 343 m/s
 * 1 meter precisie -> indien 3ms precisie bij synchronisatie
 * 
 * Accelerometer
 */
class LocalizedNode {
private:
	SynchronisedNode	*_node;
	
public:
	LocalizedNode(SynchronisedNode *node); //(microfoon en speaker) en (accelerometer en gyroscope)
	
};

#endif
