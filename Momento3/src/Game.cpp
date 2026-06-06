#include "hds/Game.h"
#include "hds/Nivel1.h"
#include "hds/Nivel2.h"
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QFont>
#include <QTimer>
#include <QPixmap>

// Dimensiones del menu (proporcion 1645x926 reducida a ~820x462)
static const int GM_MENU_W  = 820;
static const int GM_MENU_H  = 462;
static const int GM_NIVEL_W = 800;
static const int GM_NIVEL_H = 450;

Game::Game(QWidget *parent)
    : QMainWindow(parent), nivel_(nullptr), overlay_(nullptr)
{
    setWindowTitle("Futbol Intergalactico - Futurama 3000");

    view_ = new QGraphicsView(this);
    view_->setRenderHint(QPainter::Antialiasing);
    view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCentralWidget(view_);

    mostrarMenu();
}

Game::~Game() {
    limpiarNivel();
}

void Game::redimensionar(int w, int h) {
    view_->setFixedSize(w, h);
    setFixedSize(sizeHint());
}

void Game::mostrarMenu() {
    limpiarNivel();
    redimensionar(GM_MENU_W, GM_MENU_H);

    QGraphicsScene *menu = new QGraphicsScene(this);
    menu->setSceneRect(0, 0, GM_MENU_W, GM_MENU_H);
    view_->setScene(menu);

    // Fondo: menu.png escalado a la ventana
    QPixmap fondoMenu(":/assets/menu.png");
    if (!fondoMenu.isNull()) {
        QGraphicsPixmapItem *bg = menu->addPixmap(
            fondoMenu.scaled(GM_MENU_W, GM_MENU_H,
                             Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation));
        bg->setZValue(-1);
    } else {
        // Fallback si no encuentra la imagen
        menu->setBackgroundBrush(QBrush(QColor(10, 10, 40)));
    }

    // Overlay con los 4 botones
    overlay_ = new QWidget(view_);
    overlay_->setStyleSheet("background: transparent;");
    overlay_->setGeometry(0, 0, GM_MENU_W, GM_MENU_H);

    auto btnStyle = QString(
        "QPushButton {"
        "background:rgba(10,10,80,200); color:white; border:2px solid #4444ff;"
        "border-radius:8px; font-size:15px; padding:10px 20px;}"
        "QPushButton:hover{background:rgba(50,50,170,220);}");

    QVBoxLayout *vlay = new QVBoxLayout(overlay_);
    vlay->setAlignment(Qt::AlignCenter);
    vlay->addSpacing(230);

    QPushButton *btn1v1  = new QPushButton("Nivel 1 - 1 vs 1  (teclado)",  overlay_);
    QPushButton *btn1vIA = new QPushButton("Nivel 1 - 1 vs Maquina (IA)",  overlay_);
    QPushButton *btn2v1  = new QPushButton("Nivel 2 - 1 vs 1  (teclado)",  overlay_);
    QPushButton *btn2vIA = new QPushButton("Nivel 2 - 1 vs Maquina (IA)",  overlay_);

    for (auto *b : {btn1v1, btn1vIA, btn2v1, btn2vIA}) {
        b->setStyleSheet(btnStyle);
        b->setFixedWidth(320);
        vlay->addWidget(b, 0, Qt::AlignCenter);
        vlay->addSpacing(8);
    }

    connect(btn1v1,  &QPushButton::clicked, this, [this]{ iniciarNivel1(false); });
    connect(btn1vIA, &QPushButton::clicked, this, [this]{ iniciarNivel1(true);  });
    connect(btn2v1,  &QPushButton::clicked, this, [this]{ iniciarNivel2(false); });
    connect(btn2vIA, &QPushButton::clicked, this, [this]{ iniciarNivel2(true);  });

    overlay_->show();
}

void Game::iniciarNivel1(bool vsIA) {
    limpiarNivel();
    redimensionar(GM_NIVEL_W, GM_NIVEL_H);
    Nivel1 *n = new Nivel1(vsIA ? Nivel::VS_MAQUINA : Nivel::VS_HUMANO);
    n->inicializar();
    nivel_ = n;
    view_->setScene(static_cast<QGraphicsScene*>(nivel_));
    view_->setFocus();
    connect(nivel_, &Nivel::golAnotado,     this, &Game::onGol);
    connect(nivel_, &Nivel::nivelTerminado, this, &Game::onNivelTerminado);
}

void Game::iniciarNivel2(bool vsIA) {
    limpiarNivel();
    redimensionar(GM_NIVEL_W, 600);
    Nivel2 *n = new Nivel2(vsIA ? Nivel::VS_MAQUINA : Nivel::VS_HUMANO);
    n->inicializar();
    nivel_ = n;
    view_->setScene(static_cast<QGraphicsScene*>(nivel_));
    view_->setFocus();
    connect(nivel_, &Nivel::golAnotado,     this, &Game::onGol);
    connect(nivel_, &Nivel::nivelTerminado, this, &Game::onNivelTerminado);
}

void Game::onGol(int jugador) {
    Q_UNUSED(jugador);
}

void Game::onNivelTerminado() {
    QTimer::singleShot(2000, this, &Game::mostrarMenu);
}

void Game::limpiarNivel() {
    if (overlay_) { delete overlay_; overlay_ = nullptr; }
    if (nivel_) {
        view_->setScene(nullptr);
        delete nivel_;
        nivel_ = nullptr;
    }
}

void Game::setupView() {}
