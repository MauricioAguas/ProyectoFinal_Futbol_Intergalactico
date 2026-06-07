#ifndef GAME_H
#define GAME_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QApplication>
#include "Nivel.h"

// Vista personalizada que bloquea el scroll con teclas de flechas
class GameView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GameView(QWidget *parent = nullptr) : QGraphicsView(parent) {}
protected:
    void keyPressEvent(QKeyEvent *event) override {
        // Reenviar la tecla a la escena sin activar el scroll de QGraphicsView
        if (scene()) QApplication::sendEvent(scene(), event);
    }
    void keyReleaseEvent(QKeyEvent *event) override {
        if (scene()) QApplication::sendEvent(scene(), event);
    }
};

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

    GameView      *view_;
    Nivel         *nivel_;
    QWidget       *overlay_;       // menu principal
    QWidget       *overlayIngame_; // pausa / resultado
};

#endif // GAME_H
