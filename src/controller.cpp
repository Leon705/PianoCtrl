#include "controller.h"

#include <qdebug.h>

Controller::Controller()
{}

void Controller::initialize() {
    this->synth_.reset(sfizz_create_synth());

    if (!this->synth_) {
        std::cerr << "Error: Could not create synth" << std::endl;
        return;
    }

    this->audioEngine_ = std::make_unique<AudioEngine>(this->synth_.get());
    this->audioEngine_->start();

    sfizz_set_sample_rate(this->synth_.get(), this->audioEngine_->getSampleRate());
    sfizz_set_samples_per_block(this->synth_.get(), this->audioEngine_->getBufferSize());

    bool ok = sfizz_load_file(this->synth_.get(), "/home/leon/Dokumente/sfz_samplelibs/kamoepiano301/kamoepiano301.sfz");
    qDebug() << "loaded?: " << ok;
    sfizz_send_note_on(this->synth_.get(), 0, 65, 100);

    this->midiHandler_ = std::make_unique<MidiHandler>();
    this->midiHandler_->initialize();
    this->midiHandler_->setSynth(this->synth_.get());
    this->midiHandler_->openPort(2);
}

sfizz_synth_t *Controller::getSynth() const
{
    return this->synth_.get();
}

void Controller::SfizzSynthDeleter::operator()(sfizz_synth_t *synth) const
{
    if (synth)
    {
        sfizz_free(synth);
    }
}
