#ifndef BALON_H
#define BALON_H

#include "Entidad.h"


/*
Implementa las fisicas definidas en el Momento 1:
 - Nivel 1 (vista lateral): trayectoria parabolica
     x(t) = x0 + vx*t
     y(t) = y0 + vy*t + 0.5*g*t^2
  - Nivel 2 (vista cenital): rebote en paredes
     vx = -vx  /  vy = -vy  al colisionar con borde
 */
class Balon : public Entidad {
    Q_OBJECT

public:
    explicit Balon(QGraphicsItem *parent = nullptr);
    ~Balon();

    // --- Entidad interface ---
    void actualizar() override;
    void reiniciar()  override;

    // --- Fisicas ---
    void lanzar(float vx, float vy);          // inicia movimiento
    void aplicarRebote(bool horizontal);      // nivel 2: rebote en pared
    void setBounds(int w, int h) { anchoCampo_ = w; altoCampo_ = h; }

    // Getters
    float getVx() const { return vx_; }
    float getVy() const { return vy_; }
    bool  estaEnMovimiento() const { return enMovimiento_; }

private:
    float vx_, vy_;           // velocidades actuales
    float tiempo_;            // tiempo acumulado (para parabola)
    bool  enMovimiento_;
    bool  modoParabolico_;    // true=nivel1, false=nivel2 rebote
    int   anchoCampo_;
    int   altoCampo_;

    static constexpr float GRAVEDAD    = 0.4f;  // pixeles/tick^2
    static constexpr float ROZAMIENTO  = 0.99f; // factor de frenado nivel2
};

#endif // BALON_H
