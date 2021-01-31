#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *mqtt_host = "192.168.10.104";
const char *wifi_ssid = "X";
const char *wifi_pwd = "X";

static const int ledPin = 10;
static const int buttonDog = 12;
static const int buttonAtHome = 13;
static const int buttonDim = 14;
static const int buttonSleep = 15;
static const int buttonNone = 0;

int pwm_counter = 0;
int led_dim = 0;

int activeButton = buttonNone;
int buttonTimer = 0;

int timer = 0;

WiFiClient espClient;
PubSubClient mqttClient(espClient);


void setupPins();

void setupSerial();

void setupWifi();

void setupMQTT();

void dimLED(int period, int lowInterval, int highInterval);

void setup() {
    // Serial.println(">>> Serial       .. INIT");
    setupSerial();
    Serial.println(">>> Serial       .. OK");

    Serial.println(">>> Pin          .. INIT");
    setupPins();
    Serial.println(">>> Pin          .. OK");

    Serial.println(">>> Wifi         .. INIT");
    setupWifi();
    Serial.println(">>> Wifi         .. OK");

    Serial.println(">>> MQTT         .. INIT");
    setupMQTT();
    Serial.println(">>> MQTT         .. OK");

    Serial.println(">>> SETUP        .. OK");
}

void setupMQTT() {

    digitalWrite(ledPin, HIGH);

    mqttClient.setServer(mqtt_host, 1883);

    while (!mqttClient.connected()) {
        digitalWrite(ledPin, HIGH);

        Serial.print("Attempting MQTT connection...");
        if (mqttClient.connect("Fjernkontroll")) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in a few seconds");
            dimLED(3000, 250, 250);

        }
    }
}

void setupSerial() {
    Serial.begin(9600);
    delay(2000);
}

void setupPins() {
    pinMode(ledPin, OUTPUT);
    pinMode(buttonDog, INPUT);
    pinMode(buttonAtHome, INPUT);
    pinMode(buttonDim, INPUT);
    pinMode(buttonSleep, INPUT);
}


void setupWifi() {
    WiFi.begin(wifi_ssid, wifi_pwd);

    Serial.print("Connecting: ");
    while (WiFi.status() != WL_CONNECTED) {
        dimLED(250, 2, 1);
        dimLED(250, 1, 2);
        Serial.print(".");
    }
    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
}

void dimLED(int period, int lowInterval, int highInterval) {
    for (int i = 0; i < period / (lowInterval + highInterval); i++) {
        digitalWrite(ledPin, HIGH);
        delay(highInterval);
        digitalWrite(ledPin, LOW);
        delay(lowInterval);
    }
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
        mqttClient.publish("homie/program/athome/state", "start");
        setActiveButton(buttonAtHome);
    }

    if (activeButton == buttonNone && digitalRead(buttonSleep)) {
        mqttClient.publish("homie/program/sleep/state", "start");
        setActiveButton(buttonSleep);
    }

    if (activeButton == buttonNone && digitalRead(buttonDog)) {
        mqttClient.publish("homie/program/awaylights/state", "start");
        setActiveButton(buttonDog);
    }

    if (activeButton == buttonNone && digitalRead(buttonDim)) {
        mqttClient.publish("homie/program/dimmed/state", "start");
        setActiveButton(buttonDim);
    }


    if (!mqttClient.connected()) {
        setupMQTT();
    }

    mqttClient.loop();
    delay(1);

}



