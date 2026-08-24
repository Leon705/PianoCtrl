#include "soundlibraryrepository.h"

SoundLibraryRepository::SoundLibraryRepository(DatabaseManager &databaseManager)
    : databaseManager_(databaseManager)
{ }

// TODO implementation

std::expected<std::vector<SoundLibrary>, DatabaseManager::Error> SoundLibraryRepository::getAll()
{

}
