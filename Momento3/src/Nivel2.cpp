#include "hds/Nivel2.h"
#include "hds/Jugador.h"
#include "hds/JugadorIA.h"
#include "hds/Balon.h"
#include "hds/Arco.h"
#include <QKeyEvent>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QPixmap>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QtMath>
#include <cmath>

Nivel2::Nivel2(ModoJuego modo, QObject *parent)
    : Nivel(modo, parent), tiempoOsc_(0.0f),
      j1vx_(0), j1vy_(0), j2vx_(0), j2vy_(0),
      j1arr_(false), j1aba_(false), j1izq_(false), j1der_(false),
      j2arr_(false), j2aba_(false), j2izq_(false), j2der_(false)
{}

Nivel2::~Nivel2() {}

void Nivel2::inicializar() {
    anchoEscena_ = 800;
    altoEscena_  = 600;
    tiempoRestante_ = 60;
    setSceneRect(0, 0, anchoEscena_, altoEscena_);

    QPixmap fondo(":/assets/fondo_nivel2.png");
    if (!fondo.isNull())
        setBackgroundBrush(QBrush(fondo.scaled(
            anchoEscena_, altoEscena_,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation)));
    else
        setBackgroundBrush(QBrush(QColor(5, 5, 30)));

    const float CAN_X  =  90.0f;
    const float CAN_Y  =  60.0f;
    const float CAN_W  = 620.0f;
    const float CAN_H  = 480.0f;

    const float ARCO_A     =  10.0f;
    const float ARCO_ALTO  = 100.0f;
    const float ARCO_Y     = 230.0f;

    // Arco izq: 5px dentro de la cancha para que el balon lo alcance antes de rebotar
    // Arco der: 10px dentro de la cancha (antes del limite de rebote en pared derecha)
    const float ARCO_IZQ_X = CAN_X - ARCO_A + 5.0f;
    const float ARCO_DER_X = CAN_X + CAN_W  - 10.0f;

    canX_ = CAN_X; canY_ = CAN_Y; canW_ = CAN_W; canH_ = CAN_H;
    centroY_  = CAN_Y + CAN_H / 2.0f;
    spawnIzq_ = CAN_X + CAN_W * 0.25f;
    spawnDer_ = CAN_X + CAN_W * 0.75f;

    addRect(CAN_X, CAN_Y, CAN_W, CAN_H,
            QPen(QColor(255, 255, 0, 180), 3, Qt::DashLine),
            QBrush(Qt::NoBrush))->setZValue(5);
    addRect(ARCO_IZQ_X, ARCO_Y, ARCO_A, ARCO_ALTO,
            QPen(QColor(255, 100, 0, 200), 2),
            QBrush(QColor(255, 100, 0, 80)))->setZValue(5);
    addRect(ARCO_DER_X, ARCO_Y, ARCO_A, ARCO_ALTO,
            QPen(QColor(0, 200, 255, 200), 2),
            QBrush(QColor(0, 200, 255, 80)))->setZValue(5);

    const float CENTRO_X = CAN_X + CAN_W / 2.0f;

    jugador1_ = new Jugador("Fry", 4.0f,
                            Qt::Key_A, Qt::Key_D, Qt::Key_W,
                            QColor(100, 200, 255),
                            ":/assets/fry.png", "", false);
    static_cast<Jugador*>(jugador1_)->setSuelo(99999.0f);
    addItem(jugador1_);
    jugador1_->setPosicion(CAN_X + 100, centroY_);

    if (modo_ == VS_HUMANO) {
        Jugador *j2 = new Jugador("Bender", 4.0f,
                                  Qt::Key_Left, Qt::Key_Right, Qt::Key_Up,
                                  QColor(180, 180, 180),
                                  ":/assets/bender.png", "", true);
        j2->setSuelo(99999.0f);
        jugador2_ = j2;
    } else {
        JugadorIA *ia = new JugadorIA("BenderIA", 3.5f, ARCO_DER_X);
        jugador2_ = ia;
    }
    addItem(jugador2_);
    jugador2_->setPosicion(CAN_X + CAN_W - 100, centroY_);

    balon_ = new Balon();
    balon_->setModoParabolico(false);
    balon_->setBounds(anchoEscena_, altoEscena_);
    addItem(balon_);
    balon_->setPosicion(CENTRO_X, centroY_);
    balon_->lanzar(5.0f, 4.0f);

    arcoIzq_ = new Arco(Arco::PLANET_EXPRESS, ARCO_ALTO);
    addItem(arcoIzq_);
    arcoIzq_->setPosicion(ARCO_IZQ_X, ARCO_Y);

    arcoDer_ = new Arco(Arco::OMICRON_XI, ARCO_ALTO);
    addItem(arcoDer_);
    arcoDer_->setPosicion(ARCO_DER_X, ARCO_Y);

    crearObstaculos();

    QFont fontHUD("Arial", 18, QFont::Bold);
    marcador_ = addText("0  -  0", fontHUD);
    marcador_->setDefaultTextColor(Qt::white);
    marcador_->setPos(anchoEscena_/2 - marcador_->boundingRect().width()/2, 8);
    marcador_->setZValue(10);

    temporizador_ = addText("60s", QFont("Arial", 14, QFont::Bold));
    temporizador_->setDefaultTextColor(QColor(255, 220, 50));
    temporizador_->setPos(anchoEscena_ - 60, 8);
    temporizador_->setZValue(10);

    connect(timerFrame_,   &QTimer::timeout, this, &Nivel2::tickHockey);
    connect(timerFrame_,   &QTimer::timeout, this, &Nivel2::actualizarObstaculos);
    connect(this,          &Nivel::golAnotado,    this, &Nivel2::actualizarHUD);
    connect(timerSegundo_, &QTimer::timeout, this, [this]{ actualizarHUD(); });

    activo_ = true;
    timerFrame_->start(16);
    timerSegundo_->start(1000);
}

void Nivel2::actualizarHUD(int) {
    if (marcador_)
        marcador_->setPlainText(
            QString("%1  -  %2").arg(goles_[0]).arg(goles_[1]));
    if (temporizador_)
        temporizador_->setPlainText(
            QString("%1s").arg(tiempoRestante_));
}

void Nivel2::verificarGol() {
    // Este metodo existe por el override pero la logica real esta en tickHockey
    // para garantizar el orden: gol primero, rebote despues
}

void Nivel2::tickHockey() {
    if (!activo_) return;

    const float VEL       = 4.0f;
    const float MAR_X     = 20.0f;
    const float MAR_Y_TOP = 10.0f;
    const float MAR_Y_BOT = 10.0f;

    float dx1 = 0, dy1 = 0;
    if (j1izq_) dx1 -= VEL;
    if (j1der_) dx1 += VEL;
    if (j1arr_) dy1 -= VEL;
    if (j1aba_) dy1 += VEL;
    float nx1 = qBound(canX_ + MAR_X,     jugador1_->getX() + dx1, canX_ + canW_ - MAR_X);
    float ny1 = qBound(canY_ + MAR_Y_TOP, jugador1_->getY() + dy1, canY_ + canH_ + MAR_Y_BOT);
    jugador1_->mover(nx1 - jugador1_->getX(), ny1 - jugador1_->getY());

    if (modo_ == VS_HUMANO) {
        float dx2 = 0, dy2 = 0;
        if (j2izq_) dx2 -= VEL;
        if (j2der_) dx2 += VEL;
        if (j2arr_) dy2 -= VEL;
        if (j2aba_) dy2 += VEL;
        float nx2 = qBound(canX_ + MAR_X,     jugador2_->getX() + dx2, canX_ + canW_ - MAR_X);
        float ny2 = qBound(canY_ + MAR_Y_TOP, jugador2_->getY() + dy2, canY_ + canH_ + MAR_Y_BOT);
        jugador2_->mover(nx2 - jugador2_->getX(), ny2 - jugador2_->getY());
    }

    if (!balon_) return;

    float bx = balon_->getX();
    float by = balon_->getY();
    float vx = balon_->getVx();
    float vy = balon_->getVy();

    // 1. Verificar gol PRIMERO
    bool fueGol = false;
    if (arcoIzq_ && arcoIzq_->detectarGol(bx, by, vx, vy)) {
        goles_[1]++;
        emit golAnotado(1);
        balon_->setPosicion(spawnIzq_, centroY_);
        balon_->lanzar(4.0f, 0.0f);
        fueGol = true;
    } else if (arcoDer_ && arcoDer_->detectarGol(bx, by, vx, vy)) {
        goles_[0]++;
        emit golAnotado(0);
        balon_->setPosicion(spawnDer_, centroY_);
        balon_->lanzar(-4.0f, 0.0f);
        fueGol = true;
    }

    // 2. Rebotar en paredes SOLO si no fue gol
    if (!fueGol) {
        bx = balon_->getX();
        by = balon_->getY();
        vx = balon_->getVx();
        vy = balon_->getVy();

        // Pared izquierda (rebota normal)
        if (bx <= canX_ + 8 && vx < 0)         { balon_->aplicarRebote(true);  balon_->setPosicion(canX_ + 9, by); }
        // Pared derecha: rebota solo si NO hay arco ahi (el arco ya esta 10px adentro)
        if (bx >= canX_ + canW_ - 8 && vx > 0) { balon_->aplicarRebote(true);  balon_->setPosicion(canX_ + canW_ - 9, by); }
        // Paredes superior e inferior
        if (by <= canY_ + 8 && vy < 0)          { balon_->aplicarRebote(false); balon_->setPosicion(bx, canY_ + 9); }
        if (by >= canY_ + canH_ - 8 && vy > 0)  { balon_->aplicarRebote(false); balon_->setPosicion(bx, canY_ + canH_ - 9); }

        float speed = qSqrt(vx*vx + vy*vy);
        if (speed < 2.0f && speed > 0.0f)
            balon_->lanzar(balon_->getVx() * (2.0f/speed), balon_->getVy() * (2.0f/speed));
        else if (speed < 0.1f)
            balon_->lanzar(4.0f, 3.0f);
    }
}

void Nivel2::crearObstaculos() {
    auto agregar = [&](float xb, float yb, float amp, float frec, float fase) {
        QGraphicsEllipseItem *item = addEllipse(-15, -15, 30, 30,
                                                QPen(QColor(255,200,0)),
                                                QBrush(QColor(200,100,0,180)));
        item->setPos(xb, yb);
        obstaculos_.append({item, xb, yb, amp, frec, fase});
    };
    agregar(300, 250, 60.0f, 0.8f, 0.0f);
    agregar(500, 380, 50.0f, 1.0f, 1.57f);
    agregar(400, 310, 40.0f, 0.6f, 3.14f);
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
    switch (static_cast<Qt::Key>(event->key())) {
        case Qt::Key_W:     j1arr_ = true; break;
        case Qt::Key_S:     j1aba_ = true; break;
        case Qt::Key_A:     j1izq_ = true; break;
        case Qt::Key_D:     j1der_ = true; break;
        case Qt::Key_Up:    j2arr_ = true; break;
        case Qt::Key_Down:  j2aba_ = true; break;
        case Qt::Key_Left:  j2izq_ = true; break;
        case Qt::Key_Right: j2der_ = true; break;
        default: break;
    }
}

void Nivel2::keyReleaseEvent(QKeyEvent *event) {
    switch (static_cast<Qt::Key>(event->key())) {
        case Qt::Key_W:     j1arr_ = false; break;
        case Qt::Key_S:     j1aba_ = false; break;
        case Qt::Key_A:     j1izq_ = false; break;
        case Qt::Key_D:     j1der_ = false; break;
        case Qt::Key_Up:    j2arr_ = false; break;
        case Qt::Key_Down:  j2aba_ = false; break;
        case Qt::Key_Left:  j2izq_ = false; break;
        case Qt::Key_Right: j2der_ = false; break;
        default: break;
    }
}
