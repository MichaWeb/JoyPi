#include <inttypes.h>													// needed for using int types like uint8_t

class SevenSegment {
	// commands
	#define CMD_SYSTEM_SETUP    		0x20
	#define CMD_KEY_DATA_ADDRESS		0x40
	#define CMD_INT_FLAG_ADDRESS		0x60
	#define CMD_DISPLAY_SETUP           0x80
	#define CMD_ROWINT_SET				0xA0
	#define CMD_DIMMING_SET      		0xE0

	// values
	#define OSCILLATOR_OFF       		0x00
	#define OSCILLATOR_ON       		0x01
	#define DISPLAY_OFF       			0x00
	#define DISPLAY_ON       			0x01
	#define BLINK_DISPLAY_2Hz			0x02
	#define BLINK_DISPLAY_1Hz			0x04
	#define BLINK_DISPLAY_halfHz		0x06
	const uint8_t dimmer[16] = {0x0F,0x0E,0x0D,0x0C,0x0B,0x0A,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00}; // used as dimmer, define the pulse width for the 7-segment LED display
	
	public:
		SevenSegment(int _i2c_addr);
		/* constructor of this class.
		 * He will be initalise the PiGPIO. If this fails, program will be display an message and exit with errorcode "EXIT_FAILTURE".
		 * Then it will be try to open an i2c comunication with the given _i2c_addr. If this fails, program will be display second message and exit with errorcode "EXIT_FAILTURE" too.
		 * If all works, then the HT16K33 LED driver and the 7-segment display will be initalise. 
		 * see Datasheet pg. 32
		*/
		~SevenSegment();
		/* destructor of this class
		 * He will stop the 7-segment display and the oscillator, close the i2c connection and terminate the PiGPIO. 
		*/
		void set_oscillator(bool _on);
		/* This function will be set the internal oscillator from the HT16K33 LED driver to the state from _on. 
		*/
		void set_display(bool _on);
		/* This function will be set the state of the display to the state from _on. 
		*/
		void set_brightness(int _dimming);
		/* This function will be set the brightness of the 7-segment display.
		 * It works with the pulse width control of the HT16K33 LED dirver.
		 * see datasheet pg 14. / pg. 15
		 * 
		 * int _dimming can be 1 ... 16
		 * _dimming 1 is the lowest dimming and the higest brightness of the display. (16/16 pulse width for the LED)
		 * _dimming 16 is the highest dimmin an the lowest brightness of the display. (01/16 pulse width for the LED)
		 * 
		 * it takes the values from uint8_t dimmer at position _dimming - 1.
		*/
		void set_blink(int _speed);
		/*This function will set the blinking speed of the 7-segment LED display by the given value od _speed.
		 * 
		 * possible values of _speed:
		 * 0 = no blinkting
		 * 1 = blinking with 0.5Hz
		 * 2 = blinkting with 1Hz
		 * 3 = blinkting with 2Hz
		*/
		void set_mirrored(bool _on=false);
		/* Function to set the mirrored option. Displayed numbers are on the head and left to right.
		 * This function set the value of _on (by default false) to the _mirrored storage.
		 * It will be used in the set_Digit function to display the Digigt mirrored or normal.
		*/
		void set_inverted(bool _on=false);
		/* Function to set the inverted option. Display LEDs will be inverted (on is off and off is on).
		 * This function set the value of _on (by default false) to the _inverted storage.
		 * It will be used in the set_Digit function to display the Digigt inverted or normal.
		*/
		void set_collon(bool _on=true);
		/* This function set the collon ":" to the value of _on (default is true).
		 * it set the dats 0x02 to the data register 0x04 to enable or data 0x00 to disable the collon.
		*/
		void set_digit(int _pos, uint8_t _data, bool _decimal=false);
		/* This function set on the digit on given _pos of the 7-segment display an hex value of given _data.
		 * This function show into the get_bitmask function to convert the given _data to the raw data of the 7-Segment LEDs. 
		 * 
		 * possible values:
		 * _pos: 0 - 3 --> stands for the digit position. 
		 * 0 = the first digit,
		 * 3 = the 4th digit of the 7-segment display
		 * 
		 * _data: 0x00 - 0x0F --> displays "0" - "F" on the display
		 * all other _data: clear the digit, not a posible value.
		 * 
		 * If _decimal true, the decimal point will be shown at this _pos.
		*/
		void set_digit_raw(int _pos, uint8_t _data);
		/* This function set on the digit on given _pos of the 7-segment display the LED bitmask by given _data.
		 * This function is NOT looking into get_bitmask() to get the LED raw data.
		 * So you can every LED address directly to power on/off.
		 * 0b11111111
		 *   ^^^^^^^^
		 *   -> MSB = decimal point										   A
		 *    -> LED "G" of the 7-segemnt display						   -
		 *     -> LED "F" of the 7-segemnt display						F | | B
		 *      -> LED "E" of the 7-segemnt display						 G -
		 *       -> LED "D" of the 7-segemnt display					E | | C
		 *        -> LED "C" of the 7-segemnt display					   - .
		 *         -> LED "B" of the 7-segemnt display					   D G
		 *          -> LED "A" of the 7-segemnt display					
		 * 
		 * possible values:
		 * _pos: 0 - 3 --> stands for the digit position. 
		 * 0 = the first digit,
		 * 3 = the 4th digit of the 7-segment display
		 * 
		 * _data: 0x00 - 0xFF --> set power on/off the LED of this digit
		*/
		void display_clear();
		/* This function clear the display and all data registers.
		 * 
		*/
		int display_selftest(bool _automatic=false);
		/* This function makes a litte selftest for the HT16K33 LED driver and the 7-segment LED display.
		 * First, it compare write and read bits to all data register. automatic test.
		 * Second, it displayed all LED. user must input that all LED's will work fine.
		 * Third, it test the 3 (4 with off) blinking modes. user must input that it works.
		 * Fourth, it test the brightness with dimming. user must input that it works. 
		 * 
		 * Input: if _automatic true, there is no userinput needed, but it can't detect errors at test 2 - 4.
		 * 
		 * return values:
		 * 0 = all test passed
		 * 1 = r/w error at the first test 
		 * 2 = Error by displaying all LED's. possible one or more LEDs are failture
		 * 3 = Error by blinking test
		 * 4 = Error by brightness test
		 * 
		 * clear display after selftest
		 * 
		 * */
	
	private:
		int _handle;
		/* id used by the i2c comunication
		*/
		bool _inverted = false;
		/* is used for inverting the display
		*/
		bool _mirrored = false;
		/* is used to mirrored the display (on the head and left to right)
		*/
		
		void send_command(uint8_t _data);
		/* This function send command to the HT16K33 LED driver
		 * 
		 * Input value: _data were set by other functions
		*/
		void send_data(int _pos, uint8_t _data);
		/* This function sends the _data to the _pos register to display it on the 7-segment display
		 * 
		 * input value: _pos and _data were set by other functions (see set_digit functions)
		*/
		
		uint8_t get_bitmask(uint8_t _data);
		/* This function convert the given _data (0 - 15) to the LED bitmask for one digit (0 - F)
		 * If mirrored true, there return an alternative bitmask.
		 * 
		 * return value: 8bit bitmask for the display
		 * 
		*/
};
