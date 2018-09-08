//load libraries
#include "IRremote.h"
#include "SendOnlySoftwareSerial.h"
#include "avr/interrupt.h"

//pinout
#define relayPin 5
#define ledPin 4
#define buttonPin 2
#define serialOutPin 6
#define irPin 7

int ledState = HIGH;        // state of standby led. turned HIGH when system is off
int powerState = LOW;       // power state, LOW when system is off

int debounceDelay = 100;                        // debounce time
unsigned long powerStateTimer = 0;              // time snapshot when power state changed
long powerStateDelay = 15000;                   // power state delay time

// IR setup
IRrecv irrecv(irPin);
decode_results results;

// Software serial
//SendOnlySoftwareSerial mySerial (serialOutPin);       // Tx pin

void setup() {
    //serial setup
    //mySerial.begin (9600);
    Serial.begin (9600);
    digitalWrite(serialOutPin, HIGH);   //test

    //pinout setup
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);
    pinMode(irPin, INPUT);
    pinMode(relayPin, OUTPUT);
    pinMode(serialOutPin, OUTPUT);
    //attachInterrupt(digitalPinToInterrupt(buttonPin), btnDown, CHANGE);
    //attachInterrupt(digitalPinToInterrupt(buttonPin), btnUp, RISING);

    //defaults
    digitalWrite(ledPin, ledState);
    digitalWrite(relayPin, powerState);

    //ir setup
    irrecv.enableIRIn();
}

void loop() {
    //startup delay and powerdown blinking led delay
    while (millis() - powerStateTimer < powerStateDelay) {
        digitalWrite(ledPin, HIGH);
        delay(50);
        digitalWrite(ledPin, LOW);
        delay(950);
    }
    //reset led to correct state
    digitalWrite(ledPin, ledState);


    if (irrecv.decode(&results)) {
        if (results.value != 4294967295) {
            Serial.print ("Read code: ");
            Serial.println (results.value);
        }
        if (results.value == 18556633) {
            digitalWrite(serialOutPin, HIGH);    //test
        }
        irrecv.resume(); // Receive the next value
    }

    
    //read button
    //read ir
    //set relay
    //send serial data
}

void btnDown() {
    Serial.println ("button down sub");
    digitalWrite(serialOutPin, LOW);
    delay(debounceDelay);
}

void btnUp() {
    Serial.println ("button up sub");
    digitalWrite(serialOutPin, HIGH);
    delay(debounceDelay);
}
