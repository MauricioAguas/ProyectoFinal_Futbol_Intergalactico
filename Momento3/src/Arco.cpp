#include "hds/Arco.h"
#include <QPainter>

Arco::Arco(Equipo equipo, QGraphicsItem *parent)
    : Entidad(parent), equipo_(equipo)
{}

Arco::~Arco() {}

void Arco::actualizar() {}
void Arco::reiniciar()  {}

QRectF Arco::boundingRect() const {
    return QRectF(0, 0, ANCHO, ALTO);
}

void Arco::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *,
                 QWidget *)
{
    // Sin dibujo — el arco del fondo ya esta en la imagen
    Q_UNUSED(painter);
}

bool Arco::detectarGol(float balX, float balY, float vx, float vy) const {
    // El balon debe estar dentro del rectangulo del arco
    if (balX < x_ || balX > x_ + ANCHO) return false;
    if (balY < y_ || balY > y_ + ALTO)  return false;

    // El balon no puede entrar por arriba del travesano:
    // debe venir desde el campo (horizontalmente), no caer verticalmente
    // Si viene casi en vertical puro (vy muy grande vs vx) y ademas
    // esta cerca del borde superior del arco, se considera que pego en el travesano
    if (balY <= y_ + ALTO_TRAVESANO && qAbs(vy) > qAbs(vx) * 1.5f) return false;

    // Arco izquierdo (PLANET_EXPRESS): el balon debe entrar desde la derecha (vx < 0)
    if (equipo_ == PLANET_EXPRESS && vx >= 0.0f) return false;

    // Arco derecho (OMICRON_XI): el balon debe entrar desde la izquierda (vx > 0)
    if (equipo_ == OMICRON_XI && vx <= 0.0f) return false;

    return true;
}
