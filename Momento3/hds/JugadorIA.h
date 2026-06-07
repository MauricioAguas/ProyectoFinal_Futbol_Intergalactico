#ifndef JUGADORIA_H
#define JUGADORIA_H

#include "Personaje.h"
#include "Balon.h"
#include <QTimer>
#include <QList>
#include <QColor>
#include <QPixmap>

// Representa al jugador controlado por la inteligencia artificial.
// Implementa dos modos de comportamiento:
//   - Nivel1: se mueve con gravedad (futbol clasico), usa lerp por eje.
//   - Nivel2 (hockey): usa maquina de estados PORTERO/ATACANTE + steering Arrive.
// Ademas incorpora aprendizaje basico: registra el historial de posicion del balon
// para predecir su tendencia y ajustar el modificador de reaccion.
class JugadorIA : public Personaje {
    Q_OBJECT

public:
    // nombre    : nombre del personaje IA
    // velocidad : velocidad base en pixeles/frame
    // xArco     : coordenada X del arco que debe defender (referencia para PORTERO)
    explicit JugadorIA(const QString &nombre,
                       float velocidad,
                       float xArco,
                       QGraphicsItem *parent = nullptr);
    ~JugadorIA();

    void mover(float dx, float dy) override;
    void contacto(Balon *balon)    override;
    void actualizar()              override;  // solo Nivel1 (gravedad)
    void reiniciar()               override;

    QRectF boundingRect() const override;
    void   paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget = nullptr) override;

    // Actualiza la percepcion del entorno: posicion del balon y del rival
    void percibir(float balonX, float balonY,
                  float jugadorX, float jugadorY);

    // Avisa que acaban de marcar un gol; reinicia variables de defensa
    void alertarGol();

    // Nivel2: calcula dx,dy con maquina de estados + Arrive (sin gravedad)
    void calcularMovHockey(float &outDx, float &outDy);
    // Nivel2: aplica el movimiento y respeta los limites del campo de hockey
    void moverHockey(float dx, float dy, float limIzq, float limDer,
                     float limTop, float limBot);

    // Nivel1: calcula solo el delta horizontal usando lerp
    float calcularDx();
    // Nivel1: calcula solo el delta vertical usando lerp
    float calcularDy();

    // Modo hockey: desactiva zapato y gravedad en paint/actualizar
    void setModoHockey(bool hockey) { modoHockey_ = hockey; }

    // Limita el movimiento horizontal de la IA dentro del campo
    void setLimites(float xMin, float xMax) { xMin_ = xMin; xMax_ = xMax; }
    // Referencia al jugador humano para tomar decisiones de ataque/defensa
    void setOtroJugador(Personaje *otro)    { otroJugador_ = otro; }
    // Define la coordenada Y del suelo (Nivel1)
    void setSuelo(float suelo)              { suelo_ = suelo; }

private:
    bool modoHockey_ = false; // true en Nivel2 (sin gravedad)

    float balonX_, balonY_; // Ultima posicion percibida del balon
    float rivalX_, rivalY_; // Ultima posicion percibida del rival humano
    bool  balonVisible_;    // true si el balon esta dentro del rango de percepcion

    // Estados de la maquina de estados para el modo hockey
    enum EstadoHockey { PORTERO, ATACANTE };
    EstadoHockey estadoHockey_; // Estado actual de la IA en modo hockey

    // Umbrales de distancia para cambiar de estado PORTERO <-> ATACANTE
    static constexpr float DIST_ATAQUE   = 280.0f; // Si el balon esta mas cerca, ataca
    static constexpr float DIST_DEFENSA  = 350.0f; // Si el balon se aleja, defiende
    static constexpr float OFFSET_PORTERO = 60.0f; // Distancia al poste al defender
    static constexpr float ARRIVE_SLOW_R  = 80.0f; // Radio de frenado del steering Arrive

    float objetivoX_, objetivoY_; // Coordenadas del objetivo actual de movimiento
    bool  debeAtacar_;            // true cuando la IA decide atacar el balon
    bool  defendiendo_;           // true mientras ejecuta maniobra defensiva
    int   framesDefensa_;         // Contador de frames restantes en defensa forzada
    static constexpr int FRAMES_DEFENSA = 120; // Duracion minima de un ciclo defensivo
    void  razonar(); // Evalua el estado del juego y actualiza objetivoX_/Y_

    float xArco_;        // Coordenada X del arco propio (referencia de porteria)
    bool  enSuelo_;      // true si la IA toca el suelo (Nivel1)
    float vy_;           // Velocidad vertical actual (Nivel1)
    float suelo_;        // Coordenada Y del suelo (Nivel1)
    float velActualX_;   // Velocidad horizontal suavizada (Nivel2)
    float velActualY_;   // Velocidad vertical suavizada (Nivel2)
    int   cooldownSalto_;// Frames restantes antes de poder saltar de nuevo (Nivel1)

    // Parametros fisicos de la IA en Nivel1
    static constexpr float GRAVEDAD              = 0.5f;  // Gravedad por frame
    static constexpr float IMPULSO_SALTO         = -9.0f; // Velocidad inicial de salto
    static constexpr float MIN_DIST_SALTO        = 80.0f; // Distancia minima al balon para saltar
    static constexpr float DIST_H_SALTO          = 120.0f;// Distancia horizontal que activa el salto
    static constexpr int   FRAMES_COOLDOWN_SALTO = 90;    // Cooldown entre saltos consecutivos

    // Dimensiones de los sprites de la IA
    static constexpr float ANCHO_SPRITE  = 48.0f;
    static constexpr float ALTO_SPRITE   = 64.0f;
    static constexpr float ANCHO_ZAPATO  = 32.0f;
    static constexpr float ALTO_ZAPATO   = 20.0f;
    static constexpr float ZAP_PIVOT_X   = 8.0f;
    static constexpr float ZAP_PIVOT_Y   = 0.0f;

    float xMin_ =    0.0f; // Limite izquierdo del area de movimiento
    float xMax_ = 800.0f;  // Limite derecho del area de movimiento

    Personaje *otroJugador_ = nullptr; // Puntero al jugador humano

    QPixmap pixPersonaje_; // Sprite del personaje IA
    QPixmap pixZapato_;    // Sprite del zapato animado de la IA

    // Historial de posiciones del balon para el modulo de aprendizaje
    QList<float> historialBalonX_;    // Ultimas posiciones X registradas del balon
    float        tendenciaRival_;     // Direccion estimada de movimiento del rival
    float        modificadorReaccion_;// Factor que ajusta la velocidad de respuesta
    void         aprender(); // Analiza historial y actualiza tendencia y modificador

    QTimer *timerDecision_; // Temporizador que dispara cicloDecision cada N ms

private slots:
    // Reevalua el estado y actualiza objetivos periodicamente
    void cicloDecision();
};

#endif // JUGADORIA_H
