#ifndef NIVEL1_H
#define NIVEL1_H

#include "Nivel.h"
#include <QGraphicsTextItem>

// Implementa el modo de futbol clasico con gravedad.
// Escena: campo horizontal, dos jugadores (humano vs humano o humano vs IA),
// balon con fisica parabolica y arcos en los extremos laterales.
// El HUD muestra marcador y temporizador en pantalla.
class Nivel1 : public Nivel {
    Q_OBJECT

public:
    explicit Nivel1(ModoJuego modo, QObject *parent = nullptr);
    ~Nivel1();

    // Construye la escena: fondo, arcos, jugadores y balon
    void inicializar() override;

protected:
    // Reparte los eventos de teclado a jugador1_ y jugador2_
    void keyPressEvent(QKeyEvent *event)   override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    // Refresca marcador y temporizador en pantalla; jugador=-1 actualiza todo
    void actualizarHUD(int jugador = -1);

private:
    QGraphicsTextItem *marcador_;     // Texto "0  -  0" del marcador
    QGraphicsTextItem *temporizador_; // Texto "60s" del cronometro
};

#endif // NIVEL1_H
