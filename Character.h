#ifndef CHARACTER_H
#define CHARACTER_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QLabel>
#include <vector>
#include "Platform.h"

// 前向声明
class AttackEffect;
class KnifeAttackEffect;

class Character : public QWidget
{
    Q_OBJECT
public:
    enum Weapon { FIST, KNIFE, BALL, RIFLE, SNIPER }; // 武器类型

    Character(const QString& spritePath, bool isPlayer1, QWidget *parent = nullptr);
    ~Character();

    // 设置角色位置
    void setPos(int x, int y);

    // 设置平台列表
    void setPlatforms(std::vector<Platform>* p);

    // 设置移动方向 (-1=左, 1=右, 0=停止)
    void setMoveDirection(int direction);

    // 设置下蹲状态
    void setCrouching(bool crouch);

    // 攻击方法（根据当前武器类型）
    void attack();

    // 小刀攻击方法
    void knifeAttack();

    // 拳头攻击方法
    void punch();

    // 实心球攻击方法
    void throwBall();

    // 步枪攻击方法
    void rifleAttack();

    // 狙击枪攻击方法
    void sniperAttack();

    // 装备小刀
    void equipKnife();

    // 装备拳头
    void equipFist();

    // 装备实心球
    void equipBall();

    // 装备步枪
    void equipRifle();

    // 装备狙击枪
    void equipSniper();

    // 装备护甲
    void equipLightArmor();

    // 装备防弹衣
    void equipBulletproofVest();

    // 判断角色是否面向右边
    bool isFacingRight() const;

    // 获取当前武器
    Weapon getCurrentWeapon() const;

    // 获取实心球使用次数
    int getBallUses() const;

    // 获取步枪弹药
    int getRifleAmmo() const;

    // 获取狙击枪弹药
    int getSniperAmmo() const;

    // 跳跃方法
    void jump();

    // 获取角色底部Y坐标
    int bottom() const;

    // 获取角色位置
    int getX() const;
    int getY() const;
    int getHeight() const;
    int getWidth() const;

    // 是否处于下蹲状态
    bool isCharacterCrouching() const;

    // 获取攻击特效
    AttackEffect* getAttackEffect() const;

    // 获取小刀攻击特效
    KnifeAttackEffect* getKnifeEffect() const;

    // 获取生命值
    int getHealth() const;

    // 受到伤害 - 新增无敌帧
    void takeDamage(int damage, Weapon damageSource = FIST); // 修改：添加伤害来源参数

    // 恢复生命值
    void heal(int amount);

    // 完全恢复生命值
    void fullHeal();

    // 获取攻击范围矩形
    QRect getAttackRange() const;

    // 新增：检查是否处于无敌状态
    bool isInvincibleState() const;

    // 检查地形效果
    void checkTerrainEffects();

    // 激活肾上腺素效果
    void activateAdrenaline();

    // 检查肾上腺素是否激活
    bool isAdrenalineActiveState() const;

    // 检查是否有锁子甲
    bool hasLightArmor() const { return lightArmorEquipped; }

    // 检查是否有防弹衣
    bool hasBulletproofVest() const { return bulletproofVestEquipped; }

    // 获取防弹衣耐久度
    int getVestDurability() const { return vestDurability; }

signals:
    void healthChanged(int newHealth);
    void ballThrown(int startX, int startY, bool directionRight, Character* thrower); // 添加投掷者参数
    void bulletShot(int startX, int startY, bool directionRight, Character* shooter); // 新增：子弹发射信号
    void sniperBulletShot(int startX, int startY, bool directionRight, Character* shooter); // 新增：狙击枪子弹发射信号

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void updateFrame();
    void applyGravity();
    void updateArmorPosition();

    QPixmap spriteSheet;
    QPixmap knifeRightPixmap; // 角色朝右时的小刀图片
    QPixmap knifeLeftPixmap;  // 角色朝左时的小刀图片
    QPixmap ballPixmap;       // 实心球图片
    QPixmap rifleRightPixmap; // 角色朝右时的步枪图片
    QPixmap rifleLeftPixmap;  // 角色朝左时的步枪图片
    QPixmap sniperRightPixmap; // 角色朝右时的狙击枪图片
    QPixmap sniperLeftPixmap;  // 角色朝左时的狙击枪图片
    QTimer *animationTimer;
    QTimer *moveTimer;
    QTimer *gravityTimer;
    AttackEffect *attackEffect;
    KnifeAttackEffect *knifeEffect; // 小刀攻击特效
    QTimer *invincibleTimer; // 新增：无敌帧定时器
    QTimer *terrainEffectTimer; // 新增：地形效果检测定时器
    QTimer *adrenalineTimer; // 新增：肾上腺素效果定时器
    QTimer *rifleShootTimer; // 新增：步枪射击间隔定时器
    QTimer *sniperShootTimer; // 新增：狙击枪射击间隔定时器
    QLabel *armorLabel = nullptr; // 新增：护甲显示标签（锁子甲）
    QLabel *vestLabel = nullptr;  // 新增：防弹衣显示标签

    // 平台指针（指向游戏中的平台列表）
    std::vector<Platform>* platforms = nullptr;

    int frameWidth = 0;
    int frameHeight = 0;
    int currentFrame = 0;   // 当前帧索引 (0-3)
    int currentRow = 1;     // 当前行 (0=下蹲, 1=向左, 2=向右)
    int lastDirectionRow = 1;
    int characterX = 0;     // 角色X位置
    int characterY = 0;     // 角色Y位置
    int baseMoveSpeed = 8;  // 基础移动速度
    int moveSpeed = 8;      // 当前移动速度
    int animationSpeed = 80; // 动画速度 (毫秒)
    int moveDirection = 0;  // 水平移动方向 (-1=左, 1=右, 0=停止)
    bool isCrouching = false; // 是否处于下蹲状态
    bool player1 = true;    // 是否是玩家1
    int health = 100;       // 角色生命值
    Weapon currentWeapon = FIST; // 当前武器
    int ballUses = 0;       // 实心球剩余使用次数
    int rifleAmmo = 0;      // 新增：步枪弹药数量
    int sniperAmmo = 0;     // 新增：狙击枪弹药数量
    bool isInvincible = false; // 新增：是否处于无敌状态
    bool isOnGrass = false; // 新增：是否在草地上
    bool isOnIce = false;   // 新增：是否在冰面上
    bool lightArmorEquipped = false; // 新增：是否装备锁子甲
    bool bulletproofVestEquipped = false; // 新增：是否装备防弹衣
    int vestDurability = 0; // 新增：防弹衣耐久度
    bool armorDamageEffect = false; // 新增：护甲受击效果标志
    QColor damageColor = Qt::red; // 新增：受击效果颜色

    // 肾上腺素效果相关 - 新增
    bool isAdrenalineActive = false; // 是否激活肾上腺素
    static constexpr int ADRENALINE_DURATION = 10000; // 10秒持续时间
    int adrenalineRemainingTime = 0; // 剩余持续时间（毫秒）

    // 重力相关变量
    const int GRAVITY = 1;          // 重力加速度
    const int JUMP_VELOCITY = -21;   // 跳跃初速度 (负值表示向上)
    int verticalVelocity = 0;       // 垂直速度
    bool isInAir = false;           // 是否在空中
    bool canJump = true;            // 是否可以跳跃
    bool doubleJumpUsed = false;     // 是否使用了二段跳
};

#endif // CHARACTER_H
