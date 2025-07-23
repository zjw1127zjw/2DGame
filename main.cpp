#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QStackedWidget>
#include <QTimer>
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <vector>
#include <algorithm>
#include <QVector>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QGridLayout>
#include <QSpacerItem>

// 前向声明
class Character;

// 平台结构体
struct Platform {
    int x, y, width, height;
    int type; // 0: 普通, 1: 草地, 2: 冰面
    Platform(int x, int y, int w, int h, int t = 0) : x(x), y(y), width(w), height(h), type(t) {}

    // 检查点是否在平台上
    bool contains(int px, int py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }

    // 检查矩形是否与平台相交
    bool intersects(int rx, int ry, int rw, int rh) const {
        return !(rx + rw < x || rx > x + width || ry + rh < y || ry > y + height);
    }

    // 获取平台顶部Y坐标
    int top() const { return y; }
};

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

// 道具基类
class Item : public QWidget {
    Q_OBJECT
public:
    enum ItemType { BANDAGE, MEDKIT, ADRENALINE, KNIFE, BALL, RIFLE, SNIPER, LIGHT_ARMOR, BULLETPROOF_VEST }; // 新增防弹衣类型

    Item(ItemType type, QWidget *parent = nullptr)
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
            itemPixmap = QPixmap(":/new/prefix1/res/knife.png"); // 小刀道具
            break;
        case BALL:
            itemPixmap = QPixmap(":/new/prefix1/res/ball.png"); // 实心球道具
            break;
        case RIFLE:
            itemPixmap = QPixmap(":/new/prefix1/res/AKM.png"); // 步枪道具
            break;
        case SNIPER:
            itemPixmap = QPixmap(":/new/prefix1/res/juji.png"); // 狙击枪道具
            break;
        case LIGHT_ARMOR: // 新增：锁子甲
            itemPixmap = QPixmap(":/new/prefix1/res/suo.png");
            break;
        case BULLETPROOF_VEST: // 新增：防弹衣
            itemPixmap = QPixmap(":/new/prefix1/res/fangdan.png");
            break;
        // 其他道具类型可以在这里扩展
        default:
            break;
        }

        // 调整道具大小（比角色稍小）
        int size = 40; // 道具尺寸
        if (!itemPixmap.isNull()) {
            itemPixmap = itemPixmap.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        setFixedSize(size, size);
    }

    // 更新位置（应用重力）
    void updatePosition(const std::vector<Platform>& platforms) {
        if (isOnGround) return; // 已经在地面上就不动了

        // 应用重力
        velocityY += GRAVITY;
        int newY = y() + velocityY;

        // 检查是否碰到平台或地面
        bool collided = false;
        for (const Platform& p : platforms) {
            // 检查道具底部是否在平台顶部
            if (newY + height() >= p.top() &&
                y() + height() <= p.top() + 10 &&  // 10像素容差
                x() + width() > p.x &&
                x() < p.x + p.width) {

                // 将道具放置在平台顶部
                move(x(), p.top() - height());
                velocityY = 0;
                isOnGround = true;
                collided = true;
                break;
            }
        }

        // 如果没有碰到平台，继续下落
        if (!collided) {
            move(x(), newY);

            // 检查是否掉出屏幕底部（视为地面）
            if (y() > 800) {
                move(x(), 800 - height());
                isOnGround = true;
            }
        }
    }

    // 检查点是否在道具范围内
    bool contains(int px, int py) const {
        return px >= x() && px <= x() + width() &&
               py >= y() && py <= y() + height();
    }

    // 获取道具类型
    ItemType getType() const { return itemType; }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        if (!itemPixmap.isNull()) {
            painter.drawPixmap(0, 0, itemPixmap);
        } else {
            // 如果没有图片，绘制一个占位符
            painter.setBrush(Qt::yellow);
            painter.drawRect(rect());
        }
    }

private:
    static constexpr int GRAVITY = 1; // 重力加速度

    ItemType itemType;
    QPixmap itemPixmap;
    int velocityY;
    bool isOnGround;
};

// 小刀攻击特效类
class KnifeAttackEffect : public QWidget {
    Q_OBJECT
public:
    KnifeAttackEffect(QWidget *parent = nullptr) : QWidget(parent) {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        effectTimer = new QTimer(this);
        connect(effectTimer, &QTimer::timeout, this, &KnifeAttackEffect::hideEffect);
    }

    // 开始小刀攻击动画
    void startAttack(bool isRight, int characterX, int characterY, int characterWidth, int characterHeight) {
        directionRight = isRight;
        visible = true;

        // 设置特效大小为角色大小的150%
        int effectWidth = characterWidth * 1.5;
        int effectHeight = characterHeight * 1.5;
        setFixedSize(effectWidth, effectHeight);

        // 设置特效位置（根据角色方向调整）
        int offsetX;
        if (directionRight) {
            offsetX = characterWidth * 0.6;  // 在角色右侧
            knifePixmap = QPixmap(":/new/prefix1/res/daoguang2.png"); // 右侧刀光
        } else {
            offsetX = -characterWidth * 1.1;  // 在角色左侧
            knifePixmap = QPixmap(":/new/prefix1/res/daoguang.png"); // 左侧刀光
        }

        // 缩放图片到特效大小
        if (!knifePixmap.isNull()) {
            knifePixmap = knifePixmap.scaled(effectWidth, effectHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        int posX = characterX + offsetX;
        int posY = characterY + (characterHeight - effectHeight)/2+10;
        move(posX, posY);

        // 启动动画定时器（0.5秒后隐藏）
        effectTimer->start(200);
        show();
        raise();
    }

    // 是否可见
    bool isVisible() const { return visible; }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        if (!visible || knifePixmap.isNull()) return;

        QPainter painter(this);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawPixmap(0, 0, knifePixmap);
    }

private slots:
    void hideEffect() {
        effectTimer->stop();
        visible = false;
        hide();
    }

private:
    QPixmap knifePixmap;
    QTimer *effectTimer;
    bool visible = false;
    bool directionRight = true;
};

// 攻击特效类 - 已修改为拳头特效
class AttackEffect : public QWidget {
    Q_OBJECT
public:
    AttackEffect(QWidget *parent = nullptr) : QWidget(parent) {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        animationTimer = new QTimer(this);
        connect(animationTimer, &QTimer::timeout, this, &AttackEffect::updateFrame);
    }

    // 开始攻击动画 - 已修改为拳头攻击
    void startAttack(bool isRight, int characterX, int characterY, int characterWidth, int characterHeight) {
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

    // 加载动画帧 - 已修改为使用拳头资源
    void loadFrames() {
        frames.clear();
        QString basePath = directionRight ?
                               ":/new/prefix1/res/sm_gs_superskill1_45_hit_%1.png" :
                               ":/new/prefix1/res/sm_gs_superskill1_225_hit_%1.png";

        for (int i = 1; i <= 10; i++) {
            QString path = basePath.arg(i, 4, 10, QChar('0'));
            QPixmap frame(path);
            if (!frame.isNull()) {
                frames.append(frame.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            } else {
                qDebug() << "加载攻击帧失败:" << path;
            }
        }
    }

    // 是否可见
    bool isVisible() const { return visible; }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        if (!visible || frames.isEmpty()) return;
        if (currentFrame >= frames.size()) return;

        QPainter painter(this);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawPixmap(0, 0, frames[currentFrame]);
    }

private slots:
    void updateFrame() {
        currentFrame++;
        if (currentFrame >= frames.size()) {
            animationTimer->stop();
            visible = false;
            hide();
        } else {
            update();
        }
    }

private:
    QVector<QPixmap> frames;
    QTimer *animationTimer;
    int currentFrame = 0;
    bool visible = false;
    bool directionRight = true;
};

// 实心球投射物类
class BallProjectile : public QWidget {
    Q_OBJECT
public:
    BallProjectile(int startX, int startY, bool directionRight, Character* thrower, QWidget *parent = nullptr)
        : QWidget(parent), directionRight(directionRight), thrower(thrower) {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);

        // 初始位置
        x = startX;
        y = startY;

        // 加载图片
        ballPixmap = QPixmap(":/new/prefix1/res/ball.png");
        if (!ballPixmap.isNull()) {
            // 设置大小（比道具大，与角色大小相似）
            ballPixmap = ballPixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            setFixedSize(ballPixmap.size());
        }

        // 初始速度：水平方向根据投掷方向，垂直方向为负（向上）
        velocityX = directionRight ? 10 : -10;
        velocityY = -15; // 向上

        // 移动到初始位置
        move(x, y);

        // 定时器更新位置
        updateTimer = new QTimer(this);
        connect(updateTimer, &QTimer::timeout, this, &BallProjectile::updatePosition);
        updateTimer->start(16); // 约60FPS
    }

    // 获取碰撞矩形
    QRect getRect() const {
        return QRect(x, y, width(), height());
    }

    // 获取投掷者
    Character* getThrower() const { return thrower; }

    // 是否活动状态
    bool isActive() const { return active; }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        if (!ballPixmap.isNull()) {
            painter.drawPixmap(0, 0, ballPixmap);
        } else {
            painter.setBrush(Qt::red);
            painter.drawEllipse(rect());
        }
    }

private slots:
    void updatePosition() {
        // 应用重力
        velocityY += GRAVITY;

        // 更新位置
        x += velocityX;
        y += velocityY;

        // 获取父窗口宽度（屏幕宽度）
        int screenWidth = parentWidget() ? parentWidget()->width() : 1200;

        // 检查是否碰到左右边界（距离边缘50像素时反弹）
        if (x < 5 && velocityX < 0) { // 左边界反弹
            velocityX = -velocityX; // 水平速度反向
            x = 5; // 防止球卡在边界
        } else if (x > screenWidth - 5 - width() && velocityX > 0) { // 右边界反弹
            velocityX = -velocityX; // 水平速度反向
            x = screenWidth - 5 - width(); // 防止球卡在边界
        }

        move(x, y);

        // 检查是否超出屏幕
        if (y > 800 || x < -100 || x > screenWidth + 100) {
            active = false; // 标记为非活动状态
            hide();
        }
    }

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

// 角色类 - 处理动画和移动
class Character : public QWidget {
    Q_OBJECT
public:
    enum Weapon { FIST, KNIFE, BALL, RIFLE, SNIPER }; // 武器类型

    Character(const QString& spritePath, bool isPlayer1, QWidget *parent = nullptr);
    ~Character() {}

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

    // 获取角色高度
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

// 子弹类的实现
Bullet::Bullet(int startX, int startY, bool directionRight, Character* shooter, int charWidth, int charHeight, QWidget *parent)
    : QWidget(parent), directionRight(directionRight), shooter(shooter),
    bulletSpeed(12) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);

    // 设置子弹大小（角色大小的200%）
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

// 角色类的实现
Character::Character(const QString& spritePath, bool isPlayer1, QWidget *parent)
    : QWidget(parent), player1(isPlayer1) {
    // 加载角色精灵图
    spriteSheet = QPixmap(spritePath);

    if (spriteSheet.isNull()) {
        qDebug() << "角色精灵图加载失败:" << spritePath;
    } else {
        // 精灵图参数设置 (4x4网格)
        frameWidth = spriteSheet.width() / 4;   // 每帧宽度
        frameHeight = spriteSheet.height() / 4;  // 每帧高度
        setFixedSize(frameWidth, frameHeight);
    }

    // 初始化生命值
    health = 100;

    // 初始化武器
    currentWeapon = FIST;

    // 动画定时器
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, [this]() {
        updateFrame();
    });

    // 水平移动定时器
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, [this]() {
        if (moveDirection != 0 && !isCrouching) { // 下蹲时不能移动
            // 持续水平移动
            int newX = characterX + moveDirection * moveSpeed;

            // 水平碰撞检测
            bool collision = false;
            if (platforms) {
                for (const Platform& p : *platforms) {
                    // 检查角色底部是否在平台上
                    bool onPlatform = (characterY + frameHeight >= p.y) &&
                                      (characterY + frameHeight <= p.y + 5) &&  // 5像素容差
                                      (newX + frameWidth > p.x) &&
                                      (newX < p.x + p.width);

                    // 检查侧面碰撞
                    bool sideCollision = p.intersects(newX, characterY, frameWidth, frameHeight);

                    // 如果角色在平台上移动，忽略侧面碰撞
                    if (!onPlatform && sideCollision) {
                        collision = true;
                        break;
                    }
                }
            }

            // 如果没有碰撞，则更新位置
            if (!collision) {
                characterX = newX;
                move(characterX, characterY);
                updateArmorPosition(); // 更新护甲位置
            }
        }
    });

    // 重力定时器
    gravityTimer = new QTimer(this);
    connect(gravityTimer, &QTimer::timeout, this, [this]() {
        applyGravity();
    });
    gravityTimer->start(16); // 约60FPS

    // 创建攻击特效
    attackEffect = new AttackEffect(parentWidget());
    attackEffect->hide();

    // 创建小刀攻击特效
    knifeEffect = new KnifeAttackEffect(parentWidget());
    knifeEffect->hide();

    // 无敌帧定时器 - 新增
    invincibleTimer = new QTimer(this);
    invincibleTimer->setSingleShot(true); // 单次触发
    connect(invincibleTimer, &QTimer::timeout, this, [this]() {
        isInvincible = false;
        armorDamageEffect = false; // 重置护甲受击效果
        update(); // 重绘以移除无敌效果
    });

    // 地形效果检测定时器
    terrainEffectTimer = new QTimer(this);
    connect(terrainEffectTimer, &QTimer::timeout, this, [this]() {
        checkTerrainEffects();
    });
    terrainEffectTimer->start(100); // 每100ms检测一次地形效果

    // 肾上腺素效果定时器
    adrenalineTimer = new QTimer(this);
    connect(adrenalineTimer, &QTimer::timeout, this, [this]() {
        if (isAdrenalineActive) {
            // 每250ms恢复1点生命值（相当于每秒4点）
            heal(1);

            // 更新剩余时间
            adrenalineRemainingTime -= 250;

            // 如果时间用完，移除肾上腺素效果
            if (adrenalineRemainingTime <= 0) {
                isAdrenalineActive = false;
                moveSpeed = baseMoveSpeed; // 恢复原始移动速度
                // 重新检查地形效果（冰面等）
                checkTerrainEffects();
                update(); // 重绘角色
            }
        }
    });

    // 加载小刀图片
    knifeRightPixmap = QPixmap(":/new/prefix1/res/knife.png");
    knifeLeftPixmap = QPixmap(":/new/prefix1/res/knife2.png");

    if (!knifeRightPixmap.isNull()) {
        knifeRightPixmap = knifeRightPixmap.scaled(frameWidth, frameHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    if (!knifeLeftPixmap.isNull()) {
        knifeLeftPixmap = knifeLeftPixmap.scaled(frameWidth, frameHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // 加载实心球图片
    ballPixmap = QPixmap(":/new/prefix1/res/ball.png");
    if (!ballPixmap.isNull()) {
        // 缩放到角色大小的80%
        int size = qMin(frameWidth, frameHeight) * 0.5;
        ballPixmap = ballPixmap.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // 加载步枪图片
    rifleRightPixmap = QPixmap(":/new/prefix1/res/AKM.png");
    rifleLeftPixmap = QPixmap(":/new/prefix1/res/AKM2.png");

    if (!rifleRightPixmap.isNull()) {
        // 缩放到角色大小的80%
        int width = frameWidth * 0.8;
        int height = frameHeight * 0.8;
        rifleRightPixmap = rifleRightPixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    if (!rifleLeftPixmap.isNull()) {
        // 缩放到角色大小的80%
        int width = frameWidth * 0.8;
        int height = frameHeight * 0.8;
        rifleLeftPixmap = rifleLeftPixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // 加载狙击枪图片
    sniperRightPixmap = QPixmap(":/new/prefix1/res/juji.png");
    sniperLeftPixmap = QPixmap(":/new/prefix1/res/juji2.png");

    if (!sniperRightPixmap.isNull()) {
        // 缩放到角色大小的80%
        int width = frameWidth * 0.8;
        int height = frameHeight * 0.8;
        sniperRightPixmap = sniperRightPixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    if (!sniperLeftPixmap.isNull()) {
        // 缩放到角色大小的80%
        int width = frameWidth * 0.8;
        int height = frameHeight * 0.8;
        sniperLeftPixmap = sniperLeftPixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // 步枪射击间隔定时器
    rifleShootTimer = new QTimer(this);
    rifleShootTimer->setSingleShot(true);

    // 狙击枪射击间隔定时器
    sniperShootTimer = new QTimer(this);
    sniperShootTimer->setSingleShot(true);

    // 初始化护甲标签（锁子甲）
    armorLabel = new QLabel(parentWidget());
    armorLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    armorLabel->setScaledContents(true);
    armorLabel->hide();

    // 初始化防弹衣标签
    vestLabel = new QLabel(parentWidget());
    vestLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    vestLabel->setScaledContents(true);
    vestLabel->hide();
}

void Character::setPos(int x, int y) {
    characterX = x;
    characterY = y;
    move(characterX, characterY);
    updateArmorPosition();
}

void Character::setPlatforms(std::vector<Platform>* p) {
    platforms = p;
}

void Character::setMoveDirection(int direction) {
    // 下蹲时不能移动
    if (isCrouching) return;

    moveDirection = direction;

    if (direction != 0) {
        // 设置动画方向
        currentRow = (direction < 0) ? 1 : 2;

        // 启动动画定时器
        if (!animationTimer->isActive()) {
            animationTimer->start(animationSpeed);
            currentFrame = 0; // 重置到第一帧
        }

        // 启动移动定时器
        if (!moveTimer->isActive()) {
            moveTimer->start(30); // 每30ms移动一次
        }
    } else {
        // 停止移动定时器
        moveTimer->stop();

        // 注意：不要停止动画定时器，让它完成当前循环
        // 当动画完成一轮后，会在updateFrame中自动停止
        currentFrame = 0; // 重置到第一帧
        update(); // 立即更新显示站立状态
    }
}

void Character::setCrouching(bool crouch) {
    if (isCrouching == crouch) return; // 状态未改变

    isCrouching = crouch;

    if (isCrouching) {
        // 进入下蹲状态
        if (currentRow != 0) { // 如果不是已经下蹲状态
            lastDirectionRow = currentRow;
        }
        currentRow = 0; // 第一行是下蹲图片
        currentFrame = 0; // 使用第一帧
        animationTimer->stop(); // 停止动画（下蹲是静态图片）
        update(); // 立即重绘

        // 如果当前在草地上，立即隐身
        if (isOnGrass) {
            setVisible(false);
        }
    } else {
        // 退出下蹲状态
        // 恢复到站立姿势（使用之前的移动方向或默认方向）
        currentRow = lastDirectionRow;
        if (moveDirection != 0) {
            // 如果有移动方向，恢复移动动画

            animationTimer->start(animationSpeed);
        } else {
            // 没有移动方向，使用默认站立姿势

            currentFrame = 0; // 第一帧
            update(); // 立即重绘
        }

        // 无论是否在草地上，退出下蹲状态时立即恢复可见
        setVisible(true);
    }
}

void Character::attack() {
    if (currentWeapon == FIST) {
        // 拳头攻击
        punch();
    } else if (currentWeapon == KNIFE) {
        // 小刀攻击
        knifeAttack();
    } else if (currentWeapon == BALL) {
        // 实心球攻击
        throwBall();
    } else if (currentWeapon == RIFLE) {
        // 步枪攻击
        rifleAttack();
    } else if (currentWeapon == SNIPER) {
        // 狙击枪攻击
        sniperAttack();
    }
}

void Character::knifeAttack() {
    // 计算攻击方向（右：true，左：false）
    bool attackRight = isFacingRight();

    // 启动小刀攻击特效
    knifeEffect->startAttack(attackRight, characterX, characterY, frameWidth, frameHeight);
    knifeEffect->raise(); // 确保特效在角色上方
}

void Character::punch() {
    // 计算攻击方向（右：true，左：false）
    bool attackRight = isFacingRight();

    // 启动攻击特效
    attackEffect->startAttack(attackRight, characterX, characterY, frameWidth, frameHeight);
    attackEffect->raise(); // 确保特效在角色上方
}

void Character::throwBall() {
    // 减少使用次数
    ballUses--;

    // 发射实心球信号
    bool throwRight = isFacingRight();
    emit ballThrown(characterX, characterY, throwRight, this); // 添加this指针作为投掷者

    // 如果使用次数为0，切换回拳头
    if (ballUses <= 0) {
        equipFist();
    }
}

void Character::rifleAttack() {
    // 检查射击间隔
    if (rifleShootTimer->isActive() || rifleAmmo <= 0) {
        return;
    }

    // 减少弹药
    rifleAmmo--;

    // 发射子弹
    bool shootRight = isFacingRight();
    int startX = characterX;
    int startY = characterY;

    if (shootRight) {
        // 面向右时，从角色右侧发射
        startX += frameWidth * 0.4;  // 增加这个值使子弹更靠右
    } else {
        // 面向左时，从角色左侧发射
        startX -= frameWidth * 0.1;  // 增加这个值使子弹更靠左
    }

    // 垂直位置调整（增加这个值使子弹更靠下）
    startY += frameHeight * 0.5;
    emit bulletShot(startX, startY, shootRight, this);

    // 设置射击间隔（0.5秒）
    rifleShootTimer->start(500);

    // 如果弹药耗尽，切换回拳头
    if (rifleAmmo <= 0) {
        equipFist();
    }
}

void Character::sniperAttack() {
    // 检查射击间隔
    if (sniperShootTimer->isActive() || sniperAmmo <= 0) {
        return;
    }

    // 减少弹药
    sniperAmmo--;

    // 发射子弹
    bool shootRight = isFacingRight();
    int startX = characterX;
    int startY = characterY;

    if (shootRight) {
        // 面向右时，从角色右侧发射
        startX += frameWidth * 0.4;  // 增加这个值使子弹更靠右
    } else {
        // 面向左时，从角色左侧发射
        startX -= frameWidth * 0.1;  // 增加这个值使子弹更靠左
    }

    // 垂直位置调整（增加这个值使子弹更靠下）
    startY += frameHeight * 0.5;
    emit sniperBulletShot(startX, startY, shootRight, this);

    // 设置射击间隔（2秒）
    sniperShootTimer->start(2000);

    // 如果弹药耗尽，切换回拳头
    if (sniperAmmo <= 0) {
        equipFist();
    }
}

void Character::equipKnife() {
    currentWeapon = KNIFE;
}

void Character::equipFist() {
    currentWeapon = FIST;
}

void Character::equipBall() {
    currentWeapon = BALL;
    ballUses = 3; // 可以使用3次
}

void Character::equipRifle() {
    currentWeapon = RIFLE;
    rifleAmmo = 20; // 20发子弹
}

void Character::equipSniper() {
    currentWeapon = SNIPER;
    sniperAmmo = 5; // 5发子弹
}

void Character::equipLightArmor() {
    // 移除其他护甲
    if (bulletproofVestEquipped) {
        bulletproofVestEquipped = false;
        vestLabel->hide();
    }

    lightArmorEquipped = true;

    // 加载护甲图片
    QPixmap armorPix(":/new/prefix1/res/dun.png");
    if (!armorPix.isNull()) {
        // 设置为角色大小的三倍
        int size = qMax(frameWidth, frameHeight) *0.5;
        armorLabel->setFixedSize(size*2, size);
        armorPix = armorPix.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        armorLabel->setPixmap(armorPix);
    }

    // 显示护甲图标
    armorLabel->show();
    armorLabel->raise();
    updateArmorPosition();
}

void Character::equipBulletproofVest() {
    // 移除其他护甲
    if (lightArmorEquipped) {
        lightArmorEquipped = false;
        armorLabel->hide();
    }

    bulletproofVestEquipped = true;
    vestDurability = 100; // 设置初始耐久度

    // 加载防弹衣图片
    QPixmap vestPix(":/new/prefix1/res/dun2.png");
    if (!vestPix.isNull()) {
        // 设置为角色大小的三倍
        int size = qMax(frameWidth, frameHeight) *0.5;
        vestLabel->setFixedSize(size, size);
        vestPix = vestPix.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        vestLabel->setPixmap(vestPix);
    }

    // 显示防弹衣图标
    vestLabel->show();
    vestLabel->raise();
    updateArmorPosition();
}

bool Character::isFacingRight() const {
    if (isCrouching) {
        return lastDirectionRow == 2;
    }
    return currentRow == 2;
}

Character::Weapon Character::getCurrentWeapon() const {
    return currentWeapon;
}

int Character::getBallUses() const {
    return ballUses;
}

int Character::getRifleAmmo() const {
    return rifleAmmo;
}

int Character::getSniperAmmo() const {
    return sniperAmmo;
}

void Character::jump() {
    // 下蹲时不能跳跃
    if (isCrouching) return;

    if (canJump) {
        // 应用跳跃速度
        verticalVelocity = JUMP_VELOCITY;
        canJump = false;

        // 如果是二段跳，设置标志
        if (isInAir) {
            doubleJumpUsed = true;
        }

        isInAir = true;
    }
}

int Character::bottom() const {
    return characterY + frameHeight;
}

int Character::getX() const { return characterX; }
int Character::getY() const { return characterY; }
int Character::getHeight() const { return frameHeight; }
int Character::getWidth() const { return frameWidth; }
bool Character::isCharacterCrouching() const { return isCrouching; }
AttackEffect* Character::getAttackEffect() const { return attackEffect; }
KnifeAttackEffect* Character::getKnifeEffect() const { return knifeEffect; }
int Character::getHealth() const { return health; }

void Character::takeDamage(int damage, Weapon damageSource) {
    // 如果处于无敌状态，则不扣血
    if (isInvincible) return;

    // 根据护甲类型调整伤害
    if (lightArmorEquipped) {
        if (damageSource == FIST) {
            damage = 0; // 免疫拳头伤害
        } else if (damageSource == KNIFE) {
            damage = 2; // 小刀伤害降为2点
        }
    }
    // 处理防弹衣
    else if (bulletproofVestEquipped) {
        // 只有枪械类武器会受到防弹衣影响
        if (damageSource == RIFLE) {
            // 步枪伤害降为2点，减少10点耐久
            damage = 2;
            vestDurability -= 10;
        } else if (damageSource == SNIPER) {
            // 狙击枪伤害降为10点，减少40点耐久
            damage = 10;
            vestDurability -= 40;
        }

        // 检查防弹衣耐久度
        if (vestDurability <= 0) {
            bulletproofVestEquipped = false;
            vestLabel->hide();
        }
    }

    // 设置受击效果颜色
    // 设置受击效果颜色（无论伤害是否大于0）
    if (lightArmorEquipped && (damageSource == FIST || damageSource == KNIFE)) {
        damageColor = Qt::yellow; // 护甲受击效果
    } else if (bulletproofVestEquipped && (damageSource == RIFLE || damageSource == SNIPER)) {
        damageColor = Qt::yellow; // 防弹衣受击效果
    } else if (damage > 0) {
        damageColor = Qt::red; // 普通受击效果
    }

    health -= damage;
    if (health < 0) health = 0;
    emit healthChanged(health);

    // 进入无敌状态0.3秒
    isInvincible = true;
    invincibleTimer->start(300); // 300毫秒 = 0.3秒
}

void Character::heal(int amount) {
    health += amount;
    if (health > 100) health = 100;
    emit healthChanged(health);
}

void Character::fullHeal() {
    health = 100;
    emit healthChanged(health);
}

QRect Character::getAttackRange() const {
    int attackWidth = frameWidth; // 攻击范围宽度为角色宽度
    int attackHeight = frameHeight * 0.8; // 攻击范围高度为角色高度的80%

    int attackX;
    if (isFacingRight()) {
        attackX = characterX + frameWidth; // 面向右时，攻击范围在右侧
    } else {
        attackX = characterX - attackWidth; // 面向左时，攻击范围在左侧
    }
    int attackY = characterY + frameHeight * 0.1; // 垂直位置稍微调整

    return QRect(attackX, attackY, attackWidth, attackHeight);
}

bool Character::isInvincibleState() const { return isInvincible; }

void Character::checkTerrainEffects() {
    if (!platforms) return;

    // 重置状态
    isOnGrass = false;
    isOnIce = false;

    // 检查角色是否在平台上
    for (const Platform& p : *platforms) {
        // 检查角色底部是否在平台上
        bool onPlatform = (characterY + frameHeight >= p.y) &&
                          (characterY + frameHeight <= p.y + 5) &&  // 5像素容差
                          (characterX + frameWidth > p.x) &&
                          (characterX < p.x + p.width);

        if (onPlatform) {
            if (p.type == 1) { // 草地
                isOnGrass = true;
            } else if (p.type == 2) { // 冰面
                isOnIce = true;
            }
        }
    }

    // 应用草地效果（隐身）：仅在草地上且下蹲时隐身
    if (isOnGrass && isCrouching) {
        setVisible(false); // 隐身：不可见
    } else if (isOnGrass && !isCrouching) {
        setVisible(true); // 在草地上但未下蹲：可见
    }
    // 注意：离开草地时的可见性在setCrouching(false)中已处理

    // 应用冰面效果（加速）
    if (isOnIce) {
        // 如果有肾上腺素效果，移动速度增加100%（基础50% + 冰面50%）
        moveSpeed = isAdrenalineActive ? baseMoveSpeed * 2.0 : baseMoveSpeed * 1.5;
    } else {
        // 如果有肾上腺素效果，移动速度增加50%
        moveSpeed = isAdrenalineActive ? baseMoveSpeed * 1.5 : baseMoveSpeed;
    }
}

void Character::activateAdrenaline() {
    // 如果已经激活，重置持续时间
    if (isAdrenalineActive) {
        adrenalineRemainingTime = ADRENALINE_DURATION;
        return;
    }

    // 设置肾上腺素状态
    isAdrenalineActive = true;
    adrenalineRemainingTime = ADRENALINE_DURATION;

    // 增加移动速度
    moveSpeed = baseMoveSpeed * 1.5; // 增加50%

    // 启动肾上腺素效果定时器
    if (!adrenalineTimer->isActive()) {
        adrenalineTimer->start(250); // 每250ms触发一次
    }

    // 重新检查地形效果（确保冰面效果正确应用）
    checkTerrainEffects();

    // 更新角色显示（可以添加视觉效果）
    update();
}

bool Character::isAdrenalineActiveState() const {
    return isAdrenalineActive;
}

void Character::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    if (spriteSheet.isNull()) {
        QWidget::paintEvent(event);
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // 绘制当前帧
    painter.drawPixmap(0, 0,
                       spriteSheet,
                       currentFrame * frameWidth,
                       currentRow * frameHeight,
                       frameWidth,
                       frameHeight);

    bool facingRight = isFacingRight();

    // 绘制小刀（如果装备了）
    if (currentWeapon == KNIFE) {
        if (facingRight && !knifeRightPixmap.isNull()) {
            painter.drawPixmap(0, 20, knifeRightPixmap);
        } else if (!facingRight && !knifeLeftPixmap.isNull()) {
            painter.drawPixmap(0, 20, knifeLeftPixmap);
        }
    }

    // 绘制实心球（如果装备了）
    if (currentWeapon == BALL && !ballPixmap.isNull()) {
        // 调整位置：在角色身前（稍微贴近身体）
        int offsetX, offsetY;
        int ballWidth = ballPixmap.width();
        int ballHeight = ballPixmap.height();

        if (isFacingRight()) {
            // 右侧：球从角色右侧边缘开始，球的左侧紧贴角色右侧
            offsetX = frameWidth * 0.48; // 向右调整
        } else {
            // 左侧：球从角色左侧边缘开始，球的右侧紧贴角色左侧
            offsetX = -ballWidth * 0.11; // 向左调整
        }

        // 垂直位置：球在角色高度的中间位置
        offsetY = (frameHeight - ballHeight) / 2 + frameHeight * 0.1+15;

        painter.drawPixmap(offsetX, offsetY, ballPixmap);
    }

    // 绘制步枪（如果装备了）
    if (currentWeapon == RIFLE && (!rifleRightPixmap.isNull() || !rifleLeftPixmap.isNull())) {
        QPixmap* riflePixmap = isFacingRight() ? &rifleRightPixmap : &rifleLeftPixmap;
        if (!riflePixmap->isNull()) {
            int offsetX, offsetY;
            int rifleWidth = riflePixmap->width();
            int rifleHeight = riflePixmap->height();

            if (isFacingRight()) {
                offsetX = frameWidth * 0.2; // 右侧
            } else {
                offsetX = -rifleWidth * 0.05; // 左侧
            }

            offsetY = (frameHeight - rifleHeight) / 2 +25;
            painter.drawPixmap(offsetX, offsetY, *riflePixmap);
        }
    }

    // 绘制狙击枪（如果装备了）
    if (currentWeapon == SNIPER && (!sniperRightPixmap.isNull() || !sniperLeftPixmap.isNull())) {
        QPixmap* sniperPixmap = isFacingRight() ? &sniperRightPixmap : &sniperLeftPixmap;
        if (!sniperPixmap->isNull()) {
            int offsetX, offsetY;
            int sniperWidth = sniperPixmap->width();
            int sniperHeight = sniperPixmap->height();

            if (isFacingRight()) {
                offsetX = frameWidth * 0.2; // 右侧
            } else {
                offsetX = -sniperWidth * 0.05; // 左侧
            }

            offsetY = (frameHeight - sniperHeight) / 2 +15;
            painter.drawPixmap(offsetX, offsetY, *sniperPixmap);
        }
    }

    // 绘制无敌状态效果
    if (isInvincible) {
        // 根据伤害类型设置不同颜色
        painter.fillRect(rect(), QColor(damageColor.red(), damageColor.green(), damageColor.blue(), 100));
    }

    // 绘制肾上腺素效果
    if (isAdrenalineActive) {
        // 半透明蓝色覆盖
        painter.fillRect(rect(), QColor(0, 100, 255, 100));
    }
}

void Character::updateFrame() {
    // 切换到下一帧
    currentFrame = (currentFrame + 1) % 4;
    update(); // 触发重绘

    // 如果移动停止且当前帧回到第一帧，停止动画
    if (moveDirection == 0 && currentFrame == 0) {
        animationTimer->stop();
    }
}

void Character::applyGravity() {
    // 应用重力加速度
    verticalVelocity += GRAVITY;

    // 计算新位置
    int newY = characterY + verticalVelocity;
    bool onPlatform = false;

    // 检测平台碰撞
    if (platforms) {
        for (const Platform& p : *platforms) {
            // 检查角色是否在平台顶部 - 修复了括号错误
            if (newY + frameHeight >= p.top() &&
                characterY + frameHeight <= p.top() + 5 &&  // 5像素容差
                characterX + frameWidth > p.x &&
                characterX < p.x + p.width &&
                verticalVelocity >= 0) {

                // 将角色放置在平台顶部
                characterY = p.top() - frameHeight;
                verticalVelocity = 0;
                isInAir = false;
                canJump = true;
                doubleJumpUsed = false;
                onPlatform = true;
                break;
            }
        }
    }

    // 如果没有在平台上，更新位置
    if (!onPlatform) {
        characterY = newY;

        // 检测是否掉出地图
        if (characterY > 800) {
            // 重置位置（在实际游戏中可能是扣血或重生）
            characterY = 100;
            characterX = 600;
            verticalVelocity = 0;
        }

        // 标记为在空中
        if (verticalVelocity != 0) {
            isInAir = true;
        }
    }

    // 更新位置
    move(characterX, characterY);
    updateArmorPosition();
}

void Character::updateArmorPosition() {
    if (armorLabel && armorLabel->isVisible()) {
        // 将护甲图标置于角色头顶中央
        int armorX = characterX - (armorLabel->width() - frameWidth) / 2;
        int armorY = characterY - armorLabel->height() + frameHeight * 0.5;
        armorLabel->move(armorX, armorY);
    }

    if (vestLabel && vestLabel->isVisible()) {
        // 将防弹衣图标置于角色头顶中央
        int vestX = characterX - (vestLabel->width() - frameWidth) / 2;
        int vestY = characterY - vestLabel->height() + frameHeight * 0.5;
        vestLabel->move(vestX, vestY);
    }
}

// 游戏界面类 - 处理键盘事件
class GameScreen : public QWidget {
    Q_OBJECT
public:
    GameScreen(QWidget *parent = nullptr);
    ~GameScreen() {}

    // 开始生成道具
    void startSpawningItems();

    // 公开设置背景方法
    void setBackground(const QPixmap &pixmap);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:
    void gameOver(int winner);  // 游戏结束信号，winner=1表示玩家1获胜，2表示玩家2获胜

private:
    // 创建游戏平台
    void createPlatforms();

    // 生成道具
    void spawnBandage();
    void spawnMedkit();
    void spawnAdrenaline();
    void spawnKnife();
    void spawnBall();
    void spawnRifle();
    void spawnSniper();
    void spawnLightArmor(); // 新增：生成锁子甲
    void spawnBulletproofVest(); // 新增：生成防弹衣

    // 更新道具位置
    void updateItems();

    // 更新血条显示
    void updateHealthBar(int player, int health);

    // 检查攻击碰撞
    void checkAttack();

    // 检查游戏结束条件
    void checkGameOver();

    // 检查道具拾取
    void checkItemPickup(Character* character);

    // 显示治疗特效
    void showHealEffect(Character* character, const QString& text);

    Character *character1; // 玩家1角色
    Character *character2; // 玩家2角色
    QLabel *background = nullptr;
    QTimer *platformDebugTimer; // 平台调试绘制定时器
    QTimer *attackCheckTimer;   // 攻击检测定时器
    QTimer *gameOverTimer;      // 游戏结束检测定时器
    QTimer *bandageSpawnTimer;  // 绷带生成定时器
    QTimer *medkitSpawnTimer;   // 急救包生成定时器
    QTimer *adrenalineSpawnTimer; // 新增：肾上腺素生成定时器
    QTimer *knifeSpawnTimer;    // 新增：小刀生成定时器
    QTimer *ballSpawnTimer;     // 新增：实心球生成定时器
    QTimer *rifleSpawnTimer;    // 新增：步枪生成定时器
    QTimer *sniperSpawnTimer;   // 新增：狙击枪生成定时器
    QTimer *lightArmorSpawnTimer; // 新增：锁子甲生成定时器
    QTimer *bulletproofVestSpawnTimer; // 新增：防弹衣生成定时器
    QTimer *itemUpdateTimer;    // 道具更新定时器
    QWidget *gameArea;          // 游戏区域容器

    // 血条相关
    QWidget *healthContainer1 = nullptr; // 玩家1血条容器
    QLabel *healthBar1 = nullptr;        // 玩家1血条（红色）
    QLabel *healthText1 = nullptr;       // 玩家1血量数值
    QWidget *healthContainer2 = nullptr; // 玩家2血条容器
    QLabel *healthBar2 = nullptr;        // 玩家2血条（红色）
    QLabel *healthText2 = nullptr;       // 玩家2血量数值

    // 平台列表
    std::vector<Platform> platforms;

    // 道具列表
    QList<Item*> items;

    // 实心球投射物列表
    QList<BallProjectile*> ballProjectiles;

    // 子弹列表
    QList<Bullet*> bullets;

    // 狙击枪子弹列表
    QList<Bullet*> sniperBullets;

    // 高台图片标签
    QLabel *grassLabel = nullptr; // 左侧高台草地图片标签
    QLabel *snowLabel = nullptr;  // 右侧高台雪堆图片标签

    // 调试选项
    bool drawAttackRange = false; // 是否绘制攻击范围
};

GameScreen::GameScreen(QWidget *parent) : QWidget(parent) {
    // 设置焦点策略以接收键盘事件
    setFocusPolicy(Qt::StrongFocus);

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 创建顶部血条容器
    QWidget *topBar = new QWidget(this);
    topBar->setFixedHeight(50);
    topBar->setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 10, 20, 10);

    // 玩家1血条容器
    healthContainer1 = new QWidget(topBar);
    healthContainer1->setFixedSize(204, 24);
    healthContainer1->setStyleSheet("background-color: black; border: 2px solid #555;");

    // 玩家1血条（红色）
    healthBar1 = new QLabel(healthContainer1);
    healthBar1->setGeometry(2, 2, 200, 20); // 内部填充
    healthBar1->setStyleSheet("background-color: red;");

    // 玩家1血量文本
    healthText1 = new QLabel(healthContainer1);
    healthText1->setGeometry(0, 0, 200, 20);
    healthText1->setAlignment(Qt::AlignCenter);
    healthText1->setStyleSheet("color: white; font-weight: bold;");
    healthText1->setText("100");

    // 玩家2血条容器
    healthContainer2 = new QWidget(topBar);
    healthContainer2->setFixedSize(204, 24);
    healthContainer2->setStyleSheet("background-color: black; border: 2px solid #555;");

    // 玩家2血条（红色）
    healthBar2 = new QLabel(healthContainer2);
    healthBar2->setGeometry(2, 2, 200, 20); // 内部填充
    healthBar2->setStyleSheet("background-color: red;");

    // 玩家2血量文本
    healthText2 = new QLabel(healthContainer2);
    healthText2->setGeometry(0, 0, 200, 20);
    healthText2->setAlignment(Qt::AlignCenter);
    healthText2->setStyleSheet("color: white; font-weight: bold;");
    healthText2->setText("100");

    // 添加到顶部布局
    topLayout->addWidget(healthContainer1);
    topLayout->addStretch();
    topLayout->addWidget(healthContainer2);

    // 游戏区域容器（角色将放在这里）
    gameArea = new QWidget(this);
    gameArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 添加到主布局
    mainLayout->addWidget(topBar);
    mainLayout->addWidget(gameArea);

    // 创建平台
    createPlatforms();

    // 创建角色1 (玩家1)
    character1 = new Character(":/new/prefix1/res/role1.png", true, gameArea);
    character1->setPlatforms(&platforms);

    // 设置玩家1出生点在左侧平台 - 修改
    character1->setPos(200, 450 - character1->getHeight()); // 地面平台高度450
    character1->raise(); // 确保角色显示在血条之上

    // 连接生命值变化信号
    connect(character1, &Character::healthChanged, this, [this](int health) {
        updateHealthBar(1, health);
    });

    // 创建角色2 (玩家2)
    character2 = new Character(":/new/prefix1/res/role2.png", false, gameArea);
    character2->setPlatforms(&platforms);

    // 设置玩家2出生点在右侧平台 - 修改
    character2->setPos(900, 450 - character2->getHeight()); // 地面平台高度450
    character2->raise(); // 确保角色显示在血条之上

    // 连接生命值变化信号
    connect(character2, &Character::healthChanged, this, [this](int health) {
        updateHealthBar(2, health);
    });

    // 连接实心球投掷信号
    connect(character1, &Character::ballThrown, this, [this](int x, int y, bool right, Character* thrower) {
        BallProjectile* ball = new BallProjectile(x, y, right, thrower, gameArea);
        ball->show();
        ball->raise();
        ballProjectiles.append(ball);
    });

    connect(character2, &Character::ballThrown, this, [this](int x, int y, bool right, Character* thrower) {
        BallProjectile* ball = new BallProjectile(x, y, right, thrower, gameArea);
        ball->show();
        ball->raise();
        ballProjectiles.append(ball);
    });

    // 连接步枪子弹发射信号
    connect(character1, &Character::bulletShot, this, [this](int x, int y, bool right, Character* shooter) {
        Bullet* bullet = new Bullet(x, y, right, shooter,
                                    character1->getWidth(), character1->getHeight(),
                                    gameArea);
        bullet->show();
        bullet->raise();
        bullets.append(bullet);
    });

    connect(character2, &Character::bulletShot, this, [this](int x, int y, bool right, Character* shooter) {
        Bullet* bullet = new Bullet(x, y, right, shooter,
                                    character2->getWidth(), character2->getHeight(),
                                    gameArea);
        bullet->show();
        bullet->raise();
        bullets.append(bullet);
    });

    // 连接狙击枪子弹发射信号
    connect(character1, &Character::sniperBulletShot, this, [this](int x, int y, bool right, Character* shooter) {
        Bullet* bullet = new Bullet(x, y, right, shooter,
                                    character1->getWidth(), character1->getHeight(),
                                    gameArea);
        // 设置狙击枪子弹图片 - 使用新方法
        if (right) {
            bullet->setBulletPixmap(QPixmap(":/new/prefix1/res/bulletj1.png"));
        } else {
            bullet->setBulletPixmap(QPixmap(":/new/prefix1/res/bulletj2.png"));
        }
        bullet->show();
        bullet->raise();
        sniperBullets.append(bullet);
    });

    connect(character2, &Character::sniperBulletShot, this, [this](int x, int y, bool right, Character* shooter) {
        Bullet* bullet = new Bullet(x, y, right, shooter,
                                    character2->getWidth(), character2->getHeight(),
                                    gameArea);
        // 设置狙击枪子弹图片 - 使用新方法
        if (right) {
            bullet->setBulletPixmap(QPixmap(":/new/prefix1/res/bulletj1.png"));
        } else {
            bullet->setBulletPixmap(QPixmap(":/new/prefix1/res/bulletj2.png"));
        }
        bullet->show();
        bullet->raise();
        sniperBullets.append(bullet);
    });

    // 绘制平台（调试用）
    platformDebugTimer = new QTimer(this);
    connect(platformDebugTimer, &QTimer::timeout, this, [this]() {
        update();
    });
    platformDebugTimer->start(100);

    // 创建攻击检测定时器
    attackCheckTimer = new QTimer(this);
    connect(attackCheckTimer, &QTimer::timeout, this, &GameScreen::checkAttack);
    attackCheckTimer->start(50); // 每50ms检测一次攻击

    // 创建游戏结束检测定时器
    gameOverTimer = new QTimer(this);
    connect(gameOverTimer, &QTimer::timeout, this, &GameScreen::checkGameOver);
    gameOverTimer->start(100); // 每100ms检测一次游戏结束

    // 创建左侧高台草地图片标签
    grassLabel = new QLabel(gameArea);
    grassLabel->setPixmap(QPixmap(":/new/prefix1/res/grass.png").scaled(210, 60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    grassLabel->setGeometry(210, 250, 210, 60);
    grassLabel->lower(); // 确保在角色之下

    // 创建右侧高台雪堆图片标签
    snowLabel = new QLabel(gameArea);
    snowLabel->setPixmap(QPixmap(":/new/prefix1/res/xuedui.png").scaled(210, 60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    snowLabel->setGeometry(775, 250, 210, 60);
    snowLabel->lower(); // 确保在角色之下

    // 创建道具生成定时器
    bandageSpawnTimer = new QTimer(this);
    connect(bandageSpawnTimer, &QTimer::timeout, this, &GameScreen::spawnBandage);

    medkitSpawnTimer = new QTimer(this);
    connect(medkitSpawnTimer, &QTimer::timeout, this, &GameScreen::spawnMedkit);

    // 新增：肾上腺素生成定时器
    adrenalineSpawnTimer = new QTimer(this);
    connect(adrenalineSpawnTimer, &QTimer::timeout, this, &GameScreen::spawnAdrenaline);

    // 新增：小刀生成定时器
    knifeSpawnTimer = new QTimer(this);
    connect(knifeSpawnTimer, &QTimer::timeout, this, &GameScreen::spawnKnife);

    // 新增：实心球生成定时器
    ballSpawnTimer = new QTimer(this);
    connect(ballSpawnTimer, &QTimer::timeout, this, &GameScreen::spawnBall);

    // 新增：步枪生成定时器
    rifleSpawnTimer = new QTimer(this);
    connect(rifleSpawnTimer, &QTimer::timeout, this, &GameScreen::spawnRifle);

    // 新增：狙击枪生成定时器
    sniperSpawnTimer = new QTimer(this);
    connect(sniperSpawnTimer, &QTimer::timeout, this, &GameScreen::spawnSniper);

    // 新增：锁子甲生成定时器
    lightArmorSpawnTimer = new QTimer(this);
    connect(lightArmorSpawnTimer, &QTimer::timeout, this, &GameScreen::spawnLightArmor);

    // 新增：防弹衣生成定时器
    bulletproofVestSpawnTimer = new QTimer(this);
    connect(bulletproofVestSpawnTimer, &QTimer::timeout, this, &GameScreen::spawnBulletproofVest);

    // 道具更新定时器（立即启动）
    itemUpdateTimer = new QTimer(this);
    connect(itemUpdateTimer, &QTimer::timeout, this, &GameScreen::updateItems);
    itemUpdateTimer->start(16); // 约60FPS
}

void GameScreen::setBackground(const QPixmap &pixmap) {
    if (!pixmap.isNull()) {
        background = new QLabel(this);
        background->setPixmap(pixmap);
        background->setGeometry(0, 0, width(), height());
        background->lower(); // 背景在底层

        // 确保图片在背景之上
        if (grassLabel) grassLabel->raise();
        if (snowLabel) snowLabel->raise();
    }
}

void GameScreen::startSpawningItems() {
    bandageSpawnTimer->start(20000); // 每20秒生成一个绷带
    medkitSpawnTimer->start(30000);  // 每30秒生成一个急救包
    adrenalineSpawnTimer->start(60000); // 每60秒生成一个肾上腺素
    knifeSpawnTimer->start(45000);     // 每45秒生成一个小刀
    ballSpawnTimer->start(60000);      // 每60秒生成一个实心球
    rifleSpawnTimer->start(70000);     // 每70秒生成一个步枪
    sniperSpawnTimer->start(90000);    // 每90秒生成一个狙击枪
    lightArmorSpawnTimer->start(55000); // 每55秒生成一个锁子甲
    bulletproofVestSpawnTimer->start(65000); // 每65秒生成一个防弹衣
}

void GameScreen::createPlatforms() {
    // 底部大地板（覆盖整个屏幕底部） - 修改为连续地面
    platforms.push_back(Platform(100, 450, 1000, 100, 0));  // 从x=100到x=1100，普通类型

    // 两侧高台
    platforms.push_back(Platform(210, 280, 210, 1, 1));   // 左侧高台（草地）
    platforms.push_back(Platform(775, 280, 210, 1, 2));   // 右侧高台（冰面）

    // 中间高台
    platforms.push_back(Platform(500, 100, 200, 1, 0));   // 中间高台，普通类型
}

void GameScreen::spawnBandage() {
    // 在屏幕顶部随机位置生成绷带
    int x = QRandomGenerator::global()->bounded(100, 1000);
    Item* bandage = new Item(Item::BANDAGE, gameArea);
    bandage->move(x, 0);
    bandage->show();
    bandage->raise(); // 确保在背景之上
    items.append(bandage);
}

void GameScreen::spawnMedkit() {
    // 在屏幕顶部随机位置生成急救包
    int x = QRandomGenerator::global()->bounded(100, 1000);
    Item* medkit = new Item(Item::MEDKIT, gameArea);
    medkit->move(x, 0);
    medkit->show();
    medkit->raise(); // 确保在背景之上
    items.append(medkit);
}

void GameScreen::spawnAdrenaline() {
    // 在屏幕顶部随机位置生成肾上腺素
    int x = QRandomGenerator::global()->bounded(100, 1000);
    Item* adrenaline = new Item(Item::ADRENALINE, gameArea);
    adrenaline->move(x, 0);
    adrenaline->show();
    adrenaline->raise(); // 确保在背景之上
    items.append(adrenaline);
}

void GameScreen::spawnKnife() {
    // 在屏幕顶部随机位置生成小刀
    int x = QRandomGenerator::global()->bounded(100, 1000);
    Item* knife = new Item(Item::KNIFE, gameArea);
    knife->move(x, 0);
    knife->show();
    knife->raise(); // 确保在背景之上
    items.append(knife);
}

void GameScreen::spawnBall() {
    // 在屏幕顶部随机位置生成实心球
    int x = QRandomGenerator::global()->bounded(100, 1000);
    Item* ball = new Item(Item::BALL, gameArea);
    ball->move(x, 0);
    ball->show();
    ball->raise(); // 确保在背景之上
    items.append(ball);
}

void GameScreen::spawnRifle() {
    // 在屏幕顶部随机位置生成步枪
    int x = QRandomGenerator::global()->bounded(100, 1000);
    Item* rifle = new Item(Item::RIFLE, gameArea);
    rifle->move(x, 0);
    rifle->show();
    rifle->raise(); // 确保在背景之上
    items.append(rifle);
}

void GameScreen::spawnSniper() {
    // 在屏幕顶部随机位置生成狙击枪
    int x = QRandomGenerator::global()->bounded(100, 1000);
    Item* sniper = new Item(Item::SNIPER, gameArea);
    sniper->move(x, 0);
    sniper->show();
    sniper->raise(); // 确保在背景之上
    items.append(sniper);
}

void GameScreen::spawnLightArmor() {
    // 在屏幕顶部随机位置生成锁子甲
    int x = QRandomGenerator::global()->bounded(100, 1000);
    Item* lightArmor = new Item(Item::LIGHT_ARMOR, gameArea);
    lightArmor->move(x, 0);
    lightArmor->show();
    lightArmor->raise(); // 确保在背景之上
    items.append(lightArmor);
}

void GameScreen::spawnBulletproofVest() {
    // 在屏幕顶部随机位置生成防弹衣
    int x = QRandomGenerator::global()->bounded(100, 1000);
    Item* vest = new Item(Item::BULLETPROOF_VEST, gameArea);
    vest->move(x, 0);
    vest->show();
    vest->raise(); // 确保在背景之上
    items.append(vest);
}

void GameScreen::updateItems() {
    for (Item* item : items) {
        item->updatePosition(platforms);
    }
}

void GameScreen::updateHealthBar(int player, int health) {
    QLabel* bar = (player == 1) ? healthBar1 : healthBar2;
    QLabel* text = (player == 1) ? healthText1 : healthText2;

    // 计算血条宽度
    int width = (health * 200) / 100;
    if (width < 0) width = 0;

    bar->setFixedWidth(width);
    text->setText(QString::number(health));

    // 血量低于20%时变黄
    if (health < 20) {
        bar->setStyleSheet("background-color: yellow;");
    } else {
        bar->setStyleSheet("background-color: red;");
    }
}

void GameScreen::checkAttack() {
    // 为两个角色创建矩形
    QRect charRect1(character1->getX(), character1->getY(),
                    character1->getWidth(), character1->getHeight());
    QRect charRect2(character2->getX(), character2->getY(),
                    character2->getWidth(), character2->getHeight());

    // 检查玩家1是否攻击到玩家2
    if (character1->getAttackEffect()->isVisible() ||
        character1->getKnifeEffect()->isVisible()) {

        QRect attackRange = character1->getAttackRange();

        if (attackRange.intersects(charRect2)) {
            // 获取攻击者状态和目标状态
            bool shooterCrouching = character1->isCharacterCrouching();
            bool targetCrouching = character2->isCharacterCrouching();

            // 应用新规则：
            // 1. 攻击者下蹲时，无论目标状态如何都能击中
            // 2. 攻击者站立时，只有目标站立时才能击中
            if (shooterCrouching || !targetCrouching) {
                // 根据武器类型决定伤害值
                int damage = (character1->getCurrentWeapon() == Character::FIST) ? 2 : 5;
                character2->takeDamage(damage, character1->getCurrentWeapon());
            }
        }
    }

    // 检查玩家2是否攻击到玩家1
    if (character2->getAttackEffect()->isVisible() ||
        character2->getKnifeEffect()->isVisible()) {

        QRect attackRange = character2->getAttackRange();

        if (attackRange.intersects(charRect1)) {
            // 获取攻击者状态和目标状态
            bool shooterCrouching = character2->isCharacterCrouching();
            bool targetCrouching = character1->isCharacterCrouching();

            // 应用相同的规则
            if (shooterCrouching || !targetCrouching) {
                // 根据武器类型决定伤害值
                int damage = (character2->getCurrentWeapon() == Character::FIST) ? 2 : 5;
                character1->takeDamage(damage, character2->getCurrentWeapon());
            }
        }
    }

    // 检查实心球碰撞 - 只检测对方角色
    for (int i = ballProjectiles.size() - 1; i >= 0; i--) {
        BallProjectile* ball = ballProjectiles[i];

        // 检查实心球是否活动
        if (!ball->isActive()) {
            ball->deleteLater();
            ballProjectiles.removeAt(i);
            continue;
        }

        QRect ballRect = ball->getRect();

        // 获取投掷者
        Character* thrower = ball->getThrower();

        // 只检测对方角色
        Character* target = (thrower == character1) ? character2 : character1;

        QRect charRect = (thrower == character1) ? charRect2 : charRect1;

        if (ballRect.intersects(charRect)) {
            // 获取攻击者状态和目标状态
            bool throwerCrouching = thrower->isCharacterCrouching();
            bool targetCrouching = target->isCharacterCrouching();

            // 应用相同的规则
            if (throwerCrouching || !targetCrouching) {
                target->takeDamage(20, Character::BALL);
                ball->deleteLater();
                ballProjectiles.removeAt(i);
                continue;
            }
        }

        // 检测是否与平台碰撞（碰到平台也消失）
        bool hitPlatform = false;
        for (const Platform& p : platforms) {
            if (p.intersects(ballRect.x(), ballRect.y(), ballRect.width(), ballRect.height())) {
                hitPlatform = true;
                break;
            }
        }
        if (hitPlatform) {
            ball->deleteLater();
            ballProjectiles.removeAt(i);
        }
    }

    // 检查子弹碰撞
    for (int i = bullets.size() - 1; i >= 0; i--) {
        Bullet* bullet = bullets[i];

        // 检查子弹是否活动
        if (!bullet->isActive()) {
            bullet->deleteLater();
            bullets.removeAt(i);
            continue;
        }

        QRect bulletRect = bullet->getRect();

        // 获取射击者
        Character* shooter = bullet->getShooter();

        // 只检测对方角色
        Character* target = (shooter == character1) ? character2 : character1;

        QRect charRect = (shooter == character1) ? charRect2 : charRect1;
        bool targetCrouching = target->isCharacterCrouching();
        if (bulletRect.intersects(charRect)) {
            // 获取攻击者状态和目标状态
            bool shooterCrouching = shooter->isCharacterCrouching();
            bool targetCrouching = target->isCharacterCrouching();

            // 应用相同的规则
            if (shooterCrouching || !targetCrouching) {
                target->takeDamage(5, Character::RIFLE);
                bullet->setActive(false);
                bullet->hide();
                continue;
            }
        }

        // 检测是否与平台碰撞（碰到平台消失）
        /*bool hitPlatform = false;
        for (const Platform& p : platforms) {
            if (p.intersects(bulletRect.x(), bulletRect.y(), bulletRect.width(), bulletRect.height())) {
                bullet->setActive(false);
                bullet->hide();
                hitPlatform = true;
                break;
            }
        }*/
    }

    // 检查狙击枪子弹碰撞
    for (int i = sniperBullets.size() - 1; i >= 0; i--) {
        Bullet* bullet = sniperBullets[i];

        // 检查子弹是否活动
        if (!bullet->isActive()) {
            bullet->deleteLater();
            sniperBullets.removeAt(i);
            continue;
        }

        QRect bulletRect = bullet->getRect();

        // 获取射击者
        Character* shooter = bullet->getShooter();

        // 只检测对方角色
        Character* target = (shooter == character1) ? character2 : character1;

        QRect charRect = (shooter == character1) ? charRect2 : charRect1;

        bool targetCrouching = target->isCharacterCrouching();

        if (bulletRect.intersects(charRect)) {
            // 获取攻击者状态和目标状态
            bool shooterCrouching = shooter->isCharacterCrouching();
            bool targetCrouching = target->isCharacterCrouching();

            // 应用相同的规则
            if (shooterCrouching || !targetCrouching) {
                target->takeDamage(30, Character::SNIPER);
                bullet->setActive(false);
                bullet->hide();
                continue;
            }
        }

        // 检测是否与平台碰撞（碰到平台消失）
        /*bool hitPlatform = false;
        for (const Platform& p : platforms) {
            if (p.intersects(bulletRect.x(), bulletRect.y(), bulletRect.width(), bulletRect.height())) {
                bullet->setActive(false);
                bullet->hide();
                hitPlatform = true;
                break;
            }
        }*/
    }
}

void GameScreen::checkGameOver() {
    if (character1->getHealth() <= 0) {
        emit gameOver(2); // 玩家2胜利
    } else if (character2->getHealth() <= 0) {
        emit gameOver(1); // 玩家1胜利
    }
}

void GameScreen::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::green);
    painter.setBrush(QBrush(QColor(100, 200, 100, 150)));

    // 绘制所有平台
    for (const Platform& p : platforms) {
        painter.drawRect(p.x, p.y, p.width, p.height);
    }

    // 绘制下蹲状态提示
    if (character1->isCharacterCrouching()) {
        painter.setPen(Qt::red);
        painter.drawText(10, 70, "玩家1: 下蹲状态");
    }

    if (character2->isCharacterCrouching()) {
        painter.setPen(Qt::blue);
        painter.drawText(10, 90, "玩家2: 下蹲状态");
    }

    // 绘制攻击范围（调试用）
    if (drawAttackRange) {
        painter.setPen(Qt::red);
        painter.setBrush(Qt::NoBrush);

        // 绘制玩家1攻击范围
        QRect attackRange1 = character1->getAttackRange();
        painter.drawRect(attackRange1);

        // 绘制玩家2攻击范围
        QRect attackRange2 = character2->getAttackRange();
        painter.drawRect(attackRange2);
    }

    // 绘制无敌状态提示 - 修改：使用公共方法
    if (character1->getHealth() > 0 && character1->isInvincibleState()) {
        painter.setPen(Qt::red);
        painter.drawText(character1->getX(), character1->getY() - 20, "无敌");
    }
    if (character2->getHealth() > 0 && character2->isInvincibleState()) {
        painter.setPen(Qt::red);
        painter.drawText(character2->getX(), character2->getY() - 20, "无敌");
    }

    // 绘制肾上腺素状态提示 - 新增
    if (character1->isAdrenalineActiveState()) {
        painter.setPen(Qt::blue);
        painter.drawText(character1->getX(), character1->getY() - 40, "肾上腺素激活");
    }
    if (character2->isAdrenalineActiveState()) {
        painter.setPen(Qt::blue);
        painter.drawText(character2->getX(), character2->getY() - 40, "肾上腺素激活");
    }

    // 绘制武器状态提示
    painter.setPen(Qt::white);
    if (character1->getCurrentWeapon() == Character::KNIFE) {
        painter.drawText(character1->getX(), character1->getY() - 60, "装备: 小刀");
    }
    if (character2->getCurrentWeapon() == Character::KNIFE) {
        painter.drawText(character2->getX(), character2->getY() - 60, "装备: 小刀");
    }
    if (character1->getCurrentWeapon() == Character::BALL) {
        painter.drawText(character1->getX(), character1->getY() - 80,
                         QString("装备: 实心球 (%1/3)").arg(character1->getBallUses()));
    }
    if (character2->getCurrentWeapon() == Character::BALL) {
        painter.drawText(character2->getX(), character2->getY() - 80,
                         QString("装备: 实心球 (%1/3)").arg(character2->getBallUses()));
    }
    if (character1->getCurrentWeapon() == Character::RIFLE) {
        painter.drawText(character1->getX(), character1->getY() - 100,
                         QString("装备: 步枪 (%1/20)").arg(character1->getRifleAmmo()));
    }
    if (character2->getCurrentWeapon() == Character::RIFLE) {
        painter.drawText(character2->getX(), character2->getY() - 100,
                         QString("装备: 步枪 (%1/20)").arg(character2->getRifleAmmo()));
    }
    if (character1->getCurrentWeapon() == Character::SNIPER) {
        painter.drawText(character1->getX(), character1->getY() - 120,
                         QString("装备: 狙击枪 (%1/5)").arg(character1->getSniperAmmo()));
    }
    if (character2->getCurrentWeapon() == Character::SNIPER) {
        painter.drawText(character2->getX(), character2->getY() - 120,
                         QString("装备: 狙击枪 (%1/5)").arg(character2->getSniperAmmo()));
    }

    // 绘制护甲状态提示
    if (character1->hasLightArmor()) {
        painter.setPen(Qt::yellow);
        painter.drawText(character1->getX(), character1->getY() - 140, "装备: 锁子甲");
    } else if (character1->hasBulletproofVest()) {
        painter.setPen(Qt::yellow);
        painter.drawText(character1->getX(), character1->getY() - 140,
                         QString("装备: 防弹衣 (%1/100)").arg(character1->getVestDurability()));
    }

    if (character2->hasLightArmor()) {
        painter.setPen(Qt::yellow);
        painter.drawText(character2->getX(), character2->getY() - 140, "装备: 锁子甲");
    } else if (character2->hasBulletproofVest()) {
        painter.setPen(Qt::yellow);
        painter.drawText(character2->getX(), character2->getY() - 140,
                         QString("装备: 防弹衣 (%1/100)").arg(character2->getVestDurability()));
    }

    // 绘制地形效果提示
    painter.drawText(10, 110, "左侧高台: 按住下蹲键(S/↓)隐身");
    painter.drawText(10, 130, "右侧高台: 移动速度增加50%");
}

void GameScreen::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_A: // A键 - 角色1向左移动
        character1->setMoveDirection(-1);
        break;
    case Qt::Key_D: // D键 - 角色1向右移动
        character1->setMoveDirection(1);
        break;
    case Qt::Key_W: // W键 - 角色1跳跃
        character1->jump();
        break;
    case Qt::Key_S: // S键 - 角色1下蹲
        character1->setCrouching(true);

        // 检查是否拾取道具
        checkItemPickup(character1);
        break;
    case Qt::Key_F: // F键 - 角色1攻击
        character1->attack();
        break;
    case Qt::Key_Left: // 左箭头 - 角色2向左移动
        character2->setMoveDirection(-1);
        break;
    case Qt::Key_Right: // 右箭头 - 角色2向右移动
        character2->setMoveDirection(1);
        break;
    case Qt::Key_Up: // 上箭头 - 角色2跳跃
        character2->jump();
        break;
    case Qt::Key_Down: // 下箭头 - 角色2下蹲
        character2->setCrouching(true);

        // 检查是否拾取道具
        checkItemPickup(character2);
        break;
    case Qt::Key_L: // 1键 - 角色2攻击
        character2->attack();
        break;
    case Qt::Key_R: // R键 - 切换攻击范围显示（调试用）
        drawAttackRange = !drawAttackRange;
        update();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void GameScreen::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_A: // A键释放
    case Qt::Key_D: // D键释放
        character1->setMoveDirection(0); // 停止角色1移动
        break;
    case Qt::Key_S: // S键释放 - 停止下蹲
        character1->setCrouching(false);
        break;
    case Qt::Key_Left: // 左箭头释放
    case Qt::Key_Right: // 右箭头释放
        character2->setMoveDirection(0); // 停止角色2移动
        break;
    case Qt::Key_Down: // 下箭头释放 - 停止下蹲
        character2->setCrouching(false);
        break;
    default:
        QWidget::keyReleaseEvent(event);
    }
}

void GameScreen::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (background) {
        background->setGeometry(0, 0, width(), height());
    }
}

void GameScreen::checkItemPickup(Character* character) {
    // 获取角色位置
    int charX = character->getX();
    int charY = character->getY();
    int charWidth = character->getWidth();
    int charHeight = character->getHeight();

    // 计算角色底部中心点（下蹲时拾取位置）
    int pickupX = charX + charWidth / 2;
    int pickupY = charY + charHeight - 10; // 角色脚部附近

    // 遍历所有道具
    for (int i = items.size() - 1; i >= 0; i--) {
        Item* item = items[i];

        // 检查道具是否在角色拾取范围内
        if (item->contains(pickupX, pickupY)) {
            // 根据道具类型应用效果
            if (item->getType() == Item::BANDAGE) {
                // 绷带恢复20点生命值
                character->heal(20);

                // 显示恢复效果
                showHealEffect(character, "+20 HP");
            }
            else if (item->getType() == Item::MEDKIT) {
                // 急救包恢复全部生命值
                character->fullHeal();

                // 显示恢复效果
                showHealEffect(character, "+100 HP");
            }
            else if (item->getType() == Item::ADRENALINE) { // 新增肾上腺素处理
                // 激活肾上腺素效果
                character->activateAdrenaline();

                // 显示效果
                showHealEffect(character, "肾上腺素激活!");
            }
            else if (item->getType() == Item::KNIFE) { // 新增小刀处理
                // 装备小刀
                character->equipKnife();

                // 显示效果
                showHealEffect(character, "装备小刀!");
            }
            else if (item->getType() == Item::BALL) { // 新增实心球处理
                // 装备实心球
                character->equipBall();

                // 显示效果
                showHealEffect(character, "装备实心球!");
            }
            else if (item->getType() == Item::RIFLE) { // 新增步枪处理
                // 装备步枪
                character->equipRifle();

                // 显示效果
                showHealEffect(character, "装备步枪!");
            }
            else if (item->getType() == Item::SNIPER) { // 新增狙击枪处理
                // 装备狙击枪
                character->equipSniper();

                // 显示效果
                showHealEffect(character, "装备狙击枪!");
            }
            else if (item->getType() == Item::LIGHT_ARMOR) { // 新增锁子甲处理
                // 装备锁子甲
                character->equipLightArmor();

                // 显示效果
                showHealEffect(character, "装备锁子甲!");
            }
            else if (item->getType() == Item::BULLETPROOF_VEST) { // 新增防弹衣处理
                // 装备防弹衣
                character->equipBulletproofVest();

                // 显示效果
                showHealEffect(character, "装备防弹衣!");
            }

            // 移除道具
            item->deleteLater();
            items.removeAt(i);
            break; // 一次只拾取一个道具
        }
    }
}

void GameScreen::showHealEffect(Character* character, const QString& text) {
    // 创建治疗特效标签
    QLabel* healEffect = new QLabel(gameArea);
    healEffect->setText(text);
    healEffect->setStyleSheet("font-size: 16px; font-weight: bold; color: #00FF00;");
    healEffect->setGeometry(character->getX(), character->getY() - 30, 100, 30);
    healEffect->show();
    healEffect->raise();

    // 设置特效动画
    QTimer* effectTimer = new QTimer(healEffect);
    int duration = 1000; // 1秒
    connect(effectTimer, &QTimer::timeout, [=]() {
        static int elapsed = 0;
        elapsed += 50;

        // 向上移动
        healEffect->move(healEffect->x(), healEffect->y() - 2);

        // 淡出效果
        if (elapsed > duration / 2) {
            QColor color(0, 255, 0, 255 * (duration - elapsed) / (duration / 2));
            healEffect->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1;")
                                          .arg(color.name(QColor::HexArgb)));
        }

        // 结束动画
        if (elapsed >= duration) {
            effectTimer->stop();
            healEffect->deleteLater();
        }
    });
    effectTimer->start(50);
}

// 游戏结束界面
class GameOverScreen : public QWidget {
    Q_OBJECT
public:
    GameOverScreen(QWidget *parent = nullptr) : QWidget(parent) {
        // 设置背景
        bgLabel = new QLabel(this);
        originalBg = QPixmap(":/new/prefix1/res/background.png");
        if (!originalBg.isNull()) {
            // 初始设置
            bgLabel->setPixmap(originalBg);
            bgLabel->setGeometry(0, 0, width(), height());
        }
        bgLabel->lower();

        // 主布局
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setAlignment(Qt::AlignCenter);

        // 胜利者文本
        winnerLabel = new QLabel(this);
        winnerLabel->setStyleSheet("font-size: 36px; font-weight: bold; color: white;");
        winnerLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(winnerLabel);

        // 游戏结束图片
        gameOverLabel = new QLabel(this);
        QPixmap gameOverPixmap(":/new/prefix1/res/gameover.png");
        if (!gameOverPixmap.isNull()) {
            gameOverLabel->setPixmap(gameOverPixmap);
            gameOverLabel->setAlignment(Qt::AlignCenter);
            gameOverLabel->setFixedSize(gameOverPixmap.size());
        }
        layout->addWidget(gameOverLabel);

        // 返回按钮
        returnButton = new QPushButton("返回", this);
        returnButton->setFixedSize(200, 50);
        returnButton->setStyleSheet("font-size: 24px; font-weight: bold; background-color: #4CAF50; color: white;");
        layout->addWidget(returnButton, 0, Qt::AlignCenter);

        // 连接按钮信号
        connect(returnButton, &QPushButton::clicked, this, &GameOverScreen::returnToStart);
    }

    // 设置胜利者
    void setWinner(int winner) {
        if (winner == 1) {
            winnerLabel->setText("Winner: Role 1");
        } else if (winner == 2) {
            winnerLabel->setText("Winner: Role 2");
        }
    }

    // 重写resizeEvent确保背景正确调整大小
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);

        // 更新背景大小
        bgLabel->setGeometry(0, 0, width(), height());

        // 如果背景图片存在，重新缩放以适应窗口
        if (!originalBg.isNull()) {
            QPixmap scaled = originalBg.scaled(
                size(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );
            bgLabel->setPixmap(scaled);
        }
    }

signals:
    void returnToStartRequested();

private slots:
    void returnToStart() {
        emit returnToStartRequested();
    }

private:
    QPixmap originalBg; // 保存原始背景图片
    QLabel *bgLabel;
    QLabel *winnerLabel;
    QLabel *gameOverLabel;
    QPushButton *returnButton;
};

// 修改后的帮助界面类
class HelpScreen : public QWidget {
    Q_OBJECT
public:
    HelpScreen(QWidget *parent = nullptr) : QWidget(parent) {
        // 设置背景
        bgLabel = new QLabel(this);
        originalBg = QPixmap(":/new/prefix1/res/background.png");
        if (!originalBg.isNull()) {
            bgLabel->setPixmap(originalBg);
            bgLabel->setGeometry(0, 0, width(), height());
        }
        bgLabel->lower();

        // 主布局
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignCenter);
        mainLayout->setSpacing(20);

        // 标题
        QLabel *titleLabel = new QLabel("游戏帮助", this);
        titleLabel->setStyleSheet("font-size: 36px; font-weight: bold; color: white;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // 内容区域
        QWidget *contentWidget = new QWidget();
        QGridLayout *gridLayout = new QGridLayout(contentWidget);

        // 操作说明标题 - 红色
        QLabel *controlsTitle = new QLabel("操作说明", contentWidget);
        controlsTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: red;");
        gridLayout->addWidget(controlsTitle, 0, 0, 1, 2, Qt::AlignCenter);

        // 玩家1操作标题 - 红色
        QLabel *player1Label = new QLabel("玩家1 (蓝色角色):", contentWidget);
        player1Label->setStyleSheet("font-size: 18px; font-weight: bold; color: red;");
        gridLayout->addWidget(player1Label, 1, 0, Qt::AlignLeft);

        // 玩家1操作内容 - 黄色
        QLabel *player1Controls = new QLabel(
            "W - 跳跃\n"
            "A - 向左移动\n"
            "D - 向右移动\n"
            "S - 下蹲/隐身\n"
            "F - 攻击", contentWidget);
        player1Controls->setStyleSheet("font-size: 16px; color: yellow;");
        gridLayout->addWidget(player1Controls, 2, 0, Qt::AlignLeft);

        // 玩家2操作标题 - 红色
        QLabel *player2Label = new QLabel("玩家2 (红色角色):", contentWidget);
        player2Label->setStyleSheet("font-size: 18px; font-weight: bold; color: red;");
        gridLayout->addWidget(player2Label, 3, 0, Qt::AlignLeft);

        // 玩家2操作内容 - 黄色
        QLabel *player2Controls = new QLabel(
            "↑ - 跳跃\n"
            "← - 向左移动\n"
            "→ - 向右移动\n"
            "↓ - 下蹲/隐身\n"
            "L - 攻击", contentWidget);
        player2Controls->setStyleSheet("font-size: 16px; color: yellow;");
        gridLayout->addWidget(player2Controls, 4, 0, Qt::AlignLeft);

        // 道具说明标题 - 红色
        QLabel *itemsTitle = new QLabel("道具介绍", contentWidget);
        itemsTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: red;");
        gridLayout->addWidget(itemsTitle, 1, 1, Qt::AlignLeft);

        // 道具内容 - 黄色
        QLabel *itemsDesc = new QLabel(
            "绷带 - 恢复20点生命值\n"
            "急救包 - 恢复全部生命值\n"
            "肾上腺素 - 增加移动速度并持续恢复\n"
            "小刀 - 装备后增加近战伤害\n"
            "实心球 - 可投掷3次的高伤害武器\n"
            "步枪 - 远程武器，20发子弹\n"
            "狙击枪 - 高伤害远程武器，5发子弹\n"
            "锁子甲 - 免疫拳头伤害，减少小刀伤害\n"
            "防弹衣 - 减少枪械伤害，有耐久度", contentWidget);
        itemsDesc->setStyleSheet("font-size: 16px; color: yellow;");
        gridLayout->addWidget(itemsDesc, 2, 1, Qt::AlignLeft);

        // 地形效果标题 - 红色
        QLabel *terrainTitle = new QLabel("地形效果", contentWidget);
        terrainTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: red;");
        gridLayout->addWidget(terrainTitle, 3, 1, Qt::AlignLeft);

        // 地形内容 - 黄色
        QLabel *terrainDesc = new QLabel(
            "草地 - 下蹲时隐身\n"
            "冰面 - 增加移动速度", contentWidget);
        terrainDesc->setStyleSheet("font-size: 16px; color: yellow;");
        gridLayout->addWidget(terrainDesc, 4, 1, Qt::AlignLeft);

        mainLayout->addWidget(contentWidget);

        // 返回按钮 - 放置在右下角
        returnButton = new QPushButton("返回", this);
        returnButton->setFixedSize(200, 50);
        returnButton->setStyleSheet("font-size: 24px; font-weight: bold;");

        // 创建布局将按钮放在右下角
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch(); // 添加拉伸因子
        buttonLayout->addWidget(returnButton);

        mainLayout->addLayout(buttonLayout);

        // 连接按钮信号
        connect(returnButton, &QPushButton::clicked, this, &HelpScreen::returnToStart);
    }

    // 重写resizeEvent确保背景正确调整大小
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);

        // 更新背景大小
        bgLabel->setGeometry(0, 0, width(), height());

        // 如果背景图片存在，重新缩放以适应窗口
        if (!originalBg.isNull()) {
            QPixmap scaled = originalBg.scaled(
                size(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );
            bgLabel->setPixmap(scaled);
        }
    }

signals:
    void returnToStartRequested();

private slots:
    void returnToStart() {
        emit returnToStartRequested();
    }

private:
    QPixmap originalBg; // 保存原始背景图片
    QLabel *bgLabel;
    QPushButton *returnButton;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 创建主窗口
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("2D横版射击游戏 - 武器系统");
    mainWindow.resize(1200, 800);

    // 创建堆叠窗口管理界面
    QStackedWidget *stackedWidget = new QStackedWidget(&mainWindow);
    mainWindow.setCentralWidget(stackedWidget);

    // 加载背景图片
    QPixmap backgroundPixmap(":/new/prefix1/res/background.jpg");
    if (!backgroundPixmap.isNull()) {
        backgroundPixmap = backgroundPixmap.scaled(mainWindow.size(),
                                                   Qt::IgnoreAspectRatio,
                                                   Qt::SmoothTransformation);
    }

    // =====================
    // 1. 开始界面
    // =====================
    QWidget *startScreen = new QWidget();

    // 开始界面背景
    QLabel *startBackground = new QLabel(startScreen);
    if (!backgroundPixmap.isNull()) {
        startBackground->setPixmap(backgroundPixmap);
        startBackground->setGeometry(0, 0, mainWindow.width(), mainWindow.height());
    }

    // 垂直布局
    QVBoxLayout *startLayout = new QVBoxLayout(startScreen);
    startLayout->setAlignment(Qt::AlignCenter);

    // 按钮容器
    QWidget *buttonContainer = new QWidget(startScreen);
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonContainer);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(30); // 设置按钮间的垂直间距

    // 开始按钮
    QPushButton *startButton = new QPushButton(buttonContainer);
    QPixmap startButtonPixmap(":/new/prefix1/res/start.png");
    if (!startButtonPixmap.isNull()) {
        startButton->setFixedSize(startButtonPixmap.size());
        startButton->setIcon(startButtonPixmap);
        startButton->setIconSize(startButtonPixmap.size());
        startButton->setStyleSheet("border: none; background: transparent;");
    } else {
        startButton->setText("开始游戏");
        startButton->setFixedSize(200, 60);
        startButton->setStyleSheet("font-size: 24px;");
    }

    // 帮助按钮容器 - 用于实现向右偏移
    QWidget *helpButtonContainer = new QWidget(buttonContainer);
    QHBoxLayout *helpButtonLayout = new QHBoxLayout(helpButtonContainer);
    helpButtonLayout->setContentsMargins(0, 0, 0, 0); // 移除内边距

    QPushButton *helpButton = new QPushButton("游戏帮助", helpButtonContainer);
    helpButton->setFixedSize(200, 60);
    helpButton->setStyleSheet("font-size: 24px;");

    // 在这里添加偏移量 - 修改这个值调整帮助按钮向右偏移的距离
    const int offsetValue = 10; // 调整这个值来控制偏移量
    helpButtonLayout->addSpacing(offsetValue); // 添加空白来实现向右偏移
    helpButtonLayout->addWidget(helpButton);
    helpButtonLayout->addStretch(); // 右侧添加拉伸

    // 添加按钮到垂直布局（上下排列）
    buttonLayout->addStretch(); // 上方弹性空间
    buttonLayout->addWidget(startButton, 0, Qt::AlignCenter); // 开始按钮居中
    buttonLayout->addWidget(helpButtonContainer, 0, Qt::AlignCenter); // 帮助按钮容器居中
    buttonLayout->addStretch(); // 下方弹性空间

    // 设置按钮容器在屏幕中央
    startLayout->addWidget(buttonContainer, 0, Qt::AlignCenter);
    // 2. 游戏界面
    // =====================
    GameScreen *gameScreen = new GameScreen();
    if (!backgroundPixmap.isNull()) {
        gameScreen->setBackground(backgroundPixmap);
    }

    // =====================
    // 3. 结束界面
    // =====================
    GameOverScreen *gameOverScreen = new GameOverScreen();

    // =====================
    // 4. 帮助界面
    // =====================
    HelpScreen *helpScreen = new HelpScreen();

    // =====================
    // 5. 添加界面到堆叠窗口
    // =====================
    stackedWidget->addWidget(startScreen);    // 索引0 - 开始界面
    stackedWidget->addWidget(gameScreen);     // 索引1 - 游戏界面
    stackedWidget->addWidget(gameOverScreen); // 索引2 - 结束界面
    stackedWidget->addWidget(helpScreen);     // 索引3 - 帮助界面

    // =====================
    // 6. 连接信号与槽
    // =====================
    // 开始按钮 -> 游戏界面
    QObject::connect(startButton, &QPushButton::clicked, [&]() {
        stackedWidget->setCurrentIndex(1);
        gameScreen->setFocus();
        gameScreen->startSpawningItems();
    });

    // 帮助按钮 -> 帮助界面
    QObject::connect(helpButton, &QPushButton::clicked, [&]() {
        stackedWidget->setCurrentIndex(3);
    });

    // 游戏结束 -> 结束界面
    QObject::connect(gameScreen, &GameScreen::gameOver, [&](int winner) {
        gameOverScreen->setWinner(winner);
        stackedWidget->setCurrentIndex(2);
    });

    // 结束界面返回 -> 开始界面
    QObject::connect(gameOverScreen, &GameOverScreen::returnToStartRequested, [&]() {
        stackedWidget->setCurrentIndex(0);
        stackedWidget->removeWidget(gameScreen);
        delete gameScreen;
        gameScreen = new GameScreen();
        if (!backgroundPixmap.isNull()) {
            gameScreen->setBackground(backgroundPixmap);
        }
        stackedWidget->insertWidget(1, gameScreen);
        QObject::connect(gameScreen, &GameScreen::gameOver, [&](int winner) {
            gameOverScreen->setWinner(winner);
            stackedWidget->setCurrentIndex(2);
        });
    });

    // 帮助界面返回 -> 开始界面
    QObject::connect(helpScreen, &HelpScreen::returnToStartRequested, [&]() {
        stackedWidget->setCurrentIndex(0);
    });

    // 设置初始界面为开始界面
    stackedWidget->setCurrentIndex(0);

    // 显示窗口
    mainWindow.show();

    return app.exec();
}

#include "main.moc"
