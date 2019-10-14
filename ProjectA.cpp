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
#include <mcp3004.h>//for ADC
#include "ProjectA.h"

#define BASE 100
#define SPI_CHAN 0
//functions
void alarmPWM(void);
void TableValues(void);
//Global variables
int hours, mins, secs;
long lastInterruptTime = 0; //Used for button debounce
int RTC; //Holds the RTC instance
int Temp;
int Light;
int Humidity;
double Vout;
double TableTemp;
double TableHumidity;
double TableLight;
bool start=false;
int HH,MM,SS;
const char RTCAddr = 0x6f;
const char SEC = 0x00; // see register table in datasheet
const char MIN = 0x01;
const char HOUR = 0x02;
void initGPIO(void);
void alarm (void);
void resetfunc (void);
void  startRTC (void);
int d;
int mode=1;
void freqfunc (void);
int VoutAlarm;

void initGPIO(void){
// Sets GPIO using wiringPi pins. see pinout.xyz for specific wiringPi pi
	printf("Setting up\n");
	RTC = wiringPiI2CSetup(RTCAddr); //Set up the RTC
	// setting up buttons
	const int BTNS[] = {26,27,28,29}; // B0, B1
	for(int j=0; j < 4; j++)
	{
		pinMode(BTNS[j], INPUT);
		pullUpDnControl(BTNS[j], PUD_UP);
	}
	pinMode(1, OUTPUT);
	wiringPiISR (27, INT_EDGE_FALLING,startRTC); // setting up interupt for button to call method hourInc.
	wiringPiISR (26, INT_EDGE_FALLING,resetfunc);
	wiringPiISR (28, INT_EDGE_FALLING,freqfunc);
	wiringPiISR (29, INT_EDGE_FALLING,alarm);
	printf("BTNS done\n");
	printf("Setup done\n");
} // end of setup function

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
	//usleep(500000);
}

//Calculate Vout
void CalcVout()
{
  Vout =((Light/(double)1023)*(Humidity/(double)1023))*3.3;

}//end calc Vout


void TableValues(){
	//temp
	
	TableTemp = (((Temp/(double)1023)*3.3)-0.5)/(double)0.1;
	TableHumidity = (Humidity/(double)1023)*3.3;
	TableLight= Light;
}//end Table Values


// PWM for alarm
void alarmPWM(void)
{
  if((Vout<0.25)||(Vout>2.65))
  {double PWMVout=(Light/(double)1023)*Humidity;
   pinMode(1,PWM_OUTPUT);
   pwmWrite(1,PWMVout);
   printf("PWMVout is: %2f\n",PWMVout);
   for (int i=0;i<10;i++)
    {
          delay(1000);
          digitalWrite(1,LOW);
     }
	
  }
}

int main () {
	wiringPiSetup();
	setUpADC();
	initGPIO();	
	printf("setup complete\n");
	// Loop indefinitely, waiting for 100ms between each set of data
	for(;;)
	{
	}
}

void resetfunc (void){
long interruptTime = millis();
 if (interruptTime - lastInterruptTime>250){
    printf("resetfunc triggered");
    wiringPiI2CWriteReg8(RTC, HOUR, 0x0); // Writing a default time to the RTC
    wiringPiI2CWriteReg8(RTC, MIN, 0x0);
    wiringPiI2CWriteReg8(RTC, SEC, 0x80);
    mode=1;
    d=1000;
    printf("%x:%x:%x\n", HH, MM, SS); //} end for loop
  } // end dounounce if
 lastInterruptTime = interruptTime;
}   // end of reset function

void freqfunc (void)
{ long interruptTime = millis();
    if (interruptTime - lastInterruptTime>250){
    printf("freqfunc triggered");
    mode = mode  + 1;
    if (mode ==1 )
        {
                d= 1000;

        }// end of  if 1
     if (mode ==2)
        {
                d= 2000;

        }
      if (mode==3)
        {
                d=5000;
                mode=0;
        }
        printf("%d",mode);
}// end debounce

lastInterruptTime = interruptTime; // debouncing

}// end of delay fuunction


void  startRTC (void)
{
d=1000;
long interruptTime = millis();
if (interruptTime - lastInterruptTime>250){
        printf("startRTC triggered");
        //Fetch the time from the RTC
        wiringPiI2CWriteReg8(RTC, HOUR, 0x0); // Writing a default time to the RTC
        wiringPiI2CWriteReg8(RTC, MIN, 0x0);
        wiringPiI2CWriteReg8(RTC, SEC, 0x80);


for(;;)
{
        HH=wiringPiI2CReadReg8(RTC, HOUR);
        MM=wiringPiI2CReadReg8(RTC, MIN); // fetching time from RTC and storing it
        SS=wiringPiI2CReadReg8(RTC, SEC);
        SS= SS-0x80;
        delay(d);
        printf("%x:%x:%x\n", HH, MM, SS);
	readADC();
        CalcVout();
        printf("Vout is: %4f\n",Vout);
        alarmPWM();
        printf("\n");
        TableValues();
        printf("TempTable is: %4f\n",TableTemp);
        printf("Light is: %4f\n",TableLight);
        printf("Humidity is: %4f\n",TableHumidity);
        printf("\n");

}// end of for loop
}//end of if

lastInterruptTime = interruptTime;
}

/*void alarm (void)
{
long interruptTime = millis();
if(interrputTime - lastInterrputTime>250)
{
  printf ("alarm triggered");  
  // start sensing
  printf("dismiss alarm");
  digitalWrite(1, LOW);

}// end longof if statement
lastInterruptTime = interruptTime;
}//end of alarm*/

void alarm(){
	long interruptTime = millis();
	if (interruptTime - lastInterruptTime>250){
		printf("dismiss alarm");
		digitalWrite(1, LOW);
	
	}//end iff
	lastInterruptTime=interruptTime;
}
