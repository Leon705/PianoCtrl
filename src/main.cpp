#include "controller.h"
#include "uicontroller.h"

#include <QGuiApplication>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

#include <src/data/databasemanager.h>
#include <src/data/samplelibraryrepository.h>
#include <src/data/soundlibraryrepository.h>

const QCommandLineOption midiPortOption(
    QStringList() << QStringLiteral("port") << QStringLiteral("p"),
    QStringLiteral("Specifies the midi-port used to control the synth."),
    QStringLiteral("port-index"));

void initializeCommandLineParser(QCommandLineParser &parser)
{
    parser.setApplicationDescription(QStringLiteral("PianoCtrl"));
    parser.addHelpOption();
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

    if (auto res = controller.loadSoundLibrary(1); !res) std::cerr << Controller::errorToQString(res.error()).toStdString() << std::endl; // TODO remove me

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

    UiController uiController(&controller);
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("uiController"), &uiController);
    engine.load(QUrl(QStringLiteral("qrc:/src/ui/mainui.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
