#ifndef APPSTATE_H
#define APPSTATE_H

#include "iserializable.h"

class AppState: public ISerializable
{
public:
    AppState();

    QString getLastSampleLibraryPath() const;
    void setLastSampleLibraryPath(const QString &newLastSampleLibraryPath);
    float getMasterVolume() const;
    void setMasterVolume(float newMasterVolume);
    int getSelectedMidiChannel() const;
    void setSelectedMidiChannel(int newSelectedMidiChannel);

private:
    QString lastSampleLibraryPath;
    float masterVolume = 0.75f;
    int selectedMidiChannel = 1;

    static constexpr auto KeyLastSampleLibraryPath  = QLatin1StringView("lastSfzPath");
    static constexpr auto KeyMasterVolume           = QLatin1StringView("masterVolume");
    static constexpr auto KeySelectedMidiChannel    = QLatin1StringView("midiChannel");

    // ISerializable interface
public:
    QJsonObject toJson() const override;
    std::expected<void, ISerializable::Error> fromJson(const QByteArray &json) override;
};

#endif // APPSTATE_H
