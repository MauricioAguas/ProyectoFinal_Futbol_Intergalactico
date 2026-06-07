#ifndef JUGADOR_H
#define JUGADOR_H

#include "Personaje.h"
#include "Balon.h"
#include <QKeyEvent>
#include <QTimer>
#include <QColor>
#include <QPixmap>

// Representa al jugador controlado por el humano.
// Recibe entradas de teclado, aplica fisica de salto y ejecuta patadas.
// Soporta el power-up Turbo Cafeina que incrementa la velocidad temporalmente.
class Jugador : public Personaje {
    Q_OBJECT

public:
    // nombre        : nombre del personaje (ej. "Fry")
    // velocidad     : pixeles por frame de desplazamiento horizontal
    // teclaIzq/Der  : teclas de movimiento horizontal
    // teclaSalto    : tecla de salto
    // color         : color de respaldo si no hay sprite cargado
    // spritePersonaje/Zapato : rutas a los pixmaps del personaje y su zapato
    // reflejar      : true para el jugador 2 (se espeja horizontalmente)
    explicit Jugador(const QString &nombre,
                     float velocidad,
                     Qt::Key teclaIzq,
                     Qt::Key teclaDer,
                     Qt::Key teclaSalto,
                     const QColor &color             = QColor(100, 200, 255),
                     const QString &spritePersonaje  = "",
                     const QString &spriteZapato     = "",
                     bool  reflejar                  = false,
                     QGraphicsItem *parent           = nullptr);
    ~Jugador();

    // Aplica dx a la posicion horizontal respetando los limites del campo
    void mover(float dx, float dy) override;
    // Calcula el angulo y fuerza de patada al balon segun posicion relativa
    void contacto(Balon *balon)    override;
    // Ejecutado cada frame: gravedad, movimiento horizontal y animacion de patada
    void actualizar()              override;
    // Vuelve al jugador a su posicion y estado de inicio de partido
    void reiniciar()               override;

    QRectF       boundingRect() const override;
    QPainterPath shape()        const override;
    void         paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

    // Registra la tecla presionada (llamado desde el evento de escena)
    void keyPress(Qt::Key key);
    // Libera la tecla (llamado desde el evento de escena)
    void keyRelease(Qt::Key key);

    // true si el jugador esta apoyado sobre el suelo (no puede saltar de nuevo)
    bool estaEnElSuelo() const { return enSuelo_; }
    // Define la coordenada Y del suelo para este jugador
    void setSuelo(float y)     { suelo_ = y; }
    // Activa el power-up Turbo Cafeina durante un tiempo limitado
    void activarTurboCafeina();
    // Asigna la tecla de patada (separada del movimiento)
    void setTeclaPatada(Qt::Key k) { teclaPatada_ = k; }
    // Limita el desplazamiento horizontal entre xMin y xMax
    void setLimites(float xMin, float xMax) { xMin_ = xMin; xMax_ = xMax; }
    // Referencia al otro jugador para calcular colisiones de cuerpo
    void setOtroJugador(Personaje *otro)    { otroJugador_ = otro; }

private slots:
    void desactivarTurbo();  // Slot: expira el timer del Turbo Cafeina
    void desactivarPanico(); // Slot: expira el estado de panico del jugador
    void detenerPatada();    // Slot: finaliza la animacion de patada

private:
    // Teclas de control asignadas al jugador
    Qt::Key teclaIzq_, teclaDer_, teclaSalto_, teclaPatada_;
    // Estado actual de las teclas (presionada = true)
    bool    presIzq_, presDer_, presSalto_;

    float anguloPatada_; // Angulo actual de la animacion de patada (grados)
    bool  pateando_;     // true mientras se ejecuta la animacion de patada

    float vy_;     // Velocidad vertical actual (positivo = hacia abajo)
    bool  enSuelo_;// true si el jugador toca el suelo este frame
    float suelo_;  // Coordenada Y del suelo asignada por el nivel

    float xMin_  =    0.0f; // Limite izquierdo de movimiento
    float xMax_  = 800.0f;  // Limite derecho de movimiento

    Personaje *otroJugador_ = nullptr; // Puntero al otro jugador (para bodyblock)

    // Constantes fisicas y geometricas del jugador
    static constexpr float GRAVEDAD      = 0.5f;   // Aceleracion gravitacional
    static constexpr float IMPULSO_SALTO = -12.0f; // Velocidad vertical inicial al saltar
    static constexpr float RADIO_CABEZA  = 24.0f;  // Radio del circulo de la cabeza
    static constexpr float ANCHO_CUERPO  = 18.0f;  // Ancho del rectangulo del cuerpo
    static constexpr float ALTO_CUERPO   = 20.0f;  // Alto del rectangulo del cuerpo
    static constexpr float ALTO_SPRITE   = 72.0f;  // Alto del sprite del personaje
    static constexpr float ANCHO_SPRITE  = 48.0f;  // Ancho del sprite del personaje
    static constexpr float ANCHO_ZAPATO  = 32.0f;  // Ancho del sprite del zapato
    static constexpr float ALTO_ZAPATO   = 22.0f;  // Alto del sprite del zapato
    static constexpr float ZAP_PIVOT_X   = -4.0f;  // Offset X del pivot del zapato
    static constexpr float ZAP_PIVOT_Y   =  2.0f;  // Offset Y del pivot del zapato

    float  velocidadBase_;         // Velocidad sin modificadores
    float  modificadorVelocidad_;  // Multiplicador activo (1.0 normal, >1 turbo)
    QTimer *timerTurbo_;           // Temporizador de duracion del Turbo Cafeina
    QTimer *timerPatada_;          // Temporizador de duracion de la animacion de patada

    bool   enPanico_;    // true cuando el jugador recibe un impacto fuerte
    QTimer *timerPanico_;// Temporizador de duracion del estado de panico

    QColor  color_;        // Color de respaldo para dibujo sin sprite
    QPixmap pixPersonaje_; // Sprite del personaje
    QPixmap pixZapato_;    // Sprite del zapato animado
    bool    reflejar_;     // true para el jugador 2 (espejo horizontal)
};

#endif // JUGADOR_H
