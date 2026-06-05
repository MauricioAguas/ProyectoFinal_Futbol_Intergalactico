#ifndef BALON_H
#define BALON_H

#include "Entidad.h"

/*
 Fisica del balon:
  - Nivel 1 (lateral): trayectoria parabolica
  - Nivel 2 (cenital): rebote en paredes
*/
class Balon : public Entidad {
    Q_OBJECT

public:
    explicit Balon(QGraphicsItem *parent = nullptr);
    ~Balon();

    void actualizar() override;
    void reiniciar()  override;

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void   paint(QPainter *painter,
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

    static constexpr float GRAVEDAD   = 0.4f;
    static constexpr float ROZAMIENTO = 0.99f;
    static constexpr float RADIO      = 12.0f;
};

#endif // BALON_H
