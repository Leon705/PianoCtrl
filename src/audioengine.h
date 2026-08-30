#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <atomic>
#include <jack/jack.h>
#include <sfizz.h>
#include <QString>
#include <expected>
#include "coreerror.h"

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

    AudioEngine(sfizz_synth_t* synth);
    ~AudioEngine();

    std::expected<void, AudioEngine::Error> start();
    void stop();

    uint32_t getSampleRate() const;

    uint32_t getBufferSize() const;

    static QString errorToQString(const AudioEngine::Error& error) noexcept;

    float getVolume() const;
    void setVolume(float volume);

private:
    static int processCallback(jack_nframes_t nframes, void* arg); // static Jack callback matching the C API

    int process(jack_nframes_t nframes);
    void updateVolume();

    sfizz_synth_t* synth_;
    jack_client_t* client_;
    jack_port_t* outputPortLeft_;
    jack_port_t* outputPortRight_;
    std::atomic<float> masterVolume_{1.0f};
    float lastVolumeLinear_{-1.0f};
};

#endif // AUDIOENGINE_H
