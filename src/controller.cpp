#include "controller.h"

#include <qdebug.h>

Controller::Controller()
{}

std::expected<void, Controller::SystemError> Controller::initialize() {
    this->synth_.reset(sfizz_create_synth());

    if (!this->synth_)
    {
        return std::unexpected(Controller::ErrorCode::ErrorCreateSynth);
    }

    this->audioEngine_ = std::make_unique<AudioEngine>(this->synth_.get());
    if (auto success = this->audioEngine_->start(); !success)
    {
        return std::unexpected(std::move(success.error()));
    }

    sfizz_set_sample_rate(this->synth_.get(), this->audioEngine_->getSampleRate());
    sfizz_set_samples_per_block(this->synth_.get(), this->audioEngine_->getBufferSize());

    if (auto success = this->loadSampleLibrary(QStringLiteral("/home/leon/Dokumente/sfz_samplelibs/kamoepiano301/kamoepiano301.sfz")); !success)
    {
        return std::unexpected(std::move(success.error()));
    }

    sfizz_send_note_on(this->synth_.get(), 0, 65, 100);

    this->midiHandler_ = std::make_unique<MidiHandler>();
    this->midiHandler_->initialize();
    this->midiHandler_->setSynth(this->synth_.get());
    this->midiHandler_->openPort(2);

    return {};
}

std::expected<void, Controller::SystemError> Controller::loadSampleLibrary(const QString &path)
{
    const QByteArray pathUtf8 = path.toUtf8();
    if (!sfizz_load_file(this->synth_.get(), pathUtf8))
    {
        return std::unexpected(Controller::Error{
            Controller::ErrorCode::ErrorLoadSampleLibrary,
            path
        });
    }

    return {};
}

sfizz_synth_t *Controller::getSynth() const
{
    return this->synth_.get();
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

    return QString();
}

void Controller::SfizzSynthDeleter::operator()(sfizz_synth_t *synth) const
{
    if (synth)
    {
        sfizz_free(synth);
    }
}
