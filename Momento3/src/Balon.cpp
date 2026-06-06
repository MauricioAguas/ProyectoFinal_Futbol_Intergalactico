#include "hds/Balon.h"
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QtMath>

Balon::Balon(QGraphicsItem *parent)
    : Entidad(parent),
      vx_(0.0f), vy_(0.0f), tiempo_(0.0f),
      enMovimiento_(true), modoParabolico_(true),
      anchoCampo_(800), altoCampo_(450)
{}

Balon::~Balon() {}

QRectF Balon::boundingRect() const {
    return QRectF(-RADIO, -RADIO, RADIO * 2, RADIO * 2);
}

QPainterPath Balon::shape() const {
    QPainterPath path;
    path.addEllipse(QRectF(-RADIO_HIT, -RADIO_HIT, RADIO_HIT * 2, RADIO_HIT * 2));
    return path;
}

void Balon::paint(QPainter *painter,
                  const QStyleOptionGraphicsItem *,
                  QWidget *)
{
    static QPixmap pix;
    if (pix.isNull()) pix.load(":/assets/balon.png");

    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    if (!pix.isNull()) {
        painter->drawPixmap(
            QRectF(-RADIO, -RADIO, RADIO * 2, RADIO * 2),
            pix, QRectF(pix.rect()));
    } else {
        painter->setBrush(Qt::white);
        painter->setPen(QPen(Qt::black, 1.5));
        painter->drawEllipse(QRectF(-RADIO, -RADIO, RADIO * 2, RADIO * 2));
    }
}

void Balon::lanzar(float vx, float vy) {
    vx_ = vx; vy_ = vy;
    tiempo_ = 0.0f;
    enMovimiento_ = true;
}

void Balon::actualizar() {
    if (modoParabolico_) {
        tiempo_ += 1.0f;
        x_ += vx_;
        y_ += vy_ + 0.5f * GRAVEDAD * tiempo_;

        // Friccion horizontal en suelo
        if (y_ >= altoCampo_ - RADIO) {
            vx_ *= ROZAMIENTO_SUELO;
        }

        // Paredes laterales
        if (x_ <= RADIO) {
            x_  = RADIO;
            vx_ = qAbs(vx_) * ROZAMIENTO_PARED;
        } else if (x_ >= anchoCampo_ - RADIO) {
            x_  = anchoCampo_ - RADIO;
            vx_ = -qAbs(vx_) * ROZAMIENTO_PARED;
        }
        // Techo
        if (y_ <= RADIO) {
            y_   = RADIO;
            vy_  = qAbs(vy_) * ROZAMIENTO_PARED;
            tiempo_ = 0.0f;
        }
        // Suelo: rebota con perdida de energia
        if (y_ >= altoCampo_ - RADIO) {
            y_      = altoCampo_ - RADIO;
            vy_     = -qAbs(vy_) * ROZAMIENTO_REBOTE;
            tiempo_ = 0.0f;
            // Si casi no tiene energia vertical, queda rodando en el suelo
            if (qAbs(vy_) < 1.0f) vy_ = 0.0f;
        }

        // Si esta casi quieto en el suelo, el balon para (puede quedar estatico)
        if (y_ >= altoCampo_ - RADIO - 1.0f &&
            qAbs(vx_) < 0.3f && qAbs(vy_) < 0.3f) {
            vx_ = 0.0f; vy_ = 0.0f;
            enMovimiento_ = false;
        }

    } else {
        // Nivel 2: rebote en todas las paredes con perdida de energia
        x_ += vx_;
        y_ += vy_;
        vx_ *= ROZAMIENTO_AIRE;
        vy_ *= ROZAMIENTO_AIRE;
        if (x_ <= RADIO)               { x_ = RADIO;               vx_ =  qAbs(vx_) * ROZAMIENTO_PARED; }
        if (x_ >= anchoCampo_ - RADIO) { x_ = anchoCampo_ - RADIO; vx_ = -qAbs(vx_) * ROZAMIENTO_PARED; }
        if (y_ <= RADIO)               { y_ = RADIO;                vy_ =  qAbs(vy_) * ROZAMIENTO_PARED; }
        if (y_ >= altoCampo_ - RADIO)  { y_ = altoCampo_ - RADIO;  vy_ = -qAbs(vy_) * ROZAMIENTO_PARED; }
        // Velocidad minima para que no se detenga del todo en nivel 2
        float speed = qSqrt(vx_*vx_ + vy_*vy_);
        if (speed > 0.1f && speed < 1.2f) {
            vx_ = (vx_ / speed) * 1.2f;
            vy_ = (vy_ / speed) * 1.2f;
        }
    }

    setPos(x_, y_);
    update();
}

void Balon::aplicarRebote(bool horizontal) {
    if (horizontal) { vx_ = -vx_ * ROZAMIENTO_PARED; x_ = qBound((float)RADIO, x_, (float)(anchoCampo_ - RADIO)); }
    else            { vy_ = -vy_ * ROZAMIENTO_PARED; y_ = qBound((float)RADIO, y_, (float)(altoCampo_  - RADIO)); }
}

void Balon::reiniciar() {
    x_ = anchoCampo_ / 2.0f;
    y_ = altoCampo_  / 2.0f;
    vx_ = 4.0f; vy_ = -6.0f;
    tiempo_ = 0.0f;
    enMovimiento_ = true;
    setPos(x_, y_);
    update();
}
