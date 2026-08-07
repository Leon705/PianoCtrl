#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <memory>
#include <sfizz.h>
#include "audioengine.h"
#include "midihandler.h"

class Controller
{
public:
    Controller();

    void initialize();
    sfizz_synth_t* getSynth() const;

private:
    struct SfizzSynthDeleter {
        void operator()(sfizz_synth_t* synth) const;
    };

    std::unique_ptr<sfizz_synth_t, SfizzSynthDeleter> synth_;
    std::unique_ptr<AudioEngine> audioEngine_;
    std::unique_ptr<MidiHandler> midiHandler_;
};

#endif // CONTROLLER_H
