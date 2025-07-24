#include "Item.h"
#include <QPainter>

Item::Item(ItemType type, QWidget *parent)
    : QWidget(parent), itemType(type), velocityY(0), isOnGround(false) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);

    // 根据道具类型设置图片
    switch (itemType) {
    case BANDAGE:
        itemPixmap = QPixmap(":/new/prefix1/res/beng.png");
        break;
    case MEDKIT:
        itemPixmap = QPixmap(":/new/prefix1/res/jijiu.png");
        break;
    case ADRENALINE:
        itemPixmap = QPixmap(":/new/prefix1/res/shen.png");
        break;
    case KNIFE:
        itemPixmap = QPixmap(":/new/prefix1/res/knife.png");
        break;
    case BALL:
        itemPixmap = QPixmap(":/new/prefix1/res/ball.png");
        break;
    case RIFLE:
        itemPixmap = QPixmap(":/new/prefix1/res/AKM.png");
        break;
    case SNIPER:
        itemPixmap = QPixmap(":/new/prefix1/res/juji.png");
        break;
    case LIGHT_ARMOR:
        itemPixmap = QPixmap(":/new/prefix1/res/suo.png");
        break;
    case BULLETPROOF_VEST:
        itemPixmap = QPixmap(":/new/prefix1/res/fangdan.png");
        break;
    }

    // 调整道具大小
    int size = 40;
    if (!itemPixmap.isNull()) {
        itemPixmap = itemPixmap.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    setFixedSize(size, size);
}

void Item::updatePosition(const std::vector<Platform>& platforms) {
    if (isOnGround) return;

    // 应用重力
    velocityY += GRAVITY;
    int newY = y() + velocityY;

    // 检查平台碰撞
    bool collided = false;
    for (const Platform& p : platforms) {
        if (newY + height() >= p.top() &&
            y() + height() <= p.top() + 10 &&
            x() + width() > p.x &&
            x() < p.x + p.width) {
            move(x(), p.top() - height());
            velocityY = 0;
            isOnGround = true;
            collided = true;
            break;
        }
    }

    if (!collided) {
        move(x(), newY);
        if (y() > 800) {
            move(x(), 800 - height());
            isOnGround = true;
        }
    }
}

bool Item::contains(int px, int py) const {
    return px >= x() && px <= x() + width() &&
           py >= y() && py <= y() + height();
}

void Item::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    if (!itemPixmap.isNull()) {
        painter.drawPixmap(0, 0, itemPixmap);
    } else {
        painter.setBrush(Qt::yellow);
        painter.drawRect(rect());
    }
}
