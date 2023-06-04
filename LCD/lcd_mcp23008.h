#include <inttypes.h>													// used for the int types like uint8_t

class LCD_MCP23008_I2C{
	/* class for an LCD Display with an MCP23008 controler and an I2C comunication. 
	 * This is used by JoyPi  
	 * 
	 *  Initialise the MCP23008 controler
	 * 
	 * 			The MCP23008:			The LCD Display
	 *          .---   ---.
	 *    SCK-->|1  \_/ 18|<--VDD
	 *     SI-->|    M    |<->GPA7 ----> Backlight
	 *     SO<--|    C    |<->GPA6 <---> data bit 3
	 *     A1-->|    P    |<->GPA5 <---> data bit 2
	 *     A0-->|    2    |<->GPA4 <---> data bit 1
	 * ~RESET-->|    3    |<->GPA3 <---> data bit 0
	 *    ~CS-->|    0    |<->GPA2 ----> Enable bit
	 *    INT<--|    0    |<->GPA1 ----> R/W bit
	 *    VSS-->|    8    |<->GPA0 ----> Command / RS bit
	 *          '---------'
	 * 
	 * Hardware Address Pins(A0-A2)
	 *        A2    A1    A0        7-bit slave address
	 *       GND   GND   GND              0x20
	 *       GND   GND   VCC              0x21
	 *       GND   VCC   GND              0x22
	 *       GND   VCC   VCC              0x23
	 *       VCC   GND   GND              0x24
	 *       VCC   GND   VCC              0x25
	 *       VCC   VCC   GND              0x26
	 *       VCC   VCC   VCC              0x27
	 * 
	 */
	
	/* Used by MCP23008 */
	#define REGISTER_IODIR				0x00								// register for Input / Output direction (0 = output / 1 = input for each pin)
	#define REGISTER_IOPOL				0x01								// register for Input / Output polarisation (0 = noninverted / 1 = inverted for each pin)
	#define REGISTER_GPINTEN			0x02
	#define REGISTER_DEFVAL				0x03
	#define REGISTER_INTCON				0x04
	#define REGISTER_IOCON				0x05
	#define REGISTER_GPPU				0x06								// register for PullUp resistors (0 = disable pullup resistors / 1 = configure internal PullUp resistor for pin)
	#define REGISTER_INTF				0x07
	#define REGISTER_INTCAP				0x08
	#define REGISTER_GPIO				0x09								// register for pin state ( 0 = GPIO-Pin are low / 1 = GPIO-Pin are high)
	#define REGISTER_OLAT				0x0A
	#define MCP23008_IODIR_ALL_OUTPUT	0x00
	#define MCP23008_IODIR_ALL_INPUT	0xFF
	#define MCP23008_IPOL_ALL_NORMAL    0x00
	#define MCP23008_IPOL_ALL_INVERTED  0xFF
	#define MCP23008_GPPU_ALL_DISABLED  0x00
	#define MCP23008_GPPU_ALL_ENABLED   0xFF
	#define MCP23008_GPPU_ALL_DISABLED  0x00
	#define MCP23008_GPPU_ALL_ENABLED   0xFF
	#define MCP23008_GPIO_ALL_LOW     	0x00
	#define MCP23008_GPIO_ALL_HIGH    	0xFF

	/* used by the LCD display */
	#define LCD_EN 						4									// Enable Bit
	#define LCD_RW 						2									// Read/Write Bit
	#define LCD_RS 						1									// Register select Bit
	#define LCD_CMD 					0									// Bit for Commands

	#define LCD_CLEARDISPLAY 			0x01								// command "clear display"
	#define LCD_RETURNHOME 				0x02								// command "return home"
	#define LCD_ENTRYMODESET 			0x04								// command "entry mode"
	#define LCD_DISPLAYCONTROL 			0x08								// command "display control"
	#define LCD_CURSORSHIFT 			0x10								// command "cursor shift"
	#define LCD_FUNCTIONSET 			0x20								// command "funclion set"
	#define LCD_SETCGRAMADDR 			0x40								// command "set CGRAM adress"
	#define LCD_SETDDRAMADDR 			0x80								// command "set DDRAM adress"

	#define LCD_8BITMODE 				0x10
	#define LCD_4BITMODE 				0x00
	#define LCD_2LINE 					0x08
	#define LCD_1LINE 					0x00
	#define LCD_5x10DOTS 				0x04
	#define LCD_5x8DOTS 				0x00

	#define LCD_ENTRYRIGHT 				0x00
	#define LCD_ENTRYLEFT 				0x02
	#define LCD_ENTRYSHIFTINCREMENT 	0x01
	#define LCD_ENTRYSHIFTDECREMENT 	0x00

	#define LCD_DISPLAYON 				0x04
	#define LCD_DISPLAYOFF 				0x00
	#define LCD_CURSORON 				0x02
	#define LCD_CURSOROFF 				0x00
	#define LCD_BLINKON 				0x01
	#define LCD_BLINKOFF 				0x00

	#define LCD_DISPLAYMOVE 			0x08
	#define LCD_CURSORMOVE 				0x00
	#define LCD_MOVERIGHT 				0x04
	#define LCD_MOVELEFT 				0x00

	#define LCD_BACKLIGHT 				0x08
	#define LCD_NOBACKLIGHT 			0x00
	
public:
	/* public functions for the user */
	LCD_MCP23008_I2C(int _addr, int rows, int cols);													// constructor --> set variables for the class
	virtual ~LCD_MCP23008_I2C();																		// destructor
	void Init();																						// initialise the display conection and config
	void Term();																						// terminate the display
	void Backlight(bool _on);																			// turn on/off the backlight
	void Display(bool _on);																				// turn on/off the display
	void ShowCursor(bool _on);																			// turn on/off the cursor
	void BlinkCursor(bool _on);																			// turn on/off the blinkcursor
	void ScrollDisplay(bool _direction);
	void Justify(bool _direction);
	void LeftToRight(bool _direction);
	void Clear();																						// clear the display
	void Home();																						// set cursor to start position (0,0)
	void SetCursor(uint8_t _row, uint8_t _col);															// set cursor to Poition (x,y)
	void Print(const char _text[], int _delay);															// print an text at current position
	void PrintLine(const char _text[], uint8_t _line);													// print an text at the given line starts on position 0

private:
	/* private functions for the MCP23008 expander */
	uint8_t MCP23008_reg_read(uint8_t reg);
	void MCP23008_reg_write(uint8_t reg, uint8_t data);

	/* private functions for the LCD Display */
	void Send(uint8_t _data, uint8_t _mode);
	void Send4Bits(uint8_t _data, uint8_t _mode);
	void Command(uint8_t _cmd);


	/* private variables for the class */
	uint8_t addr;
	uint8_t rows;
	uint8_t cols;
	uint8_t lines;
	unsigned _handle;
	uint8_t backlightval;
	uint8_t _displayfunction;
	uint8_t _displaycontrol;
	uint8_t _displaymode;
};
