#include "hds/Game.h"
#include "hds/Nivel1.h"
#include "hds/Nivel2.h"
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QTimer>
#include <QPixmap>
#include <QPainter>

static const int GM_MENU_W  = 820;
static const int GM_MENU_H  = 462;
static const int GM_NIVEL_W = 800;
static const int GM_NIVEL_H = 450;

static QString btnIngameStyle() {
    return QString(
        "QPushButton {"
        "  background: rgba(10,10,80,210);"
        "  color: white;"
        "  border: 2px solid #4444ff;"
        "  border-radius: 10px;"
        "  font-size: 16px;"
        "  padding: 12px 30px;"
        "}"
        "QPushButton:hover { background: rgba(50,50,180,230); }");
}

Game::Game(QWidget *parent)
    : QMainWindow(parent), nivel_(nullptr), overlay_(nullptr), overlayIngame_(nullptr)
{
    setWindowTitle("Futbol Intergalactico - Futurama 3000");

    view_ = new GameView(this);
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

void Game::limpiarOverlay() {
    if (overlayIngame_) { delete overlayIngame_; overlayIngame_ = nullptr; }
}

void Game::mostrarMenu() {
    limpiarNivel();
    redimensionar(GM_MENU_W, GM_MENU_H);

    QGraphicsScene *menu = new QGraphicsScene(this);
    menu->setSceneRect(0, 0, GM_MENU_W, GM_MENU_H);
    view_->setScene(menu);

    QPixmap fondoMenu(":/assets/menu.png");
    if (!fondoMenu.isNull()) {
        QGraphicsPixmapItem *bg = menu->addPixmap(
            fondoMenu.scaled(GM_MENU_W, GM_MENU_H,
                             Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation));
        bg->setZValue(-1);
    } else {
        menu->setBackgroundBrush(QBrush(QColor(10, 10, 40)));
    }

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
    vlay->addSpacing(100);

    QPushButton *btn1v1  = new QPushButton("Nivel 1 - 1 vs 1  (teclado)",  overlay_);
    QPushButton *btn1vIA = new QPushButton("Nivel 1 - 1 vs Maquina (IA)",  overlay_);
    QPushButton *btn2v1  = new QPushButton("Nivel 2 - 1 vs 1  (teclado)",  overlay_);
    QPushButton *btn2vIA = new QPushButton("Nivel 2 - 1 vs Maquina (IA)",  overlay_);

    for (auto *b : {btn1v1, btn1vIA, btn2v1, btn2vIA}) {
        b->setStyleSheet(btnStyle);
        b->setFixedWidth(240);
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
    connect(nivel_, &Nivel::golAnotado,      this, &Game::onGol);
    connect(nivel_, &Nivel::nivelTerminado,  this, &Game::onNivelTerminado);
    connect(nivel_, &Nivel::pausaToggled,    this, &Game::onPausaToggled);
    connect(nivel_, &Nivel::resultadoFinal,  this, &Game::onResultadoFinal);
}

void Game::iniciarNivel2(bool vsIA) {
    limpiarNivel();
    redimensionar(GM_NIVEL_W, 600);
    Nivel2 *n = new Nivel2(vsIA ? Nivel::VS_MAQUINA : Nivel::VS_HUMANO);
    n->inicializar();
    nivel_ = n;
    view_->setScene(static_cast<QGraphicsScene*>(nivel_));
    view_->setFocus();
    connect(nivel_, &Nivel::golAnotado,      this, &Game::onGol);
    connect(nivel_, &Nivel::nivelTerminado,  this, &Game::onNivelTerminado);
    connect(nivel_, &Nivel::pausaToggled,    this, &Game::onPausaToggled);
    connect(nivel_, &Nivel::resultadoFinal,  this, &Game::onResultadoFinal);
}

void Game::onGol(int jugador) {
    Q_UNUSED(jugador);
}

void Game::onNivelTerminado() {}

void Game::onPausaToggled(bool pausado) {
    if (pausado) {
        mostrarOverlayPausa();
    } else {
        limpiarOverlay();
        view_->setFocus();
    }
}

void Game::onResultadoFinal(int ganador) {
    mostrarOverlayResultado(ganador);
}

void Game::mostrarOverlayPausa() {
    limpiarOverlay();

    int vw = view_->width();
    int vh = view_->height();

    overlayIngame_ = new QWidget(view_);
    overlayIngame_->setGeometry(0, 0, vw, vh);
    overlayIngame_->setStyleSheet("background: rgba(0,0,0,150);");

    QVBoxLayout *vlay = new QVBoxLayout(overlayIngame_);
    vlay->setAlignment(Qt::AlignCenter);
    vlay->setSpacing(20);

    QLabel *titulo = new QLabel("PAUSA", overlayIngame_);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("color: white; font-size: 32px; font-weight: bold;"
                          "font-family: Arial; background: transparent;");

    QPushButton *btnContinuar = new QPushButton("Continuar", overlayIngame_);
    QPushButton *btnMenu      = new QPushButton("Volver al Menu Principal", overlayIngame_);

    btnContinuar->setStyleSheet(btnIngameStyle());
    btnMenu->setStyleSheet(btnIngameStyle());
    btnContinuar->setFixedWidth(260);
    btnMenu->setFixedWidth(260);

    vlay->addWidget(titulo);
    vlay->addSpacing(10);
    vlay->addWidget(btnContinuar, 0, Qt::AlignCenter);
    vlay->addWidget(btnMenu,      0, Qt::AlignCenter);

    connect(btnContinuar, &QPushButton::clicked, this, [this]{
        if (nivel_) nivel_->reanudar();
    });
    connect(btnMenu, &QPushButton::clicked, this, [this]{
        mostrarMenu();
    });

    overlayIngame_->show();
}

void Game::mostrarOverlayResultado(int ganador) {
    limpiarOverlay();

    int vw = view_->width();
    int vh = view_->height();

    overlayIngame_ = new QWidget(view_);
    overlayIngame_->setGeometry(0, 0, vw, vh);
    overlayIngame_->setStyleSheet("background: rgba(0,0,0,160);");

    QVBoxLayout *vlay = new QVBoxLayout(overlayIngame_);
    vlay->setAlignment(Qt::AlignCenter);
    vlay->setSpacing(18);

    QString linea1, linea2;
    bool vsIA = (nivel_ && nivel_->getModo() == Nivel::VS_MAQUINA);

    if (ganador == -1) {
        linea1 = "EMPATE";
        linea2 = "";
    } else if (vsIA) {
        if (ganador == 0) {
            linea1 = "VICTORIA";
            linea2 = "Ganaste contra la maquina";
        } else {
            linea1 = "DERROTA";
            linea2 = "La maquina te ha ganado";
        }
    } else {
        linea1 = "VICTORIA";
        linea2 = QString("Gana el Jugador %1").arg(ganador + 1);
    }

    QLabel *lblTitulo = new QLabel(linea1, overlayIngame_);
    lblTitulo->setAlignment(Qt::AlignCenter);
    QString colorTitulo = (linea1 == "DERROTA") ? "#ff4444" :
                          (linea1 == "EMPATE")   ? "#ffdd44" : "#44ff88";
    lblTitulo->setStyleSheet(QString(
        "color: %1; font-size: 40px; font-weight: bold;"
        "font-family: Arial; background: transparent;").arg(colorTitulo));

    if (!linea2.isEmpty()) {
        QLabel *lblSub = new QLabel(linea2, overlayIngame_);
        lblSub->setAlignment(Qt::AlignCenter);
        lblSub->setStyleSheet("color: white; font-size: 20px;"
                              "font-family: Arial; background: transparent;");
        vlay->addWidget(lblTitulo);
        vlay->addWidget(lblSub);
    } else {
        vlay->addWidget(lblTitulo);
    }

    if (nivel_) {
        QString marcador = QString("  %1  -  %2  ")
            .arg(nivel_->getGoles(0)).arg(nivel_->getGoles(1));
        QLabel *lblMarcador = new QLabel(marcador, overlayIngame_);
        lblMarcador->setAlignment(Qt::AlignCenter);
        lblMarcador->setStyleSheet(
            "color: white; font-size: 26px; font-weight: bold;"
            "font-family: Arial; background: transparent;");
        vlay->addWidget(lblMarcador);
    }

    vlay->addSpacing(14);

    QPushButton *btnMenu = new QPushButton("Volver al Menu Principal", overlayIngame_);
    btnMenu->setStyleSheet(btnIngameStyle());
    btnMenu->setFixedWidth(280);
    vlay->addWidget(btnMenu, 0, Qt::AlignCenter);

    connect(btnMenu, &QPushButton::clicked, this, [this]{ mostrarMenu(); });

    overlayIngame_->show();
}

void Game::limpiarNivel() {
    limpiarOverlay();
    if (overlay_) { delete overlay_; overlay_ = nullptr; }
    if (nivel_) {
        view_->setScene(nullptr);
        delete nivel_;
        nivel_ = nullptr;
    }
}

void Game::setupView() {}
