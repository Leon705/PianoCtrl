#include "appstate.h"

AppState::AppState() {}

int AppState::getLastSoundLibraryId() const
{
    return this->lastSoundLibraryId;
}

void AppState::setLastSoundLibraryId(const int id)
{
    this->lastSoundLibraryId = id;
}

int AppState::getLastSampleLibraryId() const
{
    return this->lastSampleLibraryId;
}

void AppState::setLastSampleLibraryId(const int id)
{
    this->lastSampleLibraryId = id;
}

float AppState::getMasterVolume() const
{
    return masterVolume;
}

void AppState::setMasterVolume(float newMasterVolume)
{
    masterVolume = newMasterVolume;
}

int AppState::getSelectedMidiChannel() const
{
    return selectedMidiChannel;
}

void AppState::setSelectedMidiChannel(int newSelectedMidiChannel)
{
    selectedMidiChannel = newSelectedMidiChannel;
}

QJsonObject AppState::toJson() const
{
    QJsonObject json;
    json[AppState::KeyLastSoundLibraryId]       = this->lastSoundLibraryId;
    json[AppState::KeyLastSampleLibraryId]      = this->lastSampleLibraryId;
    json[AppState::KeyMasterVolume]             = static_cast<double>(this->masterVolume);
    json[AppState::KeySelectedMidiChannel]      = this->selectedMidiChannel;

    return json;
}

std::expected<void, ISerializable::Error> AppState::fromJson(const QByteArray &json)
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        return std::unexpected(ISerializable::Error{
            ISerializable::ErrorCode::ErrorInvalidFormat,
            parseError.errorString()
        });
    }

    const QJsonObject& jsonObject = doc.object();

    this->lastSoundLibraryId = jsonObject[AppState::KeyLastSoundLibraryId].toInt(1);
    this->lastSampleLibraryId = jsonObject[AppState::KeyLastSampleLibraryId].toInt(1);
    this->masterVolume = static_cast<float>(jsonObject[AppState::KeyMasterVolume].toDouble(0.75));
    this->selectedMidiChannel = jsonObject[AppState::KeySelectedMidiChannel].toInt(1);

    return {};
}

