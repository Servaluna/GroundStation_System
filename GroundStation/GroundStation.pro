QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../Common
INCLUDEPATH += ../Common

SOURCES += \
    core/network/deviceconnector.cpp \
    # core/network/filetransfermanager.cpp \
    main.cpp \
    core/database/databasemanager.cpp \
    core/database/models/user.cpp \
    core/ui/logindialog.cpp \
    core/ui/mainwindow.cpp \
    core/network/serverconnector.cpp

HEADERS += \
    core/database/databasemanager.h \
    core/database/models/user.h \
    core/network/deviceconnector.h \
    # core/network/filetransfermanager.h \
    core/ui/logindialog.h \
    core/ui/mainwindow.h \
    core/network/serverconnector.h

FORMS += \
    core/network/deviceconnector.ui \
    core/ui/logindialog.ui \
    core/ui/mainwindow.ui \
    core/network/serverconnector.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
