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

    float getX() const { return x_; }
    float getY() const { return y_; }
    void  setPosicion(float x, float y);

    virtual void actualizar() = 0;
    virtual void reiniciar()  = 0;

    // Subclases deben definir su forma y dibujo
    QRectF boundingRect() const override = 0;
    void   paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget = nullptr) override = 0;

protected:
    float x_;
    float y_;
};

#endif // ENTIDAD_H
