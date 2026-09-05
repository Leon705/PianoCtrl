#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include <cstdint>

struct MidiEvent {
    enum class Type : uint8_t {
        NoteOn        = 0x90,
        NoteOff       = 0x80,
        ControlChange = 0xB0,
        PitchBend     = 0xE0
    };

    Type type;
    uint8_t channel{0};
    uint8_t data1{0};
    uint8_t data2{0};
    double deltaTimeSeconds{0};
};

#endif // MIDIEVENT_H
