//load libraries
#include "IRremote.h"
#include "SendOnlySoftwareSerial.h"

//pinout
#define relayPin PB2
#define ledPin PB1
#define buttonPin PB0
#define serialOutPin PB4
#define irPin PB3

int ledState = HIGH;        // state of standby led. turned HIGH when system is off
int powerState = LOW;       // power state, LOW when system is off

unsigned long powerStateTimer = 0;               // time snapshot when power state changed
long powerStateDelay = 15000;                    // power state delay time

// IR setup
IRrecv irrecv(irPin);
decode_results results;

// Software serial
SendOnlySoftwareSerial mySerial (serialOutPin);       // Tx pin

void setup() {
    //serial setup
    mySerial.begin(9600);

    //pinout setup
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);
    pinMode(irPin, INPUT);
    pinMode(relayPin, OUTPUT);

    //defaults
    digitalWrite(ledPin, LED_State);
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

    //read button
    //read ir
    //set relay
    //send serial data
}