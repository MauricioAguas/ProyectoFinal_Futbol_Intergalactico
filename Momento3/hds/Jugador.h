#ifndef JUGADOR_H
#define JUGADOR_H

#include "Personaje.h"
#include "Balon.h"
#include <QKeyEvent>
#include <QTimer>
#include <QColor>
#include <QPixmap>

class Jugador : public Personaje {
    Q_OBJECT

public:
    explicit Jugador(const QString &nombre,
                     float velocidad,
                     Qt::Key teclaIzq,
                     Qt::Key teclaDer,
                     Qt::Key teclaSalto,
                     const QColor &color             = QColor(100, 200, 255),
                     const QString &spritePersonaje  = "",
                     const QString &spriteZapato     = "",
                     bool  reflejar                  = false,
                     QGraphicsItem *parent           = nullptr);
    ~Jugador();

    void mover(float dx, float dy) override;
    void contacto(Balon *balon)    override;
    void actualizar()              override;
    void reiniciar()               override;

    QRectF       boundingRect() const override;
    QPainterPath shape()        const override;
    void         paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

    void keyPress(Qt::Key key);
    void keyRelease(Qt::Key key);

    bool estaEnElSuelo() const { return enSuelo_; }
    void setSuelo(float y)     { suelo_ = y; }
    void activarTurboCafeina();
    void setTeclaPatada(Qt::Key k) { teclaPatada_ = k; }

private slots:
    void desactivarTurbo();
    void desactivarPanico();
    void detenerPatada();

private:
    Qt::Key teclaIzq_, teclaDer_, teclaSalto_, teclaPatada_;
    bool    presIzq_, presDer_, presSalto_;

    float anguloPatada_;  // 0 = reposo, +60 = patada hacia arriba
    bool  pateando_;

    float vy_;
    bool  enSuelo_;
    float suelo_;

    static constexpr float GRAVEDAD      = 0.5f;
    static constexpr float IMPULSO_SALTO = -12.0f;
    static constexpr float RADIO_CABEZA  = 24.0f;
    static constexpr float ANCHO_CUERPO  = 18.0f;
    static constexpr float ALTO_CUERPO   = 20.0f;
    static constexpr float ALTO_SPRITE   = 72.0f;
    static constexpr float ANCHO_SPRITE  = 48.0f;
    static constexpr float ANCHO_ZAPATO  = 32.0f;
    static constexpr float ALTO_ZAPATO   = 22.0f;
    static constexpr float ZAP_PIVOT_X   = -4.0f;
    static constexpr float ZAP_PIVOT_Y   =  2.0f;

    float  velocidadBase_;
    float  modificadorVelocidad_;
    QTimer *timerTurbo_;
    QTimer *timerPatada_;

    bool   enPanico_;
    QTimer *timerPanico_;

    QColor  color_;
    QPixmap pixPersonaje_;
    QPixmap pixZapato_;
    bool    reflejar_;
};

#endif // JUGADOR_H
