#ifndef NIVEL2_H
#define NIVEL2_H

#include "Nivel.h"
#include <QList>

class Arco;

/*
 Nivel 2 — Vista cenital (hockey con paredes)
 - Sin gravedad: el balon rebota en las 4 paredes como en hockey.
 - Los jugadores se mueven en las 4 direcciones.
 - Fisica de rebote: vx = -vx / vy = -vy al tocar borde.
 - Rozamiento: velocidad del balon *= ROZAMIENTO cada tick.
 - Fisica adicional: oscilacion sinusoidal de obstaculos centrales
   que perturban la trayectoria del balon (aumenta dificultad).
*/
class Nivel2 : public Nivel {
    Q_OBJECT

public:
    explicit Nivel2(ModoJuego modo, QObject *parent = nullptr);
    ~Nivel2();

    void inicializar() override;

protected:
    void keyPressEvent(QKeyEvent *event)   override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    // Obstaculos oscilantes (fisica sinusoidal — Momento 1)
    struct Obstaculo {
        QGraphicsEllipseItem *item;
        float xBase;    // posicion central del oscilador
        float yBase;
        float amplitud;
        float frecuencia;
        float fase;
    };
    QList<Obstaculo> obstaculos_;
    float tiempoOsc_;  // tiempo acumulado para la oscilacion

    void actualizarObstaculos();  // llamado en tickJuego via override
    void crearObstaculos();
};

#endif // NIVEL2_H
