#ifndef NIVEL_H
#define NIVEL_H

#include <QGraphicsScene>
#include <QObject>
#include <QTimer>
#include "Jugador.h"
#include "JugadorIA.h"
#include "Personaje.h"
#include "Balon.h"
#include "Arco.h"

/*
 Clase base abstracta para los niveles del juego.
 Hereda de QGraphicsScene: cada nivel ES la escena Qt.
*/
class Nivel : public QGraphicsScene {
    Q_OBJECT

public:
    enum ModoJuego { VS_HUMANO, VS_MAQUINA };

    explicit Nivel(ModoJuego modo, QObject *parent = nullptr);
    virtual ~Nivel();

    virtual void inicializar() = 0;

    int  getGoles(int jugador) const { return goles_[jugador]; }
    int  getTiempoRestante()   const { return tiempoRestante_; }
    bool estaActivo()          const { return activo_; }

signals:
    void golAnotado(int jugador);
    void tiempoAgotado();
    void nivelTerminado();

protected slots:
    void tickJuego();
    void tickTimer();

protected:
    Jugador   *jugador1_;    // siempre humano
    Personaje *jugador2_;    // Jugador o JugadorIA segun modo
    Balon     *balon_;
    Arco      *arcoIzq_;
    Arco      *arcoDer_;

    ModoJuego modo_;
    int       goles_[2];
    int       tiempoRestante_;
    bool      activo_;

    QTimer *timerFrame_;
    QTimer *timerSegundo_;

    void verificarGol();
    void verificarContactos();

    int anchoEscena_;
    int altoEscena_;
};

#endif // NIVEL_H
