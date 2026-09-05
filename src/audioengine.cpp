#include "audioengine.h"
#include <iostream>

AudioEngine::AudioEngine(sfizz_synth_t* synth, MidiHandler* midiHandler)
    :synth_(synth), midiHandler_(midiHandler)
{ }

AudioEngine::~AudioEngine()
{
    this->stop();
}

std::expected<void, AudioEngine::Error> AudioEngine::start()
{
    jack_status_t status;

    this->client_ = jack_client_open("PianoCtrl", JackNullOption, &status);
    if (!this->client_) {
        return std::unexpected(AudioEngine::ErrorCode::ErrorLaunchJack);
    }

    this->outputPortLeft_   = jack_port_register(this->client_, "output_1", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    this->outputPortRight_  = jack_port_register(this->client_, "output_2", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    this->midiInputPort_    = jack_port_register(this->client_, "midi_in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);

    jack_set_process_callback(this->client_, AudioEngine::processCallback, this);

    jack_nframes_t sampleRate = jack_get_sample_rate(this->client_);
    sfizz_set_sample_rate(synth_, static_cast<float>(sampleRate));

    if (jack_activate(this->client_) != 0) {
        return std::unexpected(AudioEngine::ErrorCode::ErrorActivateJack);
    }

    const char** ports = jack_get_ports(this->client_, NULL, NULL, JackPortIsPhysical | JackPortIsInput);
    if (ports) {
        if (ports[0]) jack_connect(this->client_, jack_port_name(this->outputPortLeft_), ports[0]);
        if (ports[1]) jack_connect(this->client_, jack_port_name(this->outputPortRight_), ports[1]);
        jack_free(ports);
    }

    std::cout << "Successfully launched JACK: SampleRate " << sampleRate << " Hz" << std::endl;

    return {};
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

QString AudioEngine::errorToQString(const AudioEngine::Error& error) noexcept
{
    QString baseMessage;
    switch (error.code) {
        case AudioEngine::ErrorCode::ErrorLaunchJack:
            baseMessage = QStringLiteral("Unable launch JACK");
            break;

        case AudioEngine::ErrorCode::ErrorActivateJack:
            baseMessage = QStringLiteral("Unable activate JACK");
            break;

        case AudioEngine::ErrorCode::ErrorInvalidVolume:
            baseMessage = QStringLiteral("Invalid value for volume provided");
            break;

        case AudioEngine::ErrorCode::ErrorSynthNotInitialized:
            baseMessage = QStringLiteral("Error synth not initialized");
            break;

        default:
            baseMessage = QString();
            break;
    }

    if (error.message.isEmpty()) {
        return baseMessage;
    }

    return baseMessage + QLatin1StringView(" (") + error.message + QLatin1StringView(")");
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

    if (!outL || !outR)
    {
        return 0;
    }

    this->updateVolume();

    if (this->midiInputPort_)
    {
        void* portBuffer = jack_port_get_buffer(this->midiInputPort_, nframes);
        if (portBuffer)
        {
            jack_nframes_t eventCount = jack_midi_get_event_count(portBuffer);
            for (jack_nframes_t i = 0; i < eventCount; ++i)
            {
                jack_midi_event_t event;
                if (jack_midi_event_get(&event, portBuffer, i) == 0)
                {
                    if (event.size < 3) continue;

                    const uint8_t status = event.buffer[0];
                    const uint8_t command = status & 0xF0;
                    const uint8_t data1 = event.buffer[1];
                    const uint8_t data2 = event.buffer[2];
                    const uint32_t sampleOffset = event.time;

                    if (command == static_cast<uint8_t>(MidiEvent::Type::NoteOn) && data2 == 0)
                    {
                        sfizz_send_note_off(this->synth_, sampleOffset, data1, 64);
                    }
                    else if (command == static_cast<uint8_t>(MidiEvent::Type::NoteOn))
                    {
                        sfizz_send_note_on(this->synth_, sampleOffset, data1, data2);
                    }
                    else if (command == static_cast<uint8_t>(MidiEvent::Type::NoteOff))
                    {
                        sfizz_send_note_off(this->synth_, sampleOffset, data1, data2);
                    }
                    else if (command == static_cast<uint8_t>(MidiEvent::Type::ControlChange))
                    {
                        sfizz_send_cc(this->synth_, sampleOffset, data1, data2);
                    }
                    else if (command == static_cast<uint8_t>(MidiEvent::Type::PitchBend))
                    {
                        const int pitch = ((data2 << 7) | data1) - 8192;
                        sfizz_send_pitch_wheel(this->synth_, sampleOffset, pitch);
                    }
                }
            }
        }
    }

    if (this->midiHandler_)
    {
        MidiEvent event;
        while (this->midiHandler_->midiQueue().try_dequeue(event))
        {
            uint8_t sampleOffset = 0;
            switch (event.type)
            {
                case MidiEvent::Type::NoteOn:
                    sfizz_send_note_on(this->synth_, sampleOffset, event.data1, event.data2);
                    break;

                case MidiEvent::Type::NoteOff:
                    sfizz_send_note_off(this->synth_, sampleOffset, event.data1, event.data2);
                    break;

                case MidiEvent::Type::ControlChange:
                    sfizz_send_cc(this->synth_, sampleOffset, event.data1, event.data2);
                    break;

                case MidiEvent::Type::PitchBend:
                {
                    const int pitch = ((event.data2 << 7) | event.data1) - 8192;
                    sfizz_send_pitch_wheel(this->synth_, sampleOffset, pitch);
                    break;
                }
            }
        }
    }

    float* channels[2] = { outL, outR };
    sfizz_render_block(this->synth_, channels, 2, static_cast<int>(nframes));

    return 0;
}

float AudioEngine::getVolume() const
{
    return this->masterVolume_.load(std::memory_order_relaxed);
}

void AudioEngine::setVolume(float volume)
{
    this->masterVolume_.store(std::clamp(volume, 0.0f, 1.0f), std::memory_order_relaxed);
}

void AudioEngine::updateVolume()
{
    const float targetVolume = this->masterVolume_.load(std::memory_order_relaxed);

    if (targetVolume != this->lastVolumeLinear_)
    {
        const float volumeDb = (targetVolume > 0.0001f)
            ? 20.0f * std::log10(targetVolume)
            : -80.0f;

        sfizz_set_volume(this->synth_, volumeDb);
        this->lastVolumeLinear_ = targetVolume;
    }
}
