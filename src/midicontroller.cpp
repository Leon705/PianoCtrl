#include "midicontroller.h"

#include <qdebug.h>

MidiController::MidiController()
{
    this->midiOut = new RtMidiOut();
    this->midiOut->openVirtualPort("PianoCtrl");
}

MidiController::~MidiController()
{
    delete this->midiOut;
}

void MidiController::sendCC(unsigned char cc, unsigned char value)
{
    std::vector<unsigned char> message = {0xB0, cc, value};
    midiOut->sendMessage(&message);
    qDebug() << "Sending Midi signal " << cc << " " << (int) value;
}


