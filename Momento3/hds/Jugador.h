#ifndef JUGADOR_H
#define JUGADOR_H

#include "Personaje.h"
#include "Balon.h"
#include <QObject>
#include <QKeyEvent>
#include <QTimer>

/*
 Jugador controlado por teclado.
 Representa al cabezon en pantalla (Fry o Bender segun lado).

 Caracteristicas de personalidad (Momento 1):
  - Turbo-cafeina : al activarse duplica velocidad por 5 segundos.
  - Panico espacial: al recibir contacto con el balon a alta velocidad
    el movimiento se vuelve erratico por 3 segundos.

 Mecanica de cabezon:
  - Puede moverse horizontalmente.
  - Puede saltar (fisica parabolica: vy inicial negativa + gravedad).
  - Al tocar el balon con la cabeza/cuerpo aplica un impulso
    proporcional a su velocidad y direccion actual.
*/
class Jugador : public Personaje {
    Q_OBJECT

public:
    // teclaIzq/teclaDer/teclaSalto: codigos Qt::Key asignados a este jugador
    explicit Jugador(const QString &nombre,
                     float velocidad,
                     Qt::Key teclaIzq,
                     Qt::Key teclaDer,
                     Qt::Key teclaSalto,
                     QGraphicsItem *parent = nullptr);
    ~Jugador();

    // --- Personaje interface ---
    void mover(float dx, float dy) override;
    void contacto(Balon *balon)    override;  // aplica impulso al balon
    void actualizar()              override;  // fisica de salto + movimiento
    void reiniciar()               override;

    // --- Control de teclado (llamado por el Nivel) ---
    void keyPress(Qt::Key key);
    void keyRelease(Qt::Key key);

    // --- Salto ---
    bool estaEnElSuelo() const { return enSuelo_; }

    // --- Habilidades de personalidad ---
    void activarTurboCafeina();    // duplica velocidad 5 s

private slots:
    void desactivarTurbo();        // slot del QTimer turbo
    void desactivarPanico();       // slot del QTimer panico

private:
    // Teclas asignadas
    Qt::Key teclaIzq_;
    Qt::Key teclaDer_;
    Qt::Key teclaSalto_;

    // Estado de teclas presionadas
    bool presIzq_;
    bool presDer_;
    bool presSalto_;

    // Fisica de salto
    float vy_;          // velocidad vertical actual
    bool  enSuelo_;     // true si esta pisando el suelo
    float suelo_;       // coordenada Y del suelo para este jugador

    static constexpr float GRAVEDAD      = 0.5f;   // px/tick^2
    static constexpr float IMPULSO_SALTO = -12.0f; // vy inicial al saltar

    // Personalidad: turbo-cafeina
    float  velocidadBase_;          // velocidad sin modificador
    float  modificadorVelocidad_;   // 1.0 normal / 2.0 turbo
    QTimer *timerTurbo_;

    // Personalidad: panico espacial
    bool   enPanico_;
    QTimer *timerPanico_;
};

#endif // JUGADOR_H
