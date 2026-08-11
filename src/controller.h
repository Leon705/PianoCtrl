#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <memory>
#include <sfizz.h>
#include "audioengine.h"
#include "midihandler.h"
#include "coreerror.h"
#include <QTimer>
#include "src/data/appstate.h"
#include "src/data/filemanager.h"

class Controller: public QObject
{
    Q_OBJECT
public:
    enum class ErrorCode: uint8_t {
        ErrorCreateSynth,
        ErrorLoadSampleLibrary,
        ErrorUnexpected,
    };
    using Error = ::Error<ErrorCode>;
    using SystemError = std::variant<Controller::Error, AudioEngine::Error, MidiHandler::Error, FileManager::Error>;

    explicit Controller(QObject *parent = nullptr);

    std::expected<void, Controller::SystemError> initialize();
    std::expected<void, Controller::SystemError> loadSampleLibrary(const QString &path);
    std::expected<void, Controller::SystemError> saveAppState();

    sfizz_synth_t* getSynth() const;

    void setMasterVolume(const float volume);

    static QString errorToQString(const Controller::SystemError &error) noexcept;

private:
    struct SfizzSynthDeleter {
        void operator()(sfizz_synth_t* synth) const;
    };

    std::unique_ptr<sfizz_synth_t, SfizzSynthDeleter> synth_;
    std::unique_ptr<AudioEngine> audioEngine_;
    std::unique_ptr<MidiHandler> midiHandler_;

    AppState appState_;
    QTimer saveTimer_;
private slots:
    void persistAppState();
};

#endif // CONTROLLER_H
