#include "hds/Jugador.h"
#include <QPainter>
#include <cstdlib>

Jugador::Jugador(const QString &nombre,
                 float velocidad,
                 Qt::Key teclaIzq,
                 Qt::Key teclaDer,
                 Qt::Key teclaSalto,
                 const QColor &color,
                 QGraphicsItem *parent)
    : Personaje(nombre, 3, velocidad, parent),
      teclaIzq_(teclaIzq), teclaDer_(teclaDer), teclaSalto_(teclaSalto),
      presIzq_(false), presDer_(false), presSalto_(false),
      vy_(0.0f), enSuelo_(true), suelo_(400.0f),
      velocidadBase_(velocidad), modificadorVelocidad_(1.0f),
      enPanico_(false), color_(color)
{
    timerTurbo_  = new QTimer(this);
    timerPanico_ = new QTimer(this);
    timerTurbo_->setSingleShot(true);
    timerPanico_->setSingleShot(true);
    connect(timerTurbo_,  &QTimer::timeout, this, &Jugador::desactivarTurbo);
    connect(timerPanico_, &QTimer::timeout, this, &Jugador::desactivarPanico);
}

Jugador::~Jugador() {}

// ---------------------------------------------------------------------------
// Forma y dibujo — cabezon: circulo grande + rectangulo cuerpo
// ---------------------------------------------------------------------------
QRectF Jugador::boundingRect() const {
    // Abarca cabeza + cuerpo
    float totalAlto = RADIO_CABEZA * 2 + ALTO_CUERPO;
    float totalAncho = qMax(RADIO_CABEZA * 2, ANCHO_CUERPO);
    return QRectF(-totalAncho / 2, -RADIO_CABEZA * 2, totalAncho, totalAlto);
}

void Jugador::paint(QPainter *painter,
                    const QStyleOptionGraphicsItem *,
                    QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);

    // Cuerpo (rectangulo debajo de la cabeza)
    painter->setBrush(color_.darker(130));
    painter->setPen(Qt::NoPen);
    painter->drawRect(QRectF(-ANCHO_CUERPO / 2, 0, ANCHO_CUERPO, ALTO_CUERPO));

    // Cabeza (circulo grande — estilo cabezon)
    QColor colorCabeza = enPanico_ ? Qt::red : color_;
    if (modificadorVelocidad_ > 1.0f) colorCabeza = Qt::yellow;  // turbo
    painter->setBrush(colorCabeza);
    painter->setPen(QPen(Qt::white, 1.5));
    painter->drawEllipse(QRectF(-RADIO_CABEZA, -RADIO_CABEZA * 2,
                                RADIO_CABEZA * 2, RADIO_CABEZA * 2));

    // Ojos (dos puntos blancos)
    painter->setBrush(Qt::white);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(QRectF(-10, -RADIO_CABEZA * 1.6f, 7, 7));
    painter->drawEllipse(QRectF(3,   -RADIO_CABEZA * 1.6f, 7, 7));

    // Nombre debajo
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 7));
    painter->drawText(QRectF(-24, ALTO_CUERPO + 2, 48, 12),
                      Qt::AlignCenter, nombre_);
}

// ---------------------------------------------------------------------------
// Control de teclado
// ---------------------------------------------------------------------------
void Jugador::keyPress(Qt::Key key) {
    if (key == teclaIzq_)  presIzq_  = true;
    if (key == teclaDer_)  presDer_  = true;
    if (key == teclaSalto_ && enSuelo_) {
        vy_      = IMPULSO_SALTO;
        enSuelo_ = false;
    }
}

void Jugador::keyRelease(Qt::Key key) {
    if (key == teclaIzq_) presIzq_ = false;
    if (key == teclaDer_) presDer_ = false;
}

// ---------------------------------------------------------------------------
// actualizar
// ---------------------------------------------------------------------------
void Jugador::actualizar() {
    if (!activo_) return;

    float velReal = velocidadBase_ * modificadorVelocidad_;
    float dx = 0.0f;
    if (presIzq_) dx -= velReal;
    if (presDer_) dx += velReal;

    if (enPanico_)
        dx += (float)(rand() % 5 - 2);

    if (!enSuelo_) vy_ += GRAVEDAD;

    float nuevoY = y_ + vy_;
    if (nuevoY >= suelo_) {
        nuevoY   = suelo_;
        vy_      = 0.0f;
        enSuelo_ = true;
    }

    mover(dx, nuevoY - y_);
    update();  // forzar repaint de la forma
}

void Jugador::mover(float dx, float dy) {
    x_ += dx;
    y_ += dy;
    setPos(x_, y_);
}

// ---------------------------------------------------------------------------
// contacto
// ---------------------------------------------------------------------------
void Jugador::contacto(Balon *balon) {
    float velReal = velocidadBase_ * modificadorVelocidad_;

    float impX = 0.0f;
    if (presIzq_) impX = -velReal * 1.5f;
    if (presDer_) impX =  velReal * 1.5f;
    if (!presIzq_ && !presDer_) impX = (x_ < 400) ? 4.0f : -4.0f;

    float impY = enSuelo_ ? -6.0f : vy_ * 1.2f;
    balon->lanzar(impX, impY);

    float velBalon = balon->getVx() * balon->getVx() + balon->getVy() * balon->getVy();
    if (velBalon > 100.0f && !enPanico_) {
        enPanico_ = true;
        timerPanico_->start(3000);
    }
}

// ---------------------------------------------------------------------------
// Habilidades
// ---------------------------------------------------------------------------
void Jugador::activarTurboCafeina() {
    modificadorVelocidad_ = 2.0f;
    timerTurbo_->start(5000);
}

void Jugador::desactivarTurbo()  { modificadorVelocidad_ = 1.0f; }
void Jugador::desactivarPanico() { enPanico_ = false; }

// ---------------------------------------------------------------------------
void Jugador::reiniciar() {
    enSuelo_              = true;
    vy_                   = 0.0f;
    modificadorVelocidad_ = 1.0f;
    enPanico_             = false;
    presIzq_ = presDer_   = presSalto_ = false;
    timerTurbo_->stop();
    timerPanico_->stop();
    activo_ = true;
    update();
}
