#include <Arduino.h>
#include <MIDIUSB.h>

#define LED_PIN 17
#define NUM_PINS 13

int pinArray[NUM_PINS] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}; // Define the pins you want to read
int oldPinStates[NUM_PINS] = {0};

void setup()
{
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);

    for (int i = 0; i < NUM_PINS; i++)
    {
        pinMode(pinArray[i], INPUT_PULLUP);
        oldPinStates[i] = digitalRead(pinArray[i]); // Store initial pin states
    }
}

void sendMidiOn(int note)
{
    int pitch = 60 + note;                                   // Middle C plus note number
    midiEventPacket_t noteOn = {0x09, 0x90 | 0, pitch, 100}; // channel 1, calculated pitch, velocity 100
    MidiUSB.sendMIDI(noteOn);
    MidiUSB.flush();
}

void sendMidiOff(int note)
{
    int pitch = 60 + note;                                  // Middle C plus note number
    midiEventPacket_t noteOff = {0x08, 0x80 | 0, pitch, 0}; // channel 1, calculated pitch, velocity 0
    MidiUSB.sendMIDI(noteOff);
    MidiUSB.flush();
}

void loop()
{
    for (int i = 0; i < NUM_PINS; i++)
    {
        int pinState = digitalRead(pinArray[i]);

        if (pinState != oldPinStates[i])
        { // If pin state has changed
            if (pinState == HIGH)
            {
                sendMidiOff(i); // Now sends i (the pin number) as the note number
                digitalWrite(LED_PIN, LOW);
            }
            else
            {
                sendMidiOn(i); // Now sends i (the pin number) as the note number
                digitalWrite(LED_PIN, HIGH);
            }

            oldPinStates[i] = pinState; // Update old pin state
        }
    }
}
