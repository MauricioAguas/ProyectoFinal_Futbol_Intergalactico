#ifndef NIVEL1_H
#define NIVEL1_H

#include "Nivel.h"

/*
 Nivel 1 — Vista lateral (cabezon clasico)
 - Campo con gravedad: el balon sigue trayectoria parabolica.
 - Los jugadores saltan para cabecear el balon.
 - Fondo: estadio intergalactico de Futurama.
 - Duracion: 90 segundos (timer de partido).
 - Fisicas: parabola del balon + salto del jugador (ambos con GRAVEDAD).
*/
class Nivel1 : public Nivel {
    Q_OBJECT

public:
    explicit Nivel1(ModoJuego modo, QObject *parent = nullptr);
    ~Nivel1();

    void inicializar() override;

protected:
    void keyPressEvent(QKeyEvent *event)   override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // NIVEL1_H
