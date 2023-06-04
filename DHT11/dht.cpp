/* JoyPi C++ driver with PIGPIO */
#include "dht.h"																								// own header file
#include <pigpio.h>																								// used for PiGPIO
#include <stdlib.h>																								// used for exit function
#include <stdio.h>																								// for printf


/* DHT temperature and huminity sensor */
DHT::DHT(int _pin, int _type){
	
	if (gpioInitialise() < 0){											// if initialisation of pigpio failed
		printf("##############################################\n");
		printf("#              Can't use PIGPIO!             #\n");
		printf("# Maybe PIGPIO is used by an other instance? #\n");
		printf("##############################################\n");
		exit (EXIT_FAILURE);											// exit the program
	}
	else{																// pigpio initialised okay.
		if (_pin>=0 && _pin<=26){ 										// pin number looks valid? (BCM number needed)
			DHT::pin = _pin;											// set pin to given _pin number
		}
		else {
			DHT::pin = DHT_PIN;											// set default pin
		}
		DHT::_type = _type;
		gpioSetMode(DHT::pin, PI_OUTPUT);								// set the pin as output
		gpioSetPullUpDown(DHT::pin, PI_PUD_OFF);						// turn off every Pull resistors
	}	
}

DHT::~DHT(){
	gpioTerminate();													// close GPIO conection
}

int DHT::Read(){
	/* Initialize the values */
	int returnValue = 0;
	uint8_t lststate = PI_HIGH;
	uint8_t counter = 0, j = 0, i = 0;
	for (i = 0; i < 5; i++)
	{
		DHT_val[i] = 0;
	}
	
	/* Signal the sensor to send data */
	gpioSetMode(DHT::pin, PI_OUTPUT);									// set pin as output
	gpioWrite(DHT::pin,PI_LOW);											// set pin to low (0)
	gpioDelay(20000);													// Datasheet states that we should wait 18ms --> 20ms to be save
	
	/* Set the pin to high and switch to input mode */
	gpioWrite(DHT::pin,PI_HIGH);										// set pin to high (1)
	gpioDelay(50);														// sleep 20us - 40us --> 50µs to be save
	gpioSetMode(DHT::pin, PI_INPUT);									// set pin as input
	gpioSetPullUpDown(DHT::pin,PI_PUD_UP);								// turn on pull up resistor
	
	
	/* Get the bits */
	for (i = 0; i < DHT_MAX_TIME; i++)
	{
		counter = 0;
		while (gpioRead(DHT::pin) == lststate)
		{
			counter++;
			gpioDelay(1);

			if (counter == 255)
			{
				break;
			}
		}

		lststate = gpioRead(DHT::pin);

		if (counter == 255)
		{
			break;
		}

		// ***
		// *** Top 3 transitions are ignored
		// ***
		if ((i >= 4) && (i % 2 == 0))
		{
			DHT_val[j / 8] <<= 1;
			
			if (counter > 16)
			{
				DHT_val[j / 8] |= 1;
			}

			j++;
		}
	}
	
	/* Verify checksum and print the verified data */
	if ((j >= 40) && (DHT_val[4] == ((DHT_val[0] + DHT_val[1] + DHT_val[2] + DHT_val[3]) & 0xFF))){
		returnValue = 1;
	}
	else{
		returnValue = 0;
	}
	
	return returnValue;
}

float DHT::Get_Temp(){
	float returnTemp=0.0;
	
	switch(DHT::_type){
		case DHT11:
			returnTemp = int(DHT_val[2]);
			if (DHT_val[3] & 0x80){
				returnTemp = -1 - returnTemp;
			}
			returnTemp += int(DHT_val[3] & 0x0f)/10.0;
			break;
		case DHT22:
			returnTemp = ((int(DHT_val[2] & 0x7F) << 8) + int(DHT_val[3])) / 10.0;
			if (DHT_val[2] & 0x80){
				returnTemp *= -1;
			}
			break;
	}
	
	return returnTemp;
}

float DHT::Get_Humi(){
	float returnHumi=0.0;

	switch(DHT::_type){
		case DHT11:
			returnHumi= int(DHT_val[0]) + int(DHT_val[1])/10.0;
			break;
		case DHT22:
			returnHumi = ((int(DHT_val[0] << 8)) + int(DHT_val[1])) / 10.0;
			break;
	}
	
	
	return returnHumi;
}

void DHT::Terminate(){
	gpioTerminate();
}
