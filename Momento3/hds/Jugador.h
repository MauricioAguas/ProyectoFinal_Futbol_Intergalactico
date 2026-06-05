#ifndef JUGADOR_H
#define JUGADOR_H

#include "Personaje.h"
#include "Balon.h"
#include <QObject>
#include <QKeyEvent>
#include <QTimer>
#include <QColor>

/*
 Jugador controlado por teclado.
 Se dibuja como un circulo de cabeza grande (cabezon) con
 un cuerpo pequeño debajo, usando formas Qt puras.
 Cuando se tengan los sprites se reemplaza solo el paint().

 Caracteristicas de personalidad:
  - Turbo-cafeina : duplica velocidad 5 segundos.
  - Panico espacial: movimiento erratico 3 segundos al recibir
    un balon a alta velocidad.
*/
class Jugador : public Personaje {
    Q_OBJECT

public:
    explicit Jugador(const QString &nombre,
                     float velocidad,
                     Qt::Key teclaIzq,
                     Qt::Key teclaDer,
                     Qt::Key teclaSalto,
                     const QColor &color = Qt::cyan,
                     QGraphicsItem *parent = nullptr);
    ~Jugador();

    // --- Personaje interface ---
    void mover(float dx, float dy) override;
    void contacto(Balon *balon)    override;
    void actualizar()              override;
    void reiniciar()               override;

    // --- QGraphicsItem interface ---
    QRectF boundingRect() const override;
    void   paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget = nullptr) override;

    // --- Control de teclado ---
    void keyPress(Qt::Key key);
    void keyRelease(Qt::Key key);

    bool estaEnElSuelo() const { return enSuelo_; }
    void activarTurboCafeina();

private slots:
    void desactivarTurbo();
    void desactivarPanico();

private:
    Qt::Key teclaIzq_;
    Qt::Key teclaDer_;
    Qt::Key teclaSalto_;
    bool presIzq_;
    bool presDer_;
    bool presSalto_;

    float vy_;
    bool  enSuelo_;
    float suelo_;
    static constexpr float GRAVEDAD      = 0.5f;
    static constexpr float IMPULSO_SALTO = -12.0f;

    float  velocidadBase_;
    float  modificadorVelocidad_;
    QTimer *timerTurbo_;

    bool   enPanico_;
    QTimer *timerPanico_;

    QColor color_;     // color del cabezon (diferencia jugador 1 y 2)

    // Dimensiones del cabezon (formas Qt)
    static constexpr float RADIO_CABEZA = 24.0f;
    static constexpr float ANCHO_CUERPO = 18.0f;
    static constexpr float ALTO_CUERPO  = 20.0f;
};

#endif // JUGADOR_H
