#include "hds/Nivel1.h"
#include "hds/Jugador.h"
#include "hds/JugadorIA.h"
#include "hds/Balon.h"
#include "hds/Arco.h"
#include <QKeyEvent>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>

Nivel1::Nivel1(ModoJuego modo, QObject *parent)
    : Nivel(modo, parent)
{}

Nivel1::~Nivel1() {}

void Nivel1::inicializar() {
    anchoEscena_ = 800;
    altoEscena_  = 500;
    setSceneRect(0, 0, anchoEscena_, altoEscena_);

    // Fondo: intentar cargar imagen, si no existe usar color solido
    QPixmap fondoPx(":/assets/fondo_nivel1.png");
    if (!fondoPx.isNull()) {
        setBackgroundBrush(QBrush(fondoPx.scaled(anchoEscena_, altoEscena_)));
    } else {
        setBackgroundBrush(QBrush(QColor(10, 10, 40)));  // azul espacial
    }

    // Suelo visual
    QGraphicsRectItem *suelo = addRect(0, 440, 800, 20,
                                       QPen(Qt::NoPen),
                                       QBrush(QColor(40, 120, 40)));
    Q_UNUSED(suelo);

    // Jugador 1 — cabezon cyan (Fry)
    jugador1_ = new Jugador("Fry", 4.0f,
                            Qt::Key_A, Qt::Key_D, Qt::Key_W,
                            QColor(100, 200, 255));
    jugador1_->setPosicion(150, 380);
    addItem(jugador1_);

    // Jugador 2 — cabezon gris metalico (Bender) o IA
    if (modo_ == VS_HUMANO) {
        Jugador *j2 = new Jugador("Bender", 4.0f,
                                  Qt::Key_Left, Qt::Key_Right, Qt::Key_Up,
                                  QColor(180, 180, 180));
        j2->setPosicion(600, 380);
        jugador2_ = j2;
    } else {
        JugadorIA *ia = new JugadorIA("BenderIA", 3.5f, 680.0f);
        ia->setPosicion(600, 380);
        jugador2_ = ia;
    }
    addItem(jugador2_);

    // Balon
    balon_ = new Balon();
    balon_->setBounds(anchoEscena_, altoEscena_);
    balon_->setPosicion(390, 360);
    balon_->lanzar(3.0f, -8.0f);
    addItem(balon_);

    // Arcos
    arcoIzq_ = new Arco(Arco::PLANET_EXPRESS);
    arcoIzq_->setPosicion(0, 340);
    addItem(arcoIzq_);

    arcoDer_ = new Arco(Arco::OMICRON_XI);
    arcoDer_->setPosicion(760, 340);
    addItem(arcoDer_);

    activo_ = true;
    timerFrame_->start(16);
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
