#ifndef __scale__
#define __scale__


#include <Ticker.h>

/*#define D5 14
  #define D6 12
  #define D7 13
  #define D8 15*/
//byte stepPinsArr[] = {D5,D6,D7,D8};
byte stepIdx, stepPin, stepIdxPrev;
//int stepPinsArr[] = {16, 14, 12, 13};
int stepPinsArr[] = {15, 14, 12, 13};
int stepHalfArr[] = {0b1000, 0b1001, 0b0001, 0b0011, 0b0010, 0b0110, 0b0100, 0b1100};

//const int opticalLedPin = 15;
//const int opticalLedPin = 16;
const int opticalLedPin = 0;


//optical sensor
int opticalValue, /*v0, v1,*/ optDiff, diffMin, diffMinBuf, diffMax, diffMaxBuf, diffPrev, diffSens, diffLowPass, diffCC;
long diffLastMin, diffLastMax;
long weightRaw;

//stepper motor

long motorPosition, motorLastPos, motorTarget;
int motorDirection;

bool motorOn;
#define periodSuperFast 3
#define periodFast 80
#define periodSlow 150
#define periodSuperSlow 300



#define debugMotor 0


//timer
long period;
unsigned long lastStep, lastOpticalLoop, lastReadReg;

//pesee
#define stepToGram 0.64


int motorCheck;
//#define motorCheckPin 3
#define motorCheckPin 16

Ticker Timer1;


void scaleInit();
void timerIsr();
void opticalLoop();
long rechercheEquilibre();
void motorStepIdx0();
void rechercheMotorCheck();

#endif
