#ifndef JUGADORIA_H
#define JUGADORIA_H

#include "Personaje.h"
#include "Balon.h"
#include <QTimer>
#include <QList>
#include <QColor>

/*
 JugadorIA — agente autonomo (Bender).
 Percepcion, razonamiento, accion y aprendizaje.
 Se dibuja como cabezon gris metalico.
*/
class JugadorIA : public Personaje {
    Q_OBJECT

public:
    explicit JugadorIA(const QString &nombre,
                       float velocidad,
                       float xArco,
                       QGraphicsItem *parent = nullptr);
    ~JugadorIA();

    void mover(float dx, float dy) override;
    void contacto(Balon *balon)    override;
    void actualizar()              override;
    void reiniciar()               override;

    QRectF boundingRect() const override;
    void   paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget = nullptr) override;

    void percibir(float balonX, float balonY,
                  float jugadorX, float jugadorY);

private:
    float balonX_, balonY_, rivalX_, rivalY_;
    bool  balonVisible_;
    static constexpr float RADIO_PERCEPCION = 250.0f;

    float objetivoX_;
    bool  debeAtacar_;
    void  razonar();

    float xArco_;
    bool  enSuelo_;
    float vy_;
    float suelo_;
    static constexpr float GRAVEDAD      = 0.5f;
    static constexpr float IMPULSO_SALTO = -12.0f;
    static constexpr float RADIO_CABEZA  = 24.0f;
    static constexpr float ANCHO_CUERPO  = 18.0f;
    static constexpr float ALTO_CUERPO   = 20.0f;

    QList<float> historialBalonX_;
    float        tendenciaRival_;
    float        modificadorReaccion_;
    void         aprender();

    QTimer *timerDecision_;

private slots:
    void cicloDecision();
};

#endif // JUGADORIA_H
