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
    bool estaPausado()         const { return pausado_; }
    ModoJuego getModo()        const { return modo_; }

    void pausa();
    void reanudar();

signals:
    void golAnotado(int jugador);
    void tiempoAgotado();
    void nivelTerminado();
    void pausaToggled(bool pausado);
    // emitido al acabar el tiempo con resultado: 0=j1 gana, 1=j2 gana, -1=empate
    void resultadoFinal(int ganador);

protected slots:
    virtual void tickJuego();
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
    bool      pausado_;

    QTimer *timerFrame_;
    QTimer *timerSegundo_;

    virtual void verificarGol();
    void verificarContactos();

    void keyPressEvent(QKeyEvent *event) override;

    int anchoEscena_;
    int altoEscena_;
};

#endif // NIVEL_H
