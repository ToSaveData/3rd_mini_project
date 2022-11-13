QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatserverform.cpp \
    clienthandlerform.cpp \
    logthread.cpp \
    main.cpp \
    cs_app.cpp \
    orderhandlerform.cpp \
    producthandlerform.cpp

HEADERS += \
    chatserverform.h \
    clienthandlerform.h \
    cs_app.h \
    logthread.h \
    orderhandlerform.h \
    producthandlerform.h

FORMS += \
    chatserverform.ui \
    clienthandlerform.ui \
    cs_app.ui \
    orderhandlerform.ui \
    producthandlerform.ui

TRANSLATIONS += \
    CSApp_ko_KR.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon_resources.qrc
