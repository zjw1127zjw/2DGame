#ifndef ATTACK_EFFECT_H
#define ATTACK_EFFECT_H

#include <QWidget>
#include <QVector>
#include <QPixmap>
#include <QPainter>
#include <QTimer>

// 攻击特效类 - 已修改为拳头特效
class AttackEffect : public QWidget {
    Q_OBJECT
public:
    AttackEffect(QWidget *parent = nullptr);

    // 开始攻击动画 - 已修改为拳头攻击
    void startAttack(bool isRight, int characterX, int characterY, int characterWidth, int characterHeight);

    // 加载动画帧 - 已修改为使用拳头资源
    void loadFrames();

    // 是否可见
    bool isVisible() const { return visible; }

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateFrame();

private:
    QVector<QPixmap> frames;
    QTimer *animationTimer;
    int currentFrame = 0;
    bool visible = false;
    bool directionRight = true;
};

#endif // ATTACK_EFFECT_H
