QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    board.cpp \
    engine.cpp \
    main.cpp \
    move.cpp \
    piece_value.cpp \
    window.cpp

HEADERS += \
    board.h \
    engine.h \
    move.h \
    piece_value.h \
    window.h

FORMS += \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../res/resources.qrc \
    vars/variables.qrc

DISTFILES +=
