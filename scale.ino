#include "scale.h"

void scaleInit() {
  for (int i = 0; i < 4; i++) {
    pinMode(stepPinsArr[i], OUTPUT);
    digitalWrite(stepPinsArr[i], LOW);
  }

  motorOn = false;

  
  Timer1.attach_ms(2, timerIsr);
  lastStep = millis();
  period = periodSuperFast;


  //pinMode(0, INPUT);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);


  //optical sensor
  pinMode(opticalLedPin, OUTPUT); //power supply
  digitalWrite(opticalLedPin, HIGH);
  pinMode(A0, INPUT);

  pinMode(motorCheckPin,INPUT);  
}

void timerIsr()
{


  if (((millis() - lastStep) >= 2000) && motorOn) {
    if (stepIdx != 0) {
      motorTarget -= stepIdx;
    } else {
      motorOn = false;
      for (int i = 0; i < 4; i++)
        digitalWrite(stepPinsArr[i], LOW);

      if (debugMotor) {
        debug("M0" + String(stepIdx));
        StoreEEPROM();
      }
    }
  }

  if ((millis() - lastStep) >= period) {
    motorDirection = (motorTarget == motorPosition) ? 0 : (motorTarget < motorPosition ? -1 : 1);

    if (motorDirection != 0) {
      lastStep = millis();

      if (motorOn) {
        motorPosition += motorDirection;
        stepIdx += motorDirection;
      } else {
        if (debugMotor) {
          debug("M>" + String(stepIdx));
        }
      }

      motorOn = true;

      if ((stepIdx == -1) || (stepIdx == 255)) stepIdx = 7;
      if (stepIdx == 8) stepIdx = 0;

      for (int i = 0; i < 4; i++) {
        if ((motorLastPos != motorPosition) && ((stepHalfArr[stepIdx] >> i) & 1))
          digitalWrite(stepPinsArr[i], HIGH);
        else
          digitalWrite(stepPinsArr[i], LOW);
      }


      motorLastPos = motorPosition;
    }

  }
}


void opticalLoop() {

  opticalValue = 1023 - analogRead(A0);
  //v1 = analogRead(A1);
  motorCheck = digitalRead(motorCheckPin);
    
  optDiff = opticalValue - 550;


  if (diffSens == 1) {
    if (optDiff < diffPrev) diffLowPass ++;
    else diffLowPass = 0;

    if (optDiff > diffMaxBuf) diffMaxBuf = optDiff;

    if (diffLowPass > 2) {
      diffMinBuf = optDiff;
      diffMax = diffMaxBuf;
      diffSens = -1;
      diffLastMax = millis();
    }

  } else {
    if (optDiff > diffPrev) diffLowPass ++;
    else diffLowPass = 0;

    if (optDiff < diffMinBuf) diffMinBuf = optDiff;

    if (diffLowPass > 2) {
      diffMaxBuf = optDiff;
      diffMin = diffMinBuf;
      diffSens = 1;
      diffLastMin = millis();
    }
  }
  diffPrev = optDiff;


  if (((millis() - diffLastMin) > 4000) || ((millis() - diffLastMax) > 4000))
    diffCC = optDiff;
  else
    diffCC = (diffMax + diffMin) / 2;

}


long rechercheEquilibre() {
  bool continueLoop = true;
  unsigned long lastMotorAction = millis();
  unsigned long timeout = millis() + 60000;
  long motorDirection;
  long motorLastBalance;

  opticalLoop();


  if ((opticalValue < 700) && (opticalValue > 300)) {
    motorTarget -= 50;
    delayWithWebHandle(1000);
  }


  if (opticalValue > 700) {
    while ((opticalValue > 700) && (millis() < timeout)) {
      motorTarget -= 2;
      delayWithWebHandle(10);
      opticalLoop();
    }
  }

  delayWithWebHandle(2000);
  opticalLoop();


  while ((opticalValue < 300) && (millis() < timeout)) {
    motorTarget += 2;
    delayWithWebHandle(10);
    opticalLoop();
  }


  delayWithWebHandle(2000);
  opticalLoop();

  while ((opticalValue > 700) && (millis() < timeout)) {
    motorTarget -= 2;
    delayWithWebHandle(100);
    opticalLoop();
    if (debugMotor) {
      debug("lent 100 ", true);
      debug(motorPosition, true);
      debug(" / ", true);
      debug(opticalValue);
    }
  }


  for (int i = 0; i < 40; i += 2) {
    motorTarget += 2;
    delayWithWebHandle(100);
  }


  delayWithWebHandle(1000);
  opticalLoop();

  while ((opticalValue < 400) && (millis() < timeout)) {
    motorTarget += 2;
    delayWithWebHandle(1000);
    opticalLoop();
    if (debugMotor) {
      debug("lent 1000 %i", true);
      debug(motorPosition, true);
      debug(" / ", true);
      debug(opticalValue);
    }
  }


  motorLastBalance = motorPosition;

  debug("rechercheEquilibre terminé : ", true);
  debug(motorLastBalance);

  motorStepIdx0();

  return motorLastBalance;
}

void motorStepIdx0() {
  motorTarget -= stepIdx;
  while (motorTarget != motorPosition) delay(10);
  StoreEEPROM();
}


void rechercheMotorCheck() {
  long positionResult;
  bool continueLoop = true;
  unsigned long lastMotorAction = millis();
  unsigned long timeout = millis() + 5000;
  long motorDirection;

  motorCheck = digitalRead(motorCheckPin);
  if (motorCheck) {
    motorTarget -= 200;
    delayWithWebHandle(1000);
  }
  motorCheck = digitalRead(motorCheckPin);


  while ((!motorCheck) && (millis() < timeout)) {
    motorTarget += 2;
    delayWithWebHandle(10);
    motorCheck = digitalRead(motorCheckPin);
  }

  positionResult = motorPosition;

  motorStepIdx0();

  debug("checkMotor : ", true);
  debug(positionResult);
}

