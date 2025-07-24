#include <QApplication>
#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include "GameScreen.h"
#include "GameOverScreen.h"
#include "HelpScreen.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 创建主窗口
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("2D横版射击游戏 - 武器系统");
    mainWindow.resize(1200, 800);

    // 创建堆叠窗口
    QStackedWidget *stackedWidget = new QStackedWidget(&mainWindow);
    mainWindow.setCentralWidget(stackedWidget);

    // 加载背景图片
    QPixmap backgroundPixmap(":/new/prefix1/res/background.jpg");
    if (!backgroundPixmap.isNull()) {
        backgroundPixmap = backgroundPixmap.scaled(mainWindow.size(),
                                                   Qt::IgnoreAspectRatio,
                                                   Qt::SmoothTransformation);
    }

    // 1. 开始界面
    QWidget *startScreen = new QWidget();
    QLabel *startBackground = new QLabel(startScreen);
    if (!backgroundPixmap.isNull()) {
        startBackground->setPixmap(backgroundPixmap);
        startBackground->setGeometry(0, 0, mainWindow.width(), mainWindow.height());
    }

    QVBoxLayout *startLayout = new QVBoxLayout(startScreen);
    startLayout->setAlignment(Qt::AlignCenter);

    QWidget *buttonContainer = new QWidget(startScreen);
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonContainer);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(30);

    QPushButton *startButton = new QPushButton(buttonContainer);
    QPixmap startButtonPixmap(":/new/prefix1/res/start.png");
    if (!startButtonPixmap.isNull()) {
        startButton->setFixedSize(startButtonPixmap.size());
        startButton->setIcon(startButtonPixmap);
        startButton->setIconSize(startButtonPixmap.size());
        startButton->setStyleSheet("border: none; background: transparent;");
    } else {
        startButton->setText("开始游戏");
        startButton->setFixedSize(200, 60);
        startButton->setStyleSheet("font-size: 24px;");
    }

    QWidget *helpButtonContainer = new QWidget(buttonContainer);
    QHBoxLayout *helpButtonLayout = new QHBoxLayout(helpButtonContainer);
    helpButtonLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton *helpButton = new QPushButton("游戏帮助", helpButtonContainer);
    helpButton->setFixedSize(200, 60);
    helpButton->setStyleSheet("font-size: 24px;");

    const int offsetValue = 10;
    helpButtonLayout->addSpacing(offsetValue);
    helpButtonLayout->addWidget(helpButton);
    helpButtonLayout->addStretch();

    buttonLayout->addStretch();
    buttonLayout->addWidget(startButton, 0, Qt::AlignCenter);
    buttonLayout->addWidget(helpButtonContainer, 0, Qt::AlignCenter);
    buttonLayout->addStretch();

    startLayout->addWidget(buttonContainer, 0, Qt::AlignCenter);

    // 2. 游戏界面
    GameScreen *gameScreen = new GameScreen();
    if (!backgroundPixmap.isNull()) {
        gameScreen->setBackground(backgroundPixmap);
    }

    // 3. 结束界面
    GameOverScreen *gameOverScreen = new GameOverScreen();

    // 4. 帮助界面
    HelpScreen *helpScreen = new HelpScreen();

    // 添加界面到堆叠窗口
    stackedWidget->addWidget(startScreen);    // 索引0
    stackedWidget->addWidget(gameScreen);     // 索引1
    stackedWidget->addWidget(gameOverScreen); // 索引2
    stackedWidget->addWidget(helpScreen);     // 索引3

    // 连接信号与槽
    QObject::connect(startButton, &QPushButton::clicked, [&]() {
        stackedWidget->setCurrentIndex(1);
        gameScreen->setFocus();
        gameScreen->startSpawningItems();
    });

    QObject::connect(helpButton, &QPushButton::clicked, [&]() {
        stackedWidget->setCurrentIndex(3);
    });

    QObject::connect(gameScreen, &GameScreen::gameOver, [&](int winner) {
        gameOverScreen->setWinner(winner);
        stackedWidget->setCurrentIndex(2);
    });

    QObject::connect(gameOverScreen, &GameOverScreen::returnToStartRequested, [&]() {
        stackedWidget->setCurrentIndex(0);
        stackedWidget->removeWidget(gameScreen);
        delete gameScreen;
        gameScreen = new GameScreen();
        if (!backgroundPixmap.isNull()) {
            gameScreen->setBackground(backgroundPixmap);
        }
        stackedWidget->insertWidget(1, gameScreen);
        QObject::connect(gameScreen, &GameScreen::gameOver, [&](int winner) {
            gameOverScreen->setWinner(winner);
            stackedWidget->setCurrentIndex(2);
        });
    });

    QObject::connect(helpScreen, &HelpScreen::returnToStartRequested, [&]() {
        stackedWidget->setCurrentIndex(0);
    });

    // 设置初始界面
    stackedWidget->setCurrentIndex(0);

    // 显示窗口
    mainWindow.show();

    return app.exec();
}
