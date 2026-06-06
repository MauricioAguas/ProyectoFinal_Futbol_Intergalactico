#include "hds/Balon.h"
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QtMath>

Balon::Balon(QGraphicsItem *parent)
    : Entidad(parent),
      vx_(0.0f), vy_(0.0f), tiempo_(0.0f),
      enMovimiento_(false), modoParabolico_(true),
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
    if (pix.isNull())
        pix.load(":/assets/balon.png");

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
    if (!enMovimiento_) return;

    if (modoParabolico_) {
        tiempo_ += 1.0f;
        x_ += vx_;
        y_ += vy_ + 0.5f * GRAVEDAD * tiempo_;

        // Paredes laterales
        if (x_ <= RADIO) {
            x_  = RADIO;
            vx_ = qAbs(vx_);
        } else if (x_ >= anchoCampo_ - RADIO) {
            x_  = anchoCampo_ - RADIO;
            vx_ = -qAbs(vx_);
        }
        // Techo
        if (y_ <= RADIO) {
            y_   = RADIO;
            vy_  = qAbs(vy_);
            tiempo_ = 0.0f;
        }
        // Suelo: rebota siempre, nunca se detiene
        if (y_ >= altoCampo_ - RADIO) {
            y_   = altoCampo_ - RADIO;
            vy_  = -qAbs(vy_) * 0.6f;
            vx_ *= 0.9f;
            tiempo_ = 0.0f;
            if (qAbs(vy_) < 2.0f) vy_ = -3.0f;  // impulso minimo para que siga vivo
        }
    } else {
        // Nivel 2 — rebote puro en todas las paredes
        x_ += vx_;
        y_ += vy_;
        vx_ *= ROZAMIENTO;
        vy_ *= ROZAMIENTO;
        if (x_ <= RADIO)               { x_ = RADIO;               vx_ =  qAbs(vx_); }
        if (x_ >= anchoCampo_ - RADIO) { x_ = anchoCampo_ - RADIO; vx_ = -qAbs(vx_); }
        if (y_ <= RADIO)               { y_ = RADIO;                vy_ =  qAbs(vy_); }
        if (y_ >= altoCampo_ - RADIO)  { y_ = altoCampo_ - RADIO;  vy_ = -qAbs(vy_); }
        // En nivel 2 tampoco se detiene
        if (qAbs(vx_) < 1.5f) vx_ = (vx_ >= 0) ? 1.5f : -1.5f;
        if (qAbs(vy_) < 1.5f) vy_ = (vy_ >= 0) ? 1.5f : -1.5f;
    }

    setPos(x_, y_);
    update();
}

void Balon::aplicarRebote(bool horizontal) {
    if (horizontal) { vx_ = -vx_; x_ = qBound((float)RADIO, x_, (float)(anchoCampo_ - RADIO)); }
    else            { vy_ = -vy_; y_ = qBound((float)RADIO, y_, (float)(altoCampo_  - RADIO)); }
}

void Balon::reiniciar() {
    x_ = anchoCampo_ / 2.0f;
    y_ = altoCampo_  / 2.0f;
    vx_ = 4.0f; vy_ = -6.0f;   // siempre arranca con movimiento
    tiempo_ = 0.0f;
    enMovimiento_ = true;
    setPos(x_, y_);
    update();
}
