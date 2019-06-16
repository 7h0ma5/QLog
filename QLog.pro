#-------------------------------------------------
#
# Project created by QtCreator 2019-06-10T09:13:09
#
#-------------------------------------------------

QT       += core gui sql network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qlog
TEMPLATE = app
VERSION = 0.1.0

DEFINES += VERSION=\\\"$$VERSION\\\"

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        core/dxcc.cpp \
        core/fldigi.cpp \
        core/hamqth.cpp \
        core/main.cpp \
        core/migration.cpp \
        core/rig.cpp \
        core/utils.cpp \
        logformat/adiformat.cpp \
        logformat/adxformat.cpp \
        logformat/jsonformat.cpp \
        logformat/logformat.cpp \
        models/bandmodel.cpp \
        models/rigtypemodel.cpp \
        ui/dbdialog.cpp \
        ui/dxwidget.cpp \
        ui/exportdialog.cpp \
        ui/importdialog.cpp \
        ui/logbookwidget.cpp \
        ui/mainwindow.cpp \
        ui/mapwidget.cpp \
        ui/newcontactwidget.cpp \
        ui/settingsdialog.cpp

HEADERS += \
        core/dxcc.h \
        core/fldigi.h \
        core/hamqth.h \
        core/migration.h \
        core/rig.h \
        core/utils.h \
        logformat/adiformat.h \
        logformat/adxformat.h \
        logformat/jsonformat.h \
        logformat/logformat.h \
        models/bandmodel.h \
        models/rigtypemodel.h \
        ui/dbdialog.h \
        ui/dxwidget.h \
        ui/exportdialog.h \
        ui/importdialog.h \
        ui/logbookwidget.h \
        ui/mainwindow.h \
        ui/mapwidget.h \
        ui/newcontactwidget.h \
        ui/settingsdialog.h

FORMS += \
        ui/dbdialog.ui \
        ui/dxwidget.ui \
        ui/exportdialog.ui \
        ui/importdialog.ui \
        ui/logbookwidget.ui \
        ui/mainwindow.ui \
        ui/mapwidget.ui \
        ui/newcontactwidget.ui \
        ui/settingsdialog.ui

RESOURCES += \
    i18n/i18n.qrc \
    res/res.qrc

OTHER_FILES += \
    res/stylesheet.css \
    res/qlog.rc \
    res/qlog.desktop

TRANSLATIONS = i18n/qlog_de.ts

unix {
  isEmpty(PREFIX) {
    PREFIX = /usr/local
  }

  target.path = $$PREFIX/bin

  desktop.path = $$PREFIX/share/applications/
  desktop.files += res/$${TARGET}.desktop

  icon.path = $$PREFIX/share/icons/hicolor/256x256/apps
  icon.files += res/$${TARGET}.png

  INSTALLS += target desktop icon
}

win32: LIBS += -L$$PWD/../lib/ -lhamlib-2
else:unix: LIBS += -L$/usr/lib/ -lhamlib

win32: INCLUDEPATH += $$PWD/../include/

win32: DEPENDPATH += $$PWD/../include/

