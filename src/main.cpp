#include "controller.h"
#include "src/data/filepaths.h"

#include <QGuiApplication>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

#include <src/data/databasemanager.h>
#include <src/data/samplelibraryrepository.h>
#include <src/data/soundlibraryrepository.h>

const QCommandLineOption sampleLibraryOption(
    QStringList() << QStringLiteral("sample-library") << QStringLiteral("slib"),
    QStringLiteral("Path to .SFZ sample library."),
    QStringLiteral("path"));

const QCommandLineOption midiPortOption(
    QStringList() << QStringLiteral("port") << QStringLiteral("p"),
    QStringLiteral("Specifies the midi-port used to control the synth."),
    QStringLiteral("port-index"));

void initializeCommandLineParser(QCommandLineParser &parser)
{
    parser.setApplicationDescription(QStringLiteral("PianoCtrl"));
    parser.addHelpOption();
    parser.addOption(sampleLibraryOption);
    parser.addOption(midiPortOption);
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCommandLineParser parser;
    initializeCommandLineParser(parser);
    parser.process(app);

    Controller controller;
    if (auto res = controller.initialize(); !res)
    {
        std::cerr << Controller::errorToQString(res.error()).toStdString() << std::endl;
        return 1;
    }

    if (parser.isSet(sampleLibraryOption))
    {
        const QString slibPath = parser.value(sampleLibraryOption);
        if (auto res = controller.loadSampleLibrary(slibPath); !res)
        {
            std::cerr << Controller::errorToQString(res.error()).toStdString() << std::endl;
        }
    }

    if (parser.isSet(midiPortOption))
    {
        bool ok = false;
        uint32_t port = parser.value(midiPortOption).toUInt(&ok);
        if (ok)
        {
            if (auto res = controller.switchMidiPort(port); !res)
            {
                std::cerr << Controller::errorToQString(res.error()).toStdString() << std::endl;
            }
        }
        else
        {
            std::cerr << "Invalid midi-port argument: "
                      << parser.value(midiPortOption).toStdString() << std::endl;
        }
    }


    DatabaseManager dbmgr(FilePaths::soundlibDatabaseFilePath());
    if (auto res = dbmgr.initDatabase(QStringLiteral(":/sql/soundlib_db.sql")); !res)
    {
        std::cerr << DatabaseManager::errorToQString(res.error()).toStdString() << std::endl;
    }

    SampleLibraryRepository sampleRepo(dbmgr);
    SoundLibraryRepository soundRepo(dbmgr);

    SampleLibrary kamoeLibrary{
        .displayName = QStringLiteral("kamoe301"),
        .path = QStringLiteral("/tmp/somewhere/kamoe301.sfz")
    };

    sampleRepo.add(kamoeLibrary);

    SoundLibrary testSoundLibrary{
        .id = 0,
        .displayName = QStringLiteral("TEST SOUND LIBRARY"),
        .description = QStringLiteral("TEST DATA"),
        .sampleLibraries = { kamoeLibrary }
    };

    auto result = soundRepo.add(testSoundLibrary);

    auto res = soundRepo.getAll();
    if (res)
    {
        std::vector<SoundLibrary> libs = res.value();
        for (const auto &lib : libs)
        {
            std::cout << lib.id << " " << lib.displayName.toStdString() << " " << lib.description.toStdString();
            for (const auto &sampleLib : lib.sampleLibraries)
            {
                std::cout << "[" << sampleLib.id << "] " << sampleLib.displayName.toStdString() << " " << sampleLib.path.toStdString() << std::endl;
            }
        }
    }
/*
    SampleLibraryRepository repo(dbmgr);

    SampleLibrary testLib{
            .displayName = QStringLiteral("kamoe301"),
            .path = QStringLiteral("/tmp/somewhere/kamoe301.sfz")
    };

    if (auto res = repo.add(testLib); !res)
    {
        std::cerr << DatabaseManager::errorToQString(res.error()).toStdString() << std::endl;
    }

    auto res = repo.getAll();
    if (!res)
    {
        std::cerr << DatabaseManager::errorToQString(res.error()).toStdString() << std::endl;
    } else {
        std::vector<SampleLibrary> libs = res.value();
        for (const auto &lib : libs)
        {
            std::cout << lib.id << " " << lib.displayName.toStdString() << " " << lib.path.toStdString() << std::endl;
        }
    }
*/
    // TODO remove this section

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("controller"), &controller);
    engine.load(QUrl(QStringLiteral("qrc:/src/ui/mainui.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
