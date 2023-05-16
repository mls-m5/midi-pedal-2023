#include <Arduino.h>
#include <MIDIUSB.h>

#define LED_PIN 17
#define NUM_PINS 13

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

struct PinState
{
    int oldState;
    unsigned long lastChangeTime;

    PinState() : oldState(HIGH), lastChangeTime(0) {}

    void handleValue(int newState, int note)
    {
        unsigned long currentTime = millis();

        // Ignore changes if not enough time has passed since the last change
        if (currentTime - lastChangeTime < 10)
        {
            return;
        }

        if (newState != oldState)
        { // If pin state has changed
            if (newState == HIGH)
            {
                sendMidiOff(note);
                digitalWrite(LED_PIN, LOW);
            }
            else
            {
                sendMidiOn(note);
                digitalWrite(LED_PIN, HIGH);
            }

            oldState = newState; // Update old pin state
            lastChangeTime = currentTime;
        }
    }
};

int pinArray[NUM_PINS] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}; // Define the pins you want to read
PinState pinStates[NUM_PINS];

void setup()
{
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);

    for (int i = 0; i < NUM_PINS; i++)
    {
        pinMode(pinArray[i], INPUT_PULLUP);
    }
}

void loop()
{
    for (int i = 0; i < NUM_PINS; i++)
    {
        int pinState = digitalRead(pinArray[i]);
        pinStates[i].handleValue(pinState, i);
    }
}
