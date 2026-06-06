#ifndef NIVEL1_H
#define NIVEL1_H

#include "Nivel.h"
#include <QGraphicsTextItem>

class Nivel1 : public Nivel {
    Q_OBJECT

public:
    explicit Nivel1(ModoJuego modo, QObject *parent = nullptr);
    ~Nivel1();

    void inicializar() override;

protected:
    void keyPressEvent(QKeyEvent *event)   override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void actualizarMarcador(int jugador);

private:
    QGraphicsTextItem *marcador_;   // "0  -  0" en la parte superior
};

#endif // NIVEL1_H
