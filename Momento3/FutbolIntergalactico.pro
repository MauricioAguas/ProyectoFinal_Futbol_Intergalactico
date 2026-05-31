QT += core gui widgets

CONFIG += c++17

TARGET   = FutbolIntergalactico
TEMPLATE = app

SOURCES += \
    src/Entidad.cpp \
    src/Personaje.cpp \
    src/Balon.cpp \
    src/Arco.cpp

HEADERS += \
    hds/Entidad.h \
    hds/Personaje.h \
    hds/Balon.h \
    hds/Arco.h
