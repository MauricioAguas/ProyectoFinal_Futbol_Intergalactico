#ifndef GAME_H
#define GAME_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QApplication>
#include "Nivel.h"

// Vista personalizada que bloquea el scroll con teclas de flechas
// y reenvía todos los eventos de teclado directamente a la escena activa.
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

// Ventana principal del juego. Administra la navegacion entre el menu
// principal, Nivel1 y Nivel2, y muestra overlays de pausa y resultado.
// Actua como controlador central: conecta las senales de Nivel con los
// slots que deciden que pantalla mostrar a continuacion.
class Game : public QMainWindow {
    Q_OBJECT

public:
    explicit Game(QWidget *parent = nullptr);
    ~Game();

private slots:
    // Destruye el nivel activo y muestra el menu principal
    void mostrarMenu();
    // Actualiza el marcador al recibir un gol del nivel
    void onGol(int jugador);
    // Limpia el nivel cuando su tiempo se agota o se cumple la condicion de fin
    void onNivelTerminado();
    // Muestra u oculta el overlay de pausa
    void onPausaToggled(bool pausado);
    // Muestra el overlay de resultado final con el ganador
    void onResultadoFinal(int ganador);

private:
    // Crea e inicializa Nivel1 en el modo indicado (vs humano o vs IA)
    void iniciarNivel1(bool vsIA);
    // Crea e inicializa Nivel2 en el modo indicado
    void iniciarNivel2(bool vsIA);
    // Desconecta senales, elimina el nivel y limpia la escena
    void limpiarNivel();
    // Configura GameView: sin scroll, sin marco, tamanio fijo
    void setupView();
    // Ajusta el tamanio de la ventana al nivel cargado
    void redimensionar(int w, int h);
    // Muestra el widget semitransparente de pausa sobre el juego
    void mostrarOverlayPausa();
    // Muestra el widget de resultado final con botones de revancha/menu
    void mostrarOverlayResultado(int ganador);
    // Elimina cualquier overlay ingame activo
    void limpiarOverlay();

    GameView      *view_;         // Vista principal donde se renderiza la escena
    Nivel         *nivel_;        // Nivel activo en este momento (puede ser null)
    QWidget       *overlay_;      // Widget del menu principal superpuesto
    QWidget       *overlayIngame_;// Widget de pausa o resultado ingame
};

#endif // GAME_H
