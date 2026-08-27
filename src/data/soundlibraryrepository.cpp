#include "soundlibraryrepository.h"

#include <QSqlQuery>
#include <QSqlError>

SoundLibraryRepository::SoundLibraryRepository(DatabaseManager &databaseManager)
    : databaseManager_(databaseManager)
{ }

// TODO implementation

std::expected<std::vector<SoundLibrary>, DatabaseManager::Error> SoundLibraryRepository::getAll()
{
    static const auto sql = QStringLiteral(
        "SELECT sl.id, sl.display_name, sl.description, smp.id, smp.display_name, smp.path "
        "FROM sound_libraries sl "
        "LEFT JOIN sound_library_samples sls ON sl.id = sls.f_sound_library_id "
        "LEFT JOIN sample_libraries smp ON sls.f_sample_library_id = smp.id "
        "ORDER BY sl.id ASC"
    );

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

}

QString SoundLibraryRepository::getBaseSql()
{
    static const auto baseQuery = QStringLiteral(
        "SELECT sl.id, sl.display_name, sl.description, smp.id, smp.display_name, smp.path "
        "FROM sound_libraries sl "
        "LEFT JOIN sound_library_samples sls ON sl.id = sls.f_sound_library_id "
        "LEFT JOIN sample_libraries smp ON sls.f_sample_library_id = smp.id "
        );
    return baseQuery;
}

SoundLibrary SoundLibraryRepository::mapCurrentRowToSoundLibrary(const QSqlQuery &query)
{

}
