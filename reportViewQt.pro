#-------------------------------------------------
#
# Project created by QtCreator 2016-12-01T13:13:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = reportViewQt
TEMPLATE = app


SOURCES += main.cpp\
        reportviewqt.cpp \
    qcustomplot.cpp \
    visualparamunit.cpp \
    chooseparamsfromfile.cpp \
    createpdf.cpp \
    graphsettings.cpp

HEADERS  += reportviewqt.h \
    qcustomplot.h \
    visualparamunit.h \
    qcustomplotpro.h \
    chooseparamsfromfile.h \
    createpdf.h \
    graphsettings.h

FORMS    += reportviewqt.ui \
    chooseparamsfromfile.ui \
    createpdf.ui \
    graphsettings.ui

RESOURCES += \
    icons.qrc
