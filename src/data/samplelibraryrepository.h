#ifndef SAMPLELIBRARYREPOSITORY_H
#define SAMPLELIBRARYREPOSITORY_H

#include "databasemanager.h"
#include "samplelibrary.h"

class SampleLibraryRepository
{
public:
    explicit SampleLibraryRepository(DatabaseManager &databaseManager);

    std::expected<std::vector<SampleLibrary>, DatabaseManager::Error> getAll();
    std::expected<SampleLibrary, DatabaseManager::Error> getById(int id);
    std::expected<void, DatabaseManager::Error> add(SampleLibrary &library);
    std::expected<void, DatabaseManager::Error> remove(int id);

private:
    DatabaseManager &databaseManager_;
};

#endif // SAMPLELIBRARYREPOSITORY_H
