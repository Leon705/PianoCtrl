#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "midicontroller.h"
#include <memory>
#include <sfizz.h>
#include "audioengine.h"

class Controller
{
public:
    Controller();

    MidiController& getMidiController();
    sfizz_synth_t* getSynth() const;

private:
    struct SfizzSynthDeleter {
        void operator()(sfizz_synth_t* synth) const;
    };

    void initialize();

    MidiController midiController_;
    std::unique_ptr<sfizz_synth_t, SfizzSynthDeleter> synth_;
    std::unique_ptr<AudioEngine> audioEngine_;
};

#endif // CONTROLLER_H
