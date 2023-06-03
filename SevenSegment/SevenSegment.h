#include <inttypes.h>  // needed for using int types like uint8_t

class SevenSegment {
	// commands
    #define CMD_SYSTEM_SETUP                0x20
	#define CMD_KEY_DATA_ADDRESS        0x40
	#define CMD_INT_FLAG_ADDRESS        0x60
	#define CMD_DISPLAY_SETUP           0x80
	#define CMD_ROWINT_SET			0xA0
	#define CMD_DIMMING_SET			0xE0

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
		void set_digit(int _pos, uint8_t _data, bool _decimal=false);
		void set_digit_raw(int _pos, uint8_t _data);
		void display_clear();
		int display_selftest();
	
	private:
		int _handle;
		bool _inverted = false;
		bool _mirrored = false;
		
		void send_command(uint8_t _data);
		void send_data(int _pos, uint8_t _data);
		uint8_t getBitmask(uint8_t _data);
}
