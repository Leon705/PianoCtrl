#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <jack/jack.h>
#include <sfizz.h>
#include <iostream>

class AudioEngine
{
public:
    AudioEngine(sfizz_synth_t* synth);
    ~AudioEngine();

    bool start();
    void stop();

    uint32_t getSampleRate() const;
    uint32_t getBufferSize() const;

private:
    static int processCallback(jack_nframes_t nframes, void* arg); // static Jack callback

    int process(jack_nframes_t nframes);

    sfizz_synth_t* synth_;
    jack_client_t* client_;
    jack_port_t* outputPortLeft_;
    jack_port_t* outputPortRight_;
};

#endif // AUDIOENGINE_H
