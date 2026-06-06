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

private:
    void iniciarNivel1(bool vsIA);
    void iniciarNivel2(bool vsIA);
    void limpiarNivel();
    void setupView();
    void redimensionar(int w, int h);   // cambia tamano de ventana y vista

    QGraphicsView *view_;
    Nivel         *nivel_;
    QWidget       *overlay_;   // widget de botones del menu
};

#endif // GAME_H
