#include "KnifeAttackEffect.h"
#include <QPainter>

KnifeAttackEffect::KnifeAttackEffect(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    effectTimer = new QTimer(this);
    connect(effectTimer, &QTimer::timeout, this, &KnifeAttackEffect::hideEffect);
}

void KnifeAttackEffect::startAttack(bool isRight, int characterX, int characterY, int characterWidth, int characterHeight) {
    directionRight = isRight;
    visible = true;

    // 设置特效大小
    int effectWidth = characterWidth * 1.5;
    int effectHeight = characterHeight * 1.5;
    setFixedSize(effectWidth, effectHeight);

    // 设置特效位置
    int offsetX;
    if (directionRight) {
        offsetX = characterWidth * 0.6;
        knifePixmap = QPixmap(":/new/prefix1/res/daoguang2.png");
    } else {
        offsetX = -characterWidth * 1.1;
        knifePixmap = QPixmap(":/new/prefix1/res/daoguang.png");
    }

    // 缩放图片
    if (!knifePixmap.isNull()) {
        knifePixmap = knifePixmap.scaled(effectWidth, effectHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    int posX = characterX + offsetX;
    int posY = characterY + (characterHeight - effectHeight)/2 + 10;
    move(posX, posY);

    // 启动动画定时器
    effectTimer->start(200);
    show();
    raise();
}

void KnifeAttackEffect::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    if (!visible || knifePixmap.isNull()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(0, 0, knifePixmap);
}

void KnifeAttackEffect::hideEffect() {
    effectTimer->stop();
    visible = false;
    hide();
}
