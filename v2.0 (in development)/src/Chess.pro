QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    board/board.cpp \
    board/coords.cpp \
    board/direction.cpp \
    board/move.cpp \
    board/possible_moves_vector.cpp \
    engine/engine.cpp \
    engine/perft.cpp \
    engine/piece_value.cpp \
    engine/zobrist_hash.cpp \
    main/main.cpp \
    window/window.cpp

HEADERS += \
    board/board.h \
    board/coords.h \
    board/direction.h \
    board/move.h \
    board/possible_moves_vector.h \
    engine/engine.h \
    engine/perft.h \
    engine/piece_value.h \
    engine/zobrist_hash.h \
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
