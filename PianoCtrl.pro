QT       += core gui
QMAKE_LFLAGS += -Wl,-rpath,'$$PWD/external/sfizz/lib'

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
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
    src/main.cpp \
    src/mainwindow.cpp \
    src/midicontroller.cpp \
    external/rt_midi/include/RtMidi.cpp

HEADERS += \
    src/audioengine.h \
    src/controller.h \
    src/mainwindow.h \
    src/midicontroller.h \
    external/rt_midi/include/RtMidi.h \
    external/sfizz/include/sfizz.h \
    external/sfizz/include/sfizz.hpp \
    external/sfizz/include/sfizz_message.h \

FORMS += \
    src/ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
