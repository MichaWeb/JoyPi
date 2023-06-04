/* Driver for the 7-segment LED display on the JoyPi.
 * it is used with an HT16K33 LED driver.
 * This driver used the PiGPIO c++ libary to comunicate with the GPIO / I2C.
 * 
*/

#include "SevenSegment.h"																			// own header file
#include <pigpio.h>																					// needed for GPIO comunication
#include <unistd.h>																					// needed for sleep / usleep
#include <stdio.h>																					// needed for printf / scanf
#include <stdlib.h>																					// needed by the exit function

SevenSegment::SevenSegment(int _i2c_addr){
	if (gpioInitialise() < 0){																		// Initialise PiGPIO, if initialisation of pigpio failed
		printf("##############################################\n");									// print error message
		printf("#              Can't use PIGPIO!             #\n");
		printf("# Maybe PIGPIO is used by an other instance? #\n");
		printf("##############################################\n");
		exit (EXIT_FAILURE);																		// and exit the program with errorcode
	}
	else {																							// if initialisation passed
		if ((SevenSegment::_handle=i2cOpen(1,_i2c_addr,0)) < 0) {									// try to open i2c comunication an if it fails
			printf("##############################################\n");								// print error message
			printf("#               Can't open I2C!              #\n");
			printf("# Maybe device is used by an other instance? #\n");
			printf("##############################################\n");
			exit (EXIT_FAILURE);																	// and exit the program with errorcode
		} 
		else {																						// if comunication passed
			// initialise the 7-segment display. see datasheet of HT13K66 page 32
			SevenSegment::set_oscillator(true); 													// send command to activate the oscillator
			SevenSegment::set_display(true); 														// send command to activate the display
			SevenSegment::set_brightness(16); 														// send command to set the display brightness to min (with dimming level 16 = 1/16 pulse width)
		}
	}
}

SevenSegment::~SevenSegment(){
	SevenSegment::set_display(false);																// send command to deactivate the display
	SevenSegment::set_oscillator(false); 															// send command to deactivate the oscillator
	i2cClose(SevenSegment::_handle);																// close i2c comunication
	gpioTerminate();																				// terminate PiGPIO
}

void SevenSegment::set_oscillator(bool _on){
	if (_on == true) {																				// if _on its true then
		SevenSegment::send_command(CMD_SYSTEM_SETUP | OSCILLATOR_ON);								// send command to HT16K33 with data 0x21 (CMD_SYSTEM_SETUP = 0x20; OSCILLATOR_ON = 0x01)
	}
	else {																							// if _on is not true
		SevenSegment::send_command(CMD_SYSTEM_SETUP | OSCILLATOR_OFF);								// send command to HT16K33 with data 0x20 (CMD_SYSTEM_SETUP = 0x20; OSCILLATOR_OFF = 0x00)
	}
}

void SevenSegment::set_display(bool _on){
	if (_on == true) {																				// if _on its true then
		SevenSegment::send_command(CMD_DISPLAY_SETUP | DISPLAY_ON);									// send command to HT16K33 with data 0x81 (CMD_DISPLAY_SETUP = 0x80; DISPLAY_ON = 0x01)
	}
	else {																							// if _on is not true
		SevenSegment::send_command(CMD_DISPLAY_SETUP | DISPLAY_OFF);								// send command to HT16K33 with data 0x80 (CMD_DISPLAY_SETUP = 0x80; DISPLAY_OFF = 0x00)
	}
}

void SevenSegment::set_brightness(int _dimming){
	if (_dimming < 1) {																				// if _dimming < 1
		_dimming = 1;																				// set _dimming to minimum (1)
	}
	if (_dimming >16){																				// if _dimming > 16
		_dimming = 16;																				// set it to maximum (16)
	}
	SevenSegment::send_command(CMD_DIMMING_SET | SevenSegment::dimmer[(_dimming-1)]);				// send command to HT16K33 with data 0xE0 + value from dimmer[(_dimming-1)]
}

void SevenSegment::set_blink(int _speed){
	if (_speed < 0) {																				// if _speed < 0
		_speed=0;																					// set _speed to minimum (0)
	}
	if (_speed > 3) {																				// if _speed > 3
		_speed=3;																					// set _speed to maximum (3)
	}
	switch (_speed) {
		case 1:																						// blink with 1/2Hz
			SevenSegment::send_command(CMD_DISPLAY_SETUP | BLINK_DISPLAY_halfHz | DISPLAY_ON);		// send command to HT16K33 with data 0x87 (CMD_DISPLAY_SETUP = 0x80; BLINK_DISPLAY_halfHz = 0x06; DISPLAY_ON = 0x01)
			break;
		case 2:																						// blink with 1Hz
			SevenSegment::send_command(CMD_DISPLAY_SETUP | BLINK_DISPLAY_1Hz | DISPLAY_ON);			// send command to HT16K33 with data 0x85 (CMD_DISPLAY_SETUP = 0x80; BLINK_DISPLAY_1Hz = 0x04; DISPLAY_ON = 0x01)
			break;
		case 3:																						// blink with 2Hz
			SevenSegment::send_command(CMD_DISPLAY_SETUP | BLINK_DISPLAY_2Hz | DISPLAY_ON);			// send command to HT16K33 with data 0x83 (CMD_DISPLAY_SETUP = 0x80; BLINK_DISPLAY_2Hz = 0x02; DISPLAY_ON = 0x01)
			break;
		default:																					// blinking off
			SevenSegment::send_command(CMD_DISPLAY_SETUP | DISPLAY_ON);								// send command to HT16K33 with data 0x81 (CMD_DISPLAY_SETUP = 0x80; DISPLAY_ON = 0x01)
			break;
	}
}

void SevenSegment::set_mirrored(bool _on){
	if (_on==true){																					// if _on its true then
		SevenSegment::_mirrored = true;																// set _mirrored to true
	}
	else {																							// if _on is not true
		SevenSegment::_mirrored = false;															// set _mirrored to false
	}
}

void SevenSegment::set_inverted(bool _on){
	if (_on==true){																					// if _on its true then
		SevenSegment::_inverted=true;																// set _inverted to true
	}
	else {																							// if _on is not true
		SevenSegment::_inverted=false;																// set _inverted to false
	}
}

void SevenSegment::set_collon(bool _on){
	if (_on == true) {																				// if _on its true then
		SevenSegment::send_data(4,0x02);															// call send_data to set data 0x02 to position (register) 0x04. collon is on.
	}
	else {																							// if _on is not true
		SevenSegment::send_data(4,0x00);															// call send_data to set data 0x00 to position (register) 0x04. collon is off.
	}
}

void SevenSegment::set_digit(int _pos, uint8_t _data, bool _decimal){
	int reg = 0x00;
	int offset = 0;																					// offset is needed to jump over the collon register (register 0x04 and 0x05).
	uint8_t bitmask= 0x00;
	
	if (_pos >= 0 && _pos <= 3){																	// do only if _pos value valid
		// calculate offset
		if (_pos < 2) {																				// if _pos 0 or 1
			offset = 0;																				// set offset to 0
		} 
		else {																						// if _pos 2 or 3
			offset = 1;																				// set offset to 1
		}
		
		// calculate target register for digit position
		if (SevenSegment::_mirrored==true){																		// if mirrored option true
			reg = 2* (4 - (_pos + offset)); 														// target register = 2 * (invert position with ( 4 - (given _pos + offset)))
		}																							// 4 = max position (3 + offset)
		else {
			reg = 2* (_pos + offset);																// target register = (given _pos + offset) * 2 
		}
		
		bitmask = SevenSegment::get_bitmask(_data);													//get LED bitmask for the hex value of _data
		
		// recalculate bitmask with _decimal option
		if (_decimal==true){																		// if _decimal option set
			bitmask = bitmask | 0x80;																// add MSB to bitmask (bitmask or 0b10000000)
		}
		else {																						// if it not set
			bitmask = bitmask & 0x7F;																// remove MSB from bitmask (bitmask and 0b01111111)
		}
		
		// recalculate bitmask with invertet option
		if (SevenSegment::_inverted==true){																		// if inverted option true
			SevenSegment::send_data(reg,(0xFF - bitmask));											// invert bitmask by calculate (0xFF - bitmask) and send it to the calculated register
		}
		else {																						// if its not true
			SevenSegment::send_data(reg,bitmask);													// send bitmask to the calculated register
		}
	}
}

void SevenSegment::set_digit_raw(int _pos, uint8_t _data){
	int reg = 0x00;
	uint8_t offset = 0x00;
	
	if (_pos >= 0 && _pos <= 3){																	// do only if _pos value valid
		// calculate offset
		if (_pos < 2) {																				// if _pos 0 or 1
			offset = 0;																				// set offset to 0
		} 
		else {																						// if _pos 2 or 3
			offset = 1;																				// set offset to 1
		}
		
		// calculate target register for digit position
		if (SevenSegment::_mirrored==true){																		// if mirrored option true
			reg = 2* (4 - (_pos + offset)); 														// target register = 2 * (invert position with ( 4 - (given _pos + offset)))
		}																							// 4 = max position (3 + offset)
		else {
			reg = 2* (_pos + offset);																// target register = (given _pos + offset) * 2 
		}
		
		// recalculate bitmask with invertet option
		if (SevenSegment::_inverted==true){															// if inverted option true
			SevenSegment::send_data(reg,(0xFF - _data));											// invert bitmask by calculate (0xFF - bitmask) and send it to the calculated register
		}
		else {																						// if its not true
			SevenSegment::send_data(reg,_data);													// send bitmask to the calculated register
		}
	}
}

void SevenSegment::display_clear(){
	for (int i = 0; i<=15; i++) {																	// for all 16 registers (0x00 - 0x0F)
		SevenSegment::send_data(i,0x00);															// send data 0x00 to clear in the register
	}
}

int SevenSegment::display_selftest(bool _automatic){
	char _input[0];
	
	// test read/write to the data register
	printf("\nTest register read/write:\n");
	printf("Register: \twrite data: \tread data: \t\tstatus:\n");
	for (uint8_t _register = 0; _register <= 0x0F; _register++){									// for all data registers (0x00 - 0x0F)
		uint8_t _bits = 0b00000001;																	// start with 0x01
		for (uint8_t _data = 0; _data <=7; _data++){												// do it 8 times
			printf("Register: %d\twrite: %d\t",_register, (_bits << _data));
			i2cWriteByteData(SevenSegment::_handle,_register,(_bits << _data));						// direct i2c access, independent from this class. send to _register the _bits moved by counter _data
			uint8_t _read = i2cReadByteData(SevenSegment::_handle, _register);						// read the register directly. save value in _read.
			printf("read: %d\t\t",_read);
			if (_data <=3){																			// only for displaying at console
				printf("\t");
			}
			if (_read != (_bits << _data)) {														// if reading data not the same to send data
				printf("ERROR\n");
				return 1;																			// return errorcode 1
			}
			else {																					// if it the same
				printf("OK\n");
			}
		}
		i2cWriteByteData(_handle,_register,0xFF);													// at the end with this register, set all LED's on, needet by next test
	}
	printf("Register r/w test finished...\n");
	
	// test all LED's are ok
	if (_automatic==false){																			// if _automatic mode disabled, ask user for result
		printf("\n\nShow the display '8.8.:8.8.'? [y/n]\n");
		scanf("%s",_input);
		switch(_input[0]){
			case 'j':
				break;
			case 'y':
				break;
			case 'z':
				break;
			default:																				// if user input is not y or z or j
				return 2;																			// return errorcode 2
				break;
		}
	}
	else {																							// if automatic mode enabled
		printf("\n\nLED test finished. Display should be shown: '8.8.:8.8.'\n");					// print and wait 5 seconds
		sleep(5);
	}
	
	// test display flashing
	printf("\n\nTest flashing:\n");
	for (int i = 1; i <= 3; i++) {																	// for the 3 speed mode
		printf("flashing speed: %d\n",i);
		SevenSegment::set_blink(i);																	// set speet to mode
		sleep(6);																					// wait 6 seconds to the next mode
	}
	printf("no flashing...\n");
	SevenSegment::set_blink(0);																		// finaly set bilk to off (mode 0)
	
	if (_automatic==false){
		printf("\nHas the display fashed with 3 defferent speeds? [y/n]\n");
		scanf("%s",_input);
		switch(_input[0]){
			case 'j':
				break;
			case 'y':
				break;
			case 'z':
				break;
			default:																				// if user input is not y or z or j
				return 3;																			// return errorcode 3
				break;
		}
	}
	else {
		printf("Flashing test finished...\n");														// print and wait 5 seconds
		sleep(5);
	}
	
	// test display dimmer
	printf("\n\nTest brightness and dimming:\n");
	for (int i = 1; i <= 16; i++) {
		printf("dimmer level: %d\n",i);
		SevenSegment::set_brightness(i);
		sleep(1);
	}
	
	if (_automatic==false){
		printf("\nWas the brightness going from max to min? [y/n]\n");
		scanf("%s",_input);
		switch(_input[0]){
			case 'j':
				break;
			case 'y':
				break;
			case 'z':
				break;
			default:																				// if user input is not y or z or j
				return 4;																			// return errorcode 4
				break;
		}
	}
	else {
		printf("Brightness test finished...\n");													// print and wait 5 seconds
		sleep(5);
	}
	
	SevenSegment::display_clear();																	// clear the display
	
	if (_automatic==false){
		printf("\n\nAll tests passed...\n");
	}
	else {
		printf("\n\nAutomatic test finished...\n");
	}
	
	return 0;																						// return no error
}

void SevenSegment::send_command(uint8_t _data){
	i2cWriteByte(SevenSegment::_handle,_data);																	// send one byte of data to HT13K66 LED Driver
}

void SevenSegment::send_data(int _pos, uint8_t _data){
	i2cWriteByteData(SevenSegment::_handle,_pos,_data);															// send _data to register at _pos
}

uint8_t SevenSegment::get_bitmask(uint8_t _data){
	uint8_t bitmask = 0;
	
    if (SevenSegment::_mirrored == true) {
		switch (_data) {
			case 0:
				bitmask = 0x3F;
				break;
			case 1:
				bitmask = 0x30;
				break;
			case 2:
				bitmask = 0x5B;
				break;
			case 3:
				bitmask = 0x79;
				break;
			case 4:
				bitmask = 0x74;
				break;
			case 5:
				bitmask = 0x6D;
				break;
			case 6:
				bitmask = 0x6F;
				break;
			case 7:
				bitmask = 0x38;
				break;
			case 8:
				bitmask = 0x7F;
				break;
			case 9:
				bitmask = 0x7D;
				break;
			case 10:
				bitmask = 0x7E;
				break;
			case 11:
				bitmask = 0x67;
				break;
			case 12:
				bitmask = 0x0F;
				break;
			case 13:
				bitmask = 0x73;
				break;
			case 14:
				bitmask = 0x4F;
				break;
			case 15:
				bitmask = 0x4E;
				break;
			default:
				bitmask = 0x00;
				break;
		}
	}
	else {
		switch (_data) {
			case 0:
				bitmask = 0x3F;
				break;
			case 1:
				bitmask = 0x06;
				break;
			case 2:
				bitmask = 0x5B;
				break;
			case 3:
				bitmask = 0x4F;
				break;
			case 4:
				bitmask = 0x66;
				break;
			case 5:
				bitmask = 0x6D;
				break;
			case 6:
				bitmask = 0x7D;
				break;
			case 7:
				bitmask = 0x07;
				break;
			case 8:
				bitmask = 0x7F;
				break;
			case 9:
				bitmask = 0x6F;
				break;
			case 10:
				bitmask = 0x77;
				break;
			case 11:
				bitmask = 0x7C;
				break;
			case 12:
				bitmask = 0x39;
				break;
			case 13:
				bitmask = 0x5E;
				break;
			case 14:
				bitmask = 0x79;
				break;
			case 15:
				bitmask = 0x71;
				break;
			default:
				bitmask = 0x00;
				break;
		}
	}
	return bitmask;
}
