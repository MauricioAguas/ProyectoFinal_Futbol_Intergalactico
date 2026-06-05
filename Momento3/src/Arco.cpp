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
    // Color segun equipo
    QColor color = (equipo_ == PLANET_EXPRESS)
                   ? QColor(255, 80, 0)    // naranja Planet Express
                   : QColor(80, 200, 255); // cian Omicron XI
    painter->setBrush(QBrush(color, Qt::Dense4Pattern));
    painter->setPen(QPen(color.lighter(150), 3));
    painter->drawRect(QRectF(0, 0, ANCHO, ALTO));
}

bool Arco::detectarGol(float balX, float balY) const {
    return (balX >= x_ && balX <= x_ + ANCHO &&
            balY >= y_ && balY <= y_ + ALTO);
}
