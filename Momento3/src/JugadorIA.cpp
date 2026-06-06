#include "hds/JugadorIA.h"
#include <QPainter>
#include <cmath>
#include <cstdlib>

JugadorIA::JugadorIA(const QString &nombre,
                     float velocidad,
                     float xArco,
                     QGraphicsItem *parent)
    : Personaje(nombre, 3, velocidad, parent),
      balonX_(0), balonY_(0), rivalX_(0), rivalY_(0),
      balonVisible_(false),
      objetivoX_(xArco), debeAtacar_(false),
      xArco_(xArco),
      enSuelo_(true), vy_(0.0f), suelo_(400.0f),
      tendenciaRival_(0.0f), modificadorReaccion_(1.0f)
{
    timerDecision_ = new QTimer(this);
    connect(timerDecision_, &QTimer::timeout, this, &JugadorIA::cicloDecision);
    timerDecision_->start(300);
}

JugadorIA::~JugadorIA() {}

QRectF JugadorIA::boundingRect() const {
    float totalAlto  = RADIO_CABEZA * 2 + ALTO_CUERPO + 16;
    float totalAncho = qMax(RADIO_CABEZA * 2, ANCHO_CUERPO) + 10;
    return QRectF(-totalAncho / 2, -RADIO_CABEZA * 2, totalAncho, totalAlto);
}

void JugadorIA::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem *,
                      QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);

    painter->setBrush(QColor(120, 120, 120));
    painter->setPen(Qt::NoPen);
    painter->drawRect(QRectF(-ANCHO_CUERPO / 2, 0, ANCHO_CUERPO, ALTO_CUERPO));

    QColor colorCabeza = balonVisible_ ? QColor(200, 200, 80) : QColor(180, 180, 180);
    painter->setBrush(colorCabeza);
    painter->setPen(QPen(Qt::white, 1.5));
    painter->drawEllipse(QRectF(-RADIO_CABEZA, -RADIO_CABEZA * 2,
                                RADIO_CABEZA * 2, RADIO_CABEZA * 2));

    painter->setBrush(QColor(255, 255, 100));
    painter->setPen(Qt::NoPen);
    painter->drawRect(QRectF(-11, -RADIO_CABEZA * 1.6f, 8, 5));
    painter->drawRect(QRectF(3,   -RADIO_CABEZA * 1.6f, 8, 5));

    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 7));
    painter->drawText(QRectF(-24, ALTO_CUERPO + 2, 48, 12),
                      Qt::AlignCenter, nombre_);
}

void JugadorIA::percibir(float balonX, float balonY,
                         float jugadorX, float jugadorY) {
    rivalX_ = jugadorX; rivalY_ = jugadorY;
    float dist = std::sqrt((balonX - x_) * (balonX - x_) +
                           (balonY - y_) * (balonY - y_));
    balonVisible_ = (dist <= RADIO_PERCEPCION);
    if (balonVisible_) { balonX_ = balonX; balonY_ = balonY; }
}

void JugadorIA::razonar() {
    if (!balonVisible_) { debeAtacar_ = false; objetivoX_ = xArco_; return; }
    float distBalonArco = std::abs(balonX_ - xArco_);
    debeAtacar_ = (distBalonArco < 300.0f);
    objetivoX_ = debeAtacar_ ? balonX_ + tendenciaRival_ * 20.0f : xArco_;
    // Respetar limites
    if (objetivoX_ < xMin_) objetivoX_ = xMin_;
    if (objetivoX_ > xMax_) objetivoX_ = xMax_;
}

void JugadorIA::aprender() {
    if (balonVisible_) {
        historialBalonX_.append(balonX_);
        if (historialBalonX_.size() > 20) historialBalonX_.removeFirst();
    }
    if (historialBalonX_.size() < 5) return;
    float sumaR = 0, sumaA = 0;
    int mitad = historialBalonX_.size() / 2;
    for (int i = 0;     i < mitad;                      i++) sumaA += historialBalonX_[i];
    for (int i = mitad; i < historialBalonX_.size();    i++) sumaR += historialBalonX_[i];
    tendenciaRival_ = ((sumaR / (historialBalonX_.size() - mitad))
                       - (sumaA / mitad)) > 0 ? 1.0f : -1.0f;
    if (modificadorReaccion_ < 1.5f) modificadorReaccion_ += 0.02f;
}

void JugadorIA::cicloDecision() { razonar(); aprender(); }

void JugadorIA::actualizar() {
    if (!activo_) return;
    float velReal = velocidad_ * modificadorReaccion_;
    float dx = 0.0f;
    float diff = objetivoX_ - x_;
    if (std::abs(diff) > 5.0f) dx = (diff > 0) ? velReal : -velReal;

    if (balonVisible_ && balonY_ < y_ - 30.0f && enSuelo_) {
        vy_ = IMPULSO_SALTO; enSuelo_ = false;
    }
    if (!enSuelo_) vy_ += GRAVEDAD;
    float nuevoY = y_ + vy_;
    if (nuevoY >= suelo_) { nuevoY = suelo_; vy_ = 0.0f; enSuelo_ = true; }

    mover(dx, nuevoY - y_);
    update();
}

void JugadorIA::mover(float dx, float dy) {
    x_ += dx;
    y_ += dy;

    // Limites de los arcos (solo en suelo)
    if (enSuelo_) {
        if (x_ < xMin_) x_ = xMin_;
        if (x_ > xMax_) x_ = xMax_;

        // Colision horizontal con el otro jugador (solo en suelo)
        if (otroJugador_) {
            float otroX = otroJugador_->getX();
            float mitad = ANCHO_CUERPO / 2.0f + 10.0f;
            if (x_ < otroX && x_ + mitad > otroX - mitad)
                x_ = otroX - mitad * 2.0f;
            else if (x_ > otroX && x_ - mitad < otroX + mitad)
                x_ = otroX + mitad * 2.0f;
        }
    }

    setPos(x_, y_);
}

void JugadorIA::contacto(Balon *balon) {
    float dirX = (xArco_ < 400) ? 1.0f : -1.0f;
    float impX = dirX * velocidad_ * modificadorReaccion_ * 1.5f;
    float impY = enSuelo_ ? -6.0f : vy_ * 1.2f;
    balon->lanzar(impX, impY);
}

void JugadorIA::reiniciar() {
    enSuelo_ = true; vy_ = 0.0f; balonVisible_ = false;
    debeAtacar_ = false; objetivoX_ = xArco_;
    tendenciaRival_ = 0.0f; modificadorReaccion_ = 1.0f;
    historialBalonX_.clear(); activo_ = true; update();
}
