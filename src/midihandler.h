#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <RtMidi.h>
#include <memory>
#include <sfizz.h>

class MidiHandler
{
public:
    MidiHandler();
    ~MidiHandler();

    void initialize();

    bool openPort(uint32_t port);
    void setSynth(sfizz_synth_t* synth);

private:
    static void midiCallback(double timeStamp, std::vector<unsigned char> *message, void *userData);
    std::unique_ptr<RtMidiIn> midiIn_;
    sfizz_synth_t* synth_;
};

#endif // MIDIHANDLER_H
