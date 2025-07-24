#ifndef KNIFE_ATTACK_EFFECT_H
#define KNIFE_ATTACK_EFFECT_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>

// 小刀攻击特效类
class KnifeAttackEffect : public QWidget {
    Q_OBJECT
public:
    KnifeAttackEffect(QWidget *parent = nullptr);

    // 开始小刀攻击动画
    void startAttack(bool isRight, int characterX, int characterY, int characterWidth, int characterHeight);

    // 是否可见
    bool isVisible() const { return visible; }

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void hideEffect();

private:
    QPixmap knifePixmap;
    QTimer *effectTimer;
    bool visible = false;
    bool directionRight = true;
};

#endif // KNIFE_ATTACK_EFFECT_H
