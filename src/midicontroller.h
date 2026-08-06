#ifndef MIDICONTROLLER_H
#define MIDICONTROLLER_H

#include <RtMidi.h>

class MidiController
{

public:
    MidiController();
    ~MidiController();

    void sendCC(unsigned char cc, unsigned char value);
private:
    RtMidiOut *midiOut;
};

#endif // MIDICONTROLLER_H
