#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <atomic>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <sfizz.h>
#include <QString>
#include <expected>
#include "coreerror.h"
#include "midihandler.h"

class AudioEngine
{
public:
    enum class ErrorCode: uint8_t {
        ErrorLaunchJack,
        ErrorActivateJack,
        ErrorInvalidVolume, // @depricated
        ErrorSynthNotInitialized,
    };
    using Error = ::Error<ErrorCode>;

    AudioEngine(sfizz_synth_t* synth, MidiHandler* midiHandler);
    ~AudioEngine();

    std::expected<void, AudioEngine::Error> start();
    void stop();

    uint32_t getSampleRate() const;
    uint32_t getBufferSize() const;

    float getVolume() const;
    void setVolume(float volume);

    static QString errorToQString(const AudioEngine::Error& error) noexcept;
private:
    static int processCallback(jack_nframes_t nframes, void* arg); // static Jack callback matching the C API

    int process(jack_nframes_t nframes);
    void updateVolume();

    sfizz_synth_t* synth_;
    MidiHandler* midiHandler_;
    jack_client_t* client_{nullptr};
    jack_port_t* outputPortLeft_{nullptr};
    jack_port_t* outputPortRight_{nullptr};
    jack_port_t* midiInputPort_{nullptr};
    std::atomic<float> masterVolume_{1.0f};
    float lastVolumeLinear_{-1.0f};
};

#endif // AUDIOENGINE_H
