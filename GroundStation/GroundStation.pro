QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../Common
INCLUDEPATH += ../Common

SOURCES += \
    core/localdatabase/localdao.cpp \
    core/localdatabase/localdatabase.cpp \
    core/localdatabase/localmodels/transferringtask.cpp \
    core/managers/taskexecutor.cpp \
    core/network/deviceconnector.cpp \
    main.cpp \
    ui/logindialog.cpp \
    ui/mainwindow.cpp \
    core/network/serverconnector.cpp

HEADERS += \
    core/localdatabase/localdao.h \
    core/localdatabase/localdatabase.h \
    core/localdatabase/localmodels/transferringtask.h \
    core/managers/taskexecutor.h \
    core/network/deviceconnector.h \
    ui/logindialog.h \
    ui/mainwindow.h \
    core/network/serverconnector.h

FORMS += \
    core/network/deviceconnector.ui \
    ui/logindialog.ui \
    ui/mainwindow.ui \
    core/network/serverconnector.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
