#include "midihandler.h"

#include <qdebug.h>

MidiHandler::MidiHandler() : midiQueue_(1024) {}

MidiHandler::~MidiHandler()
{
    if (this->midiIn_)
    {
        if (this->midiIn_->isPortOpen())
        {
            this->midiIn_->closePort();
        }
    }
}

std::expected<void, MidiHandler::Error> MidiHandler::initialize()
{
    try
    {
        this->midiIn_ = std::make_unique<RtMidiIn>(RtMidi::UNIX_JACK, "PianoCtrl");
        return {};
    }
    catch (const RtMidiError &error)
    {
        return std::unexpected(MidiHandler::Error{
            MidiHandler::ErrorCode::ErrorMidiInitFailed,
            QString::fromStdString(error.getMessage())
        });
    }
    catch (const std::bad_alloc&)
    {
        return std::unexpected(MidiHandler::ErrorCode::ErrorOutOfMemory);
    }
}

std::expected<void, MidiHandler::Error> MidiHandler::openPort()
{
    if (!this->midiIn_) return std::unexpected(MidiHandler::ErrorCode::ErrorUnexpected);


    if (this->midiIn_->isPortOpen())
    {
        this->midiIn_->closePort();
    }

    uint32_t portCount = this->midiIn_->getPortCount();
    std::cout << "available ports" << std::endl;
    for (uint32_t currentPort = 0; currentPort  < portCount; currentPort ++)
    {
        std::cout << "midi port " << currentPort  << ": " << this->midiIn_->getPortName(currentPort) << std::endl;
    }

    try
    {
        this->midiIn_->openPort(1);
        this->midiIn_->setCallback(&MidiHandler::midiCallback, this);
        this->midiIn_->ignoreTypes(true, true, true);
    }
    catch (const RtMidiError &error)
    {
        return std::unexpected(MidiHandler::ErrorCode::ErrorFailedToOpenVirtualPort);
    }

    return {};
}

MidiHandler::MidiQueue &MidiHandler::midiQueue() noexcept
{
    return this->midiQueue_;
}

QString MidiHandler::errorToQString(const Error &error) noexcept
{
    QString baseMessage;
    switch (error.code)
    {
        case MidiHandler::ErrorCode::ErrorMidiInitFailed:
            baseMessage = QStringLiteral("Unable to initialize midi-in");
            break;

        case MidiHandler::ErrorCode::ErrorOutOfMemory:
            baseMessage = QStringLiteral("Midi handler ran out of memory");
            break;

        case MidiHandler::ErrorCode::ErrorFailedToOpenVirtualPort:
            baseMessage = QStringLiteral("Failed to open virtual midi-in port");
            break;

        case MidiHandler::ErrorCode::ErrorUnexpected:
            baseMessage = QStringLiteral("Unexpected Midi error");
            break;
    }

    if (error.message.isEmpty()) {
        return baseMessage;
    }

    return baseMessage + QLatin1StringView(" (") + error.message + QLatin1StringView(")");
}

void MidiHandler::midiCallback(double timeStamp, std::vector<unsigned char> *message, void *userData)
{
    MidiHandler* self = static_cast<MidiHandler*>(userData);
    if (!self || message->empty()) return;

    const std::vector<unsigned char>& msg = *message;
    if (msg.size() < 3) return;

    // https://www.songstuff.com/recording/article/midi-message-format/
    // https://midi.org/midi-1-0-control-change-messages
    const uint8_t status = msg[0];
    const uint8_t command = status & 0xF0;

    MidiEvent event;
    event.channel = status & 0x0F;
    event.data1 = msg[1];
    event.data2 = msg[2];
    event.deltaTimeSeconds = 0;

    if (command == static_cast<uint8_t>(MidiEvent::Type::NoteOn) && event.data2 == 0)
    {
        event.type = MidiEvent::Type::NoteOff;
        event.data2 = 64;
    }
    else if (command == static_cast<uint8_t>(MidiEvent::Type::NoteOn))
    {
        event.data2 = std::max(static_cast<uint8_t>(30), event.data2);
        event.type = static_cast<MidiEvent::Type>(command);
    }
    else
    {
        event.type = static_cast<MidiEvent::Type>(command);
    }

    self->midiQueue_.enqueue(event);
}

// void MidiHandler::handlePitchBend(uint8_t data1, uint8_t data2)
// {
//     uint16_t pitch = (data2 << 7) | data1;   // 14b value - 13b max value -> centered at 0
//     sfizz_send_pitch_wheel(this->synth_, 0, pitch - 8192);  // subtract here to prevent uint underflow
// }
