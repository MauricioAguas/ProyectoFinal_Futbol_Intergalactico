#include "hds/Balon.h"
#include <QPainter>
#include <QtMath>

Balon::Balon(QGraphicsItem *parent)
    : Entidad(parent),
      vx_(0.0f), vy_(0.0f), tiempo_(0.0f),
      enMovimiento_(false), modoParabolico_(true),
      anchoCampo_(800), altoCampo_(500)
{}

Balon::~Balon() {}

QRectF Balon::boundingRect() const {
    return QRectF(-RADIO, -RADIO, RADIO * 2, RADIO * 2);
}

void Balon::paint(QPainter *painter,
                  const QStyleOptionGraphicsItem *,
                  QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    // Balon blanco con pentagono negro (placeholder visual)
    painter->setBrush(QBrush(Qt::white));
    painter->setPen(QPen(Qt::black, 1.5));
    painter->drawEllipse(QRectF(-RADIO, -RADIO, RADIO * 2, RADIO * 2));
    // Cruz interior para simular costuras
    painter->setPen(QPen(Qt::black, 1));
    painter->drawLine(QPointF(-RADIO * 0.5f, 0), QPointF(RADIO * 0.5f, 0));
    painter->drawLine(QPointF(0, -RADIO * 0.5f), QPointF(0, RADIO * 0.5f));
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
    } else {
        x_ += vx_;
        y_ += vy_;
        vx_ *= ROZAMIENTO;
        vy_ *= ROZAMIENTO;
        if (x_ <= 0 || x_ >= anchoCampo_) aplicarRebote(true);
        if (y_ <= 0 || y_ >= altoCampo_)  aplicarRebote(false);
    }

    setPos(x_, y_);

    if (!modoParabolico_ && qAbs(vx_) < 0.5f && qAbs(vy_) < 0.5f)
        enMovimiento_ = false;
}

void Balon::aplicarRebote(bool horizontal) {
    if (horizontal) { vx_ = -vx_; x_ = qBound(0.0f, x_, (float)anchoCampo_); }
    else            { vy_ = -vy_; y_ = qBound(0.0f, y_, (float)altoCampo_); }
}

void Balon::reiniciar() {
    x_ = anchoCampo_ / 2.0f;
    y_ = altoCampo_  / 2.0f;
    vx_ = vy_ = tiempo_ = 0.0f;
    enMovimiento_ = false;
    setPos(x_, y_);
}
