#include "samplelibraryrepository.h"

#include <QSqlQuery>
#include <QSqlError>

SampleLibraryRepository::SampleLibraryRepository(DatabaseManager &databaseManager)
    : databaseManager_(databaseManager)
{ }

std::expected<std::vector<SampleLibrary>, DatabaseManager::Error> SampleLibraryRepository::getAll()
{
    QSqlQuery query(this->databaseManager_.database());
    if (!query.exec(QStringLiteral("SELECT * FROM sample_libraries")))
    {
        return std::unexpected(DatabaseManager::DatabaseError{
            DatabaseManager::ErrorCode::ErrorQueryFailed,
            query.lastError().text()
        });
    }

    std::vector<SampleLibrary> result;
    while (query.next())
    {
        result.push_back(SampleLibrary{
            .id = query.value(QStringLiteral("id")).toInt(),
            .displayName = query.value(QStringLiteral("display_name")).toString(),
            .path = query.value(QStringLiteral("path")).toString()
        });
    }

    return result;
}

std::expected<SampleLibrary, DatabaseManager::Error> SampleLibraryRepository::getById(int id)
{
    QSqlQuery query(this->databaseManager_.database());
    query.prepare(QStringLiteral("SELECT id, display_name, path FROM sample_libraries WHERE id = ?"));
    query.addBindValue(id);

    if (!query.exec())
    {
        return std::unexpected(DatabaseManager::DatabaseError{
            DatabaseManager::ErrorCode::ErrorQueryFailed,
            query.lastError().text()
        });
    }

    if (!query.next())
    {
        return std::unexpected(DatabaseManager::DatabaseError{
            DatabaseManager::ErrorCode::ErrorQueryFailed,
            QStringLiteral("Unable to find sample_library with id %1").arg(id)
        });
    }

    return SampleLibrary{
        .id = query.value(QStringLiteral("id")).toInt(),
        .displayName = query.value(QStringLiteral("display_name")).toString(),
        .path = query.value(QStringLiteral("path")).toString()
    };
}

std::expected<void, DatabaseManager::Error> SampleLibraryRepository::add(SampleLibrary &library)
{
    QSqlQuery query(this->databaseManager_.database());
    query.prepare(QStringLiteral("INSERT INTO sample_libraries(display_name, path) VALUES (?,?)"));
    query.addBindValue(library.displayName);
    query.addBindValue(library.path);

    if (!query.exec())
    {
        return std::unexpected(DatabaseManager::DatabaseError{
            DatabaseManager::ErrorCode::ErrorQueryFailed,
            query.lastError().text()
        });
    }

    library.id = query.lastInsertId().toInt();  // because of auto increment

    return {};
}

std::expected<void, DatabaseManager::Error> SampleLibraryRepository::remove(int id)
{
    QSqlQuery query(this->databaseManager_.database());
    query.prepare(QStringLiteral("DELETE FROM sample_libraries WHERE id = ?"));
    query.addBindValue(id);

    if (!query.exec())
    {
        return std::unexpected(DatabaseManager::DatabaseError{
            DatabaseManager::ErrorCode::ErrorQueryFailed,
            query.lastError().text()
        });
    }

    return {};
}


