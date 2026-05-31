#ifndef ARCO_H
#define ARCO_H

#include "Entidad.h"


// porteria del campo intergalactico
// Detecta colision del balon para registrar un gol

class Arco : public Entidad {
    Q_OBJECT

public:
    enum Equipo { PLANET_EXPRESS, OMICRON_XI };

    explicit Arco(Equipo equipo, QGraphicsItem *parent = nullptr);
    ~Arco();

    void actualizar() override;
    void reiniciar()  override;

    bool detectarGol(float balX, float balY) const;
    Equipo getEquipo() const { return equipo_; }

private:
    Equipo equipo_;
    float  ancho_;
    float  alto_;
};

#endif // ARCO_H
