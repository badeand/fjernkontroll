#include <Arduino.h>

static const int ledPin = 10;
static const int buttonDog = 12;
static const int buttonAtHome = 13;
static const int buttonDim = 14;
static const int buttonSleep = 15;
static const int buttonNone = 0;

static const int ledStateFadeOut = 1;
static const int ledStateOFF = 2;

static const int buttonStateAccept = 1;
static const int buttonStateIgnore = 3;

int pwm_counter = 0;
int led_dim = 0;

int ledState = ledStateOFF;
int buttonState = buttonStateAccept;
int activeButton = buttonNone;
int buttonTimer = 0;

int timer = 0;

void setup() {
    delay(2000);
    pinMode(ledPin, OUTPUT);
    pinMode(buttonDog, INPUT);
    pinMode(buttonAtHome, INPUT);
    pinMode(buttonDim, INPUT);
    pinMode(buttonSleep, INPUT);
    Serial.begin(9600);
    delay(1000);
    Serial.println("OK");
}

void update() {

    if (activeButton != buttonNone) {
        buttonTimer--;
        led_dim = buttonTimer / 3;
        if (buttonTimer < 1) {
            activeButton = buttonNone;
        }
    }


}

void setActiveButton(int button) {
    activeButton = button;
    buttonTimer = 30;
    Serial.println(activeButton);
}

void loop() {

    pwm_counter = pwm_counter < 10 ? pwm_counter + 1 : pwm_counter = 0;

    if (pwm_counter < led_dim) {
        digitalWrite(ledPin, HIGH);
    } else {
        digitalWrite(ledPin, LOW);
    }

    if (timer > 50) {
        timer = 0;
        update();
    }
    timer++;

    if (activeButton == buttonNone && digitalRead(buttonAtHome)) {
        setActiveButton(buttonAtHome);
    }

    if (activeButton == buttonNone && digitalRead(buttonSleep)) {
        setActiveButton(buttonSleep);
    }

    if (activeButton == buttonNone && digitalRead(buttonDog)) {
        setActiveButton(buttonDog);
    }

    if (activeButton == buttonNone && digitalRead(buttonDim)) {
        setActiveButton(buttonDim);
    }


    delay(1);

}

