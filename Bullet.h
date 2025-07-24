#ifndef BULLET_H
#define BULLET_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include "Character.h"

// 子弹类
class Bullet : public QWidget {
    Q_OBJECT
public:
    Bullet(int startX, int startY, bool directionRight, Character* shooter, int charWidth, int charHeight, QWidget *parent = nullptr);
    ~Bullet() {}

    // 获取碰撞矩形
    QRect getRect() const {
        return QRect(x, y, width(), height());
    }

    // 获取射击者
    Character* getShooter() const { return shooter; }

    // 是否活动状态
    bool isActive() const { return active; }

    void setActive(bool isActive) {
        active = isActive;
    }

    // 设置子弹图片 - 新增
    void setBulletPixmap(const QPixmap& pixmap) {
        bulletPixmap = pixmap;
        if (!bulletPixmap.isNull()) {
            bulletPixmap = bulletPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updatePosition();

private:
    static constexpr int GRAVITY = 1;
    bool directionRight;
    Character* shooter;
    int x, y;
    int bulletSpeed;
    QPixmap bulletPixmap;
    QTimer *updateTimer;
    bool active = true;  // 活动状态标志
};

#endif // BULLET_H
