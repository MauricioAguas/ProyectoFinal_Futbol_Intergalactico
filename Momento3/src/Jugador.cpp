#include "hds/Jugador.h"
#include <cstdlib>   // rand()

Jugador::Jugador(const QString &nombre,
                 float velocidad,
                 Qt::Key teclaIzq,
                 Qt::Key teclaDer,
                 Qt::Key teclaSalto,
                 QGraphicsItem *parent)
    : Personaje(nombre, 3, velocidad, parent),
      teclaIzq_(teclaIzq), teclaDer_(teclaDer), teclaSalto_(teclaSalto),
      presIzq_(false), presDer_(false), presSalto_(false),
      vy_(0.0f), enSuelo_(true), suelo_(400.0f),
      velocidadBase_(velocidad), modificadorVelocidad_(1.0f),
      enPanico_(false)
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
// Control de teclado
// ---------------------------------------------------------------------------
void Jugador::keyPress(Qt::Key key) {
    if (key == teclaIzq_)    presIzq_   = true;
    if (key == teclaDer_)    presDer_   = true;
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
// actualizar — llamado cada tick por el Nivel
// ---------------------------------------------------------------------------
void Jugador::actualizar() {
    if (!activo_) return;

    float velReal = velocidadBase_ * modificadorVelocidad_;

    float dx = 0.0f;
    if (presIzq_) dx -= velReal;
    if (presDer_) dx += velReal;

    // Panico espacial: agrega desplazamiento aleatorio
    if (enPanico_) {
        dx += (float)(rand() % 5 - 2);  // -2 a +2 px extra
    }

    // Fisica de salto: aplicar gravedad
    if (!enSuelo_) {
        vy_ += GRAVEDAD;
    }

    float nuevoY = y_ + vy_;
    if (nuevoY >= suelo_) {
        nuevoY = suelo_;
        vy_    = 0.0f;
        enSuelo_ = true;
    }

    mover(dx, nuevoY - y_);
}

void Jugador::mover(float dx, float dy) {
    x_ += dx;
    y_ += dy;
    setPos(x_, y_);
}

// ---------------------------------------------------------------------------
// contacto — el Nivel llama esto cuando el balon toca al jugador
// Aplica un impulso al balon proporcional a velocidad y direccion del jugador
// ---------------------------------------------------------------------------
void Jugador::contacto(Balon *balon) {
    float velReal = velocidadBase_ * modificadorVelocidad_;

    // Impulso horizontal segun hacia donde se movia
    float impX = 0.0f;
    if (presIzq_) impX = -velReal * 1.5f;
    if (presDer_) impX =  velReal * 1.5f;
    if (!presIzq_ && !presDer_) impX = (x_ < 400) ? 4.0f : -4.0f; // empuje neutro

    // Impulso vertical: si el jugador salta, el balon sube mas
    float impY = enSuelo_ ? -6.0f : vy_ * 1.2f;

    balon->lanzar(impX, impY);

    // Panico: si el balon venia rapido, activar panico espacial
    float velBalon = balon->getVx() * balon->getVx() + balon->getVy() * balon->getVy();
    if (velBalon > 100.0f && !enPanico_) {
        enPanico_ = true;
        timerPanico_->start(3000);  // 3 segundos
    }
}

// ---------------------------------------------------------------------------
// Habilidades de personalidad
// ---------------------------------------------------------------------------
void Jugador::activarTurboCafeina() {
    modificadorVelocidad_ = 2.0f;
    timerTurbo_->start(5000);  // 5 segundos
}

void Jugador::desactivarTurbo() {
    modificadorVelocidad_ = 1.0f;
}

void Jugador::desactivarPanico() {
    enPanico_ = false;
}

// ---------------------------------------------------------------------------
void Jugador::reiniciar() {
    enSuelo_             = true;
    vy_                  = 0.0f;
    modificadorVelocidad_= 1.0f;
    enPanico_            = false;
    presIzq_ = presDer_  = presSalto_ = false;
    timerTurbo_->stop();
    timerPanico_->stop();
    activo_ = true;
}
