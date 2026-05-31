#include "hds/JugadorIA.h"
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
      enSuelo_(true), vy_(0.0f),
      tendenciaRival_(0.0f), modificadorReaccion_(1.0f)
{
    timerDecision_ = new QTimer(this);
    connect(timerDecision_, &QTimer::timeout, this, &JugadorIA::cicloDecision);
    timerDecision_->start(300);  // decide cada 300 ms
}

JugadorIA::~JugadorIA() {}

// ---------------------------------------------------------------------------
// a) Percepcion — llamado por el Nivel cada tick
// ---------------------------------------------------------------------------
void JugadorIA::percibir(float balonX, float balonY,
                         float jugadorX, float jugadorY) {
    rivalX_ = jugadorX;
    rivalY_ = jugadorY;

    float dist = std::sqrt((balonX - x_) * (balonX - x_) +
                           (balonY - y_) * (balonY - y_));
    balonVisible_ = (dist <= RADIO_PERCEPCION);
    if (balonVisible_) {
        balonX_ = balonX;
        balonY_ = balonY;
    }
}

// ---------------------------------------------------------------------------
// b) Razonamiento — interpreta el estimulo y elige accion
// ---------------------------------------------------------------------------
void JugadorIA::razonar() {
    if (!balonVisible_) {
        // No ve el balon: volver a defender el arco
        debeAtacar_ = false;
        objetivoX_  = xArco_;
        return;
    }

    // Si el balon se acerca a su arco, atacar; si esta lejos, defender
    float distBalonArco = std::abs(balonX_ - xArco_);
    debeAtacar_ = (distBalonArco < 300.0f);

    if (debeAtacar_) {
        // Ajuste por tendencia aprendida: si el rival tiende a ir a la derecha,
        // anticiparse un poco hacia ese lado
        objetivoX_ = balonX_ + tendenciaRival_ * 20.0f;
    } else {
        objetivoX_ = xArco_;
    }
}

// ---------------------------------------------------------------------------
// d) Aprendizaje — actualiza tendencia segun historial del balon
// ---------------------------------------------------------------------------
void JugadorIA::aprender() {
    if (balonVisible_) {
        historialBalonX_.append(balonX_);
        if (historialBalonX_.size() > 20)
            historialBalonX_.removeFirst();
    }

    if (historialBalonX_.size() < 5) return;

    // Calcular tendencia: promedio de la mitad reciente vs la mitad anterior
    float sumaReciente = 0, sumaAnterior = 0;
    int   mitad = historialBalonX_.size() / 2;
    for (int i = 0; i < mitad; i++)       sumaAnterior += historialBalonX_[i];
    for (int i = mitad; i < historialBalonX_.size(); i++) sumaReciente += historialBalonX_[i];

    float promAnterior = sumaAnterior / mitad;
    float promReciente = sumaReciente / (historialBalonX_.size() - mitad);
    tendenciaRival_    = (promReciente - promAnterior > 0) ? 1.0f : -1.0f;

    // Aumentar modificador de reaccion con la experiencia (max 1.5)
    if (modificadorReaccion_ < 1.5f)
        modificadorReaccion_ += 0.02f;
}

// ---------------------------------------------------------------------------
// Slot del timer: ciclo completo percibir -> razonar -> aprender cada 300ms
// ---------------------------------------------------------------------------
void JugadorIA::cicloDecision() {
    razonar();
    aprender();
}

// ---------------------------------------------------------------------------
// c) Accion — actualizar movimiento hacia el objetivo cada tick
// ---------------------------------------------------------------------------
void JugadorIA::actualizar() {
    if (!activo_) return;

    float velReal = velocidad_ * modificadorReaccion_;

    // Mover horizontalmente hacia objetivoX_
    float dx = 0.0f;
    float diff = objetivoX_ - x_;
    if (std::abs(diff) > 5.0f)
        dx = (diff > 0) ? velReal : -velReal;

    // Saltar si el balon esta mas alto y la IA esta en el suelo
    if (balonVisible_ && balonY_ < y_ - 30.0f && enSuelo_) {
        vy_      = IMPULSO_SALTO;
        enSuelo_ = false;
    }

    // Fisica de caida
    if (!enSuelo_) vy_ += GRAVEDAD;
    float nuevoY = y_ + vy_;
    if (nuevoY >= suelo_) {   // suelo_ heredado como y_ inicial
        nuevoY   = suelo_;
        vy_      = 0.0f;
        enSuelo_ = true;
    }

    mover(dx, nuevoY - y_);
}

void JugadorIA::mover(float dx, float dy) {
    x_ += dx;
    y_ += dy;
    setPos(x_, y_);
}

// ---------------------------------------------------------------------------
// contacto — el Nivel llama esto cuando el balon toca a la IA
// ---------------------------------------------------------------------------
void JugadorIA::contacto(Balon *balon) {
    // Impulso hacia el arco rival (siempre al lado opuesto de xArco_)
    float dirX  = (xArco_ < 400) ? 1.0f : -1.0f;  // direccion de ataque
    float impX  = dirX * velocidad_ * modificadorReaccion_ * 1.5f;
    float impY  = enSuelo_ ? -6.0f : vy_ * 1.2f;
    balon->lanzar(impX, impY);
}

void JugadorIA::reiniciar() {
    enSuelo_           = true;
    vy_                = 0.0f;
    balonVisible_      = false;
    debeAtacar_        = false;
    objetivoX_         = xArco_;
    tendenciaRival_    = 0.0f;
    modificadorReaccion_ = 1.0f;
    historialBalonX_.clear();
    activo_ = true;
}
