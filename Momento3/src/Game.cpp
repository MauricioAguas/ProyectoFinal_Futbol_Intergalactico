#include "hds/Game.h"
#include "hds/Nivel1.h"
#include "hds/Nivel2.h"
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QFont>
#include <QTimer>

Game::Game(QWidget *parent)
    : QMainWindow(parent), nivel_(nullptr)
{
    setWindowTitle("Futbol Intergalactico - Futurama 3000");
    setFixedSize(820, 560);

    view_ = new QGraphicsView(this);
    view_->setRenderHint(QPainter::Antialiasing);
    view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_->setFixedSize(820, 560);
    setCentralWidget(view_);

    mostrarMenu();
}

Game::~Game() {
    limpiarNivel();
}

void Game::mostrarMenu() {
    limpiarNivel();

    QGraphicsScene *menu = new QGraphicsScene(this);
    menu->setSceneRect(0, 0, 820, 560);
    menu->setBackgroundBrush(QBrush(QColor(10, 10, 40)));
    view_->setScene(menu);

    // Titulo — QGraphicsTextItem ahora esta completo con el include
    QGraphicsTextItem *titulo = menu->addText(
        "FUTBOL INTERGALACTICO", QFont("Arial", 28, QFont::Bold));
    titulo->setDefaultTextColor(QColor(255, 200, 0));
    titulo->setPos(120, 80);

    QGraphicsTextItem *sub = menu->addText(
        "Futurama - Anno 3000", QFont("Arial", 14));
    sub->setDefaultTextColor(QColor(180, 180, 255));
    sub->setPos(290, 140);

    // Botones superpuestos en la vista
    QWidget *overlay = new QWidget(view_);
    overlay->setStyleSheet("background: transparent;");
    overlay->setGeometry(0, 0, 820, 560);

    auto btnStyle = QString(
        "QPushButton {"
        "background:#1a1a5a; color:white; border:2px solid #4444ff;"
        "border-radius:8px; font-size:16px; padding:10px 30px;}"
        "QPushButton:hover{background:#3333aa;}");

    QVBoxLayout *vlay = new QVBoxLayout(overlay);
    vlay->setAlignment(Qt::AlignCenter);
    vlay->addSpacing(200);

    QPushButton *btn1v1  = new QPushButton("Nivel 1 - 1 vs 1  (teclado)",  overlay);
    QPushButton *btn1vIA = new QPushButton("Nivel 1 - 1 vs Maquina (IA)",  overlay);
    QPushButton *btn2v1  = new QPushButton("Nivel 2 - 1 vs 1  (teclado)",  overlay);
    QPushButton *btn2vIA = new QPushButton("Nivel 2 - 1 vs Maquina (IA)",  overlay);

    for (auto *b : {btn1v1, btn1vIA, btn2v1, btn2vIA}) {
        b->setStyleSheet(btnStyle);
        vlay->addWidget(b);
        vlay->addSpacing(8);
    }

    connect(btn1v1,  &QPushButton::clicked, this, [this]{ iniciarNivel1(false); });
    connect(btn1vIA, &QPushButton::clicked, this, [this]{ iniciarNivel1(true);  });
    connect(btn2v1,  &QPushButton::clicked, this, [this]{ iniciarNivel2(false); });
    connect(btn2vIA, &QPushButton::clicked, this, [this]{ iniciarNivel2(true);  });

    overlay->show();
}

void Game::iniciarNivel1(bool vsIA) {
    limpiarNivel();
    Nivel1 *n = new Nivel1(vsIA ? Nivel::VS_MAQUINA : Nivel::VS_HUMANO);
    n->inicializar();
    nivel_ = n;
    // cast explicito: Nivel hereda QGraphicsScene, pero el compilador
    // necesita verlo como QGraphicsScene* para view_->setScene()
    view_->setScene(static_cast<QGraphicsScene*>(nivel_));
    view_->setFocus();
    connect(nivel_, &Nivel::golAnotado,     this, &Game::onGol);
    connect(nivel_, &Nivel::nivelTerminado, this, &Game::onNivelTerminado);
}

void Game::iniciarNivel2(bool vsIA) {
    limpiarNivel();
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
    if (nivel_) {
        view_->setScene(nullptr);
        delete nivel_;
        nivel_ = nullptr;
    }
}

void Game::setupView() {}
