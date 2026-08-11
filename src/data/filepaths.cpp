#include "filepaths.h"

#include <QCoreApplication>

QString FilePaths::appStateFilePath()
{
    return QCoreApplication::applicationDirPath() + QStringLiteral("/appstate.json");
}

QString FilePaths::defaultSampleLibraryDirPath()
{
    return QStringLiteral("NO DEFAULT SFZ DIR PATH");
    // return QCoreApplication::applicationDirPath() + QStringLiteral("/samples");
}


