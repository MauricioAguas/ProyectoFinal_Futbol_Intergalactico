#ifndef ENTIDAD_H
#define ENTIDAD_H

#include <QGraphicsItem>
#include <QObject>
#include <QRectF>
#include <QPainter>

/*
 Clase base abstracta para todas las entidades del juego.
 Hereda de QGraphicsItem (no Pixmap) para poder dibujarse
 con formas Qt puras. Cada subclase implementa paint().
*/
class Entidad : public QObject, public QGraphicsItem {
    Q_OBJECT

public:
    explicit Entidad(QGraphicsItem *parent = nullptr);
    virtual ~Entidad();

    // Posicion
    float getX() const { return x_; }
    float getY() const { return y_; }
    void  setPosicion(float x, float y);

    // Ciclo de vida
    virtual void actualizar() = 0;
    virtual void reiniciar()  = 0;

    // QGraphicsItem interface — cada subclase define su forma y dibujo
    QRectF boundingRect() const override = 0;
    void   paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget = nullptr) override = 0;

protected:
    float x_;
    float y_;
};

#endif // ENTIDAD_H
