#include <Arduino.h>

const int vibrationPin = A0;
const int ledPin3 = 10;
const int ledPin2 = 11;
const int ledPin1 = 12;

const int    KNOCK_THRESHOLD  = 200;
const int    KNOCK_SILENCE    = 50; 
const unsigned long KNOCK_DEBOUNCE   = 400;
const unsigned long KNOCK_GAP_MAX    = 1200;
const unsigned long PAUSE_MIN        = 600;
const unsigned long PAUSE_MAX        = 3000;
const unsigned long SEQUENCE_TIMEOUT = 8000;
const unsigned long SUCCESS_HOLD     = 3000;

enum State { WAITING, GROUP1, PAUSING, GROUP2, SUCCESS, GROUP3, PAUSING2};
State state = WAITING;

int knockCount = 0;
unsigned long lastKnockTime = 0;
unsigned long stateStartTime = 0;
bool sensorArmed = true;

void setup() {
    pinMode(ledPin2, OUTPUT);
    pinMode(ledPin1, OUTPUT);
    pinMode(ledPin3, OUTPUT);
    Serial.begin(9600);
}

void resetSequence() {
    state = WAITING;
    knockCount = 0;
    sensorArmed = true;
    analogWrite(ledPin3, 0);
    analogWrite(ledPin2, 0);
    analogWrite(ledPin1, 0);
    Serial.println(">> Reset");
}
void resetSequenceAfterSuccess() {
    state = WAITING;
    knockCount = 0;
    sensorArmed = true;
    Serial.println(">> Reset");
}

void loop() {
    unsigned long now = millis();
    int val = analogRead(vibrationPin);
    bool knocked = false;

    if (sensorArmed) {
        // Sensor is ready, look for a knock
        if (val > KNOCK_THRESHOLD && (now - lastKnockTime > KNOCK_DEBOUNCE)) {
            knocked = true;
            lastKnockTime = now;
            sensorArmed = false; 
            Serial.print("Knock! val=");
            Serial.println(val);
        }
    } else {
        // Sensor is ringing, wait for it to settle back down
        if (val < KNOCK_SILENCE) {
            sensorArmed = true;
            Serial.println("Sensor settled, armed");
        }
    }

    switch (state) {

        case WAITING:
            if (knocked) {
                knockCount = 1;
                stateStartTime = now;
                state = GROUP1;
                Serial.println("State: GROUP1 (1)");
            }
            break;

        case GROUP1:
            if (knocked) {
                knockCount++;
                Serial.print("Group1 count: ");
                Serial.println(knockCount);
                if (knockCount > 2) {
                    Serial.println("Too many in group1");
                    resetSequence();
                }
            }
            if (now - lastKnockTime > KNOCK_GAP_MAX && knockCount > 0 && sensorArmed) {
                if (knockCount == 2) {
                    state = PAUSING;
                    knockCount = 0;
                    analogWrite(ledPin1, 255);
                    Serial.println("State: PAUSING (got 2)");
                } else {
                    Serial.println("Wrong count in group1");
                    resetSequence();
                }
            }
            if (now - stateStartTime > SEQUENCE_TIMEOUT) resetSequence();
            break;

        case PAUSING:
            if (knocked) {
                knockCount = 1;
                state = GROUP2;
                Serial.println("State: GROUP2 (1)");
            }
            if (now - lastKnockTime > PAUSE_MAX) {
                Serial.println("Pause timed out");
                resetSequence();
            }
            break;

        case GROUP2:
            if (knocked) {
                knockCount++;
                Serial.print("Group2 count: ");
                Serial.println(knockCount);
                if (knockCount > 3) {
                    Serial.println("Too many in group2");
                    resetSequence();
                }
            }
            if (now - lastKnockTime > KNOCK_GAP_MAX && knockCount > 0 && sensorArmed) {
                if (knockCount == 3) {
                    state = PAUSING2;
                    analogWrite(ledPin2, 255);
                    Serial.println("State: PAUSING (got 2)");
                } else {
                    Serial.println("Wrong count in group2");
                    resetSequence();
                }
                knockCount = 0;
            }
            if (now - stateStartTime > SEQUENCE_TIMEOUT) resetSequence();
            break;

        case PAUSING2:
             if (knocked) {
                knockCount = 1;
                state = GROUP3;
                Serial.println("State: GROUP3 (1)");
            }
            if (now - lastKnockTime > PAUSE_MAX) {
                Serial.println("Pause timed out");
                resetSequence();
            }
            break;

        case GROUP3:
            if(knocked){
                knockCount++;
                Serial.print("Group3 count: ");
                Serial.println(knockCount);
                if (knockCount > 1) {
                    Serial.println("Too many in group3");
                    resetSequence();
                }
            }
            if (now - lastKnockTime > KNOCK_GAP_MAX && knockCount > 0 && sensorArmed) {
                if (knockCount == 1) {
                    state = SUCCESS;
                    stateStartTime = now;
                    Serial.println("*** SUCCESS! ***");
                    analogWrite(ledPin2, 0);
                    analogWrite(ledPin1, 0);
                    analogWrite(ledPin3, 255);
                } else {
                    Serial.println("Wrong count in group3");
                    resetSequence();
                }
                knockCount = 0;
            }
            if (now - stateStartTime > SEQUENCE_TIMEOUT) resetSequence();
            break;
    

        case SUCCESS:
            if (now - stateStartTime > SUCCESS_HOLD) {
                //analogWrite(ledPin3, 0);
                analogWrite(ledPin2, 0);
                analogWrite(ledPin1, 0);
                resetSequenceAfterSuccess();
            }
            break;
    }
}