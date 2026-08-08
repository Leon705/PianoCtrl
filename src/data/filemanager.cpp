#include "filemanager.h"

#include <QFile>
#include <QSaveFile>

std::expected<void, FileManager::Error> FileManager::saveToFile(const ISerializable &model, const QString &path)
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        return std::unexpected(FileManager::FileError{
            FileManager::ErrorCode::ErrorWriteFailed,
            path
        });
    }

    const QJsonDocument doc(model.toJson());
    file.write(doc.toJson(QJsonDocument::Indented));

    if (!file.commit())
    {
        return std::unexpected(FileManager::FileError{
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
        return std::unexpected(FileManager::FileError{
            FileManager::ErrorCode::ErrorFileNotFound,
            path
        });
    }

    if (!file.open(QIODevice::ReadOnly)) {
        return std::unexpected(FileManager::FileError{
            FileManager::ErrorCode::ErrorReadFailed,
            path
        });
    }

    return model.fromJson(file.readAll());
}

QString FileManager::errorToQString(const FileManager::Error &error) noexcept
{
    QString baseMessage;

    if (std::holds_alternative<FileManager::FileError>(error))
    {
        auto fileError = std::get<FileManager::FileError>(error);

        switch (fileError.code)
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

        case FileManager::ErrorCode::ErrorPermissionDenied:
            baseMessage = QStringLiteral("Permission denied");
            break;
        }

        if (fileError.message.isEmpty())
        {
            return baseMessage;
        }

        return baseMessage + QLatin1StringView(" (") + fileError.message + QLatin1StringView(")");
    }
    else if (std::holds_alternative<ISerializable::Error>(error))
    {
        return ISerializable::errorToQString(std::get<ISerializable::Error>(error));
    }

    return QString();
}
