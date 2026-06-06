#ifndef JUGADORIA_H
#define JUGADORIA_H

#include "Personaje.h"
#include "Balon.h"
#include <QTimer>
#include <QList>
#include <QColor>
#include <QPixmap>

class JugadorIA : public Personaje {
    Q_OBJECT

public:
    explicit JugadorIA(const QString &nombre,
                       float velocidad,
                       float xArco,
                       QGraphicsItem *parent = nullptr);
    ~JugadorIA();

    void mover(float dx, float dy) override;
    void contacto(Balon *balon)    override;
    void actualizar()              override;
    void reiniciar()               override;

    QRectF boundingRect() const override;
    void   paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget = nullptr) override;

    void percibir(float balonX, float balonY,
                  float jugadorX, float jugadorY);

    // dx,dy calculados externamente (modo hockey): mueve directo con qBound aplicado afuera
    void moverHockey(float dx, float dy, float limIzq, float limDer,
                     float limTop, float limBot);

    // Devuelve la velocidad horizontal deseada segun objetivo (para que el nivel aplique qBound)
    float calcularDx();
    float calcularDy();

    void setLimites(float xMin, float xMax) { xMin_ = xMin; xMax_ = xMax; }
    void setOtroJugador(Personaje *otro)    { otroJugador_ = otro; }
    void setSuelo(float suelo)              { suelo_ = suelo; }

private:
    float balonX_, balonY_, rivalX_, rivalY_;
    bool  balonVisible_;
    static constexpr float RADIO_PERCEPCION = 800.0f;

    float objetivoX_, objetivoY_;
    bool  debeAtacar_;
    void  razonar();

    float xArco_;
    bool  enSuelo_;
    float vy_;
    float suelo_;
    float velActualX_;
    float velActualY_;

    static constexpr float GRAVEDAD      = 0.5f;
    static constexpr float IMPULSO_SALTO = -12.0f;
    // Salto solo si el balon esta al menos esta distancia vertical por encima
    static constexpr float MIN_DIST_SALTO = 80.0f;

    static constexpr float ANCHO_SPRITE  = 48.0f;
    static constexpr float ALTO_SPRITE   = 64.0f;
    static constexpr float ANCHO_ZAPATO  = 32.0f;
    static constexpr float ALTO_ZAPATO   = 20.0f;
    static constexpr float ZAP_PIVOT_X   = 8.0f;
    static constexpr float ZAP_PIVOT_Y   = 0.0f;

    float xMin_ =    0.0f;
    float xMax_ = 800.0f;

    Personaje *otroJugador_ = nullptr;

    QPixmap pixPersonaje_;
    QPixmap pixZapato_;

    QList<float> historialBalonX_;
    float        tendenciaRival_;
    float        modificadorReaccion_;
    void         aprender();

    QTimer *timerDecision_;

private slots:
    void cicloDecision();
};

#endif // JUGADORIA_H
