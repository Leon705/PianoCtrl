#ifndef SOUNDLIBRARY_H
#define SOUNDLIBRARY_H

#include "samplelibrary.h"

#include <QString>

struct SoundLibrary {
    int id {0};
    QString displayName;
    QString description;

    std::vector<SampleLibrary> sampleLibraries;
};

#endif // SOUNDLIBRARY_H
