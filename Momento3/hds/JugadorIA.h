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
    void actualizar()              override;  // solo Nivel1 (gravedad)
    void reiniciar()               override;

    QRectF boundingRect() const override;
    void   paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget = nullptr) override;

    void percibir(float balonX, float balonY,
                  float jugadorX, float jugadorY);

    void alertarGol();

    // Nivel2: calcula dx,dy con maquina de estados + Arrive (sin gravedad)
    void calcularMovHockey(float &outDx, float &outDy);
    void moverHockey(float dx, float dy, float limIzq, float limDer,
                     float limTop, float limBot);

    // Nivel1: lerp por eje
    float calcularDx();
    float calcularDy();

    // Modo hockey: desactiva zapato y gravedad en paint/actualizar
    void setModoHockey(bool hockey) { modoHockey_ = hockey; }

    void setLimites(float xMin, float xMax) { xMin_ = xMin; xMax_ = xMax; }
    void setOtroJugador(Personaje *otro)    { otroJugador_ = otro; }
    void setSuelo(float suelo)              { suelo_ = suelo; }

private:
    bool modoHockey_ = false;

    float balonX_, balonY_, rivalX_, rivalY_;
    bool  balonVisible_;

    enum EstadoHockey { PORTERO, ATACANTE };
    EstadoHockey estadoHockey_;
    static constexpr float DIST_ATAQUE   = 280.0f;
    static constexpr float DIST_DEFENSA  = 350.0f;
    static constexpr float OFFSET_PORTERO = 60.0f;
    static constexpr float ARRIVE_SLOW_R  = 80.0f;

    float objetivoX_, objetivoY_;
    bool  debeAtacar_;
    bool  defendiendo_;
    int   framesDefensa_;
    static constexpr int FRAMES_DEFENSA = 120;
    void  razonar();

    float xArco_;
    bool  enSuelo_;
    float vy_;
    float suelo_;
    float velActualX_;
    float velActualY_;
    int   cooldownSalto_;

    static constexpr float GRAVEDAD              = 0.5f;
    static constexpr float IMPULSO_SALTO         = -9.0f;
    static constexpr float MIN_DIST_SALTO        = 80.0f;
    static constexpr float DIST_H_SALTO          = 120.0f;
    static constexpr int   FRAMES_COOLDOWN_SALTO = 90;

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
