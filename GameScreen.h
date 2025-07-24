#ifndef GAME_SCREEN_H
#define GAME_SCREEN_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <vector>
#include <QList>
#include <QKeyEvent>
#include "Character.h"
#include "Bullet.h"
#include "BallProjectile.h"
#include "Item.h"
#include "Platform.h"
#include "KnifeAttackEffect.h"
#include "AttackEffect.h"

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

#endif // GAME_SCREEN_H
