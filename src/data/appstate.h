#ifndef APPSTATE_H
#define APPSTATE_H

#include "iserializable.h"

class AppState: public ISerializable
{
public:
    AppState();

    int getLastSoundLibraryId() const;
    void setLastSoundLibraryId(const int id);
    int getLastSampleLibraryId() const;
    void setLastSampleLibraryId(const int id);
    float getMasterVolume() const;
    void setMasterVolume(float newMasterVolume);
    int getSelectedMidiChannel() const;
    void setSelectedMidiChannel(int newSelectedMidiChannel);

private:
    int lastSoundLibraryId = 1;
    int lastSampleLibraryId = 1;
    float masterVolume = 0.75f;
    int selectedMidiChannel = 1;

    static constexpr auto KeyLastSoundLibraryId     = QLatin1StringView("lastSoundLibraryId");
    static constexpr auto KeyLastSampleLibraryId     = QLatin1StringView("lastSampleLibraryId");
    static constexpr auto KeyMasterVolume           = QLatin1StringView("masterVolume");
    static constexpr auto KeySelectedMidiChannel    = QLatin1StringView("midiChannel");

    // ISerializable interface
public:
    QJsonObject toJson() const override;
    std::expected<void, ISerializable::Error> fromJson(const QByteArray &json) override;
};

#endif // APPSTATE_H
