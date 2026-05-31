#ifndef PERSONAJE_H
#define PERSONAJE_H

#include "Entidad.h"
#include <QString>

// heredan de Personaje, compartiendo atributos de vida, velocidad y estado, cumpliendo correctamente el principio de herencia en POO.

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

    // Acciones comunes (polimorfismo)
    virtual void mover(float dx, float dy) = 0;
    virtual void recibirImpacto()          = 0;

    // Heredados de Entidad (cada subclase los implementa)
    void actualizar() override = 0;
    void reiniciar()  override = 0;

protected:
    QString nombre_;
    int     vida_;
    float   velocidad_;
    bool    activo_;

    // Caracteristicas de personalidad del personaje
    // Se sobreescriben en cada subclase para modular el comportamiento
    float modificadorVelocidad_;   // ej: turbo-cafeina (Fry) o modo-panico (Bender)
    bool  habilidadActiva_;
};

#endif // PERSONAJE_H
