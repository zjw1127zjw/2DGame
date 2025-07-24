#ifndef BALL_PROJECTILE_H
#define BALL_PROJECTILE_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include "Character.h"

// 实心球投射物类
class BallProjectile : public QWidget {
    Q_OBJECT
public:
    BallProjectile(int startX, int startY, bool directionRight, Character* thrower, QWidget *parent = nullptr);

    // 获取碰撞矩形
    QRect getRect() const {
        return QRect(x, y, width(), height());
    }

    // 获取投掷者
    Character* getThrower() const { return thrower; }

    // 是否活动状态
    bool isActive() const { return active; }

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updatePosition();

private:
    static constexpr int GRAVITY = 1;
    int x, y;
    int velocityX, velocityY;
    bool directionRight;
    bool active = true; // 新增：活动状态标志
    QPixmap ballPixmap;
    QTimer *updateTimer;
    Character* thrower; // 投掷者指针
};

#endif // BALL_PROJECTILE_H
