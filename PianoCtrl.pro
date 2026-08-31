QT       += core gui \
    qml \
    sql
QMAKE_LFLAGS += -Wl,-rpath,'$$PWD/external/sfizz/lib'

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++23
DEFINES += __LINUX_ALSA__

INCLUDEPATH += $$PWD/external/sfizz/include \
    $$PWD/external/rt_midi/include

LIBS += -lasound -L$$PWD/external/sfizz/lib -lsfizz -ljack

#QMAKE_LFLAGS += -Wl,-rpath,\'\$$ORIGIN/external/sfizz/lib\'

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/audioengine.cpp \
    src/controller.cpp \
    src/data/appstate.cpp \
    src/data/databasemanager.cpp \
    src/data/filemanager.cpp \
    src/data/filepaths.cpp \
    src/data/iserializable.cpp \
    src/data/samplelibraryrepository.cpp \
    src/data/soundlibraryrepository.cpp \
    src/main.cpp \
    external/rt_midi/include/RtMidi.cpp \
    src/midihandler.cpp \
    src/uicontroller.cpp

HEADERS += \
    src/coreerror.h \
    src/audioengine.h \
    src/controller.h \
    src/data/appstate.h \
    src/data/databasemanager.h \
    src/data/filemanager.h \
    src/data/filepaths.h \
    src/data/iserializable.h \
    external/rt_midi/include/RtMidi.h \
    external/sfizz/include/sfizz.h \
    external/sfizz/include/sfizz.hpp \
    external/sfizz/include/sfizz_message.h \
    src/data/samplelibrary.h \
    src/data/samplelibraryrepository.h \
    src/data/soundlibrary.h \
    src/data/soundlibraryrepository.h \
    src/midihandler.h \
    src/uicontroller.h

#FORMS += \
#    src/ui/mainwindow.ui

RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    src/ui/mainui.qml
