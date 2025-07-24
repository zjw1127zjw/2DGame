#include "BallProjectile.h"

BallProjectile::BallProjectile(int startX, int startY, bool directionRight, Character* thrower, QWidget *parent)
    : QWidget(parent), directionRight(directionRight), thrower(thrower) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);

    // 初始位置
    x = startX;
    y = startY;

    // 加载图片
    ballPixmap = QPixmap(":/new/prefix1/res/ball.png");
    if (!ballPixmap.isNull()) {
        ballPixmap = ballPixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setFixedSize(ballPixmap.size());
    }

    // 初始速度
    velocityX = directionRight ? 10 : -10;
    velocityY = -15; // 向上

    // 移动到初始位置
    move(x, y);

    // 定时器更新位置
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &BallProjectile::updatePosition);
    updateTimer->start(16); // 约60FPS
}

void BallProjectile::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    if (!ballPixmap.isNull()) {
        painter.drawPixmap(0, 0, ballPixmap);
    } else {
        painter.setBrush(Qt::red);
        painter.drawEllipse(rect());
    }
}

void BallProjectile::updatePosition() {
    // 应用重力
    velocityY += GRAVITY;

    // 更新位置
    x += velocityX;
    y += velocityY;

    // 获取父窗口宽度
    int screenWidth = parentWidget() ? parentWidget()->width() : 1200;

    // 边界反弹检测
    if (x < 5 && velocityX < 0) {
        velocityX = -velocityX;
        x = 5;
    } else if (x > screenWidth - 5 - width() && velocityX > 0) {
        velocityX = -velocityX;
        x = screenWidth - 5 - width();
    }

    move(x, y);

    // 检查是否超出屏幕
    if (y > 800 || x < -100 || x > screenWidth + 100) {
        active = false;
        hide();
    }
}
