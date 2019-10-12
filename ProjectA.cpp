
/*
 * Project A
 * Khaya Mxenge and Riselle Rawthee
 * 
 * 
 * MXNKHA002 RWTRIS001
 * Date October 2019
*/

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h> //For printf functions
#include <stdlib.h> // For system functions
#include <mcp3004.h>



#include "ProjectA.h"

#define BASE 100
#define SPI_CHAN 0
//functions
void StartStop(void);
void setUpADC(void);
void startOperation(void);
void stopOperation(void);
void alarmPWM(void);
//Global variables
int hours, mins, secs;
long lastInterruptTime = 0; //Used for button debounce
int RTC; //Holds the RTC instance
int Temp;
int Light;
int Humidity;
double Vout;
//const int BTNS[]= {0,2,3,13};
bool start=false;


//set up ADC
void setUpADC(){
    
    mcp3004Setup(BASE,SPI_CHAN);
    wiringPiSPISetup(0,500000);

}

//Read from ADC
void readADC(){
	Temp = analogRead(BASE+4);
	Light = analogRead(BASE+2);
	Humidity = analogRead(BASE);
	usleep(500000);
}

//Calculate Vout
//void

void CalcVout()
{
  Vout =((Light/(double)1023)*(Humidity/(double)1023))*3.3;

}
//setup buttons
void setupGPIO(){
	wiringPiSetup();
	//set up the LED for PWM for alarm
	pinMode(1,PWM_OUTPUT);
}


// PWM for alarm
void alarmPWM(void)
{
  double PWMVout=(Light/(double)1023)*Humidity;
  //double PWMVout=0.6;
  pwmWrite(1,PWMVout);
  printf("PWMVout is: %2f\n",PWMVout);
}
int main ()
{

	setUpADC();

	setupGPIO();
	
	printf("setup complete\n");
	// Loop indefinitely, waiting for 100ms between each set of data
	for(;;)
	{	readADC();
		printf("Temp is: %4d\n",Temp);
		printf("Light is: %4d\n",Light);
		printf("Humidity is: %4d\n",Humidity);
		CalcVout();
		printf("Vout is: %4f\n",Vout);
		alarmPWM();
		printf("\n");
		
	}
}
