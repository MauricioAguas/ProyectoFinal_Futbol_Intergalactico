#ifndef NIVEL2_H
#define NIVEL2_H

#include "Nivel.h"
#include <QList>
#include <QGraphicsTextItem>

class Arco;
class Jugador;

// Implementa el modo de hockey intergalactico (Nivel2).
// Sin gravedad: el balon y los jugadores se desplazan en un plano 2D.
// Usa un campo circular (cancha) con limites definidos por canX_/canY_/canW_/canH_.
// La IA opera en modo hockey con maquina de estados PORTERO/ATACANTE + steering Arrive.
// El nivel sobreescribe tickJuego() con implementacion vacia y usa tickHockey() propio.
class Nivel2 : public Nivel {
    Q_OBJECT

public:
    explicit Nivel2(ModoJuego modo, QObject *parent = nullptr);
    ~Nivel2();

    // Construye la escena: fondo circular, arcos, jugadores y balon sin gravedad
    void inicializar() override;

protected:
    // Reparte eventos de teclado a jugador1_ y jugador2_
    void keyPressEvent(QKeyEvent *event)   override;
    void keyReleaseEvent(QKeyEvent *event) override;
    // Verifica si el balon cruzo alguna porteria usando la logica de hockey
    void verificarGol() override;
    // Sobreescribe el tick base; la logica se maneja en tickHockey()
    void tickJuego() override {}

private slots:
    // Refresca HUD; gol=-1 actualiza todo, gol>=0 actualiza solo el marcador
    void actualizarHUD(int gol = -1);

private:
    // Orden igual al de inicializacion en el constructor (evita -Wreorder)
    float tiempoOsc_;    // Contador para efectos de oscilacion visual del campo
    // Velocidades actuales de cada jugador en modo hockey (sin gravedad)
    float j1vx_, j1vy_, j2vx_, j2vy_;
    // Estado de teclas de movimiento del jugador 1 (arriba/abajo/izq/der)
    bool  j1arr_, j1aba_, j1izq_, j1der_;
    // Estado de teclas de movimiento del jugador 2
    bool  j2arr_, j2aba_, j2izq_, j2der_;
    bool  colisionJ1_; // true si jugador1 colisiono con el balon este frame
    bool  colisionJ2_; // true si jugador2 colisiono con el balon este frame
    int   ultimoToco_; // Indice (0 o 1) del ultimo jugador que toco el balon
    int   combos_;     // Contador de toques consecutivos del mismo jugador

    // Geometria de la cancha circular
    float canX_, canY_; // Esquina superior izquierda del rectangulo de la cancha
    float canW_, canH_; // Ancho y alto de la cancha
    float centroY_;     // Centro vertical de la cancha
    float spawnIzq_;    // Posicion X de spawn del jugador izquierdo
    float spawnDer_;    // Posicion X de spawn del jugador derecho

    QGraphicsTextItem *marcador_     = nullptr; // Texto del marcador en pantalla
    QGraphicsTextItem *temporizador_ = nullptr; // Texto del cronometro en pantalla

    // Loop principal del Nivel2: mueve jugadores, balon y verifica colisiones
    void tickHockey();
    // Evita que dos jugadores se solapen (empuje mutuo)
    void resolverBodyblock();
    // Retorna true si el jugador esta lo suficientemente cerca del balon para golpearlo
    bool cercaDelBalon(Personaje *jugador);
    // Aplica impulso al balon segun la velocidad relativa del jugador al impactar
    void colisionHockey(Personaje *jugador, float dvx, float dvy, int jugadorId);
};

#endif // NIVEL2_H
