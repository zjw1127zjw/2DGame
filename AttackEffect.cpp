#include "AttackEffect.h"

AttackEffect::AttackEffect(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &AttackEffect::updateFrame);
}

void AttackEffect::startAttack(bool isRight, int characterX, int characterY, int characterWidth, int characterHeight) {
    directionRight = isRight;
    currentFrame = 0;
    visible = true;

    // 设置特效大小为角色大小的300%
    int effectWidth = characterWidth * 3;
    int effectHeight = characterHeight * 3;
    setFixedSize(effectWidth, effectHeight);

    // 设置特效位置（根据角色方向调整）
    int offsetX;
    if (directionRight) {
        offsetX = characterWidth*0.01;  // 在角色右侧
    } else {
        offsetX = -characterWidth *1.8;  // 在角色左侧
    }
    int posX = characterX + offsetX;
    int posY = characterY + (characterHeight - effectHeight)/2 + characterHeight * 0.2;
    move(posX, posY);

    // 加载动画帧
    loadFrames();

    // 启动动画定时器
    animationTimer->start(50); // 20 FPS
    show();
}

void AttackEffect::loadFrames() {
    frames.clear();
    QString basePath = directionRight ?
                           ":/new/prefix1/res/sm_gs_superskill1_45_hit_%1.png" :
                           ":/new/prefix1/res/sm_gs_superskill1_225_hit_%1.png";

    for (int i = 1; i <= 10; i++) {
        QString path = basePath.arg(i, 4, 10, QChar('0'));
        QPixmap frame(path);
        if (!frame.isNull()) {
            frames.append(frame.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void AttackEffect::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    if (!visible || frames.isEmpty()) return;
    if (currentFrame >= frames.size()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(0, 0, frames[currentFrame]);
}

void AttackEffect::updateFrame() {
    currentFrame++;
    if (currentFrame >= frames.size()) {
        animationTimer->stop();
        visible = false;
        hide();
    } else {
        update();
    }
}
