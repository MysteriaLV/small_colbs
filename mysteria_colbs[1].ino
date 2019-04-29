#include <Bounce2.h>
#include "colbs_modbus.h"

#include <ChainableLED.h>

#define NUM_LEDS  4


#define LAMP_ONE 7
#define LAMP_TWO 5 //5
#define LAMP_THREE 6 //6
#define LAMP_FOUR 9 //10
#define LAMP_FIVE 4 // 5
#define LAMP_SIX 8 //4
#define RELAY_ONE 10 //8

#define BUTTON_ONE A0
#define BUTTON_TWO A3
#define BUTTON_THREE A1
#define BUTTON_FOUR A2
#define BUTTON_START A4

ChainableLED leds(3, 2, NUM_LEDS);

Bounce debouncerOne = Bounce();
Bounce debouncerTwo = Bounce();
Bounce debouncerThree = Bounce();
Bounce debouncerFour = Bounce();
Bounce debouncerStart = Bounce();

void setup()
{
  Serial.begin(9600);

  pinMode(LAMP_ONE, OUTPUT);
  pinMode(LAMP_TWO, OUTPUT);
  pinMode(LAMP_THREE, OUTPUT);
  pinMode(LAMP_FOUR, OUTPUT);
  pinMode(LAMP_FIVE, OUTPUT);
  pinMode(LAMP_SIX, OUTPUT);
  pinMode(RELAY_ONE, OUTPUT);

  pinMode(BUTTON_ONE, INPUT_PULLUP);
  pinMode(BUTTON_TWO, INPUT_PULLUP);
  pinMode(BUTTON_THREE, INPUT_PULLUP);
  pinMode(BUTTON_FOUR, INPUT_PULLUP);
  pinMode(BUTTON_START, INPUT_PULLUP);

  debouncerOne.attach(BUTTON_ONE);
  debouncerOne.interval(30);

  debouncerTwo.attach(BUTTON_TWO);
  debouncerTwo.interval(30);

  debouncerThree.attach(BUTTON_THREE);
  debouncerThree.interval(30);

  debouncerFour.attach(BUTTON_FOUR);
  debouncerFour.interval(30);

  debouncerStart.attach(BUTTON_START);
  debouncerStart.interval(30);

  leds.init();
}

byte pos = 0;
byte lamp = 0;
int gamePosition = 0;
unsigned long previousMillis = 0;
const long interval = 1000;

int statusStart = 0;
int lampState = HIGH;

unsigned long lastChangeTime = 0;

bool demoMode = false;
bool failMode = false;
bool successMode = false;
bool proceedMode = false;

int failPosition = 0;
int proceedPosition = 0;
int demoPosition = 0;

unsigned long demoStartTime = 0;
unsigned long failStartTime = 0;
unsigned long successStartTime = 0;
unsigned long proceedStartTime = 0;

void loop()
{
  bouncerUpdate();
  modbus_loop();

  int buttonStart = debouncerStart.read();

  if(demoMode) {
    startSequence();
    return;
  }

  if(proceedMode) {
    proceed(0);
    return;
  }

  if(failMode) {
    fail();
    return;
  }

  if(successMode) {
    
  }

  if(buttonStart == LOW) {

    Serial.print("start called");

    if(!statusStart) {
      
      startSequence();
      gamePosition = 1;

      statusStart = 1;
      return;
    }
  }

  if(statusStart) {
    game();
  }
}

void game()
{

  int pressed = getPressedNumber();

  if(pressed > 0) {
    Serial.println(pressed);
    if(pressed <= 4) {
      setColor(pressed - 1);
    } else {
      fail();
      return;
    }
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (lampState == LOW) {
      lampState = HIGH;
    } else {
      lampState = LOW;
    }
  }

  switch(gamePosition) {
    case 1: {
      if(pressed) {
        if(pressed != 3) {
          fail();
          pressed = 0;
          return;
        } else {
          proceed(2);
          return;
        }
      }

      setDefault();
      digitalWrite(LAMP_THREE, lampState);
      break;
    }
    case 2: {

      if(pressed > 0) {
        if(pressed != 1) {
          fail();
          pressed = 0;
          return;
        } else {
          proceed(3);
          return;
        }
      }

      setGameStatus(0,0,1,0,0,0);
      digitalWrite(LAMP_FIVE, lampState);
      break;
    }
    case 3: {

      if(pressed > 0) {
        if(pressed != 2) {
          fail();
          pressed = 0;
          return;
        } else {
          proceed(4);
          return;
        }
      }

      setGameStatus(0,0,1,0,1,0);
      digitalWrite(LAMP_SIX, lampState);
      break;
    }
    case 4: {

      if(pressed > 0) {
        if(pressed != 1) {
          fail();
          pressed = 0;
          return;
        } else {
          proceed(5);
          return;
        }
      }

      setGameStatus(0,0,1,0,1,1);
      digitalWrite(LAMP_ONE, lampState);
      break;
    }
    case 5: {

      if(pressed > 0) {
        if(pressed != 4) {
          fail();
          pressed = 0;
          return;
        } else {
          proceed(6);
          return;
        }
      }

      setGameStatus(1,0,1,0,1,1);
      digitalWrite(LAMP_TWO, lampState);
      break;
    }
    case 6: {

      if(pressed > 0) {
        if(pressed != 2) {
          fail();
          pressed = 0;
          return;
        } else {
          proceed(7);
          return;
        }
      }

      setGameStatus(1,1,1,0,1,1);
      digitalWrite(LAMP_FOUR, lampState);
      break;
    }
    case 7: {
      setGameStatus(1,1,1,1,1,1);
      success();
      gamePosition = 8;
      return;
    }
    case 8: {
      if(pressed > 0) {
        if(pressed == 4) {
          fail();
          pressed = 0;
          return;
        }
      }
    }
  }

}

// 2, 4, 3, 4, 1

void startSequence()
{
  byte i = 0;
  int diff = millis() - demoStartTime;
  
  if(!demoMode) {
      demoStartTime = millis();
      demoMode = true;
      setColor(i);
      digitalWrite(LAMP_ONE, HIGH);
  }

  if(demoPosition > 10) {
    if(diff > 2000) {
      demoMode = false;
    }
    return;
  }

  if(demoPosition % 2) {
    switch (demoPosition) {
      case 0: {
        i = 0;
        break;
      }
      case 2: {
        i = 3;
      }
      case 4: {
        i = 2;
        break;
      }
      case 6: {
        i = 1;
        break;
      }
      case 8: {
        i = 0;
        break;
      }
      case 10: {
        i = 1;
        break;
      }
    }
    if(diff > 2000) {
      demoPosition ++;
      setDefault();
      setColorNone(i);
      demoStartTime = millis();
    }
  } else {
    if(diff > 50) {
      demoPosition ++;
      demoStartTime = millis();
    }
  }

}

void setColorNone(byte position)
{
  leds.setColorRGB(position, 0, 0, 0);
}

void setColorError(byte position)
{
  leds.setColorRGB(position, 255, 0, 0);
}

void setColor(byte position)
{
  leds.setColorRGB(position, 255, 255, 255);
//  switch(position) {
//      case 0: {
//        if(debouncerOne.read() == LOW) { // One
//          leds.setColorRGB(position, 255, 0, 0);
//        }
//        break;
//      }
//      case 1: {
//        if(debouncerFour.read() == LOW) { // Four
//          leds.setColorRGB(position, 255, 0, 0);
//        }
//        break;
//      }
//      case 2: {
//        if(debouncerTwo.read() == LOW) { // Two
//          leds.setColorRGB(position, 255, 0, 0);
//        }
//        break;
//      }
//      case 3: {
//        if(debouncerThree.read() == LOW) { // Three
//          leds.setColorRGB(position, 255, 0, 0);
//        }
//        break;
//      }
//  }

}

void setDefault()
{
  digitalWrite(LAMP_ONE, LOW);
  digitalWrite(LAMP_TWO, LOW);
  digitalWrite(LAMP_THREE, LOW);
  digitalWrite(LAMP_FOUR, LOW);
  digitalWrite(LAMP_FIVE, LOW);
  digitalWrite(LAMP_SIX, LOW);
  digitalWrite(RELAY_ONE, HIGH);
}

void setGameStatus(int a, int b, int c, int d, int e, int f)
{
  if(a) {
    digitalWrite(LAMP_ONE, HIGH);
  } else {
    digitalWrite(LAMP_ONE, LOW);
  }

  if(b) {
    digitalWrite(LAMP_TWO, HIGH);
  } else {
    digitalWrite(LAMP_TWO, LOW);
  }

  if(c) {
    digitalWrite(LAMP_THREE, HIGH);
  } else {
    digitalWrite(LAMP_THREE, LOW);
  }

  if(d) {
    digitalWrite(LAMP_FOUR, HIGH);
  } else {
    digitalWrite(LAMP_FOUR, LOW);
  }

  if(e) {
    digitalWrite(LAMP_FIVE, HIGH);
  } else {
    digitalWrite(LAMP_FIVE, LOW);
  }

  if(f) {
    digitalWrite(LAMP_SIX, HIGH);
  } else {
    digitalWrite(LAMP_SIX, LOW);
  }
}

int getPressedNumber()
{
  bouncerUpdate();
  int i = 0;

  if(debouncerOne.read() == LOW) {
    i = 1;
  }

  if(debouncerTwo.read() == LOW) {
    i = 2 + i * 5;
  }

  if(debouncerThree.read() == LOW) {
    i = 3 + i * 5;
  }

  if(debouncerFour.read() == LOW) {
    i = 4 + i * 5;
  }

  return i;
}

void success()
{
  modbus_set(COMPLETE, 1);
  
  if(!successMode) {
    successMode = true;
    successStartTime = millis();
    setColorAll(0,255,0);
    digitalWrite(RELAY_ONE, LOW);
    return;
  }

  int diff = millis() - successStartTime;

  if(diff > 5000) {
    digitalWrite(RELAY_ONE, HIGH);
    setDefault();
    successMode = false;
  }
}

void fail()
{
  modbus_set(COMPLETE, 0);
  
  if (!failMode) {
    failMode = true;
    failStartTime = millis();

    statusStart = 0;

    setDefault();

    setColorAll(255,0,0);
  }

  int diff = millis() - failStartTime;

  if(diff > 200) {
    if (failPosition > 5) {
      if(diff > 1000) {
        failMode = false;
      }
    } else {
      if (failPosition % 2) {
        setColorAll(255,0,0);
        failPosition++;
        failStartTime = millis();
      } else {
        setColorAll(0,0,0);
        failPosition++;
        failStartTime = millis();
      }
    }
  }
}

void proceed(int pos) 
{
  if(!proceedMode) {
    proceedMode = true;
    proceedStartTime = millis();

    proceedPosition = pos;
  }

  int diff = millis() - proceedStartTime;

  if(diff > 500) {
    proceedStartTime = millis();
    proceedMode = false;

    bouncerUpdate();
    setColorAll(0,0,0);
    setDefault();
    gamePosition = proceedPosition;
  }
}

void setColorAll(int r, int g, int b)
{
  leds.setColorRGB(0, r, g, b);
  leds.setColorRGB(1, r, g, b);
  leds.setColorRGB(2, r, g, b);
  leds.setColorRGB(3, r, g, b);
}

void bouncerUpdate()
{
  debouncerOne.update();
  debouncerTwo.update();
  debouncerThree.update();
  debouncerFour.update();
  debouncerStart.update();
}
