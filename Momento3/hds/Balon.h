#ifndef BALON_H
#define BALON_H

#include "Entidad.h"

// Representa el balon de futbol dentro de la escena.
// Gestiona su propia fisica: gravedad, rozamiento, rebotes en paredes/suelo
// y opcionalmente un modo parabolico para Nivel1.
class Balon : public Entidad {
    Q_OBJECT

public:
    explicit Balon(QGraphicsItem *parent = nullptr);
    ~Balon();

    // Avanza un frame: aplica gravedad, rozamiento y comprueba limites del campo
    void actualizar() override;
    // Detiene el balon y lo devuelve al centro del campo
    void reiniciar()  override;

    QRectF       boundingRect() const override;
    QPainterPath shape()        const override;
    void         paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

    // Imprime velocidad inicial (vx, vy) y pone el balon en movimiento
    void  lanzar(float vx, float vy);
    // Invierte la componente indicada de velocidad al chocar con una pared
    void  aplicarRebote(bool horizontal);
    // Define el area de juego para calcular los rebotes de borde
    void  setBounds(int w, int h) { anchoCampo_ = w; altoCampo_ = h; }
    // Activa/desactiva la trayectoria parabolica (Nivel1 = true, Nivel2 = false)
    void  setModoParabolico(bool v) { modoParabolico_ = v; }

    // Getters de velocidad y estado
    float getVx() const { return vx_; }
    float getVy() const { return vy_; }
    bool  estaEnMovimiento() const { return enMovimiento_; }

private:
    float vx_, vy_;         // Componentes de velocidad actuales
    float tiempo_;          // Contador de frames desde el ultimo lanzamiento
    bool  enMovimiento_;    // true mientras el balon tenga velocidad relevante
    bool  modoParabolico_;  // true en Nivel1 (futbol con gravedad)
    int   anchoCampo_;      // Ancho del area de juego en pixeles
    int   altoCampo_;       // Alto del area de juego en pixeles

    // Constantes fisicas del balon
    static constexpr float GRAVEDAD         = 0.4f;   // Aceleracion gravitacional por frame
    static constexpr float ROZAMIENTO_AIRE  = 0.999f; // Factor de frenado en el aire
    static constexpr float ROZAMIENTO_SUELO = 0.96f;  // Factor de frenado al rodar
    static constexpr float ROZAMIENTO_REBOTE= 0.65f;  // Perdida de energia al rebotar
    static constexpr float ROZAMIENTO_PARED = 0.80f;  // Perdida de energia en pared lateral
    static constexpr float RADIO            = 27.0f;  // +5px visual
    static constexpr float RADIO_HIT        = 19.0f;  // +5px hitbox Qt
};

#endif // BALON_H
