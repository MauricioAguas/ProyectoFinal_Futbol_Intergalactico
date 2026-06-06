#ifndef BALON_H
#define BALON_H

#include "Entidad.h"

class Balon : public Entidad {
    Q_OBJECT

public:
    explicit Balon(QGraphicsItem *parent = nullptr);
    ~Balon();

    void actualizar() override;
    void reiniciar()  override;

    QRectF       boundingRect() const override;
    QPainterPath shape()        const override;
    void         paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

    void  lanzar(float vx, float vy);
    void  aplicarRebote(bool horizontal);
    void  setBounds(int w, int h) { anchoCampo_ = w; altoCampo_ = h; }
    void  setModoParabolico(bool v) { modoParabolico_ = v; }

    float getVx() const { return vx_; }
    float getVy() const { return vy_; }
    bool  estaEnMovimiento() const { return enMovimiento_; }

private:
    float vx_, vy_;
    float tiempo_;
    bool  enMovimiento_;
    bool  modoParabolico_;
    int   anchoCampo_;
    int   altoCampo_;

    static constexpr float GRAVEDAD         = 0.4f;
    static constexpr float ROZAMIENTO_AIRE  = 0.999f;  // rozamiento en vuelo (casi nulo)
    static constexpr float ROZAMIENTO_SUELO = 0.96f;   // frena en el suelo cada frame
    static constexpr float ROZAMIENTO_REBOTE= 0.65f;   // pierde 35% de energia en cada rebote
    static constexpr float ROZAMIENTO_PARED = 0.80f;   // pierde 20% al chocar pared/techo
    static constexpr float RADIO            = 22.0f;
    static constexpr float RADIO_HIT        = 14.0f;
};

#endif // BALON_H
