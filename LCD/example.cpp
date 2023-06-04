/* example for the LCD Display
 * 
 * commands:
 * compile: g++ -Wall -c lcd_mcp23008.cpp "%f"
 * build: g++ -Wall -o "%e" lcd_mcp23008.cpp "%f" -lpigpio
*/

#include "lcd_mcp23008.h"												// include the driver
#include <unistd.h>														// for usleep
#include <stdio.h>														// for char

LCD_MCP23008_I2C LCD1(0x21,2,16);



int main(){
    LCD1.Init();
	LCD1.Backlight(true);
	LCD1.ShowCursor(true);
	LCD1.BlinkCursor(true);
	
	LCD1.Print("Hello World...",250);
	sleep(2);
	LCD1.ScrollDisplay(false);
	sleep(1);
	LCD1.ScrollDisplay(false);
	sleep(1);
	LCD1.ScrollDisplay(false);
	sleep(1);
	LCD1.ScrollDisplay(false);
	sleep(1);
	
	LCD1.SetCursor(1,5);
	sleep(2);
	
	LCD1.Backlight(false);
	LCD1.Term();
}
