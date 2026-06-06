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
    void verificarGol() override;  // respawn en lado del equipo que recibio gol

private slots:
    void actualizarHUD(int gol = -1);

private:
    struct Obstaculo {
        QGraphicsEllipseItem *item;
        float xBase;
        float yBase;
        float amplitud;
        float frecuencia;
        float fase;
    };
    QList<Obstaculo> obstaculos_;
    float tiempoOsc_;

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

    QGraphicsTextItem *marcador_     = nullptr;
    QGraphicsTextItem *temporizador_ = nullptr;

    void crearObstaculos();
    void actualizarObstaculos();
    void tickHockey();
};

#endif // NIVEL2_H
