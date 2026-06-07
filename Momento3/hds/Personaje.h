#ifndef PERSONAJE_H
#define PERSONAJE_H

#include "Entidad.h"
#include <QString>

/*
 Clase abstracta intermedia.
 Jugador y JugadorIA heredan de Personaje.
*/
class Balon;

// Personaje es la base comun para los dos tipos de jugador (humano e IA).
// Contiene los atributos compartidos: nombre, vida, velocidad y estado activo.
class Personaje : public Entidad {
    Q_OBJECT

public:
    explicit Personaje(const QString &nombre, int vida, float velocidad,
                       QGraphicsItem *parent = nullptr);
    virtual ~Personaje();

    // Getters de atributos del personaje
    QString getNombre()    const { return nombre_; }
    int     getVida()      const { return vida_; }
    float   getVelocidad() const { return velocidad_; }
    // Retorna true si el personaje esta en juego (no eliminado ni inactivo)
    bool    estaActivo()   const { return activo_; }

    // Setters para modificar estado durante el juego
    void setVida(int v)            { vida_ = v; }
    void setVelocidad(float v)     { velocidad_ = v; }
    void setActivo(bool a)         { activo_ = a; }

    // Desplaza el personaje segun el delta recibido; implementado por cada subclase
    virtual void mover(float dx, float dy)   = 0;
    // Reacciona al contacto con el balon (patada, rebote, etc.)
    virtual void contacto(Balon *balon)      = 0;
    void actualizar() override = 0;
    void reiniciar()  override = 0;

protected:
    QString nombre_;    // Nombre del personaje (ej. "Fry", "Bender")
    int     vida_;      // Puntos de vida actuales
    float   velocidad_; // Velocidad base de desplazamiento en pixeles/frame
    bool    activo_;    // false cuando el personaje es eliminado o no participa
};

#endif // PERSONAJE_H
