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
    Jugador   *jugador1_;
    Personaje *jugador2_;
    Balon     *balon_;
    Arco      *arcoIzq_;
    Arco      *arcoDer_;

    ModoJuego modo_;
    int       goles_[2];
    int       tiempoRestante_;
    bool      activo_;

    QTimer *timerFrame_;
    QTimer *timerSegundo_;

    virtual void verificarGol();   // virtual para que Nivel2 pueda sobreescribirla
    void verificarContactos();

    int anchoEscena_;
    int altoEscena_;
};

#endif // NIVEL_H
