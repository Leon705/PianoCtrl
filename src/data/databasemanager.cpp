#include "databasemanager.h"

#include "filemanager.h"

#include <QFile>
#include <QSqlQuery>
#include <QSqlError>

DatabaseManager::DatabaseManager(const QString &dbFilePath)
    : databasePath_(dbFilePath)
{ }

DatabaseManager::~DatabaseManager()
{
    if (this->database_.isOpen())
    {
        this->database_.close();
    }
}

QSqlDatabase DatabaseManager::database() const
{
    return this->database_;
}

std::expected<void, DatabaseManager::Error> DatabaseManager::initDatabase(const QString &schemeFilePath)
{
    this->database_ = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    this->database_.setDatabaseName(this->databasePath_);

    if (!this->database_.open())
    {
        return std::unexpected(DatabaseManager::DatabaseError{
            DatabaseManager::ErrorCode::ErrorOpenDatabase,
            this->database_.databaseName()
        });
    }

    return createTables(schemeFilePath);
}

std::expected<void, DatabaseManager::Error> DatabaseManager::createTables(const QString &schemeFilePath)
{
    auto sqlResult = FileManager::readTextFile(schemeFilePath);
    if (!sqlResult) {
        return std::unexpected(sqlResult.error());
    }

    QSqlQuery query(this->database_ );
    const QStringList statements = sqlResult.value().split(';', Qt::SkipEmptyParts);

    for (const QString &statement : statements) {
        const QString trimmed = statement.trimmed();
        if (trimmed.isEmpty()) continue;

        if (!query.exec(trimmed)) {
            return std::unexpected(DatabaseManager::DatabaseError{
                DatabaseManager::ErrorCode::ErrorQueryFailed,
                query.lastError().text()
            });
        }
    }

    return {};
}

QString DatabaseManager::errorToQString(const DatabaseManager::Error &error) noexcept
{
    QString baseMessage;

    if (std::holds_alternative<DatabaseManager::DatabaseError>(error))
    {
        auto databaseError = std::get<DatabaseManager::DatabaseError>(error);

        switch (databaseError.code)
        {
            case DatabaseManager::ErrorCode::ErrorOpenDatabase:
                baseMessage = QStringLiteral("Unable to open database");
                break;

            case DatabaseManager::ErrorCode::ErrorQueryFailed:
                baseMessage = QStringLiteral("Query failed");
                break;
        }

        if (databaseError.message.isEmpty())
        {
            return baseMessage;
        }

        return baseMessage + QLatin1StringView(" (") + databaseError.message + QLatin1StringView(")");
    }
    else if (std::holds_alternative<FileManager::Error>(error))
    {
        return FileManager::errorToQString(std::get<FileManager::Error>(error));
    }

    return QString();
}

