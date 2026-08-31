#include "midihandler.h"

#include <qdebug.h>

MidiHandler::MidiHandler() {}

MidiHandler::~MidiHandler()
{ } // TODO: close port

std::expected<void, MidiHandler::Error> MidiHandler::initialize()
{
    try
    {
        this->midiIn_ = std::make_unique<RtMidiIn>();        
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

bool MidiHandler::openPort(uint32_t port)
{
    if (!this->midiIn_) return false;

    uint32_t portCount = this->midiIn_->getPortCount();

    if (this->midiIn_->isPortOpen())
    {
        this->midiIn_->closePort();
    }

    std::cout << "available ports" << std::endl;
    for (uint32_t currentPort = 0; currentPort  < portCount; currentPort ++)
    {
        std::cout << "midi port " << currentPort  << ": " << this->midiIn_->getPortName(currentPort) << std::endl;
    }

    this->midiIn_->setCallback(&MidiHandler::midiCallback, this);
    this->midiIn_->ignoreTypes(true, true, true);

    this->midiIn_->openPort(port);
    return true;
}

void MidiHandler::setSynth(sfizz_synth_t* synth)
{
    this->synth_ = synth;
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
    }

    if (error.message.isEmpty()) {
        return baseMessage;
    }

    return baseMessage + QLatin1StringView(" (") + error.message + QLatin1StringView(")");
}

void MidiHandler::midiCallback(double /*timeStamp*/, std::vector<unsigned char> *message, void *userData)
{
    MidiHandler* self = static_cast<MidiHandler*>(userData);
    if (!self || !self->synth_ || message->empty()) return;

    const std::vector<unsigned char>& msg = *message;
    if (msg.size() < 3) return;

    // https://www.songstuff.com/recording/article/midi-message-format/
    // https://midi.org/midi-1-0-control-change-messages
    uint8_t status = msg[0];
    uint8_t command = status & 0xF0;
    // uint8_t channel = status & 0x0F;
    uint8_t data1 = msg[1];
    uint8_t data2 = msg[2];

    switch (command) {
        case 0x90:
            if (data2 > 0) {
                sfizz_send_note_on(self->synth_, 0, data1, data2);
                break;
            }

            [[fallthrough]];
        case 0x80:
            sfizz_send_note_off(self->synth_, 0, data1, data2);
            break;

        case 0xB0:
            sfizz_send_cc(self->synth_, 0, data1, data2);
            break;

        case 0xE0: {
            const int pitch = ((data2 << 7) | data1) - 8192;
            sfizz_send_pitch_wheel(self->synth_, 0, pitch);
            break;
        }

        default:
            break;
    }
}

// void MidiHandler::handlePitchBend(uint8_t data1, uint8_t data2)
// {
//     uint16_t pitch = (data2 << 7) | data1;   // 14b value - 13b max value -> centered at 0
//     sfizz_send_pitch_wheel(this->synth_, 0, pitch - 8192);  // subtract here to prevent uint underflow
// }
