#include "hds/Entidad.h"

Entidad::Entidad(QGraphicsItem *parent)
    : QObject(nullptr), QGraphicsItem(parent),
      x_(0.0f), y_(0.0f)
{}

Entidad::~Entidad() {}

void Entidad::setPosicion(float x, float y) {
    x_ = x;
    y_ = y;
    setPos(x_, y_);
}
