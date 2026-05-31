#ifndef PERSONAJE_H
#define PERSONAJE_H

#include "Entidad.h"
#include <QString>

/*
 Clase abstracta intermedia.
 Jugador y JugadorIA heredan de Personaje, compartiendo
 atributos de vida, velocidad y estado. Las caracteristicas
 de personalidad especificas se implementan en cada subclase.
*/
class Personaje : public Entidad {
    Q_OBJECT

public:
    explicit Personaje(const QString &nombre, int vida, float velocidad,
                       QGraphicsItem *parent = nullptr);
    virtual ~Personaje();

    // Getters comunes
    QString getNombre()    const { return nombre_; }
    int     getVida()      const { return vida_; }
    float   getVelocidad() const { return velocidad_; }
    bool    estaActivo()   const { return activo_; }

    // Setters comunes
    void setVida(int v);
    void setVelocidad(float v) { velocidad_ = v; }
    void setActivo(bool a)     { activo_ = a; }

    // Polimorfismo — cada subclase define como se mueve
    virtual void mover(float dx, float dy) = 0;

    // Llamado por el Nivel cuando el balon toca al personaje
    virtual void contacto(class Balon *balon) = 0;

    // Heredados de Entidad
    void actualizar() override = 0;
    void reiniciar()  override = 0;

protected:
    QString nombre_;
    int     vida_;
    float   velocidad_;
    bool    activo_;
};

#endif // PERSONAJE_H
