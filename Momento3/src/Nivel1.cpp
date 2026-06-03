#include "hds/Nivel1.h"
#include "hds/Jugador.h"
#include "hds/JugadorIA.h"
#include "hds/Balon.h"
#include "hds/Arco.h"
#include <QKeyEvent>
#include <QGraphicsRectItem>

Nivel1::Nivel1(ModoJuego modo, QObject *parent)
    : Nivel(modo, parent)
{}

Nivel1::~Nivel1() {}

void Nivel1::inicializar() {
    anchoEscena_ = 800;
    altoEscena_  = 500;
    setSceneRect(0, 0, anchoEscena_, altoEscena_);

    // Fondo
    setBackgroundBrush(QBrush(QColor(10, 10, 40)));

    // Suelo visual
    QGraphicsRectItem *suelo = addRect(0, 440, 800, 20,
                                       QPen(Qt::NoPen),
                                       QBrush(QColor(40, 120, 40)));
    Q_UNUSED(suelo);

    // Jugador 1 (humano) — teclas A/D/W
    jugador1_ = new Jugador("Fry", 4.0f,
                            Qt::Key_A, Qt::Key_D, Qt::Key_W);
    jugador1_->setPosicion(150, 380);
    addItem(jugador1_);

    // Jugador 2 — humano o IA
    if (modo_ == VS_HUMANO) {
        Jugador *j2 = new Jugador("Bender", 4.0f,
                                  Qt::Key_Left, Qt::Key_Right, Qt::Key_Up);
        j2->setPosicion(600, 380);
        jugador2_ = j2;
    } else {
        JugadorIA *ia = new JugadorIA("BenderIA", 3.5f, 680.0f);
        ia->setPosicion(600, 380);
        jugador2_ = ia;
    }
    addItem(jugador2_);

    // Balon — modo parabolico (nivel 1)
    balon_ = new Balon();
    balon_->setBounds(anchoEscena_, altoEscena_);
    balon_->setPosicion(390, 360);
    balon_->lanzar(3.0f, -8.0f);  // saque inicial
    addItem(balon_);

    // Arcos
    arcoIzq_ = new Arco(Arco::PLANET_EXPRESS);
    arcoIzq_->setPosicion(0, 340);
    addItem(arcoIzq_);

    arcoDer_ = new Arco(Arco::OMICRON_XI);
    arcoDer_->setPosicion(760, 340);
    addItem(arcoDer_);

    // Arrancar timers
    activo_ = true;
    timerFrame_->start(16);    // ~60 fps
    timerSegundo_->start(1000);
}

void Nivel1::keyPressEvent(QKeyEvent *event) {
    if (jugador1_) jugador1_->keyPress(static_cast<Qt::Key>(event->key()));
    if (modo_ == VS_HUMANO) {
        Jugador *j2 = dynamic_cast<Jugador*>(jugador2_);
        if (j2) j2->keyPress(static_cast<Qt::Key>(event->key()));
    }
}

void Nivel1::keyReleaseEvent(QKeyEvent *event) {
    if (jugador1_) jugador1_->keyRelease(static_cast<Qt::Key>(event->key()));
    if (modo_ == VS_HUMANO) {
        Jugador *j2 = dynamic_cast<Jugador*>(jugador2_);
        if (j2) j2->keyRelease(static_cast<Qt::Key>(event->key()));
    }
}
