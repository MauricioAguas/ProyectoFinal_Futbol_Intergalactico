#ifndef GAME_H
#define GAME_H

#include <QMainWindow>
#include <QGraphicsView>
#include "Nivel.h"

class Game : public QMainWindow {
    Q_OBJECT

public:
    explicit Game(QWidget *parent = nullptr);
    ~Game();

private slots:
    void mostrarMenu();
    void onGol(int jugador);
    void onNivelTerminado();
    void onPausaToggled(bool pausado);
    void onResultadoFinal(int ganador);

private:
    void iniciarNivel1(bool vsIA);
    void iniciarNivel2(bool vsIA);
    void limpiarNivel();
    void setupView();
    void redimensionar(int w, int h);
    void mostrarOverlayPausa();
    void mostrarOverlayResultado(int ganador);
    void limpiarOverlay();

    QGraphicsView *view_;
    Nivel         *nivel_;
    QWidget       *overlay_;       // menu principal
    QWidget       *overlayIngame_; // pausa / resultado
};

#endif // GAME_H
