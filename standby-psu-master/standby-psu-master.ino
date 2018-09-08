// load libraries
#include "tiny_IRremote.h"
#include "SendOnlySoftwareSerial.h"

// pinout
#define relayPin PB2
#define ledPin PB1
#define buttonPin PB0
#define serialOutPin PB4
#define irPin PB3

bool ledState = HIGH;                           // state of standby led. turned HIGH when system is off
bool powerState = LOW;                          // power state, LOW when system is off
bool enablePower = HIGH;                        // enable power state change
bool powerSent = LOW;                           // flag to send power down serial out

int debounceDelay = 50;                         // debounce time
unsigned long powerStateTimer = 0;              // time snapshot when power state changed
unsigned long blinkTimer = 0;                   // blink timer
int powerStateDelay = 15000;                    // power state delay time
bool pauseLoop = LOW;                           // pause main loop

// IR setup
IRrecv irrecv(irPin);
decode_results results;

unsigned long muteTimer = 0;                    // millis snapshot when we press mute button on ir (repeating signal)
bool irMute = LOW;                              // check if we can change mute state again with ir
bool irPower = LOW;                             // signal to change power state from ir

// Software serial
SendOnlySoftwareSerial mySerial (serialOutPin);       // Tx pin

void setup() {
    // serial setup
    mySerial.begin (9600);

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
    irrecv.enableIRIn();
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
    if (irrecv.decode(&results)) {
        if (results.value == 18556633) { // logo button as power on or off
//            mySerial.println("     ");
            irPower = HIGH;
        }
        if (results.value == 18575503 && irMute == LOW) { // mute button
            mySerial.println("0202");
            irMute = HIGH;
            muteTimer = millis();
        }
        if (results.value == 18548983) { // volume up
            mySerial.println("0301");
        }
        if (results.value == 18581623) { // volume down
            mySerial.println("0302");
        }
        if (results.value == 18609673) { // lines button as change output
            mySerial.println("0401");
        }
        if (results.value == 18577033) { // squares button as change input
            mySerial.println("0501");
        }
        irrecv.resume(); // Receive the next value
    }

    // read button
    int buttonPress = 0;
    while (digitalRead(buttonPin) == LOW && buttonPress < 30) {
        buttonPress++;
        delay(debounceDelay);
    }

    if ( (buttonPress > 29 || irPower == HIGH) && enablePower == HIGH) {
        // power on or off
        if (powerState == HIGH) {
            // turning off

            // send mute over serial
            mySerial.println("0201");

            // wait 3seconds after mute signal before system turns off
            powerSent = LOW;
            int turnOff = 0;
            do {
                digitalWrite(ledPin, !ledState);
                delay(debounceDelay);
                digitalWrite(ledPin, ledState);
                delay(debounceDelay);

                // send power off over serial
                if (turnOff > 10 && powerSent == LOW) {
                    powerSent = HIGH;
                    mySerial.println("0103");
                }

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
        mySerial.println("0900");
    }

    // reset mute operation after 15s
    if (millis() - muteTimer > powerStateDelay && irMute == HIGH) {
        irMute = LOW;
    }

    // pause main loop
    do {
        pauseLoop = LOW;
        if (irrecv.decode(&results)) {
            if (results.value == 4294967295) {   // ir resend data value
                if (irPower == HIGH) {
                    pauseLoop = HIGH;
                }
            }
            irrecv.resume(); // Receive the next value
        }
        if (digitalRead(buttonPin) == LOW ) {
            pauseLoop = HIGH;
        }
        delay(debounceDelay);
    } while (pauseLoop);     // wait for button release

    // reset irPower flag
    irPower = LOW;
}