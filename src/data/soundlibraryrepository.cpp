#include "soundlibraryrepository.h"

#include <QSqlQuery>
#include <QSqlError>

SoundLibraryRepository::SoundLibraryRepository(DatabaseManager &databaseManager)
    : databaseManager_(databaseManager)
{ }

std::expected<std::vector<SoundLibrary>, DatabaseManager::Error> SoundLibraryRepository::getAll()
{
    static const QString sql = this->getBaseSql() + QStringLiteral("ORDER BY sl.id ASC");

    QSqlQuery query(this->databaseManager_.database());
    if (!query.exec(sql))
    {
        return std::unexpected(DatabaseManager::DatabaseError{
            DatabaseManager::ErrorCode::ErrorQueryFailed,
            query.lastError().text()
        });
    }

    std::vector<SoundLibrary> result;
    while (query.next()) {
        const int soundId = query.value(0).toInt();

        if (result.empty() || result.back().id != soundId) {
            result.push_back(SoundLibrary{
                .id = soundId,
                .displayName = query.value(1).toString(),
                .description = query.value(2).toString(),
                .sampleLibraries = {}
            });
        }

        if (!query.value(3).isNull()) {
            result.back().sampleLibraries.push_back(SampleLibrary{
                .id = query.value(3).toInt(),
                .displayName = query.value(4).toString(),
                .path = query.value(5).toString()
            });
        }
    }

    return result;
}

std::expected<SoundLibrary, DatabaseManager::Error> SoundLibraryRepository::getById(const int id)
{
    static const QString sql = this->getBaseSql() + QStringLiteral("WHERE sl.id = ?");

    QSqlQuery query(this->databaseManager_.database());
    query.prepare(sql);
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
            QStringLiteral("Unable to find sound_library with id %1").arg(id)
        });
    }

    SoundLibrary result = this->mapCurrentRowToSoundLibrary(query);
    do {
        if (auto sample = this->extractSampleLibrary(query))
        {
            result.sampleLibraries.push_back(std::move(*sample));
        }
    } while (query.next());

    return result;
}

std::expected<void, DatabaseManager::Error> SoundLibraryRepository::add(SoundLibrary &library)
{
    if (!this->databaseManager_.database().transaction())
    {
        return std::unexpected(DatabaseManager::DatabaseError{
            DatabaseManager::ErrorCode::ErrorQueryFailed,
            this->databaseManager_.database().lastError().text()
        });
    }

    QSqlQuery insertSoundLibraryQuery(this->databaseManager_.database());
    insertSoundLibraryQuery.prepare(QStringLiteral(
        "INSERT INTO sound_libraries (display_name, description) "
        "VALUES (?, ?)"));
    insertSoundLibraryQuery.addBindValue(library.displayName);
    insertSoundLibraryQuery.addBindValue(library.description);

    if (!insertSoundLibraryQuery.exec())
    {
        this->databaseManager_.database().rollback();
        return std::unexpected(DatabaseManager::DatabaseError{
            DatabaseManager::ErrorCode::ErrorQueryFailed,
            insertSoundLibraryQuery.lastError().text()
        });
    }

    library.id = insertSoundLibraryQuery.lastInsertId().toInt(); // auto incremented id

    if (!library.sampleLibraries.empty())
    {
        QSqlQuery insertRelationQuery(this->databaseManager_.database());
        insertRelationQuery.prepare(QStringLiteral(
            "INSERT INTO sound_library_samples (f_sound_library_id, f_sample_library_id) "
            "VALUES (?, ?)"
            ));

        for (const auto &sampleLib : library.sampleLibraries) {
            insertRelationQuery.addBindValue(library.id);
            insertRelationQuery.addBindValue(sampleLib.id);

            if (!insertRelationQuery.exec()) {
                this->databaseManager_.database().rollback();
                return std::unexpected(DatabaseManager::DatabaseError{
                    DatabaseManager::ErrorCode::ErrorQueryFailed,
                    insertRelationQuery.lastError().text()
                });
            }
        }
    }

    if (!this->databaseManager_.database().commit()) {
        this->databaseManager_.database().rollback();
        return std::unexpected(DatabaseManager::DatabaseError{
            DatabaseManager::ErrorCode::ErrorQueryFailed,
            this->databaseManager_.database().lastError().text()
        });
    }

    return {};
}

std::expected<void, DatabaseManager::Error> SoundLibraryRepository::remove(int id)
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

    if (query.numRowsAffected() == 0)
    {
        return std::unexpected(DatabaseManager::DatabaseError{
            DatabaseManager::ErrorCode::ErrorQueryFailed,
            QStringLiteral("No sample_library found with id %1 to delete").arg(id)
        });
    }

    return {};
}

QString SoundLibraryRepository::getBaseSql()
{
    static const QString baseQuery = QStringLiteral(
        "SELECT sl.id, sl.display_name, sl.description, smp.id, smp.display_name, smp.path "
        "FROM sound_libraries sl "
        "LEFT JOIN sound_library_samples sls ON sl.id = sls.f_sound_library_id "
        "LEFT JOIN sample_libraries smp ON sls.f_sample_library_id = smp.id "
        );
    return baseQuery;
}

SoundLibrary SoundLibraryRepository::mapCurrentRowToSoundLibrary(const QSqlQuery &query)
{
    return SoundLibrary {
        .id = query.value(0).toInt(),
        .displayName = query.value(1).toString(),
        .description = query.value(3).toString(),
        .sampleLibraries = {}
    };
}

std::optional<SampleLibrary> SoundLibraryRepository::extractSampleLibrary(const QSqlQuery &query)
{
    if (query.value(3).isNull())
    {
        return std::nullopt;
    }

    return SampleLibrary{
        .id = query.value(3).toInt(),
        .displayName = query.value(4).toString(),
        .path = query.value(5).toString()
    };
}
