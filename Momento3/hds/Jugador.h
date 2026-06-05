#ifndef JUGADOR_H
#define JUGADOR_H

#include "Personaje.h"
#include "Balon.h"
#include <QKeyEvent>
#include <QTimer>
#include <QColor>

/*
 Jugador controlado por teclado.
 Se dibuja como un cabezon con formas Qt puras.
 Personalidades: Turbo-cafeina y Panico espacial.
*/
class Jugador : public Personaje {
    Q_OBJECT

public:
    explicit Jugador(const QString &nombre,
                     float velocidad,
                     Qt::Key teclaIzq,
                     Qt::Key teclaDer,
                     Qt::Key teclaSalto,
                     const QColor &color = QColor(100, 200, 255),
                     QGraphicsItem *parent = nullptr);
    ~Jugador();

    void mover(float dx, float dy) override;
    void contacto(Balon *balon)    override;
    void actualizar()              override;
    void reiniciar()               override;

    QRectF boundingRect() const override;
    void   paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget = nullptr) override;

    void keyPress(Qt::Key key);
    void keyRelease(Qt::Key key);

    bool estaEnElSuelo() const { return enSuelo_; }
    void activarTurboCafeina();

private slots:
    void desactivarTurbo();
    void desactivarPanico();

private:
    Qt::Key teclaIzq_, teclaDer_, teclaSalto_;
    bool    presIzq_, presDer_, presSalto_;

    float vy_;
    bool  enSuelo_;
    float suelo_;

    static constexpr float GRAVEDAD      = 0.5f;
    static constexpr float IMPULSO_SALTO = -12.0f;
    static constexpr float RADIO_CABEZA  = 24.0f;
    static constexpr float ANCHO_CUERPO  = 18.0f;
    static constexpr float ALTO_CUERPO   = 20.0f;

    float  velocidadBase_;
    float  modificadorVelocidad_;
    QTimer *timerTurbo_;

    bool   enPanico_;
    QTimer *timerPanico_;

    QColor color_;
};

#endif // JUGADOR_H
