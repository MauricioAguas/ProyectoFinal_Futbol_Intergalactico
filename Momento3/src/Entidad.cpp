#include "hds/Entidad.h"

Entidad::Entidad(QGraphicsItem *parent)
    : QObject(nullptr), QGraphicsItem(parent),
      x_(0.0f), y_(0.0f)
{
    // No llamar setPos() aqui: el item aun no esta en ninguna escena
}

Entidad::~Entidad() {}

void Entidad::setPosicion(float x, float y) {
    x_ = x;
    y_ = y;
    setPos(x_, y_);  // seguro: se llama despues de addItem()
}
