#include <inttypes.h>													// used for the int types like uint8_t

#define DHT11 						11
#define DHT22 						22
#define DHT_PIN 					4
#define DHT_MAX_TIME 				85

class DHT {
	/* class for the dht11 temperature and huminity sensor
	 * datasheet for the sensor:
	 * https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf
	 * 
	 * driver based on:
	 * https://github.com/porrey/Dht/blob/25f64f68d901da6f94aac10c1b74c9370b63c6b9/source/Raspbian/Dht11_Speed.c 
	 * 
	 * alternative base:
	 * https://github.com/Backbone81/raspberry-pi/blob/2d17d2f758db2323cbfd1430d350577db875e0e7/kernel-space/example01-dht11/dht11.c
	 * 
	*/
public:
	DHT(int _pin, int _type);														// constructor
	/* give the constructor the BCM number of the pin
	 * who the dht11 sensor are connected
	 * 
	*/
	virtual ~DHT();														// desructor
	int Read();
	/* read the 40 bit from the DHT11 sensor
	 * check the checksum and if it correct
	 * save the values in the dht11_val array.
	 * 
	 * DHT_val[0] --> humidity int
	 * DHT_val[1] --> humidity decimal
	 * DHT_val[2] --> temperature int 
	 * DHT_val[3] --> temperature decimal
	 * DHT_val[4] --> checksum 
	 * 
	 * 
	 * return the state of reading
	 * 1 = correct
	 * 0 = error
	 * 
	*/
	float Get_Temp();													// return temp as float value
	/* call Read till checksum is correct (max 5 times)
	 * then return the temperature
	 * value as an float value 
	 * 
	 * return the temerature as float
	 * dht11_val[2] + dht11_val[3]/10.0
	*/
	float Get_Humi();													// return humidity as float value
	/* call Read till checksum is correct  (max 5 times)
	 * then return the humidity
	 * value as an float value 
	 * 
	 * return the humidity as float
	 * dht11_val[0] + dht11_val[1]/10.0
	*/
	void Terminate();
	/* close and terminate the pigpio conection with the DHT11 sensor
	 * 
	*/
	
	
private:
	uint8_t pin;														// used for the pin connector number (BCM number)
	int DHT_val[5] = {0,0,0,0,0};										// array for the data from the dht11 sensor
	int _type;
};
