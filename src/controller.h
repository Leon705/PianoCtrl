#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <memory>
#include <sfizz.h>
#include "audioengine.h"
#include "midihandler.h"
#include "coreerror.h"
#include <QTimer>
#include "src/data/appstate.h"
#include "src/data/databasemanager.h"
#include "src/data/samplelibraryrepository.h"
#include "src/data/soundlibraryrepository.h"

class Controller: public QObject
{
    Q_OBJECT
public:
    enum class ErrorCode: uint8_t {
        ErrorCreateSynth,
        ErrorLoadSampleLibrary,
        ErrorOpenMidiPort,
        ErrorUnexpected,
    };
    using Error = ::Error<ErrorCode>;
    using SystemError = std::variant<Controller::Error, AudioEngine::Error, MidiHandler::Error, FileManager::Error, ISerializable::Error, DatabaseManager::Error>;

    explicit Controller(QObject *parent = nullptr);

    std::expected<void, Controller::SystemError> initialize();
    std::expected<void, Controller::SystemError> loadSoundLibrary(const int id);
    std::expected<void, Controller::SystemError> loadSoundLibrary(const SoundLibrary& soundLibrary);
    std::expected<std::vector<SampleLibrary>, Controller::SystemError> getAllSampleLibrariesInSoundLibrary(const int id);
    std::expected<void, Controller::SystemError> switchSampleLibrary(const int id);
    std::expected<void, Controller::SystemError> switchMidiPort(uint32_t port);
    std::expected<void, Controller::SystemError> saveAppState();

    sfizz_synth_t* getSynth() const;

    void setMasterVolume(const float volume);
    AppState appState() const;

    static QString errorToQString(const Controller::SystemError &error) noexcept;

    std::expected<SampleLibrary, Controller::SystemError> currentSampleLibrary();
private:
    struct SfizzSynthDeleter {
        void operator()(sfizz_synth_t* synth) const;
    };

    std::unique_ptr<DatabaseManager> soundLibraryDatabaseManager_;
    std::unique_ptr<SampleLibraryRepository> sampleLibraryRepository_;
    std::unique_ptr<SoundLibraryRepository> soundLibraryRepository_;
    std::unique_ptr<sfizz_synth_t, SfizzSynthDeleter> synth_;
    std::unique_ptr<AudioEngine> audioEngine_;
    std::unique_ptr<MidiHandler> midiHandler_;

    AppState appState_;
    QTimer saveTimer_;
private slots:
    void persistAppState();
};

#endif // CONTROLLER_H
