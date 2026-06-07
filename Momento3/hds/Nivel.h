#ifndef NIVEL_H
#define NIVEL_H

#include <QGraphicsScene>
#include <QObject>
#include <QTimer>
#include <QKeyEvent>
#include "Jugador.h"
#include "JugadorIA.h"
#include "Personaje.h"
#include "Balon.h"
#include "Arco.h"

// Clase base abstracta de todos los niveles del juego.
// Hereda de QGraphicsScene para ser mostrada directamente en GameView.
// Gestiona el loop de juego (timerFrame_ a ~60 fps), el marcador, el
// temporizador de partido y emite senales hacia Game para cambios de pantalla.
class Nivel : public QGraphicsScene {
    Q_OBJECT

public:
    // Modos de juego disponibles al iniciar un nivel
    enum ModoJuego { VS_HUMANO, VS_MAQUINA };

    explicit Nivel(ModoJuego modo, QObject *parent = nullptr);
    virtual ~Nivel();

    // Crea y posiciona todos los actores; implementado por cada subclase
    virtual void inicializar() = 0;

    // Consultas de estado del partido
    int  getGoles(int jugador) const { return goles_[jugador]; }
    int  getTiempoRestante()   const { return tiempoRestante_; }
    bool estaActivo()          const { return activo_; }
    bool estaPausado()         const { return pausado_; }
    ModoJuego getModo()        const { return modo_; }

    // Detiene el timerFrame_ y marca el partido como pausado
    void pausa();
    // Reanuda el timerFrame_ y desmarca la pausa
    void reanudar();

    // Permite a subclases desconectar tickJuego del timerFrame_ sin violar acceso
    void desconectarTickJuego();

signals:
    void golAnotado(int jugador);    // Emitido cuando se registra un gol
    void tiempoAgotado();            // Emitido cuando el temporizador llega a 0
    void nivelTerminado();           // Emitido al finalizar el nivel
    void pausaToggled(bool pausado); // Emitido al cambiar estado de pausa
    void resultadoFinal(int ganador);// Emitido con el indice del ganador (0 o 1)

protected slots:
    // Ejecutado cada frame (~60 fps): mueve entidades y verifica colisiones
    virtual void tickJuego();
    // Ejecutado cada segundo: decrementa tiempoRestante_
    void tickTimer();

protected:
    Jugador   *jugador1_; // Jugador humano (siempre presente)
    Personaje *jugador2_; // Jugador humano o IA segun el modo de juego
    Balon     *balon_;    // El balon del partido
    Arco      *arcoIzq_;  // Porteria izquierda (PLANET_EXPRESS)
    Arco      *arcoDer_;  // Porteria derecha (OMICRON_XI)

    ModoJuego modo_;          // Modo actual: VS_HUMANO o VS_MAQUINA
    int       goles_[2];      // Contador de goles por jugador [0] y [1]
    int       tiempoRestante_;// Segundos restantes en el partido
    bool      activo_;        // false al terminar el partido
    bool      pausado_;       // true mientras el juego este en pausa

    QTimer *timerFrame_;   // Dispara tickJuego cada ~16 ms (aprox. 60 fps)
    QTimer *timerSegundo_; // Dispara tickTimer cada 1000 ms

    // Revisa si el balon entro en alguno de los dos arcos
    virtual void verificarGol();
    // Revisa colisiones entre el balon y los jugadores
    void verificarContactos();

    // Reenvio de eventos de teclado a los jugadores
    void keyPressEvent(QKeyEvent *event) override;

    int anchoEscena_; // Ancho total de la escena en pixeles
    int altoEscena_;  // Alto total de la escena en pixeles
};

#endif // NIVEL_H
