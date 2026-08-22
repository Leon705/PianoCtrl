#include "controller.h"

#include <QGuiApplication>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

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

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("controller"), &controller);
    engine.load(QUrl(QStringLiteral("qrc:/src/ui/mainui.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
