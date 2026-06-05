#include "hds/Personaje.h"

Personaje::Personaje(const QString &nombre, int vida, float velocidad,
                     QGraphicsItem *parent)
    : Entidad(parent),
      nombre_(nombre), vida_(vida), velocidad_(velocidad), activo_(true)
{}

Personaje::~Personaje() {}
