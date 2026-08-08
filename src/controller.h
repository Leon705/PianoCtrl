#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <memory>
#include <sfizz.h>
#include "audioengine.h"
#include "midihandler.h"
#include "coreerror.h"

class Controller
{
public:
    enum class ErrorCode: uint8_t {
        ErrorCreateSynth,
        ErrorLoadSampleLibrary,
    };
    using Error = ::Error<ErrorCode>;
    using SystemError = std::variant<Controller::Error, AudioEngine::Error, MidiHandler::Error>;

    Controller();

    std::expected<void, Controller::SystemError> initialize();
    std::expected<void, Controller::SystemError> loadSampleLibrary(const QString &path);

    sfizz_synth_t* getSynth() const;

    static QString errorToQString(const Controller::SystemError &error) noexcept;

private:
    struct SfizzSynthDeleter {
        void operator()(sfizz_synth_t* synth) const;
    };

    std::unique_ptr<sfizz_synth_t, SfizzSynthDeleter> synth_;
    std::unique_ptr<AudioEngine> audioEngine_;
    std::unique_ptr<MidiHandler> midiHandler_;
};

#endif // CONTROLLER_H
