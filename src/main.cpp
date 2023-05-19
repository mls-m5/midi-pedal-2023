#include <Arduino.h>

#ifdef IS_PRO_MICRO
#include <MIDIUSB.h>
#endif

#define LED_PIN 17
#define NUM_PINS 13

void sendUSBMIDI(unsigned char event, int channel, unsigned char pitch, unsigned char velocity)
{
#ifdef IS_PRO_MICRO
    midiEventPacket_t noteOn = {event, channel, pitch, velocity}; // channel 1, calculated pitch, velocity 100
    MidiUSB.sendMIDI(noteOn);
    MidiUSB.flush();
#endif
}

void sendSerialMidi(uint8_t header, uint8_t data1, uint8_t data2)
{
#ifdef IS_PRO_MICRO
    auto &s = Serial1;
#else
    auto &s = Serial;
#endif

    // Send note on command over serial port
    s.write(header); // Note on / off command
    s.write(data1);  // Note number
    s.write(data2);  // Velocity
    s.flush();
}

void sendMidiOn(int note)
{
    int pitch = 60 + note; // Middle C plus note number

    sendSerialMidi(0x90, pitch, 100);
    // sendUSBMIDI(0x09, 0x90 | 0, pitch, 100);
}

void sendMidiOff(int note)
{
    int pitch = 60 + note; // Middle C plus note number

    sendSerialMidi(0x80, pitch, 0);
    // sendUSBMIDI(0x08, 0x80 | 0, pitch, 0);
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
                digitalWrite(LED_PIN, HIGH);
                sendMidiOff(note);
            }
            else
            {
                digitalWrite(LED_PIN, LOW);
                sendMidiOn(note);
            }

            oldState = newState; // Update old pin state
            lastChangeTime = currentTime;
        }
    }
};

#ifdef IS_PRO_MICRO
// All pins used in the pro micro:
//{4, 5, 6, 7, 8, 9, 10, 16, 14, 15, 18, 19, 20};
PinState pinStates[NUM_PINS] = {4, 5, 6, 7, 8, 9, 10, 16, 14, 15, 18, 19, 20};
int noteOffsets[13] = {2, 1, 0, 12, 3, 9, 10, 11, 8, 7, 6, 5, 4}; // The order the notes came out when just pushing the pins in order
#else
// TODO: Handle this for uno
PinState pinStates[NUM_PINS] = {4, 5, 6, 7, 8, 9, 10, 16, 14, 15, 18, 19, 20};
int noteOffsets[13] = {2, 1, 0, 12, 3, 9, 10, 11, 8, 7, 6, 5, 4}; // The order the notes came out when just pushing the pins in order
#endif

int correctedOffsets[13];

void setupCorrectedOffsets()
{
    for (int i = 0; i < 13; i++)
    {
        correctedOffsets[noteOffsets[i]] = i;
    }
}

void setup()
{
    setupCorrectedOffsets();
#ifdef IS_PRO_MICRO
    Serial.begin(9600);
    Serial1.begin(31250); // Begin MIDI communication on Serial1
#else
    Serial.begin(31250);                                          // Begin MIDI communication on Serial
#endif
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
        pinStates[i].handleValue(pinState, correctedOffsets[i]);
    }
}
