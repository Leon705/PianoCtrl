#include "filemanager.h"

#include <QFile>
#include <QSaveFile>

std::expected<void, FileManager::Error> FileManager::saveToFile(const ISerializable &model, const QString &path)
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        return std::unexpected(FileManager::Error{
            FileManager::ErrorCode::ErrorWriteFailed,
            path
        });
    }

    const QJsonDocument doc(model.toJson());
    file.write(doc.toJson(QJsonDocument::Indented));

    if (!file.commit())
    {
        return std::unexpected(FileManager::Error{
            FileManager::ErrorCode::ErrorWriteFailed,
            path
        });
    }

    return {};
}

std::expected<void, FileManager::Error> FileManager::loadFromFile(ISerializable &model, const QString &path)
{
    QFile file(path);
    if (!file.exists()) {
        return std::unexpected(FileManager::Error{
            FileManager::ErrorCode::ErrorFileNotFound,
            path
        });
    }

    if (!file.open(QIODevice::ReadOnly)) {
        return std::unexpected(FileManager::Error{
            FileManager::ErrorCode::ErrorReadFailed,
            path
        });
    }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        return std::unexpected(FileManager::Error{
            FileManager::ErrorCode::ErrorBadJson,
            parseError.errorString()
        });
    }

    if (auto success = model.fromJson(doc.object()); !success) {
        return std::unexpected(FileManager::Error{
            FileManager::ErrorCode::ErrorDeserializingFailed,
            std::move(success.error().message)
        });
    }

    return {};
}

QString FileManager::errorToQString(const FileManager::Error &error) noexcept
{
    QString baseMessage;
    switch (error.code)
    {
    case FileManager::ErrorCode::ErrorFileNotFound:
        baseMessage = QStringLiteral("File does not exist");
        break;

    case FileManager::ErrorCode::ErrorReadFailed:
        baseMessage = QStringLiteral("Unable to read file");
        break;

    case FileManager::ErrorCode::ErrorWriteFailed:
        baseMessage = QStringLiteral("Unable to write file");
        break;

    case FileManager::ErrorCode::ErrorBadJson:
        return error.message;

    case FileManager::ErrorCode::ErrorDeserializingFailed:
        baseMessage = error.message;
        break;

    case FileManager::ErrorCode::ErrorPermissionDenied:
        baseMessage = QStringLiteral("Permission denied");
        break;
    }

    if (error.message.isEmpty())
    {
        return baseMessage;
    }

    return baseMessage + QLatin1StringView(" (") + error.message + QLatin1StringView(")");
}
