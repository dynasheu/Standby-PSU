// load libraries
//#include "IRremote.h"
//#include "SendOnlySoftwareSerial.h"
//#include "avr/interrupt.h"

// pinout
#define relayPin 5
#define ledPin 4
#define buttonPin 2
#define serialOutPin 6
#define irPin 7

bool ledState = HIGH;       // state of standby led. turned HIGH when system is off
bool powerState = LOW;      // power state, LOW when system is off
bool enablePower = HIGH;    // enable power state change
bool enableShort = HIGH;    // ebable short button press

int debounceDelay = 50;                        // debounce time
unsigned long powerStateTimer = 0;              // time snapshot when power state changed
unsigned long blinkTimer = 0;                   // blink timer
long powerStateDelay = 15000;                   // power state delay time

// IR setup
//IRrecv irrecv(irPin);
//decode_results results;

// Software serial
//SendOnlySoftwareSerial mySerial (serialOutPin);       // Tx pin

void setup() {
    // serial setup
//    mySerial.begin (9600);
    Serial.begin (115200);
//    digitalWrite(serialOutPin, HIGH);   //test

    // pinout setup
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);
    pinMode(irPin, INPUT);
    pinMode(relayPin, OUTPUT);
    pinMode(serialOutPin, OUTPUT);
//    attachInterrupt(digitalPinToInterrupt(buttonPin), btnDown, CHANGE);
//    attachInterrupt(digitalPinToInterrupt(buttonPin), btnUp, RISING);

    // defaults
    digitalWrite(ledPin, ledState);
    digitalWrite(relayPin, powerState);

    // ir setup
//    irrecv.enableIRIn();
}

void loop() {
    // startup delay and powerdown blinking led delay
    // while (millis() - powerStateTimer < powerStateDelay) {
    //     digitalWrite(ledPin, HIGH);
    //     delay(debounceDelay);
    //     digitalWrite(ledPin, LOW);
    //     delay(debounceDelay * 19);
    // }
    // // reset led to correct state
    // digitalWrite(ledPin, ledState);

    if (millis() - powerStateTimer < powerStateDelay) {
        enablePower = LOW;
        if (powerState == LOW) {
            enableShort = LOW;
        }
        else {
            enableShort = HIGH;
        }

        // blink led every second
        if(millis() - blinkTimer > 1000) {
            digitalWrite(ledPin, HIGH);
            delay(debounceDelay);
            digitalWrite(ledPin, LOW);
            blinkTimer = millis();
        }
    }
    else {
        enablePower = HIGH;
        enableShort = HIGH;
        digitalWrite(ledPin, ledState);
    }

    // read ir
    // if (irrecv.decode(&results)) {
    //     if (results.value != 4294967295) {
    //         Serial.print ("Read code: ");
    //         Serial.println (results.value);
    //     }
    //     if (results.value == 18556633) {
    //         digitalWrite(serialOutPin, HIGH);    //test
    //     }
    //     irrecv.resume(); // Receive the next value
    // }

    // read button
    int buttonPress = 0;
    while (digitalRead(buttonPin) == LOW && buttonPress < 30) {
        buttonPress++;
        delay(debounceDelay);
        Serial.print ("Button pressed for (ms): ");
        Serial.println (buttonPress * debounceDelay);
    }

    if (buttonPress > 0) {
        Serial.println ("--");
    }

    if (buttonPress > 29 && enablePower == HIGH) {
        // power on or off
        if (powerState == HIGH) {
            // turning off

            // send mute over serial
//            mySerial.println("0001");
            serialBlink();

            // wait 3seconds after mute signal before system turns off
            int turnOff = 0;
            do {
                digitalWrite(ledPin, !ledState);
                delay(debounceDelay);
                digitalWrite(ledPin, ledState);
                delay(debounceDelay);
                turnOff++;
            } while (turnOff < 30);

            ledState = powerState;
            powerState = !powerState;
            powerStateTimer = millis();

            // write to digital pins
            digitalWrite(ledPin, ledState);
            digitalWrite(relayPin, powerState);
        }
        else if (powerState == LOW) {
            // turning on
            delay(debounceDelay);
            ledState = powerState;
            powerState = !powerState;
            powerStateTimer = millis();

            // write to digital pins
            digitalWrite(ledPin, ledState);
            digitalWrite(relayPin, powerState);
        }
    }
    else if (buttonPress > 1 && buttonPress < 20 && enableShort == HIGH) {
        // send short press signal over serial
        serialBlink();
    }
    // Serial.print ("Power state: ");
    // Serial.print (powerState);
    // Serial.print ("    Led state: ");
    // Serial.print (ledState);
    // Serial.print ("    Power state timer: ");
    // Serial.print (powerStateTimer);
    // Serial.print ("    Button state: ");
    // Serial.println (digitalRead(buttonPin));
    

}

// void btnDown() {
//     Serial.println ("button down sub");
//     digitalWrite(serialOutPin, LOW);
//     delay(debounceDelay);
// }

// void btnUp() {
//     Serial.println ("button up sub");
//     digitalWrite(serialOutPin, HIGH);
//     delay(debounceDelay);
// }

void serialBlink () {
    digitalWrite(serialOutPin, HIGH);
    delay(debounceDelay * 2);
    digitalWrite(serialOutPin, LOW);
}
