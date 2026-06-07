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
      j2arr_(false), j2aba_(false), j2izq_(false), j2der_(false),
      colisionJ1_(false), colisionJ2_(false),
      ultimoToco_(0), combos_(0)
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
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    else
        setBackgroundBrush(QBrush(QColor(5,5,30)));

    const float CAN_X  =  90.0f;
    const float CAN_Y  =  60.0f;
    const float CAN_W  = 620.0f;
    const float CAN_H  = 460.0f;

    const float ARCO_A     =  10.0f;
    const float ARCO_ALTO  = 100.0f;
    const float ARCO_Y     = 230.0f;
    const float ARCO_IZQ_X = CAN_X - ARCO_A + 5.0f;
    const float ARCO_DER_X = CAN_X + CAN_W - 10.0f;

    canX_ = CAN_X; canY_ = CAN_Y; canW_ = CAN_W; canH_ = CAN_H;
    centroY_  = CAN_Y + CAN_H / 2.0f;
    spawnIzq_ = CAN_X + CAN_W * 0.25f;
    spawnDer_ = CAN_X + CAN_W * 0.75f;

    const float CENTRO_X = CAN_X + CAN_W / 2.0f;
    const float LIM_IZQ  = canX_ + 20.0f;
    const float LIM_DER  = canX_ + canW_ - 20.0f;

    jugador1_ = new Jugador("Fry", 4.0f,
                            Qt::Key_A, Qt::Key_D, Qt::Key_W,
                            QColor(100,200,255),
                            ":/assets/fry.png", "", false);
    static_cast<Jugador*>(jugador1_)->setSuelo(99999.0f);
    addItem(jugador1_);
    jugador1_->setPosicion(CAN_X+100, centroY_);

    if (modo_ == VS_HUMANO) {
        Jugador *j2 = new Jugador("Bender", 4.0f,
                                  Qt::Key_Left, Qt::Key_Right, Qt::Key_Up,
                                  QColor(180,180,180),
                                  ":/assets/bender.png", "", true);
        j2->setSuelo(99999.0f);
        jugador2_ = j2;
    } else {
        JugadorIA *ia = new JugadorIA("BenderIA", 3.5f, ARCO_DER_X);
        ia->setSuelo(99999.0f);
        ia->setLimites(LIM_IZQ, LIM_DER);
        ia->setOtroJugador(jugador1_);
        ia->setModoHockey(true);   // desactiva gravedad, salto y zapato
        jugador1_->setOtroJugador(ia);
        jugador2_ = ia;
    }
    addItem(jugador2_);
    jugador2_->setPosicion(CAN_X+CAN_W-100, centroY_);

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

    QFont fontHUD("Arial", 18, QFont::Bold);
    marcador_ = addText("0  -  0", fontHUD);
    marcador_->setDefaultTextColor(Qt::white);
    marcador_->setPos(anchoEscena_/2 - marcador_->boundingRect().width()/2, 8);
    marcador_->setZValue(10);

    temporizador_ = addText("60s", QFont("Arial",14,QFont::Bold));
    temporizador_->setDefaultTextColor(QColor(255,220,50));
    temporizador_->setPos(anchoEscena_-60, 8);
    temporizador_->setZValue(10);

    connect(timerFrame_,   &QTimer::timeout, this, &Nivel2::tickHockey);
    connect(this, &Nivel::golAnotado, this, &Nivel2::actualizarHUD);
    connect(timerSegundo_, &QTimer::timeout, this, [this]{ actualizarHUD(); });

    activo_ = true;
    timerFrame_->start(16);
    timerSegundo_->start(1000);
}

void Nivel2::actualizarHUD(int) {
    if (marcador_)
        marcador_->setPlainText(QString("%1  -  %2").arg(goles_[0]).arg(goles_[1]));
    if (temporizador_)
        temporizador_->setPlainText(QString("%1s").arg(tiempoRestante_));
}

void Nivel2::verificarGol() {}

bool Nivel2::cercaDelBalon(Personaje *jugador) {
    if (!balon_ || !jugador) return false;
    const float R_SUM    = 35.0f;
    const float Y_OFFSET = 10.0f;
    float ex = balon_->getX() - jugador->getX();
    float ey = balon_->getY() - (jugador->getY() + Y_OFFSET);
    return (ex*ex + ey*ey) <= R_SUM * R_SUM;
}

void Nivel2::colisionHockey(Personaje *jugador, float dvx, float dvy, int jugadorId) {
    if (!balon_ || !jugador) return;
    const float R_SUM    = 35.0f;
    const float Y_OFFSET = 10.0f;
    float jx=jugador->getX(), jy=jugador->getY()+Y_OFFSET;
    float bx=balon_->getX(), by=balon_->getY();
    float ex=bx-jx, ey=by-jy;
    float dist=qSqrt(ex*ex+ey*ey);
    if (dist<0.1f){ex=1.0f;ey=0.0f;dist=1.0f;}
    float nx=ex/dist, ny=ey/dist;
    if (ny>0.3f && dvy<=0.0f) return;
    balon_->setPosicion(jx+nx*(R_SUM+1.0f), jy+ny*(R_SUM+1.0f));
    float vbx=balon_->getVx(), vby=balon_->getVy();
    float vb_n=vbx*nx+vby*ny, vj_n=dvx*nx+dvy*ny;
    if (vb_n>0.0f && vj_n<=0.0f) return;
    if (ultimoToco_!=0 && ultimoToco_!=jugadorId) combos_++;
    else combos_=0;
    ultimoToco_=jugadorId;
    float comboBoost=qMin(1.0f+combos_*0.25f,2.5f);
    float abajoBoost=(dvy>0.0f&&ny>0.0f)?1.6f:1.0f;
    float movLat=qAbs(dvx),movVer=qAbs(dvy);
    float lat=(movLat+movVer>0.0f)?movLat/(movLat+movVer):0.0f;
    float nyA=ny*(1.0f-lat*0.6f),nxA=nx;
    float lenA=qSqrt(nxA*nxA+nyA*nyA);
    if (lenA>0.01f){nxA/=lenA;nyA/=lenA;}
    float impulso=(-vb_n+vj_n*1.5f)*comboBoost*abajoBoost;
    if (impulso<5.0f) impulso=5.0f;
    float nvbx=vbx+impulso*nxA, nvby=vby+impulso*nyA;
    float velMax=qMin(14.0f+combos_*1.5f,22.0f);
    float speed=qSqrt(nvbx*nvbx+nvby*nvby);
    if (speed>velMax){nvbx=nvbx/speed*velMax;nvby=nvby/speed*velMax;}
    balon_->lanzar(nvbx, nvby);
}

void Nivel2::resolverBodyblock() {
    if (!jugador1_||!jugador2_) return;
    const float R_JUG=25.0f, R2=R_JUG*2.0f;
    float ax=jugador1_->getX(),ay=jugador1_->getY();
    float bx=jugador2_->getX(),by=jugador2_->getY();
    float ex=bx-ax,ey=by-ay;
    float dist=qSqrt(ex*ex+ey*ey);
    if (dist>=R2||dist<0.1f) return;
    float nx=ex/dist,ny=ey/dist;
    float overlap=(R2-dist)/2.0f;
    float newAx=qBound(canX_+20.0f,ax-nx*overlap,canX_+canW_-20.0f);
    float newAy=qBound(canY_+20.0f,ay-ny*overlap,canY_+canH_+40.0f);
    float newBx=qBound(canX_+20.0f,bx+nx*overlap,canX_+canW_-20.0f);
    float newBy=qBound(canY_+20.0f,by+ny*overlap,canY_+canH_+40.0f);
    jugador1_->mover(newAx-ax,newAy-ay);
    jugador2_->mover(newBx-bx,newBy-by);
}

void Nivel2::tickHockey() {
    if (!activo_) return;

    const float VEL=4.0f, MAR_X=20.0f, MAR_Y_TOP=20.0f, MAR_Y_BOT=40.0f;

    float dx1=0,dy1=0;
    if (j1izq_) dx1-=VEL; if (j1der_) dx1+=VEL;
    if (j1arr_) dy1-=VEL; if (j1aba_) dy1+=VEL;
    float nx1=qBound(canX_+MAR_X,     jugador1_->getX()+dx1, canX_+canW_-MAR_X);
    float ny1=qBound(canY_+MAR_Y_TOP, jugador1_->getY()+dy1, canY_+canH_+MAR_Y_BOT);
    jugador1_->mover(nx1-jugador1_->getX(), ny1-jugador1_->getY());

    float dx2=0, dy2=0;
    if (modo_ == VS_HUMANO) {
        if (j2izq_) dx2-=VEL; if (j2der_) dx2+=VEL;
        if (j2arr_) dy2-=VEL; if (j2aba_) dy2+=VEL;
        float nx2=qBound(canX_+MAR_X,     jugador2_->getX()+dx2, canX_+canW_-MAR_X);
        float ny2=qBound(canY_+MAR_Y_TOP, jugador2_->getY()+dy2, canY_+canH_+MAR_Y_BOT);
        jugador2_->mover(nx2-jugador2_->getX(), ny2-jugador2_->getY());
    } else {
        JugadorIA *ia = dynamic_cast<JugadorIA*>(jugador2_);
        if (ia && balon_) {
            ia->percibir(balon_->getX(), balon_->getY(),
                         jugador1_->getX(), jugador1_->getY());
            float prevX=ia->getX(), prevY=ia->getY();
            ia->calcularMovHockey(dx2, dy2);
            ia->moverHockey(dx2, dy2,
                            canX_+MAR_X, canX_+canW_-MAR_X,
                            canY_+MAR_Y_TOP, canY_+canH_+MAR_Y_BOT);
            dx2=ia->getX()-prevX;
            dy2=ia->getY()-prevY;
        }
    }

    resolverBodyblock();
    if (!balon_) return;

    bool tocaJ1=cercaDelBalon(jugador1_);
    bool tocaJ2=cercaDelBalon(jugador2_);
    if (tocaJ1&&!colisionJ1_){colisionHockey(jugador1_,dx1,dy1,1);colisionJ1_=true;}
    else if (!tocaJ1) colisionJ1_=false;
    if (tocaJ2&&!colisionJ2_){colisionHockey(jugador2_,dx2,dy2,2);colisionJ2_=true;}
    else if (!tocaJ2) colisionJ2_=false;

    float bx=balon_->getX(),by=balon_->getY();
    float vx=balon_->getVx(),vy=balon_->getVy();
    bool fueGol=false;
    if (arcoIzq_&&arcoIzq_->detectarGol(bx,by,vx,vy)) {
        goles_[1]++; emit golAnotado(1);
        balon_->setPosicion(spawnIzq_,centroY_); balon_->lanzar(4.0f,0.0f);
        colisionJ1_=colisionJ2_=false; ultimoToco_=0; combos_=0; fueGol=true;
    } else if (arcoDer_&&arcoDer_->detectarGol(bx,by,vx,vy)) {
        goles_[0]++; emit golAnotado(0);
        balon_->setPosicion(spawnDer_,centroY_); balon_->lanzar(-4.0f,0.0f);
        colisionJ1_=colisionJ2_=false; ultimoToco_=0; combos_=0; fueGol=true;
    }
    if (!fueGol) {
        bx=balon_->getX();by=balon_->getY();
        vx=balon_->getVx();vy=balon_->getVy();
        if (bx<=canX_+8&&vx<0){balon_->aplicarRebote(true);balon_->setPosicion(canX_+9,by);}
        if (bx>=canX_+canW_-8&&vx>0){balon_->aplicarRebote(true);balon_->setPosicion(canX_+canW_-9,by);}
        if (by<=canY_+8&&vy<0){balon_->aplicarRebote(false);balon_->setPosicion(bx,canY_+9);}
        if (by>=canY_+canH_-8&&vy>0){balon_->aplicarRebote(false);balon_->setPosicion(bx,canY_+canH_-9);}
        float speed=qSqrt(vx*vx+vy*vy);
        if (speed<2.0f&&speed>0.0f) balon_->lanzar(balon_->getVx()*(2.0f/speed),balon_->getVy()*(2.0f/speed));
        else if (speed<0.1f) balon_->lanzar(4.0f,3.0f);
    }
}

void Nivel2::keyPressEvent(QKeyEvent *event) {
    switch(static_cast<Qt::Key>(event->key())) {
        case Qt::Key_W:     j1arr_=true; break;
        case Qt::Key_S:     j1aba_=true; break;
        case Qt::Key_A:     j1izq_=true; break;
        case Qt::Key_D:     j1der_=true; break;
        case Qt::Key_Up:    j2arr_=true; break;
        case Qt::Key_Down:  j2aba_=true; break;
        case Qt::Key_Left:  j2izq_=true; break;
        case Qt::Key_Right: j2der_=true; break;
        default: break;
    }
}

void Nivel2::keyReleaseEvent(QKeyEvent *event) {
    switch(static_cast<Qt::Key>(event->key())) {
        case Qt::Key_W:     j1arr_=false; break;
        case Qt::Key_S:     j1aba_=false; break;
        case Qt::Key_A:     j1izq_=false; break;
        case Qt::Key_D:     j1der_=false; break;
        case Qt::Key_Up:    j2arr_=false; break;
        case Qt::Key_Down:  j2aba_=false; break;
        case Qt::Key_Left:  j2izq_=false; break;
        case Qt::Key_Right: j2der_=false; break;
        default: break;
    }
}
