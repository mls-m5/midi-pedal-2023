
#include <Arduino.h>
#include <array>

#define PIN_NUMBER 2
#define LED_PIN 17

void setup()
{
    Serial.begin(9600);
    pinMode(PIN_NUMBER, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
}

void loop()
{
    int pinState = digitalRead(PIN_NUMBER);
    Serial.println(pinState);

    // if (pinState == HIGH)
    // {
    //     digitalWrite(LED_PIN, LOW); // LED is off when pinState is HIGH (because of pull-up resistor)
    // }
    // else
    // {
    //     digitalWrite(LED_PIN, HIGH); // LED is on when pinState is LOW (switch closed)
    // }

    // delay(500); // Delay a bit to make it easier to read the output
}
