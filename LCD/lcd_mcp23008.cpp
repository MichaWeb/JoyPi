/* LCD Display with MCP23008 Controler */
#include "lcd_mcp23008.h"																						// own header file
#include <pigpio.h>																								// the PiGPIO header file
#include <unistd.h>																								// for sleep / usleep
#include <string.h>																								// for string convertion (strlen)
#include <stdlib.h>																								// used for exit function
#include <stdio.h>																								// for printf

LCD_MCP23008_I2C::LCD_MCP23008_I2C(int _addr, int _rows, int _cols){
	LCD_MCP23008_I2C::addr= _addr;
	LCD_MCP23008_I2C::rows = _rows;
	LCD_MCP23008_I2C::cols = _cols;
	LCD_MCP23008_I2C::backlightval=LCD_NOBACKLIGHT;																// set backlight off by default

	// set display rows
	if (_rows <= 1) {																							// if only 1 row to initialise,
		LCD_MCP23008_I2C::lines = LCD_1LINE;																	// set rows to LCD_1Line
	}
	else {																										// if more than 1 row to displayed,
		LCD_MCP23008_I2C::lines = LCD_2LINE;																	// set rows to LCD_2Line (even is it more than 2 lines)
	}
}

LCD_MCP23008_I2C::~LCD_MCP23008_I2C(){

}

void LCD_MCP23008_I2C::Init(){
	if (gpioInitialise() < 0){																					// if initialisation of pigpio failed
		printf("##############################################\n");
		printf("#              Can't use PIGPIO!             #\n");
		printf("# Maybe PIGPIO is used by an other instance? #\n");
		printf("##############################################\n");
		exit (EXIT_FAILURE);																					// exit the program
	}
	else{
		if ((LCD_MCP23008_I2C::_handle=i2cOpen(1,LCD_MCP23008_I2C::addr,0)) < 0) {								// try to open i2c
			printf("##############################################\n");
			printf("#               Can't open I2C!              #\n");
			printf("# Maybe device is used by an other instance? #\n");
			printf("##############################################\n");
			exit (EXIT_FAILURE);																				// exit the program
		} 
		else {
			//set MCP23008 config
			LCD_MCP23008_I2C::MCP23008_reg_write(REGISTER_IODIR,MCP23008_IODIR_ALL_OUTPUT);						// set all GPIO-Pins as output
			LCD_MCP23008_I2C::MCP23008_reg_write(REGISTER_IOPOL,MCP23008_IPOL_ALL_NORMAL);						// set all GPIO-Pins as non inverted
			LCD_MCP23008_I2C::MCP23008_reg_write(REGISTER_GPPU,MCP23008_GPPU_ALL_DISABLED);						// disable all GPIO-Pins PullUp resistors
			LCD_MCP23008_I2C::MCP23008_reg_write(REGISTER_GPIO,MCP23008_GPIO_ALL_LOW);							// set all GPIO-Pins to state low
			
			// set LCD into 4-bit mode
			LCD_MCP23008_I2C::Command(0x33);																	// send 2 times 0x3
			LCD_MCP23008_I2C::Command(0x32);																	// send 0x3 and 0x2
			
			// Set LCD functions number of lines and font size
			LCD_MCP23008_I2C::_displayfunction = LCD_4BITMODE | LCD_MCP23008_I2C::lines | LCD_5x8DOTS;			// set 4 bit mode, count of lines and LCD dots per character
			LCD_MCP23008_I2C::Command(LCD_FUNCTIONSET | LCD_MCP23008_I2C::_displayfunction); 					// send as command
		
			// Set Display to on with no coursor or blinking cursor by default
			LCD_MCP23008_I2C::_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;					// set Display on, Coursor off and Blink off
			LCD_MCP23008_I2C::Display(true);																	// set _displaycontrol by the Display function
				
			// Initialize to default text direction (for roman languages)
			LCD_MCP23008_I2C::_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;							// set direction to write on the display
			LCD_MCP23008_I2C::Command(LCD_ENTRYMODESET | LCD_MCP23008_I2C::_displaymode);						// send as command
		
			// Clear the Display and go Home
			LCD_MCP23008_I2C::Clear();																			// clear the display
		}
	}
}

void LCD_MCP23008_I2C::Term(){
	i2cClose(LCD_MCP23008_I2C::_handle);																		// close i2c connection
	gpioTerminate();																							// terminate pigpio
}

uint8_t LCD_MCP23008_I2C::MCP23008_reg_read(uint8_t reg){
	uint8_t reg_data;
	reg_data=i2cReadByteData(LCD_MCP23008_I2C::_handle, reg);													// read data of given register
	return reg_data;																							// return data
}

void LCD_MCP23008_I2C::MCP23008_reg_write(uint8_t reg, uint8_t data){
	i2cWriteByteData(LCD_MCP23008_I2C::_handle, reg, data);														// write data to given MCP register 
}

void LCD_MCP23008_I2C::Send(uint8_t _data, uint8_t _mode){
	uint8_t _lownib = _data & 0x0f;																				// extract the lowest 4 bits from _data to lownib 
	uint8_t _highnib = (_data & 0xf0)>>4;																		// extract the highest 4 bits from _data to highnib and move it 4 bits down
	
	LCD_MCP23008_I2C::Send4Bits(_highnib, _mode);																// Send the high bits and the mode
	
	LCD_MCP23008_I2C::Send4Bits(_lownib, _mode);																// Send the low bits and the mode
}
	
	
void LCD_MCP23008_I2C::Send4Bits(uint8_t _data, uint8_t _mode){
	uint8_t _sendData = 0x00;																					// sendData = empty	
	_sendData = _sendData | LCD_MCP23008_I2C::backlightval | (_data<<3) | _mode;								// sendData = backlightval and 4 bits from Send moved 3 position to the left and the modebits
	 
	_sendData |= LCD_EN;																						// set enablebit to 1 in sendData (set Pulse to on)
	LCD_MCP23008_I2C::MCP23008_reg_write(REGISTER_GPIO, _sendData);												// write senddata to MCP GPIO output register
	usleep(50);																									// sleep 50µs to wait that the display read the data from the MCP output register
	
	_sendData &= ~LCD_EN;																						// set Enablebit to 0 in sendData (set pulse to off)
	LCD_MCP23008_I2C::MCP23008_reg_write(REGISTER_GPIO, _sendData);												// write senddata to MCP GPIO output register
	usleep(50);																									// sleep 50µs to wait that the display read the data from the MCP output register
}

void LCD_MCP23008_I2C::Command(uint8_t _cmd){
	LCD_MCP23008_I2C::Send(_cmd, LCD_CMD);																		// add the CMD bit with this value
}

void LCD_MCP23008_I2C::Backlight(bool _on){
	if (_on==true) {																							// if Backlight true
		LCD_MCP23008_I2C::backlightval=LCD_BACKLIGHT<<4;														// set backlightval to 0x80 (0b10000000)
	}
	else {
		LCD_MCP23008_I2C::backlightval=LCD_NOBACKLIGHT;															// set backlightval to 0x00 (0b00000000)
	}
	LCD_MCP23008_I2C::Command(0);																				// Send an Command to set the backlightval
}

void LCD_MCP23008_I2C::Display(bool _on) {
	if (_on == true) {																							// if Display true
		LCD_MCP23008_I2C::_displaycontrol |= LCD_DISPLAYON;														// set LCD_DISPLAYON with _displaycontrol 
	}
	else {
		LCD_MCP23008_I2C::_displaycontrol &= ~LCD_DISPLAYON;													// set LCD_DISPLAYON without _displaycontrol
	}
	LCD_MCP23008_I2C::Command(LCD_DISPLAYCONTROL | LCD_MCP23008_I2C::_displaycontrol);							// Send Command LCD_DISPLAYCONTROL with the new _displaycontrol data
}

void LCD_MCP23008_I2C::ShowCursor(bool _on) {
	if (_on == true) {																							// if ShowCursor true
		LCD_MCP23008_I2C::_displaycontrol |= LCD_CURSORON;
	}
	else {
		LCD_MCP23008_I2C::_displaycontrol &= ~LCD_CURSORON;
	}
	LCD_MCP23008_I2C::Command(LCD_DISPLAYCONTROL | LCD_MCP23008_I2C::_displaycontrol);							// Send Command LCD_DISPLAYCONTROL with the new _displaycontrol data
}

void LCD_MCP23008_I2C::BlinkCursor(bool _on) {
	if (_on == true) {																							// if blink true
		LCD_MCP23008_I2C::_displaycontrol |= LCD_BLINKON;														// set _displaycontrol with LCD_BLINKON
	}
	else {
		LCD_MCP23008_I2C::_displaycontrol &= ~LCD_BLINKON;														// set _displaycontrol without LCD_BLINKON
	}
	LCD_MCP23008_I2C::Command(LCD_DISPLAYCONTROL | LCD_MCP23008_I2C::_displaycontrol);							// Send Command LCD_DISPLAYCONTROL with the new _displaycontrol data
}

void LCD_MCP23008_I2C::ScrollDisplay(bool _direction) {
	if (_direction == true) {																					// if direction true
		LCD_MCP23008_I2C::Command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);							// These command scroll the display to right without changing the RAM
	}
	else {
		LCD_MCP23008_I2C::Command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);							// These command scroll the display to left without changing the RAM
	}
}

void LCD_MCP23008_I2C::Justify(bool _direction) {
	if (_direction == true) {																					// if direction true
		LCD_MCP23008_I2C::_displaymode |= LCD_ENTRYSHIFTINCREMENT;												// set _displaymode with LCD_ENTRYSHIFTINCREMENT
	}
	else {
		LCD_MCP23008_I2C::_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;												// set _displaymode without LCD_ENTRYSHIFTINCREMENT
	}
	LCD_MCP23008_I2C::Command(LCD_ENTRYMODESET | LCD_MCP23008_I2C::_displaymode);								// Send Command LCD_ENTRYMODESET with the new _displaymode data
}

void LCD_MCP23008_I2C::LeftToRight(bool _direction) {
	if (_direction == true) {																					// if direction true
		LCD_MCP23008_I2C::_displaymode |= LCD_ENTRYLEFT;														// set _displaymode with LCD_ENTRYLEFT (text will be flows Left to Right)
	}
	else {
		LCD_MCP23008_I2C::_displaymode &= ~LCD_ENTRYLEFT;														// set _displaymode without LCD_ENTRYLEFT (text will be flows Right to Left)
	}
	LCD_MCP23008_I2C::Command(LCD_ENTRYMODESET | LCD_MCP23008_I2C::_displaymode);								// Send Command LCD_ENTRYMODESET with the new _displaymode data
}


void LCD_MCP23008_I2C::Clear() {
	LCD_MCP23008_I2C::Command(LCD_CLEARDISPLAY);																// send Command LCD_CLEARDISPLAY (clear display, set cursor position to zero)
	usleep(2000);  																								// this command takes a long time!  --> sleep 2msec
}

void LCD_MCP23008_I2C::SetCursor(uint8_t _row, uint8_t _col){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };																// where the rows starts
	if ( _row > (LCD_MCP23008_I2C::rows-1) ) {																	// if the given _row higher than the rows to initialize
		_row = LCD_MCP23008_I2C::rows-1;    																	// set the given _row to max on initialize - 1, then we start count rows by 0
	}
	LCD_MCP23008_I2C::Command(LCD_SETDDRAMADDR | (_col + row_offsets[_row]));									// send Command to set Cursor Position into DRAM
}

void LCD_MCP23008_I2C::Home() {
	LCD_MCP23008_I2C::Command(LCD_RETURNHOME);																	// set cursor position to zero
	usleep(2000);																								// this command takes a long time! see Datasheet: 1.52msec --> sleep 2msec
}

void LCD_MCP23008_I2C::Print(const char _text[], int _delay){
	if (int(strlen(_text))>LCD_MCP23008_I2C::cols){																// if the given _text longer than the cols from the display
		for (int i=0; i < LCD_MCP23008_I2C::cols; i++) {														// do for character in the text til the end of the Display arrived
			LCD_MCP23008_I2C::Send((int)(_text[i]), (LCD_RW));													// send ASCII-Code of the character and dr mode LCD_RW to Send function
			usleep((_delay*1000));																				// to set the print _delay, wait the given time in msec before the next character printed
		}
	}
	else {
		for (int i=0; i < int(strlen(_text)); i++) {															// for every character in the text
			LCD_MCP23008_I2C::Send((int)(_text[i]), (LCD_RW));													// send ASCII-Code of the character and dr mode LCD_RW to Send function
			usleep((_delay*1000));																				// to set the print _delay, wait the given time in msec before the next character printed
		}
	}
}

void LCD_MCP23008_I2C::PrintLine(const char _text[], uint8_t _line){
	LCD_MCP23008_I2C::SetCursor(_line,0);																		// set coursor to the given line on first position
	LCD_MCP23008_I2C::Print(_text,0);																			// Print the given _text without delay
}
