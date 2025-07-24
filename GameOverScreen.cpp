#include "GameOverScreen.h"

GameOverScreen::GameOverScreen(QWidget *parent) : QWidget(parent) {
    // 设置背景
    bgLabel = new QLabel(this);
    originalBg = QPixmap(":/new/prefix1/res/background.png");
    if (!originalBg.isNull()) {
        bgLabel->setPixmap(originalBg);
    }
    bgLabel->lower();

    // 主布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    // 胜利者文本
    winnerLabel = new QLabel(this);
    winnerLabel->setStyleSheet("font-size: 36px; font-weight: bold; color: white;");
    winnerLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(winnerLabel);

    // 游戏结束图片
    gameOverLabel = new QLabel(this);
    QPixmap gameOverPixmap(":/new/prefix1/res/gameover.png");
    if (!gameOverPixmap.isNull()) {
        gameOverLabel->setPixmap(gameOverPixmap);
        gameOverLabel->setAlignment(Qt::AlignCenter);
        gameOverLabel->setFixedSize(gameOverPixmap.size());
    }
    layout->addWidget(gameOverLabel);

    // 返回按钮
    returnButton = new QPushButton("返回", this);
    returnButton->setFixedSize(200, 50);
    returnButton->setStyleSheet("font-size: 24px; font-weight: bold; background-color: #4CAF50; color: white;");
    layout->addWidget(returnButton, 0, Qt::AlignCenter);

    // 连接按钮信号
    connect(returnButton, &QPushButton::clicked, this, &GameOverScreen::returnToStart);
}

void GameOverScreen::setWinner(int winner) {
    if (winner == 1) {
        winnerLabel->setText("Winner: Role 1");
    } else if (winner == 2) {
        winnerLabel->setText("Winner: Role 2");
    }
}

void GameOverScreen::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    bgLabel->setGeometry(0, 0, width(), height());
    if (!originalBg.isNull()) {
        QPixmap scaled = originalBg.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        bgLabel->setPixmap(scaled);
    }
}

void GameOverScreen::returnToStart() {
    emit returnToStartRequested();
}
