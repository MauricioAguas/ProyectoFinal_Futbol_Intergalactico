QT += core gui widgets

CONFIG += c++17

TARGET   = FutbolIntergalactico
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/Entidad.cpp \
    src/Personaje.cpp \
    src/Balon.cpp \
    src/Arco.cpp \
    src/Jugador.cpp \
    src/JugadorIA.cpp \
    src/Nivel.cpp \
    src/Nivel1.cpp \
    src/Nivel2.cpp \
    src/Game.cpp

HEADERS += \
    hds/Entidad.h \
    hds/Personaje.h \
    hds/Balon.h \
    hds/Arco.h \
    hds/Jugador.h \
    hds/JugadorIA.h \
    hds/Nivel.h \
    hds/Nivel1.h \
    hds/Nivel2.h \
    hds/Game.h
