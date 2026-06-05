#ifndef PERSONAJE_H
#define PERSONAJE_H

#include "Entidad.h"
#include <QString>

/*
 Clase abstracta intermedia.
 Jugador y JugadorIA heredan de Personaje.
*/
class Balon;

class Personaje : public Entidad {
    Q_OBJECT

public:
    explicit Personaje(const QString &nombre, int vida, float velocidad,
                       QGraphicsItem *parent = nullptr);
    virtual ~Personaje();

    QString getNombre()    const { return nombre_; }
    int     getVida()      const { return vida_; }
    float   getVelocidad() const { return velocidad_; }
    bool    estaActivo()   const { return activo_; }

    void setVida(int v)            { vida_ = v; }
    void setVelocidad(float v)     { velocidad_ = v; }
    void setActivo(bool a)         { activo_ = a; }

    virtual void mover(float dx, float dy)   = 0;
    virtual void contacto(Balon *balon)      = 0;
    void actualizar() override = 0;
    void reiniciar()  override = 0;

protected:
    QString nombre_;
    int     vida_;
    float   velocidad_;
    bool    activo_;
};

#endif // PERSONAJE_H
