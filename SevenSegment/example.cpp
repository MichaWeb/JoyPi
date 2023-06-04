/* example for the SevenSegment Display with an HT16K33 driver
 * 
 * commands:
 * compile: g++ -Wall -c SevenSegment.cpp "%f"
 * build: g++ -Wall -o "%e" SevenSegment.cpp "%f" -lpigpio
*/

#include "SevenSegment.h"																			// own header file
#include <unistd.h>																					// needed for sleep / usleep

int main(int argc, char **argv) {
    SevenSegment driver(0x70);																		// initialise the 7-segemnt as driver
    uint32_t count=0x00000000;																		// set count to 0 
    
    driver.display_clear();																			// clear the 7-segment display
	driver.set_brightness(16);																		// set brightnett to low (powersaving)
	driver.set_blink(0);																			// no blinking
	driver.set_collon(false);																		// no collon

	while (count <= 65535) {
		driver.set_digit(0,(((count/16)/16)/16)%16);												// extract the highest hex digit from count
		driver.set_digit(1,((count/16)/16)%16);
		driver.set_digit(2,(count/16)%16);
		driver.set_digit(3,count%16);																// extract the lowest hex digit from count
		count++;
	}
	sleep(5);																						// wait 5 seconts till end
	
    return 0;
}
