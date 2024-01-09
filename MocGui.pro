QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutdialog.cpp \
    appsettingsmanager.cpp \
    dbmodule.cpp \
    libscanner.cpp \
    main.cpp \
    mainwindow.cpp \
    models/playlistmodel.cpp \
    settingsdialog.cpp \
    tagsreader.cpp \
    mocp/client.c \
    mocp/common.c \
    mocp/files.c \
    mocp/lists.c \
    mocp/protocol.c \
    mocp/options.c \
    mocp/playlist.c \
    playmanager.cpp \
    utils.cpp


HEADERS += \
    aboutdialog.h \
    appsettingsmanager.h \
    consts.h \
    dbmodule.h \
    libscanner.h \
    mainwindow.h \
    models/playlistitem.h \
    models/playlistmodel.h \
    settingsdialog.h \
    tagsreader.h \
    mocp/client.h \
    mocp/protocol.h \
    mocp/common.h \
    mocp/compat.h \
    mocp/options.h \
    mocp/lists.h \
    mocp/playlist.h \
    playmanager.h

FORMS += \
    aboutdialog.ui \
    mainwindow.ui \
    settingsdialog.ui

LIBS += /usr/lib/x86_64-linux-gnu/libmediainfo.so

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    appresources.qrc

DISTFILES += \
    README.md
