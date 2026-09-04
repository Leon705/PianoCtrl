#include "controller.h"
#include "src/data/appstate.h"
#include "src/data/filemanager.h"
#include "src/data/filepaths.h"

#include <qdebug.h>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    this->saveTimer_.setSingleShot(true);
    this->saveTimer_.setInterval(1500);
    QObject::connect(&this->saveTimer_, &QTimer::timeout, this, &Controller::persistAppState);
}

std::expected<void, Controller::SystemError> Controller::initialize() {
    if (auto success = FileManager::loadFromFile(this->appState_, FilePaths::appStateFilePath()); !success)
    {
        std::cerr << Controller::errorToQString(success.error()).toStdString() << std::endl;
    }

    this->soundLibraryDatabaseManager_ = std::make_unique<DatabaseManager>(FilePaths::soundlibDatabaseFilePath());
    if (auto res = this->soundLibraryDatabaseManager_->initDatabase(QStringLiteral(":/sql/soundlib_db.sql")); !res)
    {
        return std::unexpected(res.error());
    }

    this->sampleLibraryRepository_ = std::make_unique<SampleLibraryRepository>(*this->soundLibraryDatabaseManager_);
    this->soundLibraryRepository_ = std::make_unique<SoundLibraryRepository>(*this->soundLibraryDatabaseManager_);
/*
    SampleLibrary testLib{
        .displayName = QStringLiteral("kamoe301"),
        .path = QStringLiteral("/home/leon/Documents/sfz_samplelibs/kamoepiano301/kamoepiano301/kamoepiano301.sfz")
    };

    sampleLibraryRepository_->add(testLib);

    SoundLibrary soundLib {
        .displayName = QStringLiteral("TestSoundLib"),
        .description = QStringLiteral("TEST"),
        .sampleLibraries = {testLib}
    };

    soundLibraryRepository_->add(soundLib);
*/

    this->synth_.reset(sfizz_create_synth());

    if (!this->synth_)
    {
        return std::unexpected(Controller::ErrorCode::ErrorCreateSynth);
    }

    this->midiHandler_ = std::make_unique<MidiHandler>();
    if (auto res = this->midiHandler_->initialize(); !res)
    {
        return std::unexpected(std::move(res.error()));
    }
    this->midiHandler_->setSynth(this->synth_.get());

    this->audioEngine_ = std::make_unique<AudioEngine>(this->synth_.get(), this->midiHandler_.get());
    if (auto res = this->audioEngine_->start(); !res)
    {
        return std::unexpected(res.error());
    }
    sfizz_set_sample_rate(this->synth_.get(), this->audioEngine_->getSampleRate());
    sfizz_set_samples_per_block(this->synth_.get(), this->audioEngine_->getBufferSize());
    sfizz_set_num_voices(this->synth_.get(), 64);

    return {};
}

std::expected<SampleLibrary, Controller::SystemError> Controller::currentSampleLibrary()
{
    auto res = this->sampleLibraryRepository_->getById(this->appState_.getLastSampleLibraryId());
    if (!res)
    {
        return std::unexpected(res.error());
    }

    return res.value();
}

std::expected<void, Controller::SystemError> Controller::loadSoundLibrary(const int id)
{
    if (auto res = this->soundLibraryRepository_->getById(id); !res)
    {
        return std::unexpected(res.error());
    } else {
        return this->loadSoundLibrary(res.value());
    }
}

std::expected<void, Controller::SystemError> Controller::loadSoundLibrary(const SoundLibrary &soundLibrary)
{
    if (soundLibrary.sampleLibraries.empty())
    {
        return std::unexpected(Controller::ErrorCode::ErrorUnexpected);
    }

    const auto &firstSampleLibrary = soundLibrary.sampleLibraries.front();

    this->switchSampleLibrary(firstSampleLibrary.id);
    this->appState_.setLastSoundLibraryId(soundLibrary.id);
    this->saveTimer_.start();
    return {};
}

std::expected<void, Controller::SystemError> Controller::switchSampleLibrary(const int id)
{
    if (!this->synth_)
    {
        return std::unexpected(Controller::ErrorCode::ErrorUnexpected);
    }

    auto res = this->sampleLibraryRepository_->getById(id);
    if (!res || !res.has_value())
    {
        return std::unexpected(Controller::ErrorCode::ErrorLoadSampleLibrary);
    }

    if (!sfizz_load_file(this->synth_.get(), res.value().path.toUtf8().constData()))
    {
        return std::unexpected(Controller::Error{
            Controller::ErrorCode::ErrorLoadSampleLibrary,
            res.value().path
        });
    }

    this->appState_.setLastSampleLibraryId(id);
    this->saveTimer_.start();
    return {};
}

std::expected<std::vector<SampleLibrary>, Controller::SystemError> Controller::getAllSampleLibrariesInSoundLibrary(const int id)
{
    auto res = this->soundLibraryRepository_->getById(id);

    if (!res || !res.has_value())
    {
        return std::unexpected(res.error());
    }

    return res.value().sampleLibraries;
}

std::expected<void, Controller::SystemError> Controller::switchMidiPort(uint32_t port)
{
    if (!this->midiHandler_)
    {
        return std::unexpected(Controller::ErrorCode::ErrorUnexpected);
    }

    if (!this->midiHandler_->openPort(port))
    {
        return std::unexpected(Controller::Error{
            Controller::ErrorCode::ErrorOpenMidiPort,
            QString::number(port)
        });
    }

    // TODO: save port in appState

    return {};
}

std::expected<void, Controller::SystemError> Controller::saveAppState()
{
    return FileManager::saveToFile(this->appState_, FilePaths::appStateFilePath());
}

sfizz_synth_t *Controller::getSynth() const
{
    return this->synth_.get();
}

void Controller::setMasterVolume(const float volume)
{
    if (this->audioEngine_)
    {
        this->audioEngine_->setVolume(volume);
    }

    this->appState_.setMasterVolume(volume);
    this->saveTimer_.start();
}

AppState Controller::appState() const
{
    return this->appState_;
}

QString Controller::errorToQString(const Controller::SystemError &error) noexcept
{
    if (std::holds_alternative<Controller::Error>(error))
    {
        auto controllerError = std::get<Controller::Error>(error);

        QString baseMessage;
        switch (controllerError.code)
        {
            case Controller::ErrorCode::ErrorCreateSynth:
                baseMessage = QStringLiteral("Unable to create Sfizz-synth");
                break;

            case Controller::ErrorCode::ErrorLoadSampleLibrary:
                baseMessage = QStringLiteral("Unable to load sample library");
                break;

            case Controller::ErrorCode::ErrorOpenMidiPort:
                baseMessage = QStringLiteral("Failed to open midi-port");
                break;

            case Controller::ErrorCode::ErrorUnexpected:
                baseMessage = QStringLiteral("Unexpected error");
                break;
        }

        if (controllerError.message.isEmpty()) {
            return baseMessage;
        }

        return baseMessage + QLatin1StringView(" (") + controllerError.message + QLatin1StringView(")");
    }
    else if (std::holds_alternative<AudioEngine::Error>(error))
    {
        return AudioEngine::errorToQString(std::get<AudioEngine::Error>(error));
    }
    else if (std::holds_alternative<MidiHandler::Error>(error))
    {
        return MidiHandler::errorToQString(std::get<MidiHandler::Error>(error));
    }
    else if (std::holds_alternative<ISerializable::Error>(error))
    {
        return ISerializable::errorToQString(std::get<ISerializable::Error>(error));
    }
    else if (std::holds_alternative<FileManager::Error>(error))
    {
        return FileManager::errorToQString(std::get<FileManager::Error>(error));
    }
    else if (std::holds_alternative<DatabaseManager::Error>(error))
    {
        return DatabaseManager::errorToQString(std::get<DatabaseManager::Error>(error));
    }

    return QString();
}

void Controller::persistAppState()
{
    if (auto res = this->saveAppState(); !res)
    {
        std::cerr << Controller::errorToQString(res.error()).toStdString() << std::endl;
    }
}

void Controller::SfizzSynthDeleter::operator()(sfizz_synth_t *synth) const
{
    if (synth)
    {
        sfizz_free(synth);
    }
}
