#include "hds/JugadorIA.h"
#include <QPainter>
#include <QtMath>
#include <cmath>
#include <cstdlib>

JugadorIA::JugadorIA(const QString &nombre,
                     float velocidad,
                     float xArco,
                     QGraphicsItem *parent)
    : Personaje(nombre, 3, velocidad, parent),
      balonX_(0), balonY_(0), rivalX_(0), rivalY_(0),
      balonVisible_(false),
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
    return QRectF(-ANCHO_SPRITE / 2 - 10, -ALTO_SPRITE - 10,
                   ANCHO_SPRITE + 20, ALTO_SPRITE + ALTO_ZAPATO + 20);
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
            QRectF(-ANCHO_SPRITE / 2, -ALTO_SPRITE, ANCHO_SPRITE, ALTO_SPRITE),
            pixPersonaje_, QRectF(pixPersonaje_.rect()));
        painter->restore();
    } else {
        painter->setBrush(QColor(120, 120, 120));
        painter->setPen(Qt::NoPen);
        painter->drawRect(QRectF(-9, -20, 18, 20));
        painter->setBrush(QColor(180, 180, 180));
        painter->setPen(QPen(Qt::white, 1.5));
        painter->drawEllipse(QRectF(-12, -44, 24, 24));
    }

    if (!pixZapato_.isNull()) {
        painter->save();
        painter->translate(-ZAP_PIVOT_X, ZAP_PIVOT_Y);
        painter->scale(-1, 1);
        painter->drawPixmap(
            QRectF(0, -ALTO_ZAPATO, ANCHO_ZAPATO, ALTO_ZAPATO),
            pixZapato_, QRectF(pixZapato_.rect()));
        painter->restore();
    }

    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 7));
    painter->drawText(QRectF(-24, 4, 48, 12), Qt::AlignCenter, nombre_);
}

void JugadorIA::percibir(float balonX, float balonY,
                         float jugadorX, float jugadorY) {
    rivalX_ = jugadorX; rivalY_ = jugadorY;
    // En modo defensa no actualiza objetivo al balon
    if (!defendiendo_) {
        balonVisible_ = true;
        balonX_ = balonX; balonY_ = balonY;
    }
}

void JugadorIA::alertarGol() {
    defendiendo_   = true;
    framesDefensa_ = FRAMES_DEFENSA;
    balonVisible_  = false;
    // Posicion defensiva: frente a su arco, en el suelo
    objetivoX_     = xArco_;
    cooldownSalto_ = FRAMES_COOLDOWN_SALTO; // no saltar durante el reposicionamiento
}

void JugadorIA::razonar() {
    if (defendiendo_) {
        objetivoX_ = xArco_;
        return;
    }
    if (!balonVisible_) {
        debeAtacar_ = false;
        objetivoX_ = xArco_;
        objetivoY_ = y_;
        return;
    }
    float distBalonArco = std::abs(balonX_ - xArco_);
    debeAtacar_ = (distBalonArco < 350.0f);
    objetivoX_ = debeAtacar_ ? balonX_ + tendenciaRival_ * 15.0f : xArco_;
    objetivoY_ = debeAtacar_ ? balonY_ : y_;
    if (objetivoX_ < xMin_) objetivoX_ = xMin_;
    if (objetivoX_ > xMax_) objetivoX_ = xMax_;
}

void JugadorIA::aprender() {
    if (balonVisible_) {
        historialBalonX_.append(balonX_);
        if (historialBalonX_.size() > 20) historialBalonX_.removeFirst();
    }
    if (historialBalonX_.size() < 5) return;
    float sumaR = 0, sumaA = 0;
    int mitad = historialBalonX_.size() / 2;
    for (int i = 0;     i < mitad;                    i++) sumaA += historialBalonX_[i];
    for (int i = mitad; i < historialBalonX_.size();  i++) sumaR += historialBalonX_[i];
    tendenciaRival_ = ((sumaR / (historialBalonX_.size() - mitad))
                       - (sumaA / mitad)) > 0 ? 1.0f : -1.0f;
    if (modificadorReaccion_ < 1.5f) modificadorReaccion_ += 0.02f;
}

void JugadorIA::cicloDecision() { razonar(); aprender(); }

float JugadorIA::calcularDx() {
    float velMax = velocidad_ * modificadorReaccion_;
    float diff = objetivoX_ - x_;
    float velDeseada = 0.0f;
    if (std::abs(diff) > 5.0f)
        velDeseada = (diff > 0) ? velMax : -velMax;
    const float SUAVIZADO = 0.18f;
    velActualX_ += (velDeseada - velActualX_) * SUAVIZADO;
    if (std::abs(diff) < 3.0f) velActualX_ *= 0.5f;
    return velActualX_;
}

float JugadorIA::calcularDy() {
    float velMax = velocidad_ * modificadorReaccion_;
    float diff = objetivoY_ - y_;
    float velDeseada = 0.0f;
    if (std::abs(diff) > 5.0f)
        velDeseada = (diff > 0) ? velMax : -velMax;
    const float SUAVIZADO = 0.18f;
    velActualY_ += (velDeseada - velActualY_) * SUAVIZADO;
    if (std::abs(diff) < 3.0f) velActualY_ *= 0.5f;
    return velActualY_;
}

void JugadorIA::actualizar() {
    if (!activo_) return;

    // Cuenta regresiva del modo defensa
    if (defendiendo_) {
        if (framesDefensa_ > 0) framesDefensa_--;
        else { defendiendo_ = false; balonVisible_ = true; }
    }

    float dx = calcularDx();

    if (cooldownSalto_ > 0) cooldownSalto_--;

    // Salto: solo si no esta en modo defensa y se cumplen las 3 condiciones
    if (!defendiendo_ && balonVisible_ && enSuelo_ && cooldownSalto_ == 0) {
        float distV = y_ - balonY_;
        float distH = std::abs(x_ - balonX_);
        if (distV > MIN_DIST_SALTO && distH < DIST_H_SALTO) {
            vy_ = IMPULSO_SALTO;
            enSuelo_ = false;
            cooldownSalto_ = FRAMES_COOLDOWN_SALTO;
        }
    }

    if (!enSuelo_) vy_ += GRAVEDAD;
    float nuevoY = y_ + vy_;
    if (nuevoY >= suelo_) { nuevoY = suelo_; vy_ = 0.0f; enSuelo_ = true; }

    mover(dx, nuevoY - y_);
    update();
}

void JugadorIA::moverHockey(float dx, float dy,
                             float limIzq, float limDer,
                             float limTop, float limBot) {
    float nx = qBound(limIzq, x_ + dx, limDer);
    float ny = qBound(limTop, y_ + dy, limBot);
    x_ = nx;
    y_ = ny;
    setPos(x_, y_);
    update();
}

void JugadorIA::mover(float dx, float dy) {
    x_ += dx;
    y_ += dy;
    if (x_ < xMin_) { x_ = xMin_; velActualX_ = 0.0f; }
    if (x_ > xMax_) { x_ = xMax_; velActualX_ = 0.0f; }
    if (enSuelo_ && otroJugador_) {
        float otroX = otroJugador_->getX();
        float mitad = ANCHO_SPRITE / 2.0f;
        if (x_ < otroX && x_ + mitad > otroX - mitad)
            x_ = otroX - ANCHO_SPRITE;
        else if (x_ > otroX && x_ - mitad < otroX + mitad)
            x_ = otroX + ANCHO_SPRITE;
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
    tendenciaRival_ = 0.0f; modificadorReaccion_ = 1.0f;
    velActualX_ = 0.0f; velActualY_ = 0.0f;
    cooldownSalto_ = 0;
    historialBalonX_.clear(); activo_ = true; update();
}
