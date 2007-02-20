/*
 * Copyright (C) 2006 Free Software Foundation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * See file LICENSE for further informations on licensing terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * -----------------------------------------------------------
 * Firmata, the general purpose sensorbox firmware for Arduino
 * -----------------------------------------------------------
 * 
 * Firmata turns the Arduino into a Plug-n-Play sensorbox, servo
 * controller, and/or PWM motor/lamp controller.
 *
 * It was originally designed to work with the Pd object [arduino]
 * which is included in Pd-extended.  This firmware is intended to
 * work with any host computer software package.  It can easily be
 * used with other programs like Max/MSP, Processing, or whatever can
 * do serial communications.
 *
 * @author: Hans-Christoph Steiner <hans@at.or.at>
 *   help with initial protocol redesign: Jamie Allen <jamie@heavyside.net>
 *   key bugfixes: Georg Holzmann <grh@mur.at>
 *                 Gerda Strobl <gerda.strobl@student.tugraz.at>
 * @date: 2006-05-19
 * @locations: STEIM, Amsterdam, Netherlands
 *             IDMI/Polytechnic University, Brookyn, NY, USA
 *             Electrolobby Ars Electronica, Linz, Austria
 */

/* 
 * TODO: debug hardware PWM
 * TODO: convert all non-frequent messages to SysEx (version, pinMode, report enable, etc)
 * TODO: convert to MIDI protocol using SysEx for longer messages
 * TODO: add pulseOut functionality for servos
 * TODO: add software PWM for servos, etc (servo.h or pulse.h)
 * TODO: redesign protocol to accomodate boards with more I/Os
 * TODO: add protocol version reporting
 * TODO: add device type reporting (i.e. some firmwares will use the Firmata
 *       protocol, but will only support specific devices, like ultrasound 
 *       rangefinders or servos)
 * TODO: add "pinMode all 0/1" command
 * TODO: add cycle markers to mark start of analog, digital, pulseIn, and PWM
 * TODO: use Program Control to load stored profiles from EEPROM
 */

/* cvs version: $Id: Pd_firmware.pde,v 1.23 2007-02-20 06:25:56 eighthave Exp $ */

/*==========================================================================
 * MESSAGE FORMATS
 *==========================================================================*/

/*----------------------------------------------------------------------------
 * MAPPING DATA TO MIDI
 *
 * This protocol uses the MIDI message format, but does not use the whole
 * protocol.  Most of the command mappings here will not make sense in terms
 * of MIDI controllers and synths.
 *
 * MIDI format: http://www.harmony-central.com/MIDI/Doc/table1.html
 * 
 *                              MIDI       
 * type                command  channel    first byte            second byte 
 * -----------------------------------------------------------------------------
 * analog I/O            0xE0   pin #      LSB(bits 0-6)         MSB(bits 7-13)
 * digital I/O           0x90   port base  LSB(bits 0-6)         MSB(bits 7-13)
 * report analog pin     0xC0   pin #      disable/enable(0/1)   - n/a -
 * report digital ports  0xD0   port base  disable/enable(0/1)   - n/a -
 *
 * digital pin mode(I/O) 0xF4   - n/a -    pin # (0-63)          pin state(0=in)
 * firmware version      0xF9   - n/a -    minor version         major version
 * system reset          0xFF   - n/a -    - n/a -               - n/a -
 *
 */

/* proposed extensions using SysEx
 *
 * type       SysEx start   command    data bytes                   SysEx stop
 * ---------------------------------------------------------------------------
 * pulse I/O     0xF0        0xA0      five 7-bit chunks, LSB first    0xF7
 * shiftOut      0xF0        0xB0                        
 */

/*----------------------------------------------------------------------------
 * DATA MESSAGES */

/* two byte digital data format
 * ----------------------
 * 0  digital data, 0x90-0x9F, (x & 0x0F) to get port base number
 * 1  digital pins 0-6 bitmask
 * 2  digital pins 7-13 bitmask 
 */

/* analog 14-bit data format
 * ----------------------
 * 0  analog pin, 0xE0-0xEF, (x & 0x0F) for pin number
 * 1  analog least significant 7 bits
 * 2  analog most significant 7 bits
 */

/* pulseIn/Out (uses 32-bit value)
 * ----------------------
 * 0  START_SYSEX (0xF0)
 * 1  pulseIn (0xFD)
 * 2  bits 0-6 (least significant byte)
 * 3  bits 7-13
 * 4  bits 14-20
 * 5  bits 21-27
 * 6  bits 28-34 (most significant byte)
 * 7  END_SYSEX (0xF7)
 */

/* version report format
 * Send a single byte 0xF9, Arduino will reply with:
 * ----------------------
 * 0  version report header (0xF9) (MIDI Undefined)
 * 1  minor version (0-127)
 * 2  major version (0-127)
 */

/*----------------------------------------------------------------------------
 * CONTROL MESSAGES */

/* set digital pin mode
 * ----------------------
 * 1  set digital pin mode (0xF4) (MIDI Undefined)
 * 2  pin number (0-127)
 * 3  state (INPUT/OUTPUT, 0/1)
 */

/* toggle analogIn reporting by pin
 * ----------------------
 * 0  toggle digitalIn reporting (0xC0-0xCF) (MIDI Program Change)
 * 1  disable(0)/enable(non-zero) 
 */

/* toggle digitalIn reporting by port pairs
 * ----------------------
 * 0  toggle digitalIn reporting (0xD0-0xDF) (MIDI Aftertouch)
 * 1  disable(0)/enable(non-zero) 
 */

/* request version report
 * ----------------------
 * 0  request version report (0xF9) (MIDI Undefined)
 */

/*==========================================================================
 * MACROS
 *==========================================================================*/

/* Version numbers for the protocol.  The protocol is still changing, so these
 * version numbers are important.  This number can be queried so that host
 * software can test whether it will be compatible with the currently
 * installed firmware. */
#define MAJOR_VERSION 1  // for non-compatible changes
#define MINOR_VERSION 0  // for backwards compatible changes

/* total number of digital pins supported */  
#define TOTAL_DIGITAL_PINS 14

// for comparing along with INPUT and OUTPUT
#define PWM 2

#define DIGITAL_MESSAGE         0x90 // send data for a digital pin
#define ANALOG_MESSAGE          0xE0 // send data for an analog pin (or PWM)
//#define PULSE_MESSAGE           0xA0 // proposed pulseIn/Out message (SysEx)
//#define SHIFTOUT_MESSAGE        0xB0 // proposed shiftOut message (SysEx)
#define REPORT_ANALOG_PIN       0xC0 // enable analog input by pin #
#define REPORT_DIGITAL_PORTS    0xD0 // enable digital input by port pair
#define START_SYSEX             0xF0 // start a MIDI SysEx message
#define SET_DIGITAL_PIN_MODE    0xF4 // set a digital pin to INPUT or OUTPUT 
#define END_SYSEX               0xF7 // end a MIDI SysEx message
#define REPORT_VERSION          0xF9 // report firmware version
#define SYSTEM_RESET            0xFF // reset from MIDI

/*==========================================================================
 * GLOBAL VARIABLES
 *==========================================================================*/

// maximum number of post-command data bytes (non-SysEx)
#define MAX_DATA_BYTES 2
// this flag says the next serial input will be data
byte waitForData = 0;
byte executeMultiByteCommand = 0; // command to execute after getting multi-byte data
byte storedInputData[MAX_DATA_BYTES] = {0,0}; // multi-byte data

/* this int serves as a bit-wise array to store pin status
 * 0 = INPUT, 1 = OUTPUT  */
int digitalPinStatus = 0;

/* this byte stores the status off whether PWM is on or not
 * bit 9 = PWM0, bit 10 = PWM1, bit 11 = PWM2
 * the rest of the bits are unused and should remain 0  */
int pwmStatus = 0;


/*==========================================================================
 * FUNCTIONS                                                                
 *==========================================================================*/

/* -------------------------------------------------------------------------
 * output digital bytes received from the serial port
 */
void outputDigitalBytes(byte pin0_6, byte pin7_13) {
	int i;
	int mask;
	int twoBytesForPorts;
	
	twoBytesForPorts = pin0_6 + (pin7_13 << 7);
	for(i=0; i<14; ++i) {
		mask = 1 << i;
		if( (digitalPinStatus & mask) && !(pwmStatus & mask) ) {
			digitalWrite(i, twoBytesForPorts & mask);
		} 
	}
}

/* -------------------------------------------------------------------------
 * processInput() is called whenever a byte is available on the
 * Arduino's serial port.  This is where the commands are handled.
 */
void processInput(int inputData) {
	int command, channel;

	// a few commands have byte(s) of data following the command
	if( (waitForData > 0) && (inputData < 128) ) {  
		waitForData--;
		storedInputData[waitForData] = inputData;
		if( (waitForData==0) && executeMultiByteCommand ) {
			//we got everything
			switch(executeMultiByteCommand) {
			case ANALOG_MESSAGE:
				channel = inputData & 0x0F; // get channel from command byte
				break;
			case DIGITAL_MESSAGE:
				outputDigitalBytes(storedInputData[1], storedInputData[0]);
				break;
			case SET_DIGITAL_PIN_MODE:
				setPinMode(storedInputData[1], storedInputData[0]);
				break;
			case REPORT_ANALOG_PIN:
				break;
			case REPORT_DIGITAL_PORTS:
				break;
			}
			executeMultiByteCommand = 0;
		}
	} else {
		// remove channel info from command byte if less than 0xF0
		if(inputData < 0xF0) {
			command = inputData & 0xF0;
		} else {
			command = inputData;
		}
		switch (inputData) {
		case ANALOG_MESSAGE:
		case DIGITAL_MESSAGE:
		case SET_DIGITAL_PIN_MODE:
			waitForData = 2; // two data bytes needed
			executeMultiByteCommand = inputData;
			break;
		case REPORT_ANALOG_PIN:
		case REPORT_DIGITAL_PORTS:
			waitForData = 1; // two data bytes needed
			executeMultiByteCommand = inputData;
			break;
		case SYSTEM_RESET:
			// this doesn't do anything yet
			break;
		case REPORT_VERSION:
			Serial.print(REPORT_VERSION, BYTE);
			Serial.print(MAJOR_VERSION, BYTE);
			Serial.print(MINOR_VERSION, BYTE);
			break;
		}
	}
}


/* -------------------------------------------------------------------------
 * this function checks to see if there is data waiting on the serial port 
 * then processes all of the stored data
 */

/* TODO: switch this to a timer interrupt.  The timer is set in relation to
 * the bitrate, when the interrupt is triggered, then it runs checkForInput().
 * Therefore, it only checks for input once per cycle of the serial port.
 */
void checkForInput() {
	if(Serial.available())
		processInput( Serial.read() );
}

// -------------------------------------------------------------------------
/* this function sets the pin mode to the correct state and sets the relevant
 * bits in the two bit-arrays that track Digital I/O and PWM status
 */
void setPinMode(byte pin, byte mode) {
	if(mode == INPUT) {
		digitalPinStatus = digitalPinStatus &~ (1 << pin);
		pwmStatus = pwmStatus &~ (1 << pin);
		pinMode(pin,INPUT);
	}
	else if(mode == OUTPUT) {
		digitalPinStatus = digitalPinStatus | (1 << pin);
		pwmStatus = pwmStatus &~ (1 << pin);
		pinMode(pin,OUTPUT);
	}
	else if( mode == PWM ) {
		digitalPinStatus = digitalPinStatus | (1 << pin);
		pwmStatus = pwmStatus | (1 << pin);
		pinMode(pin,OUTPUT);
	}
// TODO: save status to EEPROM here, if changed
}

// =========================================================================

// used for flashing the pin for the version number
void pin13strobe(int count, int onInterval, int offInterval) {
	byte i;
	for(i=0; i<count; i++) {
		digitalWrite(13,1);
		delay(onInterval);
		digitalWrite(13,0);
		delay(offInterval);
	}
}

/*==========================================================================
 * SETUP()
 *==========================================================================*/
void setup() {
	byte i;

// TODO: load state from EEPROM here
	Serial.begin(115200); // 9600, 14400, 38400, 57600, 115200

/* TODO: send digital inputs here, if enabled, to set the initial state on the
 * host computer, since once in the loop(), the Arduino will only send data on
 * change. */

// flash the pin 13 with the protocol minor version (add major once > 0)
	pinMode(13,OUTPUT);
	pin13strobe(10,5,20); // separator, a quick burst
	delay(500);
	pin13strobe(MAJOR_VERSION, 200, 400);
	delay(500);
	pin13strobe(10,5,20); // separator, a quick burst
	delay(500);
	pin13strobe(MINOR_VERSION, 200, 400);
	delay(500);
	pin13strobe(10,5,20); // separator, a quick burst
	delay(1000);
	for(i=0; i<TOTAL_DIGITAL_PINS; ++i) {
		setPinMode(i,OUTPUT);
	}
}

/*==========================================================================
 * LOOP()
 *==========================================================================*/
void loop() {

	checkForInput();
}
