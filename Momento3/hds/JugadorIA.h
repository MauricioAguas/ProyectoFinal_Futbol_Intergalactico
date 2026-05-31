#ifndef JUGADORIA_H
#define JUGADORIA_H

#include "Personaje.h"
#include "Balon.h"
#include <QTimer>
#include <QList>

/*
 JugadorIA — segundo jugador controlado por la maquina.
 Representa al cabezon rival (Bender) en modo 1 vs Maquina.

 Implementa el agente autonomo del Momento 1:
   a) Percepcion  : detecta posicion del balon dentro de su radio (200 px).
   b) Razonamiento: decide si atacar (ir al balon) o posicionarse (defender arco).
   c) Accion      : se mueve y salta hacia la posicion objetivo.
   d) Aprendizaje : guarda historial de posiciones del balon;
                    si el rival siempre va hacia un lado, aumenta reaccion
                    en esa direccion.

 Personalidad (Momento 1):
   - Calculador : reacciona mas rapido conforme aprende (velocidad crece).
   - Erratico   : ocasionalmente comete errores al predecir (humaniza la IA).
*/
class JugadorIA : public Personaje {
    Q_OBJECT

public:
    explicit JugadorIA(const QString &nombre,
                       float velocidad,
                       float xArco,        // coordenada X del arco que defiende
                       QGraphicsItem *parent = nullptr);
    ~JugadorIA();

    // --- Personaje interface ---
    void mover(float dx, float dy) override;
    void contacto(Balon *balon)    override;  // aplica impulso al balon
    void actualizar()              override;
    void reiniciar()               override;

    // Llamado por el Nivel cada tick para pasarle la info del mundo
    void percibir(float balonX, float balonY,
                  float jugadorX, float jugadorY);

private:
    // a) Percepcion
    float balonX_,  balonY_;    // ultima posicion percibida del balon
    float rivalX_,  rivalY_;    // ultima posicion percibida del rival
    bool  balonVisible_;        // true si el balon esta dentro del radio
    static constexpr float RADIO_PERCEPCION = 250.0f;

    // b) Razonamiento
    float objetivoX_;           // posicion X a la que decidio moverse
    bool  debeAtacar_;          // true=ir al balon, false=defender arco
    void  razonar();            // toma de decision

    // c) Accion
    float xArco_;               // X del arco propio (para posicion defensiva)
    bool  enSuelo_;
    float vy_;
    static constexpr float GRAVEDAD      = 0.5f;
    static constexpr float IMPULSO_SALTO = -12.0f;

    // d) Aprendizaje
    QList<float> historialBalonX_;          // max 20 posiciones guardadas
    float        tendenciaRival_;           // -1=siempre izq, +1=siempre der
    float        modificadorReaccion_;      // crece con el aprendizaje (max 1.5)
    void         aprender();                // actualiza tendencia y modificador

    // Timer para pulso de decision (no decide cada tick, sino cada 300 ms)
    QTimer *timerDecision_;

private slots:
    void cicloDecision();   // percibir + razonar + aprender cada 300 ms
};

#endif // JUGADORIA_H
