QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    board/board.cpp \
    board/coords.cpp \
    board/direction.cpp \
    board/move.cpp \
    engine/engine.cpp \
    engine/piece_value.cpp \
    main/main.cpp \
    window/window.cpp

HEADERS += \
    board/board.h \
    board/coords.h \
    board/direction.h \
    board/move.h \
    engine/engine.h \
    engine/piece_value.h \
    window/window.h

FORMS += \
    ../ui/window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../res/resources.qrc \
    vars/variables.qrc

DISTFILES +=
