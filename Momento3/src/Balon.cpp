#include "hds/Balon.h"
#include <QtMath>
#include <QDebug>

Balon::Balon(QGraphicsItem *parent)
    : Entidad(parent),
      vx_(0.0f), vy_(0.0f),
      tiempo_(0.0f),
      enMovimiento_(false),
      modoParabolico_(true),
      anchoCampo_(800),
      altoCampo_(500)
{}

Balon::~Balon() {}

void Balon::lanzar(float vx, float vy) {
    vx_          = vx;
    vy_          = vy;
    tiempo_      = 0.0f;
    enMovimiento_ = true;
    qDebug() << "[Balon] Lanzado con vx=" << vx << "vy=" << vy;
}

void Balon::actualizar() {
    if (!enMovimiento_) return;

    if (modoParabolico_) {
        // FISICA 1 - Trayectoria parabolica (Nivel 1 vista lateral)
        // x(t) = x0 + vx*t
        // y(t) = y0 + vy*t + 0.5*g*t^2
        tiempo_ += 1.0f;
        x_ += vx_;
        y_ += vy_ + 0.5f * GRAVEDAD * tiempo_;
    } else {
        // FISICA 2 - Rebote en paredes (Nivel 2 vista cenital)
        x_ += vx_;
        y_ += vy_;
        vx_ *= ROZAMIENTO;
        vy_ *= ROZAMIENTO;

        // Rebote horizontal
        if (x_ <= 0 || x_ >= anchoCampo_) {
            aplicarRebote(true);
        }
        // Rebote vertical
        if (y_ <= 0 || y_ >= altoCampo_) {
            aplicarRebote(false);
        }
    }

    setPos(x_, y_);

    // Frena si la velocidad es muy baja (nivel 2)
    if (!modoParabolico_ &&
        qAbs(vx_) < 0.5f && qAbs(vy_) < 0.5f) {
        enMovimiento_ = false;
    }
}

void Balon::aplicarRebote(bool horizontal) {
    if (horizontal) {
        vx_ = -vx_;
        x_  = qBound(0.0f, x_, (float)anchoCampo_);
    } else {
        vy_ = -vy_;
        y_  = qBound(0.0f, y_, (float)altoCampo_);
    }
    qDebug() << "[Balon] Rebote" << (horizontal ? "horizontal" : "vertical");
}

void Balon::reiniciar() {
    x_            = anchoCampo_ / 2.0f;
    y_            = altoCampo_  / 2.0f;
    vx_           = 0.0f;
    vy_           = 0.0f;
    tiempo_       = 0.0f;
    enMovimiento_ = false;
    setPos(x_, y_);
}
