#ifndef ENTIDAD_H
#define ENTIDAD_H

#include <QGraphicsItem>
#include <QObject>
#include <QPainter>
#include <QRectF>

/*
 Clase base abstracta para todas las entidades del juego.
 Hereda de QObject (para signals/slots) y QGraphicsItem (para
 poder ser agregada a QGraphicsScene y detectar colisiones con
 collidesWithItem).
 La macro Q_INTERFACES es obligatoria cuando se combinan ambas.
*/
class Entidad : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    explicit Entidad(QGraphicsItem *parent = nullptr);
    virtual ~Entidad();

    // Retorna la posicion actual en X del item en la escena
    float getX() const { return x_; }
    // Retorna la posicion actual en Y del item en la escena
    float getY() const { return y_; }
    // Mueve la entidad a la coordenada (x, y) dentro de la escena
    void  setPosicion(float x, float y);

    // Actualiza el estado de la entidad cada frame (logica de movimiento, fisica, etc.)
    virtual void actualizar() = 0;
    // Reinicia la entidad a su estado inicial (posicion, velocidad, flags)
    virtual void reiniciar()  = 0;

    // Subclases deben definir su forma y dibujo
    QRectF boundingRect() const override = 0;
    void   paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget = nullptr) override = 0;

protected:
    float x_; // Posicion horizontal actual
    float y_; // Posicion vertical actual
};

#endif // ENTIDAD_H
