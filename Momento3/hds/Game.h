#ifndef GAME_H
#define GAME_H

#include <QMainWindow>
#include <QGraphicsView>

class Nivel;

/*
 Game — ventana principal del juego.
 Hereda de QMainWindow.
 Responsabilidades:
   - Mostrar el menu de inicio (1 vs 1 / vs Maquina).
   - Instanciar y cambiar entre Nivel1 y Nivel2.
   - Conectar las senales de los niveles (golAnotado, nivelTerminado)
     con el HUD y la logica de flujo del juego.
*/
class Game : public QMainWindow {
    Q_OBJECT

public:
    explicit Game(QWidget *parent = nullptr);
    ~Game();

    void mostrarMenu();

private slots:
    void iniciarNivel1(bool vsIA);  // arranca Nivel1 segun modo elegido
    void iniciarNivel2(bool vsIA);  // arranca Nivel2
    void onGol(int jugador);        // actualiza marcador en el HUD
    void onNivelTerminado();        // muestra resultado y vuelve al menu

private:
    QGraphicsView *view_;    // vista Qt que muestra la escena activa
    Nivel         *nivel_;   // nivel actualmente en juego (nullptr = menu)

    void limpiarNivel();     // destruye el nivel actual de forma segura
    void setupView();
};

#endif // GAME_H
