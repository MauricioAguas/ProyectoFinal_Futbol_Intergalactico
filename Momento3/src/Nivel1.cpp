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
    : Nivel(modo, parent), marcador_(nullptr)
{}

Nivel1::~Nivel1() {}

void Nivel1::inicializar() {
    anchoEscena_ = 800;
    altoEscena_  = 450;
    setSceneRect(0, 0, anchoEscena_, altoEscena_);

    // Fondo
    setBackgroundBrush(Qt::NoBrush);
    QPixmap fondo(":/assets/fondo_nivel1.png");
    QGraphicsPixmapItem *bg = addPixmap(
        fondo.scaled(anchoEscena_, altoEscena_,
                     Qt::IgnoreAspectRatio,
                     Qt::SmoothTransformation));
    bg->setZValue(-1);

    // Suelo (Y donde pisan los jugadores)
    const float SUELO_Y = 370.0f;
    addRect(0, SUELO_Y, 800, 15, QPen(Qt::NoPen), QBrush(QColor(40, 120, 40, 120)));

    // Jugador 1 — Fry
    jugador1_ = new Jugador("Fry", 4.0f,
                            Qt::Key_A, Qt::Key_D, Qt::Key_W,
                            QColor(100, 200, 255));
    jugador1_->setSuelo(SUELO_Y);      // suelo dinamico
    addItem(jugador1_);
    jugador1_->setPosicion(150, SUELO_Y);

    // Jugador 2
    if (modo_ == VS_HUMANO) {
        Jugador *j2 = new Jugador("Bender", 4.0f,
                                  Qt::Key_Left, Qt::Key_Right, Qt::Key_Up,
                                  QColor(180, 180, 180));
        j2->setSuelo(SUELO_Y);
        jugador2_ = j2;
    } else {
        JugadorIA *ia = new JugadorIA("BenderIA", 3.5f, 680.0f);
        jugador2_ = ia;
    }
    addItem(jugador2_);
    jugador2_->setPosicion(600, SUELO_Y);

    // Balon
    balon_ = new Balon();
    balon_->setModoParabolico(true);
    balon_->setBounds(anchoEscena_, 380);
    addItem(balon_);
    balon_->setPosicion(390, SUELO_Y - 20);
    balon_->lanzar(3.0f, -8.0f);

    // Arcos laterales
    arcoIzq_ = new Arco(Arco::PLANET_EXPRESS);
    addItem(arcoIzq_);
    arcoIzq_->setPosicion(5, 270);

    arcoDer_ = new Arco(Arco::OMICRON_XI);
    addItem(arcoDer_);
    arcoDer_->setPosicion(775, 270);

    // Marcador
    marcador_ = addText("0  -  0", QFont("Arial", 20, QFont::Bold));
    marcador_->setDefaultTextColor(Qt::white);
    marcador_->setPos(340, 10);
    marcador_->setZValue(10);

    // Conectar senal de gol para actualizar marcador
    connect(this, &Nivel::golAnotado, this, &Nivel1::actualizarMarcador);

    activo_ = true;
    timerFrame_->start(16);
    timerSegundo_->start(1000);
}

void Nivel1::actualizarMarcador(int) {
    if (marcador_)
        marcador_->setPlainText(
            QString("%1  -  %2").arg(goles_[0]).arg(goles_[1]));
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
