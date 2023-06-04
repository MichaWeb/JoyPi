/* example for the DHT11 Temperature sensor
 * 
 * commands:
 * compile: g++ -Wall -c dht.cpp "%f"
 * build: g++ -Wall -o "%e" dht.cpp "%f" -lpigpio
*/

#include "dht.h"														// include the dht driver
#include <stdio.h>																								// for printf
#include <unistd.h>														// used for sleep
#include <cstdlib>														// for std::system

void clear_screen(){
#ifdef WINDOWS
    std::system("cls");
#else
    std::system("clear");
#endif
}

DHT sensor1(4,22);														// load class DHT11 as sensor1 with pin number 4
DHT sensor2(22,11);														// load class DHT11 as sensor2 with pin number 4

int main(){															
	float temp;															// create float vatiable temp for the temperature
	float humi;															// create float variable humi for the humidity
    
	while (1){		
		clear_screen();													// clear the console

		if (sensor1.Read()==1) {											// if read Sensor1 data into array is okay
			temp=sensor1.Get_Temp();										// get temperature from sensor and save it in temp
			humi=sensor1.Get_Humi();										// get huminity from sensor and save it in humi
			
			printf("Sensor1:\n");
			printf("Themperatur:\t%.1f°C\n",temp);							// print temperature as float
			printf("Luftfeuchte:\t%.1f%%\n",humi);							// print humidity as float
		}
		else {
			printf("Sensor1: error\n\n");	
		}
		if (sensor2.Read()==1) {											// if read Sensor1 data into array is okay
			temp=sensor2.Get_Temp();										// get temperature from sensor and save it in temp
			humi=sensor2.Get_Humi();										// get huminity from sensor and save it in humi
			
			printf("\n\nSensor2:\n");										// print temperature as float
			printf("Themperatur:\t%.1f°C\n",temp);							// print temperature as float
			printf("Luftfeuchte:\t%.1f%%\n",humi);							// print humidity as float
		}
		else {
			printf("\nSensor2: error\n\n");	
		}
		sleep(30);
	}

	
	sensor1.Terminate();												// close connection to sensor and terminate pigpio
	sensor2.Terminate();
	return EXIT_SUCCESS;
}
