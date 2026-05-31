#include "hds/Personaje.h"

Personaje::Personaje(const QString &nombre, int vida, float velocidad,
                     QGraphicsItem *parent)
    : Entidad(parent),
      nombre_(nombre),
      vida_(vida),
      velocidad_(velocidad),
      activo_(true),
      modificadorVelocidad_(1.0f),
      habilidadActiva_(false)
{}

Personaje::~Personaje() {}

void Personaje::setVida(int v) {
    vida_ = (v < 0) ? 0 : v;
    if (vida_ == 0) activo_ = false;
}
