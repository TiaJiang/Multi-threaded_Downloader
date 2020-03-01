QT       += core gui
QT       += network
QT += webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ThreadDownload
TEMPLATE = app


SOURCES += main.cpp\
    downloadthread.cpp \
    downloadcontrol.cpp \
    widget.cpp \
    downloadwidgets.cpp \
    mainwindow.cpp \
    qftp.cpp \
    qurlinfo.cpp \
    serverwin.cpp

HEADERS  += \
    downloadthread.h \
    downloadcontrol.h \
    widget.h \
    qftp.h \
    qurlinfo.h \
    downloadwidgets.h \
    mainwindow.h \
    serverwin.h

FORMS += \
    mainwindow.ui \
    serverwin.ui

RESOURCES += \
    res.qrc
