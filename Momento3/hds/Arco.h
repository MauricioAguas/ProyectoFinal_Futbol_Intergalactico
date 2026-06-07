#ifndef ARCO_H
#define ARCO_H

#include "Entidad.h"

// Representa uno de los dos arcos (porterias) del campo.
// Cada arco pertenece a un equipo y detecta si el balon cruzo su linea de gol.
class Arco : public Entidad {
    Q_OBJECT

public:
    // Equipos validos para identificar a que porteria pertenece el arco
    enum Equipo { PLANET_EXPRESS, OMICRON_XI };

    // equipo : equipo dueno del arco (determina lado del campo)
    // alto   : altura configurable de la porteria en pixeles
    explicit Arco(Equipo equipo, float alto = 120.0f, QGraphicsItem *parent = nullptr);
    ~Arco();

    void actualizar() override; // Los arcos son estaticos; implementacion vacia
    void reiniciar()  override; // Reinicia posicion al inicio del partido

    QRectF boundingRect() const override;
    void   paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget = nullptr) override;

    // Devuelve true si la posicion y velocidad del balon corresponden a un gol
    bool   detectarGol(float balX, float balY, float vx, float vy) const;
    // Retorna el equipo al que pertenece este arco
    Equipo getEquipo() const { return equipo_; }
    // Retorna la altura configurada de la porteria
    float  getAlto()   const { return alto_; }

private:
    Equipo equipo_;  // Equipo dueno: PLANET_EXPRESS (izq) u OMICRON_XI (der)
    float  alto_;    // Altura de la porteria; configurable por nivel
    static constexpr float ANCHO          = 20.0f;  // Ancho del poste lateral
    static constexpr float ALTO_TRAVESANO = 12.0f;  // Alto del travesano superior
};

#endif // ARCO_H
