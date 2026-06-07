#include "hds/JugadorIA.h"
#include <QPainter>
#include <QtMath>
#include <cmath>

JugadorIA::JugadorIA(const QString &nombre,
                     float velocidad,
                     float xArco,
                     QGraphicsItem *parent)
    : Personaje(nombre, 3, velocidad, parent),
      balonX_(0), balonY_(0), rivalX_(0), rivalY_(0),
      balonVisible_(false),
      estadoHockey_(PORTERO),
      objetivoX_(xArco), objetivoY_(0),
      debeAtacar_(false),
      defendiendo_(false), framesDefensa_(0),
      xArco_(xArco),
      enSuelo_(true), vy_(0.0f), suelo_(400.0f),
      velActualX_(0.0f), velActualY_(0.0f),
      cooldownSalto_(0),
      tendenciaRival_(0.0f), modificadorReaccion_(1.0f)
{
    pixPersonaje_.load(":/assets/bender.png");
    pixZapato_.load(":/assets/zapato.png");

    timerDecision_ = new QTimer(this);
    connect(timerDecision_, &QTimer::timeout, this, &JugadorIA::cicloDecision);
    timerDecision_->start(300);
}

JugadorIA::~JugadorIA() {}

QRectF JugadorIA::boundingRect() const {
    return QRectF(-ANCHO_SPRITE/2-10, -ALTO_SPRITE-10,
                   ANCHO_SPRITE+20,   ALTO_SPRITE+ALTO_ZAPATO+20);
}

void JugadorIA::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem *,
                      QWidget *)
{
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->setRenderHint(QPainter::Antialiasing);

    if (!pixPersonaje_.isNull()) {
        painter->save();
        painter->scale(-1, 1);
        painter->drawPixmap(
            QRectF(-ANCHO_SPRITE/2, -ALTO_SPRITE, ANCHO_SPRITE, ALTO_SPRITE),
            pixPersonaje_, QRectF(pixPersonaje_.rect()));
        painter->restore();
    } else {
        painter->setBrush(QColor(120,120,120));
        painter->setPen(Qt::NoPen);
        painter->drawRect(QRectF(-9,-20,18,20));
        painter->setBrush(QColor(180,180,180));
        painter->setPen(QPen(Qt::white,1.5));
        painter->drawEllipse(QRectF(-12,-44,24,24));
    }

    // Zapato solo en Nivel1
    if (!modoHockey_ && !pixZapato_.isNull()) {
        painter->save();
        painter->translate(-ZAP_PIVOT_X, ZAP_PIVOT_Y);
        painter->scale(-1,1);
        painter->drawPixmap(
            QRectF(0,-ALTO_ZAPATO,ANCHO_ZAPATO,ALTO_ZAPATO),
            pixZapato_, QRectF(pixZapato_.rect()));
        painter->restore();
    }

    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial",7));
    painter->drawText(QRectF(-24,4,48,12), Qt::AlignCenter, nombre_);
}

void JugadorIA::percibir(float balonX, float balonY,
                         float jugadorX, float jugadorY) {
    rivalX_ = jugadorX; rivalY_ = jugadorY;
    if (!defendiendo_) {
        balonVisible_ = true;
        balonX_ = balonX; balonY_ = balonY;
    }
}

void JugadorIA::alertarGol() {
    defendiendo_   = true;
    framesDefensa_ = FRAMES_DEFENSA;
    balonVisible_  = false;
    objetivoX_     = xArco_;
    estadoHockey_  = PORTERO;
    cooldownSalto_ = FRAMES_COOLDOWN_SALTO;
    velActualX_ = velActualY_ = 0.0f;
}

void JugadorIA::razonar() {
    if (defendiendo_) { objetivoX_ = xArco_; return; }
    if (!balonVisible_) {
        debeAtacar_ = false;
        objetivoX_  = xArco_;
        objetivoY_  = y_;
        return;
    }
    float distBalonArco = std::abs(balonX_ - xArco_);
    debeAtacar_ = (distBalonArco < 350.0f);
    objetivoX_  = debeAtacar_ ? balonX_ + tendenciaRival_ * 15.0f : xArco_;
    objetivoY_  = debeAtacar_ ? balonY_ : y_;
    if (objetivoX_ < xMin_) objetivoX_ = xMin_;
    if (objetivoX_ > xMax_) objetivoX_ = xMax_;
}

void JugadorIA::aprender() {
    if (balonVisible_) {
        historialBalonX_.append(balonX_);
        if (historialBalonX_.size() > 20) historialBalonX_.removeFirst();
    }
    if (historialBalonX_.size() < 5) return;
    float sumaR=0, sumaA=0;
    int mitad = historialBalonX_.size()/2;
    for (int i=0;    i<mitad;                   i++) sumaA += historialBalonX_[i];
    for (int i=mitad;i<historialBalonX_.size(); i++) sumaR += historialBalonX_[i];
    tendenciaRival_ = ((sumaR/(historialBalonX_.size()-mitad))-(sumaA/mitad)) > 0 ? 1.0f : -1.0f;
    if (modificadorReaccion_ < 1.5f) modificadorReaccion_ += 0.02f;
}

void JugadorIA::cicloDecision() {
    if (modoHockey_ && balonVisible_) {
        float distH = std::abs(balonX_ - xArco_);
        if (estadoHockey_ == PORTERO  && distH < DIST_ATAQUE)  estadoHockey_ = ATACANTE;
        if (estadoHockey_ == ATACANTE && distH > DIST_DEFENSA) estadoHockey_ = PORTERO;
    }
    razonar();
    aprender();
}

static float arriveAxis(float actual, float target, float velMax, float slowR) {
    float diff = target - actual;
    float dist = std::abs(diff);
    if (dist < 1.0f) return 0.0f;
    float speed = (dist < slowR) ? velMax * (dist / slowR) : velMax;
    if (speed < 0.5f) return 0.0f;
    return (diff > 0) ? speed : -speed;
}

// Nivel2: maquina de estados + Arrive, SIN gravedad
void JugadorIA::calcularMovHockey(float &outDx, float &outDy) {
    float velMax = velocidad_ * modificadorReaccion_;
    float tx, ty;
    if (estadoHockey_ == PORTERO) {
        // X fija cerca del arco; Y sigue al balon para interceptar
        tx = xArco_ - OFFSET_PORTERO * ((xArco_ > 400) ? 1.0f : -1.0f);
        ty = balonVisible_ ? balonY_ : y_;
    } else {
        // Atacante: perseguir balon
        tx = balonX_;
        ty = balonY_;
    }
    outDx = arriveAxis(x_, tx, velMax, ARRIVE_SLOW_R);
    outDy = arriveAxis(y_, ty, velMax, ARRIVE_SLOW_R);
}

// Nivel2: mueve con qBound; resetea vel acumulada si choca con borde
void JugadorIA::moverHockey(float dx, float dy,
                             float limIzq, float limDer,
                             float limTop, float limBot) {
    float nx = qBound(limIzq, x_ + dx, limDer);
    float ny = qBound(limTop, y_ + dy, limBot);
    if (nx != x_ + dx) velActualX_ = 0.0f;
    if (ny != y_ + dy) velActualY_ = 0.0f;
    x_ = nx; y_ = ny;
    setPos(x_, y_);
    update();
}

// Nivel1: lerp por eje
float JugadorIA::calcularDx() {
    float velMax = velocidad_ * modificadorReaccion_;
    float diff   = objetivoX_ - x_;
    float velDeseada = (std::abs(diff) > 5.0f) ? ((diff>0)?velMax:-velMax) : 0.0f;
    velActualX_ += (velDeseada - velActualX_) * 0.18f;
    if (std::abs(diff) < 3.0f) velActualX_ *= 0.5f;
    return velActualX_;
}

float JugadorIA::calcularDy() {
    float velMax = velocidad_ * modificadorReaccion_;
    float diff   = objetivoY_ - y_;
    float velDeseada = (std::abs(diff) > 5.0f) ? ((diff>0)?velMax:-velMax) : 0.0f;
    velActualY_ += (velDeseada - velActualY_) * 0.18f;
    if (std::abs(diff) < 3.0f) velActualY_ *= 0.5f;
    return velActualY_;
}

// Nivel1 unicamente: fisica con gravedad y salto
void JugadorIA::actualizar() {
    if (!activo_ || modoHockey_) return;  // en hockey Nivel2 no hacer nada aqui

    if (defendiendo_) {
        if (framesDefensa_ > 0) framesDefensa_--;
        else { defendiendo_ = false; balonVisible_ = true; }
    }

    float dx = calcularDx();
    if (cooldownSalto_ > 0) cooldownSalto_--;

    if (!defendiendo_ && balonVisible_ && enSuelo_ && cooldownSalto_ == 0) {
        float distV = y_ - balonY_;
        float distH = std::abs(x_ - balonX_);
        if (distV > MIN_DIST_SALTO && distH < DIST_H_SALTO) {
            vy_ = IMPULSO_SALTO; enSuelo_ = false;
            cooldownSalto_ = FRAMES_COOLDOWN_SALTO;
        }
    }
    if (!enSuelo_) vy_ += GRAVEDAD;
    float nuevoY = y_ + vy_;
    if (nuevoY >= suelo_) { nuevoY = suelo_; vy_ = 0.0f; enSuelo_ = true; }
    mover(dx, nuevoY - y_);
    update();
}

void JugadorIA::mover(float dx, float dy) {
    x_ += dx; y_ += dy;
    if (x_ < xMin_) { x_ = xMin_; velActualX_ = 0.0f; }
    if (x_ > xMax_) { x_ = xMax_; velActualX_ = 0.0f; }
    if (enSuelo_ && otroJugador_) {
        float otroX = otroJugador_->getX();
        float mitad = ANCHO_SPRITE / 2.0f;
        if (x_ < otroX && x_+mitad > otroX-mitad) x_ = otroX - ANCHO_SPRITE;
        else if (x_ > otroX && x_-mitad < otroX+mitad) x_ = otroX + ANCHO_SPRITE;
    }
    setPos(x_, y_);
}

void JugadorIA::contacto(Balon *balon) {
    float dirX = (xArco_ < 400) ? 1.0f : -1.0f;
    float impX = dirX * velocidad_ * modificadorReaccion_ * 1.5f;
    float impY = enSuelo_ ? -6.0f : vy_ * 1.2f;
    balon->lanzar(impX, impY);
}

void JugadorIA::reiniciar() {
    enSuelo_ = true; vy_ = 0.0f; balonVisible_ = false;
    debeAtacar_ = false; objetivoX_ = xArco_; objetivoY_ = y_;
    defendiendo_ = false; framesDefensa_ = 0;
    estadoHockey_ = PORTERO;
    tendenciaRival_ = 0.0f; modificadorReaccion_ = 1.0f;
    velActualX_ = 0.0f; velActualY_ = 0.0f;
    cooldownSalto_ = 0;
    historialBalonX_.clear(); activo_ = true; update();
}
