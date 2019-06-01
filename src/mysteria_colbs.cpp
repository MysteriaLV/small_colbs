#include <Bounce2.h>
#include "colbs_modbus.h"

#include <ChainableLED.h>

#define NUM_LEDS  4
#define LEDS_CLK_PIN  3
#define LEDS_DATA_PIN 2


#define LAMP_ONE_PIN   7
#define LAMP_TWO_PIN   5
#define LAMP_THREE_PIN 6
#define LAMP_FOUR_PIN  9
#define LAMP_FIVE_PIN  4
#define LAMP_SIX_PIN   8
#define RELAY_ONE_PIN  13

#define BUTTON_ONE_PIN   A0
#define BUTTON_TWO_PIN   A3
#define BUTTON_THREE_PIN A1
#define BUTTON_FOUR_PIN  A2
#define BUTTON_START_PIN A4


ChainableLED leds(LEDS_CLK_PIN, LEDS_DATA_PIN, NUM_LEDS);

Bounce debouncerOne = Bounce();
Bounce debouncerTwo = Bounce();
Bounce debouncerThree = Bounce();
Bounce debouncerFour = Bounce();
Bounce debouncerStart = Bounce();


// Vermut: never used? byte pos = 0;
// Vermut: never used? byte lamp = 0;
// Vermut: never used? unsigned long lastChangeTime = 0;

int gamePosition = 0;
unsigned long previousMillis = 0;
const long interval = 1000;

int statusStart = 0;
byte lampState = HIGH;

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


void bouncerUpdate() {
    debouncerOne.update();
    debouncerTwo.update();
    debouncerThree.update();
    debouncerFour.update();
    debouncerStart.update();
}


void setColorAll(byte r, byte g, byte b) {
    leds.setColorRGB(0, r, g, b);
    leds.setColorRGB(1, r, g, b);
    leds.setColorRGB(2, r, g, b);
    leds.setColorRGB(3, r, g, b);
}


void setGameStatus(byte lamp1, byte lamp2, byte lamp3, byte lamp4, byte lamp5, byte lamp6) {
    digitalWrite(LAMP_ONE_PIN, lamp1);
    digitalWrite(LAMP_TWO_PIN, lamp2);
    digitalWrite(LAMP_THREE_PIN, lamp3);
    digitalWrite(LAMP_FOUR_PIN, lamp4);
    digitalWrite(LAMP_FIVE_PIN, lamp5);
    digitalWrite(LAMP_SIX_PIN, lamp6);
}


void setDefault() {
    digitalWrite(LAMP_ONE_PIN, LOW);
    digitalWrite(LAMP_TWO_PIN, LOW);
    digitalWrite(LAMP_THREE_PIN, LOW);
    digitalWrite(LAMP_FOUR_PIN, LOW);
    digitalWrite(LAMP_FIVE_PIN, LOW);
    digitalWrite(LAMP_SIX_PIN, LOW);
    digitalWrite(RELAY_ONE_PIN, HIGH);
}


int getPressedNumber() {
    bouncerUpdate();
    int i = 0;

    if (debouncerOne.read() == LOW) {
        i = 1;
    }

    if (debouncerTwo.read() == LOW) {
        i = 2 + i * 5;
    }

    if (debouncerThree.read() == LOW) {
        i = 3 + i * 5;
    }

    if (debouncerFour.read() == LOW) {
        i = 4 + i * 5;
    }

    return i;
}

void proceed(int pos) {
    if (!proceedMode) {
        proceedMode = true;
        proceedStartTime = millis();

        proceedPosition = pos;
    }

    long diff = millis() - proceedStartTime;

    if (diff > 500) {
        proceedStartTime = millis();
        proceedMode = false;

        bouncerUpdate();
        setColorAll(0, 0, 0);
        setDefault();
        gamePosition = proceedPosition;
    }
}

void setColorNone(byte position) {
    leds.setColorRGB(position, 0, 0, 0);
}

void setColorError(byte position) {
    leds.setColorRGB(position, 255, 0, 0);
}

void setColor(byte position) {
    leds.setColorRGB(position, 255, 255, 255);
}

void game() {
    int pressed = getPressedNumber();

    if (pressed > 0) {
        Serial.println(pressed);
        if (pressed <= 4) {
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

    switch (gamePosition) {
        case 1: {
            if (pressed) {
                if (pressed != 3) {
                    fail();
                    return;
                } else {
                    proceed(2);
                    return;
                }
            }

            setDefault();
            digitalWrite(LAMP_THREE_PIN, lampState);
            break;
        }
        case 2: {

            if (pressed > 0) {
                if (pressed != 1) {
                    fail();
                    return;
                } else {
                    proceed(3);
                    return;
                }
            }

            setGameStatus(0, 0, 1, 0, 0, 0);
            digitalWrite(LAMP_FIVE_PIN, lampState);
            break;
        }
        case 3: {

            if (pressed > 0) {
                if (pressed != 2) {
                    fail();
                    return;
                } else {
                    proceed(4);
                    return;
                }
            }

            setGameStatus(0, 0, 1, 0, 1, 0);
            digitalWrite(LAMP_SIX_PIN, lampState);
            break;
        }
        case 4: {

            if (pressed > 0) {
                if (pressed != 1) {
                    fail();
                    return;
                } else {
                    proceed(5);
                    return;
                }
            }

            setGameStatus(0, 0, 1, 0, 1, 1);
            digitalWrite(LAMP_ONE_PIN, lampState);
            break;
        }
        case 5: {

            if (pressed > 0) {
                if (pressed != 4) {
                    fail();
                    return;
                } else {
                    proceed(6);
                    return;
                }
            }

            setGameStatus(1, 0, 1, 0, 1, 1);
            digitalWrite(LAMP_TWO_PIN, lampState);
            break;
        }
        case 6: {

            if (pressed > 0) {
                if (pressed != 2) {
                    fail();
                    return;
                } else {
                    proceed(7);
                    return;
                }
            }

            setGameStatus(1, 1, 1, 0, 1, 1);
            digitalWrite(LAMP_FOUR_PIN, lampState);
            break;
        }
        case 7: {
            setGameStatus(1, 1, 1, 1, 1, 1);
            success();
            gamePosition = 8;
            return;
        }
        case 8: {
            if (pressed > 0) {
                if (pressed == 4) {
                    fail();
                    return;
                }
            }
        }
    }

}




// 2, 4, 3, 4, 1
void startSequence() {
    byte i = 0;
    long diff = millis() - demoStartTime;

    if (!demoMode) {
        demoStartTime = millis();
        demoMode = true;
        setColor(i);
        digitalWrite(LAMP_ONE_PIN, HIGH);
    }

    if (demoPosition > 10) {
        if (diff > 2000) {
            demoMode = false;
        }
        return;
    }

    if (demoPosition % 2) {
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
        if (diff > 2000) {
            demoPosition++;
            setDefault();
            setColorNone(i);
            demoStartTime = millis();
        }
    } else {
        if (diff > 50) {
            demoPosition++;
            demoStartTime = millis();
        }
    }

}


void success() {
    modbus_set(COMPLETE, 1);

    if (!successMode) {
        successMode = true;
        successStartTime = millis();
        setColorAll(0, 255, 0);
        digitalWrite(RELAY_ONE_PIN, LOW);
        return;
    }

    long diff = millis() - successStartTime;

    if (diff > 5000) {
        digitalWrite(RELAY_ONE_PIN, HIGH);
        setDefault();
        successMode = false;
    }
}

void fail() {
    modbus_set(COMPLETE, 0);

    if (!failMode) {
        failMode = true;
        failStartTime = millis();

        statusStart = 0;

        setDefault();

        setColorAll(255, 0, 0);
    }

    long diff = millis() - failStartTime;

    if (diff > 200) {
        if (failPosition > 5) {
            if (diff > 1000) {
                failMode = false;
            }
        } else {
            if (failPosition % 2) {
                setColorAll(255, 0, 0);
                failPosition++;
                failStartTime = millis();
            } else {
                setColorAll(0, 0, 0);
                failPosition++;
                failStartTime = millis();
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    modbus_setup();

    pinMode(LAMP_ONE_PIN, OUTPUT);
    pinMode(LAMP_TWO_PIN, OUTPUT);
    pinMode(LAMP_THREE_PIN, OUTPUT);
    pinMode(LAMP_FOUR_PIN, OUTPUT);
    pinMode(LAMP_FIVE_PIN, OUTPUT);
    pinMode(LAMP_SIX_PIN, OUTPUT);
    pinMode(RELAY_ONE_PIN, OUTPUT);

    pinMode(BUTTON_ONE_PIN, INPUT_PULLUP);
    pinMode(BUTTON_TWO_PIN, INPUT_PULLUP);
    pinMode(BUTTON_THREE_PIN, INPUT_PULLUP);
    pinMode(BUTTON_FOUR_PIN, INPUT_PULLUP);
    pinMode(BUTTON_START_PIN, INPUT_PULLUP);

    debouncerOne.attach(BUTTON_ONE_PIN);
    debouncerOne.interval(30);

    debouncerTwo.attach(BUTTON_TWO_PIN);
    debouncerTwo.interval(30);

    debouncerThree.attach(BUTTON_THREE_PIN);
    debouncerThree.interval(30);

    debouncerFour.attach(BUTTON_FOUR_PIN);
    debouncerFour.interval(30);

    debouncerStart.attach(BUTTON_START_PIN);
    debouncerStart.interval(30);

    leds.init();
}

void loop() {
    bouncerUpdate();
    modbus_loop();

    int buttonStart = debouncerStart.read();

    if (demoMode) {
        startSequence();
        return;
    }

    if (proceedMode) {
        proceed(0);
        return;
    }

    if (failMode) {
        fail();
        return;
    }

    if (successMode) {
        success();
        return;
    }

    if (buttonStart == LOW) {

        Serial.print("start called");

        if (!statusStart) {

            startSequence();
            gamePosition = 1;

            statusStart = 1;
            return;
        }
    }

    if (statusStart) {
        game();
    }
}
