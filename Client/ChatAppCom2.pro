QT       += core gui
QT       += network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
    mainchatwin.cpp \
    qnchatmessage.cpp \
    addfriend.cpp \
    info.cpp \
    login.cpp \
    repword.cpp \
    signup.cpp \
    user.cpp \
    userlist.cpp \
    main2.cpp

HEADERS += \
    qnchatmessage.h \
    mainchatwin.h \
    addfriend.h \
    info.h \
    login.h \
    repword.h \
    signup.h \
    user.h \
    userlist.h

FORMS += \
    mainchatwin_old.ui \
    mainchatwin.ui \
    addfriend.ui \
    info.ui \
    login.ui \
    repword.ui \
    signup.ui \
    user.ui \
    userlist.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc

DISTFILES += \
    img/Customer Copy.png
