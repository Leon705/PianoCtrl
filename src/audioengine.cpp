#include "audioengine.h"

#include <qdebug.h>

AudioEngine::AudioEngine(sfizz_synth_t* synth)
    :synth_(synth)
{ }

AudioEngine::~AudioEngine()
{
    this->stop();
}

bool AudioEngine::start()
{
    jack_status_t status;

    this->client_ = jack_client_open("PianoCtrl", JackNullOption, &status);
    if (!this->client_) {
        std::cerr << "Error Could not launch JACK: " << status << std::endl;
        return false;
    }

    this->outputPortLeft_ = jack_port_register(this->client_, "output_1",
                                          JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    this->outputPortRight_ = jack_port_register(this->client_, "output_2",
                                           JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    jack_set_process_callback(this->client_, AudioEngine::processCallback, this);

    jack_nframes_t sampleRate = jack_get_sample_rate(this->client_);
    sfizz_set_sample_rate(synth_, static_cast<float>(sampleRate));

    if (jack_activate(this->client_) != 0) {
        std::cerr << "Error Could not activate JACK client!" << std::endl;
        return false;
    }

    const char** ports = jack_get_ports(this->client_, NULL, NULL,
                                        JackPortIsPhysical | JackPortIsInput);
    if (ports) {
        if (ports[0]) jack_connect(this->client_, jack_port_name(this->outputPortLeft_), ports[0]);
        if (ports[1]) jack_connect(this->client_, jack_port_name(this->outputPortRight_), ports[1]);
        jack_free(ports);
    }

    std::cout << "Successfully launched JACK: SampleRate " << sampleRate << " Hz" << std::endl;
    return true;
}

void AudioEngine::stop()
{
    if (this->client_) {
        jack_deactivate(this->client_);
        jack_client_close(this->client_);
        this->client_ = nullptr;
    }
}

uint32_t AudioEngine::getSampleRate() const
{
    return this->client_ ? jack_get_sample_rate(this->client_) : 44100;
}

uint32_t AudioEngine::getBufferSize() const
{
    return this->client_ ? jack_get_buffer_size(this->client_) : 256;
}

int AudioEngine::processCallback(jack_nframes_t nframes, void *arg)
{
    return static_cast<AudioEngine*>(arg)->process(nframes);
}

int AudioEngine::process(jack_nframes_t nframes)
{
    if (!this->synth_) return 0;

    float* outL = static_cast<float*>(jack_port_get_buffer(this->outputPortLeft_, nframes));
    float* outR = static_cast<float*>(jack_port_get_buffer(this->outputPortRight_, nframes));

    float* channels[2] = { outL, outR };
    sfizz_render_block(this->synth_, channels, 2, static_cast<int>(nframes));

    return 0;
}



