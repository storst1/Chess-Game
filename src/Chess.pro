QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    engine.cpp \
    piece_value.cpp \
    src/board.cpp \
    src/main.cpp \
    src/move.cpp \
    src/window.cpp

HEADERS += \
    engine.h \
    piece_value.h \
    src/board.h \
    src/move.h \
    src/window.h

FORMS += \
    ui/window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res/resources.qrc \
    vars/variables.qrc
