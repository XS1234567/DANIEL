
//Terminal Servo Control for Raspberry Pi2
//ServoTest.c
//by Artin Isagholian
//2/2/2017
#include <stdio.h>
#include <bcm2835.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "SystemaCV.h"
void Enable(int pinFata)
{
bcm2835_gpio_set(pinFata);
bcm2835_delayMicroseconds(100);
bcm2835_gpio_clr(pinFata);//pin18 low

}
void move(int pin,int angle, int prevangle, int hold, int align)
{
    int pulse;
    int j;
    float hightime = 10.3*(float)angle + 546;
    char c;
    if(hold == 1)
    {
        printf("Holding indefinitely\n");
        while(1)
        {
            bcm2835_gpio_set(pin);//pin18 high
            bcm2835_delayMicroseconds(hightime);
            bcm2835_gpio_clr(pin);//pin18 low
            bcm2835_delayMicroseconds(5000 - hightime);//each pulse is 20ms
        }
    } 
    if(hold == 0)
    {
        if(align == 1)
        {
            //it takes roughly 33 for 180 degrees of rotation, using 40 for good measure
            pulse = 40;
        }
        else
        {
            //calculate number of required pulses + 10 for good measure
            pulse = (int)abs(((angle - prevangle)*33)/180) + 10;
        }
        //printf("Hightime: %f\n", hightime);
        //printf("Pulse: %d\n", pulse);
        for(j=0; j<pulse; j++)//exucting pulses
        {
            //printf("%d", j);
 			bcm2835_gpio_set(pin);//pin18 high
			bcm2835_delayMicroseconds((int)hightime);
			bcm2835_gpio_clr(pin);//pin18 low
			bcm2835_delayMicroseconds(5000 - (int)hightime);//each pulse is 20ms
        }
    } 
    
}

int input_check(char* input)
{   int error = 0;
    int i;
    int length = strlen(input);
    if (length==4)
	{
		input[5] = '\n';
	} 	   
	
	if(length>4 || input[length-1]!='\n' || (length!=2 && input[0]=='0') || input[0]=='\n')
	{
		error=1;
	}
	if(length==2 && input[0]=='q' && input[1]=='\n' && error==0)
    {
        	error = 2;
	}
    if(error==0)
   	{
        for (i=0;i<(length-1); i++)
        {
            if (!isdigit(input[i]) || isspace(input[i]))
            {
                error=1;
                continue;
            }
        }
	int angle = (int)strtol(input,NULL,10);
		if ( (angle>180) || (angle<0) || (error==1)) //invalid input
		{
		error = 1;
		}
    }
    
    return error;
}
void initServo()
{
if(!bcm2835_init()) return 1;
bcm2835_gpio_fsel(17, BCM2835_GPIO_FSEL_OUTP);
bcm2835_gpio_fsel(18, BCM2835_GPIO_FSEL_OUTP);
}
void moveDC(int pin,int angle, int prevangle, int hold, int align)
{
    int pulse;
    int j;
    float hightime = 10.3*(float)angle + 546;
    char c;
    if(hold == 1)
    {
        printf("Holding indefinitely\n");
        while(1)
        {
            bcm2835_gpio_set(pin);//pin18 high
            bcm2835_delayMicroseconds(hightime);
            bcm2835_gpio_clr(pin);//pin18 low
            bcm2835_delayMicroseconds(200);//each pulse is 20ms
        }
    } 
    if(hold == 0)
    {
        if(align == 1)
        {
            //it takes roughly 33 for 180 degrees of rotation, using 40 for good measure
            pulse = 40;
        }
        else
        {
            //calculate number of required pulses + 10 for good measure
            pulse = (int)abs(((angle - prevangle)*33)/180) + 10;
        }
        //printf("Hightime: %f\n", hightime);
        //printf("Pulse: %d\n", pulse);
        for(j=0; j<pulse; j++)//exucting pulses
        {
            //printf("%d", j);
 			bcm2835_gpio_set(pin);//pin18 high
			bcm2835_delayMicroseconds((int)hightime);
			bcm2835_gpio_clr(pin);//pin18 low
			bcm2835_delayMicroseconds(5000 - (int)hightime);//each pulse is 20ms
        }
    } 
    
}
void Servo(int angle,int prevangle)
{

    
 

    //move(17,90, 90, 0, 1); //align to 0 degree point
    //move(18,0, 0, 0, 1);


move(17,65, 0, 0, 1);


}
