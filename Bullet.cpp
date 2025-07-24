#include "Bullet.h"

Bullet::Bullet(int startX, int startY, bool directionRight, Character* shooter, int charWidth, int charHeight, QWidget *parent)
    : QWidget(parent), directionRight(directionRight), shooter(shooter),
    bulletSpeed(12) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);

    // 设置子弹大小
    setFixedSize(charWidth, charHeight);

    // 加载子弹图片
    if (directionRight) {
        bulletPixmap = QPixmap(":/new/prefix1/res/bulletb2.png");
    } else {
        bulletPixmap = QPixmap(":/new/prefix1/res/bulletb1.png");
    }

    if (!bulletPixmap.isNull()) {
        bulletPixmap = bulletPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // 初始位置
    x = startX;
    y = startY;
    move(x, y);

    // 定时器更新位置
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &Bullet::updatePosition);
    updateTimer->start(16); // 约60FPS
}

void Bullet::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    if (!bulletPixmap.isNull()) {
        painter.drawPixmap(0, 0, bulletPixmap);
    } else {
        painter.setBrush(Qt::red);
        painter.drawEllipse(rect());
    }
}

void Bullet::updatePosition() {
    // 更新位置
    if (directionRight) {
        x += bulletSpeed;
    } else {
        x -= bulletSpeed;
    }

    move(x, y);

    // 检查是否碰到屏幕边缘
    int screenWidth = parentWidget() ? parentWidget()->width() : 1200;
    if (x < -50 || x > screenWidth + 50) {
        active = false;
        hide();
    }
}
