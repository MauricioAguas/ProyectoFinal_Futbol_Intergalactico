#include "hds/Arco.h"
#include <QPainter>

Arco::Arco(Equipo equipo, float alto, QGraphicsItem *parent)
    : Entidad(parent), equipo_(equipo), alto_(alto)
{}

Arco::~Arco() {}

void Arco::actualizar() {}
void Arco::reiniciar()  {}

QRectF Arco::boundingRect() const {
    return QRectF(0, 0, ANCHO, alto_);
}

void Arco::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *,
                 QWidget *)
{
    // Sin dibujo — el arco del fondo ya esta en la imagen
    Q_UNUSED(painter);
}

bool Arco::detectarGol(float balX, float balY, float vx, float vy) const {
    if (balX < x_ || balX > x_ + ANCHO) return false;
    if (balY < y_ || balY > y_ + alto_)  return false;

    if (balY <= y_ + ALTO_TRAVESANO && qAbs(vy) > qAbs(vx) * 1.5f) return false;

    if (equipo_ == PLANET_EXPRESS && vx >= 0.0f) return false;
    if (equipo_ == OMICRON_XI     && vx <= 0.0f) return false;

    return true;
}
