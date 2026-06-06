#include "hds/Nivel.h"
#include "hds/Jugador.h"
#include "hds/JugadorIA.h"
#include "hds/Balon.h"
#include "hds/Arco.h"
#include "hds/Personaje.h"

Nivel::Nivel(ModoJuego modo, QObject *parent)
    : QGraphicsScene(parent),
      jugador1_(nullptr), jugador2_(nullptr),
      balon_(nullptr), arcoIzq_(nullptr), arcoDer_(nullptr),
      modo_(modo),
      tiempoRestante_(90),
      activo_(false),
      anchoEscena_(800), altoEscena_(500)
{
    goles_[0] = goles_[1] = 0;

    timerFrame_   = new QTimer(this);
    timerSegundo_ = new QTimer(this);
    connect(timerFrame_,   &QTimer::timeout, this, &Nivel::tickJuego);
    connect(timerSegundo_, &QTimer::timeout, this, &Nivel::tickTimer);
}

Nivel::~Nivel() {}

void Nivel::tickJuego() {
    if (!activo_) return;

    if (jugador1_) jugador1_->actualizar();
    if (jugador2_) jugador2_->actualizar();
    if (balon_)    balon_->actualizar();

    if (modo_ == VS_MAQUINA) {
        JugadorIA *ia = dynamic_cast<JugadorIA*>(jugador2_);
        if (ia && balon_ && jugador1_) {
            ia->percibir(balon_->getX(), balon_->getY(),
                         jugador1_->getX(), jugador1_->getY());
        }
    }

    verificarContactos();
    verificarGol();
}

void Nivel::tickTimer() {
    if (tiempoRestante_ > 0) {
        tiempoRestante_--;
    } else {
        activo_ = false;
        timerFrame_->stop();
        timerSegundo_->stop();
        emit tiempoAgotado();
        emit nivelTerminado();
    }
}

void Nivel::verificarGol() {
    if (!balon_) return;
    float bx = balon_->getX();
    float by = balon_->getY();
    float vx = balon_->getVx();
    float vy = balon_->getVy();

    if (arcoIzq_ && arcoIzq_->detectarGol(bx, by, vx, vy)) {
        goles_[1]++;
        emit golAnotado(1);
        balon_->reiniciar();
    } else if (arcoDer_ && arcoDer_->detectarGol(bx, by, vx, vy)) {
        goles_[0]++;
        emit golAnotado(0);
        balon_->reiniciar();
    }
}

void Nivel::verificarContactos() {
    if (!balon_) return;

    if (jugador1_ && balon_->collidesWithItem(jugador1_))
        jugador1_->contacto(balon_);

    if (jugador2_) {
        Jugador   *j2h = dynamic_cast<Jugador*>(jugador2_);
        JugadorIA *j2a = dynamic_cast<JugadorIA*>(jugador2_);
        if (j2h && balon_->collidesWithItem(j2h)) j2h->contacto(balon_);
        if (j2a && balon_->collidesWithItem(j2a)) j2a->contacto(balon_);
    }
}
