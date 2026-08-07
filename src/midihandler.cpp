#include "midihandler.h"

MidiHandler::MidiHandler() {}

MidiHandler::~MidiHandler()
{ }

void MidiHandler::initialize()
{
    try {
        this->midiIn_ = std::make_unique<RtMidiIn>();
    } catch (RtMidiError error) {
        error.printMessage();
    }
}

bool MidiHandler::openPort(uint32_t port)
{
    if (!this->midiIn_) return false;

    uint32_t portCount = this->midiIn_->getPortCount();

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

void MidiHandler::midiCallback(double timeStamp, std::vector<unsigned char> *message, void *userData)
{
    MidiHandler* self = static_cast<MidiHandler*>(userData);
    if (!self || !self->synth_ || message->empty()) return;

    const std::vector<unsigned char>& msg = *message;
    if (msg.size() < 3) return;


    // https://www.songstuff.com/recording/article/midi-message-format/
    uint8_t status = msg[0];
    uint8_t command = status & 0xF0;
    // uint8_t channel = status & 0x0F;
    uint8_t data1 = msg[1];
    uint8_t data2 = msg[2];

    switch (command) {
        case 0x80:
            sfizz_send_note_off(self->synth_, 0, data1, data2);
            break;

        case 0x90:
            sfizz_send_note_on(self->synth_, 0, data1, data2);
            break;

        default:
            break;
    }
}
