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

    // defaults
    digitalWrite(ledPin, ledState);
    digitalWrite(relayPin, powerState);

    // ir setup
//    irrecv.enableIRIn();
}

void loop() {
    // startup and shutdown blinking led
    if (millis() - powerStateTimer < powerStateDelay) {
        // disable power button
        enablePower = LOW;

        // blink led every second
        if(millis() - blinkTimer > 1000) {
            digitalWrite(ledPin, HIGH);
            delay(debounceDelay);
            digitalWrite(ledPin, LOW);
            blinkTimer = millis();
        }
    }
    else {
        // enable power button
        enablePower = HIGH;

        // reset led state
        digitalWrite(ledPin, ledState);
    }

    // microsoft xbox one media remote - NEC standard
    // 18556633 - logo
    // 18577033 - window
    // 18609673 - lines
    // 18577543 - up
    // 18610183 - down
    // 18547963 - left
    // 18580603 - right
    // 18564283 - center
    // 18596923 - back
    // 18572443 - line dot
    // 18589783 - rewind
    // 18550513 - play
    // 18557143 - forward
    // 18602023 - previous
    // 18585703 - stop
    // 18569383 - next

    // repeating codes
    // 18548983 - volume up
    // 18581623 - volume down
    // 18575503 - mute
    // 18565303 - channel up
    // 18597943 - channel down

    // 4294967295 - resend data

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
    else if (buttonPress > 1 && buttonPress < 20 && powerState == HIGH) {
        // send short press signal over serial
        serialBlink();
    }
}

void serialBlink() {
    digitalWrite(serialOutPin, HIGH);
    delay(debounceDelay * 2);
    digitalWrite(serialOutPin, LOW);
}
