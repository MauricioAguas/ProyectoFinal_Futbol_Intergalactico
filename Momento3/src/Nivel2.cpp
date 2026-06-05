#include "hds/Nivel2.h"
#include "hds/Jugador.h"
#include "hds/JugadorIA.h"
#include "hds/Balon.h"
#include "hds/Arco.h"
#include <QKeyEvent>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QPixmap>
#include <cmath>

Nivel2::Nivel2(ModoJuego modo, QObject *parent)
    : Nivel(modo, parent), tiempoOsc_(0.0f)
{}

Nivel2::~Nivel2() {}

void Nivel2::inicializar() {
    anchoEscena_ = 800;
    altoEscena_  = 600;
    setSceneRect(0, 0, anchoEscena_, altoEscena_);

    QPixmap fondoPx(":/assets/fondo_nivel2.png");
    if (!fondoPx.isNull())
        setBackgroundBrush(QBrush(fondoPx.scaled(anchoEscena_, altoEscena_)));
    else
        setBackgroundBrush(QBrush(QColor(5, 5, 30)));

    // Paredes de energia
    QPen penPared(QColor(120, 0, 200), 6);
    addLine(0,   0,   800, 0,   penPared);
    addLine(0,   600, 800, 600, penPared);
    addLine(0,   0,   0,   600, penPared);
    addLine(800, 0,   800, 600, penPared);

    // Jugador 1
    jugador1_ = new Jugador("Fry", 4.0f,
                            Qt::Key_A, Qt::Key_D, Qt::Key_W,
                            QColor(100, 200, 255));
    jugador1_->setPosicion(150, 280);
    addItem(jugador1_);

    // Jugador 2
    if (modo_ == VS_HUMANO) {
        Jugador *j2 = new Jugador("Bender", 4.0f,
                                  Qt::Key_Left, Qt::Key_Right, Qt::Key_Up,
                                  QColor(180, 180, 180));
        j2->setPosicion(600, 280);
        jugador2_ = j2;
    } else {
        JugadorIA *ia = new JugadorIA("BenderIA", 3.5f, 720.0f);
        ia->setPosicion(600, 280);
        jugador2_ = ia;
    }
    addItem(jugador2_);

    // Balon (modo rebote, sin parabola)
    balon_ = new Balon();
    balon_->setModoParabolico(false);
    balon_->setBounds(anchoEscena_, altoEscena_);
    balon_->setPosicion(390, 290);
    balon_->lanzar(5.0f, 4.0f);
    addItem(balon_);

    // Arcos cenital (arriba y abajo)
    arcoIzq_ = new Arco(Arco::PLANET_EXPRESS);
    arcoIzq_->setPosicion(340, 0);
    addItem(arcoIzq_);

    arcoDer_ = new Arco(Arco::OMICRON_XI);
    arcoDer_->setPosicion(340, 565);
    addItem(arcoDer_);

    crearObstaculos();
    connect(timerFrame_, &QTimer::timeout, this, &Nivel2::actualizarObstaculos);

    activo_ = true;
    timerFrame_->start(16);
    timerSegundo_->start(1000);
}

void Nivel2::crearObstaculos() {
    auto agregar = [&](float xb, float yb, float amp, float frec, float fase) {
        QGraphicsEllipseItem *item = addEllipse(-15, -15, 30, 30,
                                                QPen(QColor(255,200,0)),
                                                QBrush(QColor(200,100,0,180)));
        item->setPos(xb, yb);
        obstaculos_.append({item, xb, yb, amp, frec, fase});
    };
    agregar(300, 200, 60.0f, 0.8f, 0.0f);
    agregar(500, 380, 50.0f, 1.0f, 1.57f);
    agregar(400, 290, 40.0f, 0.6f, 3.14f);
}

void Nivel2::actualizarObstaculos() {
    tiempoOsc_ += 0.016f;
    for (auto &obs : obstaculos_) {
        float nuevaX = obs.xBase + obs.amplitud *
                       std::sin(2.0f * M_PI * obs.frecuencia * tiempoOsc_ + obs.fase);
        obs.item->setPos(nuevaX, obs.yBase);
        if (balon_ && balon_->collidesWithItem(obs.item))
            balon_->aplicarRebote(true);
    }
}

void Nivel2::keyPressEvent(QKeyEvent *event) {
    if (jugador1_) jugador1_->keyPress(static_cast<Qt::Key>(event->key()));
    if (modo_ == VS_HUMANO) {
        Jugador *j2 = dynamic_cast<Jugador*>(jugador2_);
        if (j2) j2->keyPress(static_cast<Qt::Key>(event->key()));
    }
}

void Nivel2::keyReleaseEvent(QKeyEvent *event) {
    if (jugador1_) jugador1_->keyRelease(static_cast<Qt::Key>(event->key()));
    if (modo_ == VS_HUMANO) {
        Jugador *j2 = dynamic_cast<Jugador*>(jugador2_);
        if (j2) j2->keyRelease(static_cast<Qt::Key>(event->key()));
    }
}
