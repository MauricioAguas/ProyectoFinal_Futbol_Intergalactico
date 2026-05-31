#ifndef ENTIDAD_H
#define ENTIDAD_H

#include <QGraphicsPixmapItem>
#include <QObject>


/* Clase base abstracta para todas las entidades del juego.
 (jugador) y (antagonista IA) heredan de Personaje,
 que a su vez hereda de Entidad. Asi ambos comparten la misma raiz.
*/
class Entidad : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Entidad(QGraphicsItem *parent = nullptr);
    virtual ~Entidad();

    // Posicion
    float getX() const { return x_; }
    float getY() const { return y_; }
    void  setPosicion(float x, float y);

    // Ciclo de vida
    virtual void actualizar()  = 0;   // logica de movimiento/estado
    virtual void reiniciar()   = 0;   // volver al estado inicial

protected:
    float x_;
    float y_;
};

#endif // ENTIDAD_H
