#include "appstate.h"

AppState::AppState() {}

QString AppState::getLastSampleLibraryPath() const
{
    return lastSampleLibraryPath;
}

void AppState::setLastSampleLibraryPath(const QString &newLastSampleLibraryPath)
{
    lastSampleLibraryPath = newLastSampleLibraryPath;
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
    json[AppState::KeyLastSampleLibraryPath]    = this->lastSampleLibraryPath;
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

    if (!jsonObject.contains(AppState::KeyLastSampleLibraryPath))
    {
        return std::unexpected(ISerializable::Error{
            ISerializable::ErrorCode::ErrorInvalidFormat,
            QStringLiteral("missing key: ") + AppState::KeyLastSampleLibraryPath
        });
    }


    this->lastSampleLibraryPath = jsonObject[AppState::KeyLastSampleLibraryPath].toString();
    this->masterVolume = static_cast<float>(jsonObject[AppState::KeyMasterVolume].toDouble(0.75));
    this->selectedMidiChannel = jsonObject[AppState::KeySelectedMidiChannel].toInt(1);

    return {};
}

