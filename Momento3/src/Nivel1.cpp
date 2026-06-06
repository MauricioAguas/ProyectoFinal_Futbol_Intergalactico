#include "hds/Nivel1.h"
#include "hds/Jugador.h"
#include "hds/JugadorIA.h"
#include "hds/Balon.h"
#include "hds/Arco.h"
#include <QKeyEvent>
#include <QPixmap>
#include <QBrush>
#include <QFont>
#include <QGraphicsTextItem>

Nivel1::Nivel1(ModoJuego modo, QObject *parent)
    : Nivel(modo, parent), marcador_(nullptr), temporizador_(nullptr)
{}

Nivel1::~Nivel1() {}

void Nivel1::inicializar() {
    anchoEscena_ = 800;
    altoEscena_  = 450;
    tiempoRestante_ = 60;
    setSceneRect(0, 0, anchoEscena_, altoEscena_);

    setBackgroundBrush(Qt::NoBrush);
    QPixmap fondo(":/assets/fondo_nivel1.png");
    QGraphicsPixmapItem *bg = addPixmap(
        fondo.scaled(anchoEscena_, altoEscena_,
                     Qt::IgnoreAspectRatio,
                     Qt::SmoothTransformation));
    bg->setZValue(-1);

    const float SUELO_Y = 370.0f;
    addRect(0, SUELO_Y, 800, 15, QPen(Qt::NoPen), QBrush(QColor(40, 120, 40, 80)));

    // Arcos
    arcoIzq_ = new Arco(Arco::PLANET_EXPRESS);
    addItem(arcoIzq_);
    arcoIzq_->setPosicion(35, 250);

    arcoDer_ = new Arco(Arco::OMICRON_XI);
    addItem(arcoDer_);
    arcoDer_->setPosicion(750, 250);

    // Limites para jugadores: borde derecho arcoIzq + margen, borde izq arcoDer - margen
    // arcoIzq en x=35, ANCHO=20 -> limite izq = 55 + mitad sprite(24) = 79
    // arcoDer en x=750          -> limite der = 750 - mitad sprite(24) = 726
    const float LIM_IZQ = 35.0f + 20.0f + 24.0f;  // 79
    const float LIM_DER = 750.0f - 24.0f;           // 726

    // Jugador 1 - Fry
    jugador1_ = new Jugador("Fry", 4.0f,
                            Qt::Key_A, Qt::Key_D, Qt::Key_W,
                            QColor(100, 200, 255),
                            ":/assets/fry.png",
                            ":/assets/zapato.png",
                            false);
    jugador1_->setSuelo(SUELO_Y);
    jugador1_->setTeclaPatada(Qt::Key_Space);
    jugador1_->setLimites(LIM_IZQ, LIM_DER);
    addItem(jugador1_);
    jugador1_->setPosicion(150, SUELO_Y);

    // Jugador 2
    if (modo_ == VS_HUMANO) {
        Jugador *j2 = new Jugador("Bender", 4.0f,
                                  Qt::Key_Left, Qt::Key_Right, Qt::Key_Up,
                                  QColor(180, 180, 180),
                                  ":/assets/bender.png",
                                  ":/assets/zapato.png",
                                  true);
        j2->setSuelo(SUELO_Y);
        j2->setTeclaPatada(Qt::Key_P);
        j2->setLimites(LIM_IZQ, LIM_DER);
        j2->setOtroJugador(jugador1_);
        jugador1_->setOtroJugador(j2);
        jugador2_ = j2;
    } else {
        JugadorIA *ia = new JugadorIA("BenderIA", 3.5f, 680.0f);
        ia->setLimites(LIM_IZQ, LIM_DER);
        ia->setOtroJugador(jugador1_);
        jugador1_->setOtroJugador(ia);
        jugador2_ = ia;
    }
    addItem(jugador2_);
    jugador2_->setPosicion(600, SUELO_Y);

    // Balon
    balon_ = new Balon();
    balon_->setModoParabolico(true);
    balon_->setBounds(anchoEscena_, 370);
    addItem(balon_);
    balon_->setPosicion(390, SUELO_Y - 30);
    balon_->lanzar(3.0f, -8.0f);

    // --- HUD ---
    QFont fontHUD("Arial", 18, QFont::Bold);

    marcador_ = addText("0  -  0", fontHUD);
    marcador_->setDefaultTextColor(Qt::white);
    marcador_->setPos(anchoEscena_/2 - marcador_->boundingRect().width()/2, 8);
    marcador_->setZValue(10);

    temporizador_ = addText("60s", QFont("Arial", 14, QFont::Bold));
    temporizador_->setDefaultTextColor(QColor(255, 220, 50));
    temporizador_->setPos(anchoEscena_ - 60, 8);
    temporizador_->setZValue(10);

    connect(this, &Nivel::golAnotado,   this, &Nivel1::actualizarHUD);
    connect(timerSegundo_, &QTimer::timeout, this, [this]{ actualizarHUD(); });

    activo_ = true;
    timerFrame_->start(16);
    timerSegundo_->start(1000);
}

void Nivel1::actualizarHUD(int) {
    if (marcador_)
        marcador_->setPlainText(
            QString("%1  -  %2").arg(goles_[0]).arg(goles_[1]));
    if (temporizador_)
        temporizador_->setPlainText(
            QString("%1s").arg(tiempoRestante_));
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
