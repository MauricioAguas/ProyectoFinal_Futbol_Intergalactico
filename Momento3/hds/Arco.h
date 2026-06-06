#ifndef ARCO_H
#define ARCO_H

#include "Entidad.h"

class Arco : public Entidad {
    Q_OBJECT

public:
    enum Equipo { PLANET_EXPRESS, OMICRON_XI };

    explicit Arco(Equipo equipo, float alto = 120.0f, QGraphicsItem *parent = nullptr);
    ~Arco();

    void actualizar() override;
    void reiniciar()  override;

    QRectF boundingRect() const override;
    void   paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget = nullptr) override;

    bool   detectarGol(float balX, float balY, float vx, float vy) const;
    Equipo getEquipo() const { return equipo_; }
    float  getAlto()   const { return alto_; }

private:
    Equipo equipo_;
    float  alto_;   // configurable por nivel
    static constexpr float ANCHO          = 20.0f;
    static constexpr float ALTO_TRAVESANO = 12.0f;
};

#endif // ARCO_H
