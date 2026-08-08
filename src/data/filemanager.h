#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <expected>
#include "iserializable.h"
#include "src/coreerror.h"

class FileManager
{
public:
    enum class ErrorCode: uint8_t {
        ErrorFileNotFound,
        ErrorReadFailed,
        ErrorWriteFailed,
        ErrorBadJson,
        ErrorDeserializingFailed,
        ErrorPermissionDenied,
    };
    using Error = ::Error<FileManager::ErrorCode>;

    static std::expected<void, FileManager::Error> saveToFile(const ISerializable &model, const QString &path);
    static std::expected<void, FileManager::Error> loadFromFile(ISerializable &model, const QString &path);

    static QString errorToQString(const FileManager::Error& error) noexcept;
};

#endif // FILEMANAGER_H
