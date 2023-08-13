/* example for the DHT11 Temperature sensor
 * 
 * commands:
 * compile: g++ -Wall -c dht.cpp "%f"
 * build: g++ -Wall -o "%e" dht.cpp "%f" -lpigpio
*/

#include "dht.h"														// include the dht driver
#include <stdio.h>														// for printf
#include <unistd.h>														// used for sleep
#include <cstdlib>														// for std::system

void clear_screen(){
#ifdef WINDOWS
    std::system("cls");
#else
    std::system("clear");
#endif
}

DHT sensor1(4,DHT11);													// load class DHT11 as sensor1 with pin number 4

int main(){															
	float temp1=0.0;
	float humi1=0.0;
	bool ok1=false;
	    
	while (1){		
		clear_screen();													// clear the console

		if (sensor1.Read()==1) {										// if read Sensor1 data into array is okay
			temp1=sensor1.Get_Temp();									// get temperature from sensor and save it in temp
			humi1=sensor1.Get_Humi();									// get huminity from sensor and save it in humi
			ok1=true;													// set ok to true
		}
		printf("Messwert:\tDHT%d:\n",DHT11);							// Print console output
		printf("--------------------------------------\n");
		printf("Temperatur:\t%.1f°C\n",temp1);
		printf("Luftfeuchte:\t%.1f%%\n",humi1);
		printf("Status:\t\t%d\n",ok1);
		ok1=false;														// set ok to false
		sleep(10);
	}

	
	sensor1.Terminate();												// close connection to sensor and terminate pigpio
	return EXIT_SUCCESS;												// return exit_success
}
