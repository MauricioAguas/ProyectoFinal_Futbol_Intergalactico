#include "hds/Arco.h"

Arco::Arco(Equipo equipo, QGraphicsItem *parent)
    : Entidad(parent),
      equipo_(equipo),
      ancho_(20.0f),
      alto_(100.0f)
{}

Arco::~Arco() {}

void Arco::actualizar() {
    // El arco es estatico; no requiere logica de movimiento
}

void Arco::reiniciar() {
    // Posicion fijada por el nivel al iniciar
}

bool Arco::detectarGol(float balX, float balY) const {
    return (balX >= x_ && balX <= x_ + ancho_ &&
            balY >= y_ && balY <= y_ + alto_);
}
