#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "src/data/filemanager.h"
#include <QSqlDatabase>
#include <QString>
#include <expected>

#include <src/coreerror.h>

class DatabaseManager
{
public:
    enum class ErrorCode: uint8_t {
        ErrorOpenDatabase,
        ErrorQueryFailed,
    };

    using DatabaseError = ::Error<ErrorCode>;
    using Error = std::variant<DatabaseError, FileManager::Error>;

    explicit DatabaseManager(const QString &dbName);
    ~DatabaseManager();

    QSqlDatabase database() const;

    std::expected<void, DatabaseManager::Error> initDatabase(const QString &schemeFilePath);

    static QString errorToQString(const DatabaseManager::Error &error) noexcept;
private:
    QString databasePath_;
    QSqlDatabase database_;

    std::expected<void, DatabaseManager::Error> createTables(const QString &schemeFilePath);
};

#endif // DATABASEMANAGER_H
