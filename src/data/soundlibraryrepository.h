#ifndef SOUNDLIBRARYREPOSITORY_H
#define SOUNDLIBRARYREPOSITORY_H

#include "src/data/databasemanager.h"

#include "soundlibrary.h"

class SoundLibraryRepository
{
public:
    explicit SoundLibraryRepository(DatabaseManager &databaseManager);

    std::expected<std::vector<SoundLibrary>, DatabaseManager::Error> getAll();
    std::expected<SoundLibrary, DatabaseManager::Error> getById(int id);
    std::expected<void, DatabaseManager::Error> add(SoundLibrary &library);
    std::expected<void, DatabaseManager::Error> remove(int id);

private:
    DatabaseManager &databaseManager_;
};

#endif // SOUNDLIBRARYREPOSITORY_H
