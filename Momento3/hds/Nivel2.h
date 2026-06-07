#ifndef NIVEL2_H
#define NIVEL2_H

#include "Nivel.h"
#include <QList>
#include <QGraphicsTextItem>

class Arco;
class Jugador;

class Nivel2 : public Nivel {
    Q_OBJECT

public:
    explicit Nivel2(ModoJuego modo, QObject *parent = nullptr);
    ~Nivel2();

    void inicializar() override;

protected:
    void keyPressEvent(QKeyEvent *event)   override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void verificarGol() override;
    void tickJuego() override {}

private slots:
    void actualizarHUD(int gol = -1);

private:
    // Orden igual al de inicializacion en el constructor (evita -Wreorder)
    float tiempoOsc_;
    float j1vx_, j1vy_, j2vx_, j2vy_;
    bool  j1arr_, j1aba_, j1izq_, j1der_;
    bool  j2arr_, j2aba_, j2izq_, j2der_;
    bool  colisionJ1_;
    bool  colisionJ2_;
    int   ultimoToco_;
    int   combos_;

    float canX_;
    float canY_;
    float canW_;
    float canH_;
    float centroY_;
    float spawnIzq_;
    float spawnDer_;

    QGraphicsTextItem *marcador_     = nullptr;
    QGraphicsTextItem *temporizador_ = nullptr;

    void tickHockey();
    void resolverBodyblock();
    bool cercaDelBalon(Personaje *jugador);
    void colisionHockey(Personaje *jugador, float dvx, float dvy, int jugadorId);
};

#endif // NIVEL2_H
