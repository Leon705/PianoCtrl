#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <QString>
#include <RtMidi.h>
#include <expected>
#include <memory>
#include <sfizz.h>
#include <readerwriterqueue.h>
#include "coreerror.h"
#include "midievent.h"

class MidiHandler
{
public:
    enum class ErrorCode: uint8_t
    {
        ErrorMidiInitFailed,
        ErrorOutOfMemory,
        ErrorFailedToOpenVirtualPort,
        ErrorUnexpected,

    };
    using Error = ::Error<ErrorCode>;
    using MidiQueue = moodycamel::ReaderWriterQueue<MidiEvent>;

    MidiHandler();
    ~MidiHandler();

    std::expected<void, MidiHandler::Error> initialize();
    std::expected<void, MidiHandler::Error>  openPort();

    MidiQueue& midiQueue() noexcept;

    static QString errorToQString(const MidiHandler::Error &error) noexcept;

private:
    static void midiCallback(double timeStamp, std::vector<unsigned char> *message, void *userData);

    void handlePitchBend(uint8_t data1, uint8_t data2);

    std::unique_ptr<RtMidiIn> midiIn_;
    MidiQueue midiQueue_;
};

#endif // MIDIHANDLER_H
