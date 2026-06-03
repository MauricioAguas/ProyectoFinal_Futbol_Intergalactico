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

// ---------------------------------------------------------------------------
// tickJuego — llamado cada 16 ms (~60 fps)
// ---------------------------------------------------------------------------
void Nivel::tickJuego() {
    if (!activo_) return;

    // Actualizar entidades
    if (jugador1_) jugador1_->actualizar();
    if (jugador2_) jugador2_->actualizar();
    if (balon_)    balon_->actualizar();

    // Si es modo maquina, pasar info del mundo a la IA
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

// ---------------------------------------------------------------------------
// tickTimer — descuenta un segundo cada 1000 ms
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// verificarGol — comprueba si el balon entro en un arco
// ---------------------------------------------------------------------------
void Nivel::verificarGol() {
    if (!balon_) return;
    float bx = balon_->getX();
    float by = balon_->getY();

    if (arcoIzq_ && arcoIzq_->detectarGol(bx, by)) {
        goles_[1]++;          // gol en arco izquierdo: punto para J2
        emit golAnotado(1);
        balon_->reiniciar();
    } else if (arcoDer_ && arcoDer_->detectarGol(bx, by)) {
        goles_[0]++;          // gol en arco derecho: punto para J1
        emit golAnotado(0);
        balon_->reiniciar();
    }
}

// ---------------------------------------------------------------------------
// verificarContactos — si el balon colisiona con un jugador llama contacto()
// ---------------------------------------------------------------------------
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
