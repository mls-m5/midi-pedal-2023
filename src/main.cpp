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

    // Send note on command over serial port
    Serial1.write(0x90);  // Note on command
    Serial1.write(pitch); // Note number
    Serial1.write(100);   // Velocity
}

void sendMidiOff(int note)
{
    int pitch = 60 + note;                                  // Middle C plus note number
    midiEventPacket_t noteOff = {0x08, 0x80 | 0, pitch, 0}; // channel 1, calculated pitch, velocity 0
    MidiUSB.sendMIDI(noteOff);
    MidiUSB.flush();

    // Send note off command over serial port
    Serial1.write(0x80);  // Note off command
    Serial1.write(pitch); // Note number
    Serial1.write(0);     // Velocity
}

struct PinState
{
    int pin;
    int oldState;
    unsigned long lastChangeTime;

    PinState(int pin) : pin{pin}, oldState(HIGH), lastChangeTime(0) {}

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
                digitalWrite(LED_PIN, HIGH);
            }
            else
            {
                sendMidiOn(note);
                digitalWrite(LED_PIN, LOW);
            }

            oldState = newState; // Update old pin state
            lastChangeTime = currentTime;
        }
    }
};

// All pins used in the pro micro:
// {4, 5, 6, 7, 8, 9, 10, 16, 14, 15, 18, 19, 20};

PinState pinStates[NUM_PINS] = {4, 5, 6, 7, 8, 9, 10, 16, 14, 15, 18, 19, 20};

void setup()
{
    Serial.begin(9600);
    Serial1.begin(31250); // Begin MIDI communication on Serial1
    pinMode(LED_PIN, OUTPUT);

    for (int i = 0; i < NUM_PINS; i++)
    {
        pinMode(pinStates[i].pin, INPUT_PULLUP);
    }
}

void loop()
{
    for (int i = 0; i < NUM_PINS; i++)
    {
        int pinState = digitalRead(pinStates[i].pin);
        pinStates[i].handleValue(pinState, i);
    }
}
