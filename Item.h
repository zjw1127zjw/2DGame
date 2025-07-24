#ifndef ITEM_H
#define ITEM_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <vector>
#include "Platform.h"

// 道具基类
class Item : public QWidget {
    Q_OBJECT
public:
    enum ItemType { BANDAGE, MEDKIT, ADRENALINE, KNIFE, BALL, RIFLE, SNIPER, LIGHT_ARMOR, BULLETPROOF_VEST }; // 新增防弹衣类型

    Item(ItemType type, QWidget *parent = nullptr);

    // 更新位置（应用重力）
    void updatePosition(const std::vector<Platform>& platforms);

    // 检查点是否在道具范围内
    bool contains(int px, int py) const;

    // 获取道具类型
    ItemType getType() const { return itemType; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    static constexpr int GRAVITY = 1; // 重力加速度

    ItemType itemType;
    QPixmap itemPixmap;
    int velocityY;
    bool isOnGround;
};

#endif // ITEM_H
