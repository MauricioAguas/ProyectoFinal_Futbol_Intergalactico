#ifndef NIVEL_H
#define NIVEL_H

#include <QGraphicsScene>
#include <QObject>
#include <QTimer>

class Jugador;
class JugadorIA;
class Balon;
class Arco;

/*
 Clase base abstracta para los niveles del juego.
 Hereda de QGraphicsScene: cada nivel ES la escena Qt.
 Coordina las entidades, detecta colisiones y mantiene
 el marcador y el timer de partido.
*/
class Nivel : public QGraphicsScene {
    Q_OBJECT

public:
    enum ModoJuego { VS_HUMANO, VS_MAQUINA };

    explicit Nivel(ModoJuego modo, QObject *parent = nullptr);
    virtual ~Nivel();

    // Subclases deben implementar la inicializacion propia
    virtual void inicializar() = 0;

    // Getters de estado
    int  getGoles(int jugador) const { return goles_[jugador]; }
    int  getTiempoRestante()   const { return tiempoRestante_; }
    bool estaActivo()          const { return activo_; }

signals:
    void golAnotado(int jugador);   // emitido cuando entra un gol
    void tiempoAgotado();           // emitido cuando el timer llega a 0
    void nivelTerminado();          // emitido al acabar el nivel

protected slots:
    void tickJuego();               // avanza la logica un frame (60 fps)
    void tickTimer();               // descuenta 1 segundo del partido

protected:
    // Entidades compartidas por ambos niveles
    Jugador   *jugador1_;   // siempre humano
    Personaje *jugador2_;   // Jugador (vs humano) o JugadorIA (vs maquina)
    Balon     *balon_;
    Arco      *arcoIzq_;
    Arco      *arcoDer_;

    ModoJuego modo_;
    int       goles_[2];        // goles_[0]=J1, goles_[1]=J2
    int       tiempoRestante_;  // segundos restantes del partido
    bool      activo_;

    QTimer *timerFrame_;    // 60 fps  (16 ms)
    QTimer *timerSegundo_;  // 1 seg   (1000 ms)

    // Detecta si el balon entro en alguno de los arcos
    void verificarGol();

    // Detecta si el balon toco a algun jugador y llama contacto()
    void verificarContactos();

    // Limites del campo (subclases los fijan en inicializar)
    int anchoEscena_;
    int altoEscena_;
};

#endif // NIVEL_H
