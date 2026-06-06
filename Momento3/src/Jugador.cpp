#include "hds/Jugador.h"
#include <QPainter>
#include <QPainterPath>
#include <QtMath>
#include <cstdlib>

Jugador::Jugador(const QString &nombre,
                 float velocidad,
                 Qt::Key teclaIzq,
                 Qt::Key teclaDer,
                 Qt::Key teclaSalto,
                 const QColor &color,
                 const QString &spritePersonaje,
                 const QString &spriteZapato,
                 bool reflejar,
                 QGraphicsItem *parent)
    : Personaje(nombre, 3, velocidad, parent),
      teclaIzq_(teclaIzq), teclaDer_(teclaDer),
      teclaSalto_(teclaSalto), teclaPatada_(Qt::Key_unknown),
      presIzq_(false), presDer_(false), presSalto_(false),
      anguloPatada_(0.0f), pateando_(false),
      vy_(0.0f), enSuelo_(true), suelo_(355.0f),
      velocidadBase_(velocidad), modificadorVelocidad_(1.0f),
      enPanico_(false), color_(color), reflejar_(reflejar)
{
    if (!spritePersonaje.isEmpty()) pixPersonaje_.load(spritePersonaje);
    if (!spriteZapato.isEmpty())    pixZapato_.load(spriteZapato);

    timerTurbo_  = new QTimer(this);
    timerPanico_ = new QTimer(this);
    timerPatada_ = new QTimer(this);
    timerTurbo_->setSingleShot(true);
    timerPanico_->setSingleShot(true);
    timerPatada_->setSingleShot(true);
    connect(timerTurbo_,  &QTimer::timeout, this, &Jugador::desactivarTurbo);
    connect(timerPanico_, &QTimer::timeout, this, &Jugador::desactivarPanico);
    connect(timerPatada_, &QTimer::timeout, this, &Jugador::detenerPatada);
}

Jugador::~Jugador() {}

QRectF Jugador::boundingRect() const {
    return QRectF(-ANCHO_SPRITE / 2 - 10, -ALTO_SPRITE - 10,
                   ANCHO_SPRITE + 20, ALTO_SPRITE + ALTO_ZAPATO + 20);
}

QPainterPath Jugador::shape() const {
    QPainterPath path;

    // Hitbox de la cabeza/cuerpo (siempre activo)
    path.addEllipse(QRectF(-RADIO_CABEZA * 0.7f, -ALTO_SPRITE,
                            RADIO_CABEZA * 1.4f,  RADIO_CABEZA * 1.4f));

    // Hitbox extendida del zapato: activa al saltar y patear
    // Cubre la zona del zapato hacia abajo para detectar el balon
    if (!enSuelo_ && pateando_) {
        // En el aire pateando: zona amplia hacia abajo (zapato en pleno swing)
        path.addRect(QRectF(-ANCHO_ZAPATO / 2, ZAP_PIVOT_Y - ALTO_ZAPATO,
                             ANCHO_ZAPATO,      ALTO_ZAPATO + 18.0f));
    } else if (!enSuelo_) {
        // En el aire sin patear: zona normal del zapato
        path.addRect(QRectF(-ANCHO_ZAPATO / 2, ZAP_PIVOT_Y - ALTO_ZAPATO,
                             ANCHO_ZAPATO,      ALTO_ZAPATO));
    }

    return path;
}

void Jugador::paint(QPainter *painter,
                    const QStyleOptionGraphicsItem *,
                    QWidget *)
{
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->setRenderHint(QPainter::Antialiasing);

    // ---- Sprite del personaje ----
    if (!pixPersonaje_.isNull()) {
        painter->save();
        if (reflejar_) painter->scale(-1, 1);
        painter->drawPixmap(
            QRectF(-ANCHO_SPRITE / 2, -ALTO_SPRITE, ANCHO_SPRITE, ALTO_SPRITE),
            pixPersonaje_, QRectF(pixPersonaje_.rect()));
        painter->restore();
    } else {
        QColor cc = enPanico_ ? Qt::red
                  : (modificadorVelocidad_ > 1.0f ? Qt::yellow : color_);
        painter->setBrush(color_.darker(130)); painter->setPen(Qt::NoPen);
        painter->drawRect(QRectF(-ANCHO_CUERPO/2, -ALTO_CUERPO, ANCHO_CUERPO, ALTO_CUERPO));
        painter->setBrush(cc); painter->setPen(QPen(Qt::white, 1.5));
        painter->drawEllipse(QRectF(-RADIO_CABEZA, -ALTO_SPRITE, RADIO_CABEZA*2, RADIO_CABEZA*2));
        painter->setBrush(Qt::white); painter->setPen(Qt::NoPen);
        painter->drawEllipse(QRectF(-10, -ALTO_SPRITE+8, 7, 7));
        painter->drawEllipse(QRectF(  3, -ALTO_SPRITE+8, 7, 7));
    }

    // ---- Zapato (siempre visible, rota al patear) ----
    if (!pixZapato_.isNull()) {
        float pivotX = reflejar_ ? -ZAP_PIVOT_X : ZAP_PIVOT_X;
        painter->save();
        painter->translate(pivotX, ZAP_PIVOT_Y);
        float angulo = reflejar_ ? -anguloPatada_ : anguloPatada_;
        painter->rotate(angulo);
        if (reflejar_) painter->scale(-1, 1);
        painter->drawPixmap(
            QRectF(0, -ALTO_ZAPATO, ANCHO_ZAPATO, ALTO_ZAPATO),
            pixZapato_, QRectF(pixZapato_.rect()));
        painter->restore();
    }
    // Nombre eliminado
}

void Jugador::keyPress(Qt::Key key) {
    if (key == teclaIzq_)  presIzq_  = true;
    if (key == teclaDer_)  presDer_  = true;
    if (key == teclaSalto_ && enSuelo_) { vy_ = IMPULSO_SALTO; enSuelo_ = false; }
    if (teclaPatada_ != Qt::Key_unknown && key == teclaPatada_ && !pateando_) {
        pateando_     = true;
        anguloPatada_ = 60.0f;
        timerPatada_->start(300);
        update();
    }
}

void Jugador::keyRelease(Qt::Key key) {
    if (key == teclaIzq_) presIzq_ = false;
    if (key == teclaDer_) presDer_ = false;
}

void Jugador::actualizar() {
    if (!activo_) return;

    float velReal = velocidadBase_ * modificadorVelocidad_;
    float dx = 0.0f;
    if (presIzq_) dx -= velReal;
    if (presDer_) dx += velReal;
    if (enPanico_) dx += (float)(rand() % 5 - 2);

    if (!enSuelo_) vy_ += GRAVEDAD;
    float nuevoY = y_ + vy_;
    if (nuevoY >= suelo_) { nuevoY = suelo_; vy_ = 0.0f; enSuelo_ = true; }

    if (!pateando_ && qAbs(anguloPatada_) > 0.5f) {
        anguloPatada_ *= 0.75f;
        update();
    } else if (!pateando_) {
        anguloPatada_ = 0.0f;
    }

    mover(dx, nuevoY - y_);
    update();
}

void Jugador::mover(float dx, float dy) {
    x_ += dx; y_ += dy; setPos(x_, y_);
}

void Jugador::contacto(Balon *balon) {
    float velReal = velocidadBase_ * modificadorVelocidad_;
    float impX = 0.0f;
    if (presIzq_) impX = -velReal * 1.5f;
    if (presDer_) impX =  velReal * 1.5f;
    if (!presIzq_ && !presDer_) impX = (x_ < 400) ? 4.0f : -4.0f;
    if (pateando_) impX *= 2.0f;
    float impY = enSuelo_ ? -6.0f : vy_ * 1.2f;
    balon->lanzar(impX, impY);

    float v2 = balon->getVx()*balon->getVx() + balon->getVy()*balon->getVy();
    if (v2 > 100.0f && !enPanico_) { enPanico_ = true; timerPanico_->start(3000); }
}

void Jugador::activarTurboCafeina() { modificadorVelocidad_ = 2.0f; timerTurbo_->start(5000); }
void Jugador::desactivarTurbo()     { modificadorVelocidad_ = 1.0f; }
void Jugador::desactivarPanico()    { enPanico_ = false; }
void Jugador::detenerPatada()       { pateando_ = false; update(); }

void Jugador::reiniciar() {
    enSuelo_ = true; vy_ = 0.0f;
    modificadorVelocidad_ = 1.0f; enPanico_ = false;
    pateando_ = false; anguloPatada_ = 0.0f;
    presIzq_ = presDer_ = presSalto_ = false;
    timerTurbo_->stop(); timerPanico_->stop(); timerPatada_->stop();
    activo_ = true; update();
}
