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
    // Sobreescribir tickJuego para que no ejecute la logica del Nivel base
    // (gravedad, actualizar, etc.) ya que Nivel2 tiene su propio tickHockey
    void tickJuego()  {}

private slots:
    void actualizarHUD(int gol = -1);

private:
    float canX_ = 90.0f;
    float canY_ = 60.0f;
    float canW_ = 620.0f;
    float canH_ = 460.0f;
    float centroY_  = 290.0f;
    float spawnIzq_ = 220.0f;
    float spawnDer_ = 555.0f;

    float j1vx_, j1vy_, j2vx_, j2vy_;
    bool j1arr_, j1aba_, j1izq_, j1der_;
    bool j2arr_, j2aba_, j2izq_, j2der_;

    bool colisionJ1_;
    bool colisionJ2_;
    int  ultimoToco_;
    int  combos_;

    float tiempoOsc_;

    QGraphicsTextItem *marcador_     = nullptr;
    QGraphicsTextItem *temporizador_ = nullptr;

    void tickHockey();
    void resolverBodyblock();
    bool cercaDelBalon(Personaje *jugador);
    void colisionHockey(Personaje *jugador, float dvx, float dvy, int jugadorId);
};

#endif // NIVEL2_H
