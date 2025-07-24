#include "GameScreen.h"
#include <QPainter>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRandomGenerator>

GameScreen::GameScreen(QWidget *parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 顶部血条容器
    QWidget *topBar = new QWidget(this);
    topBar->setFixedHeight(50);
    topBar->setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 10, 20, 10);

    // 玩家1血条
    healthContainer1 = new QWidget(topBar);
    healthContainer1->setFixedSize(204, 24);
    healthContainer1->setStyleSheet("background-color: black; border: 2px solid #555;");

    healthBar1 = new QLabel(healthContainer1);
    healthBar1->setGeometry(2, 2, 200, 20);
    healthBar1->setStyleSheet("background-color: red;");

    healthText1 = new QLabel(healthContainer1);
    healthText1->setGeometry(0, 0, 200, 20);
    healthText1->setAlignment(Qt::AlignCenter);
    healthText1->setStyleSheet("color: white; font-weight: bold;");
    healthText1->setText("100");

    // 玩家2血条
    healthContainer2 = new QWidget(topBar);
    healthContainer2->setFixedSize(204, 24);
    healthContainer2->setStyleSheet("background-color: black; border: 2px solid #555;");

    healthBar2 = new QLabel(healthContainer2);
    healthBar2->setGeometry(2, 2, 200, 20);
    healthBar2->setStyleSheet("background-color: red;");

    healthText2 = new QLabel(healthContainer2);
    healthText2->setGeometry(0, 0, 200, 20);
    healthText2->setAlignment(Qt::AlignCenter);
    healthText2->setStyleSheet("color: white; font-weight: bold;");
    healthText2->setText("100");

    // 添加到顶部布局
    topLayout->addWidget(healthContainer1);
    topLayout->addStretch();
    topLayout->addWidget(healthContainer2);

    // 游戏区域
    gameArea = new QWidget(this);
    gameArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 添加到主布局
    mainLayout->addWidget(topBar);
    mainLayout->addWidget(gameArea);

    // 创建平台
    createPlatforms();

    // 创建角色
    character1 = new Character(":/new/prefix1/res/role1.png", true, gameArea);
    character1->setPlatforms(&platforms);
    character1->setPos(200, 450 - character1->getHeight());
    character1->raise();

    character2 = new Character(":/new/prefix1/res/role2.png", false, gameArea);
    character2->setPlatforms(&platforms);
    character2->setPos(900, 450 - character2->getHeight());
    character2->raise();

    // 连接信号
    connect(character1, &Character::healthChanged, this, [this](int health) { updateHealthBar(1, health); });
    connect(character2, &Character::healthChanged, this, [this](int health) { updateHealthBar(2, health); });

    connect(character1, &Character::ballThrown, this, [this](int x, int y, bool right, Character* thrower) {
        BallProjectile* ball = new BallProjectile(x, y, right, thrower, gameArea);
        ball->show();
        ball->raise();
        ballProjectiles.append(ball);
    });

    // 其他信号连接...

    // 平台调试定时器
    platformDebugTimer = new QTimer(this);
    connect(platformDebugTimer, &QTimer::timeout, this, [this]() { update(); });
    platformDebugTimer->start(100);

    // 攻击检测定时器
    attackCheckTimer = new QTimer(this);
    connect(attackCheckTimer, &QTimer::timeout, this, &GameScreen::checkAttack);
    attackCheckTimer->start(50);

    // 游戏结束检测定时器
    gameOverTimer = new QTimer(this);
    connect(gameOverTimer, &QTimer::timeout, this, &GameScreen::checkGameOver);
    gameOverTimer->start(100);

    // 地形标签
    grassLabel = new QLabel(gameArea);
    grassLabel->setPixmap(QPixmap(":/new/prefix1/res/grass.png").scaled(210, 60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    grassLabel->setGeometry(210, 250, 210, 60);
    grassLabel->lower();

    snowLabel = new QLabel(gameArea);
    snowLabel->setPixmap(QPixmap(":/new/prefix1/res/xuedui.png").scaled(210, 60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    snowLabel->setGeometry(775, 250, 210, 60);
    snowLabel->lower();

    // 道具生成定时器初始化
    bandageSpawnTimer = new QTimer(this);
    connect(bandageSpawnTimer, &QTimer::timeout, this, &GameScreen::spawnBandage);
    // 其他道具定时器...

    // 道具更新定时器
    itemUpdateTimer = new QTimer(this);
    connect(itemUpdateTimer, &QTimer::timeout, this, &GameScreen::updateItems);
    itemUpdateTimer->start(16);
}

void GameScreen::setBackground(const QPixmap &pixmap) {
    if (!pixmap.isNull()) {
        background = new QLabel(this);
        background->setPixmap(pixmap);
        background->setGeometry(0, 0, width(), height());
        background->lower();
        if (grassLabel) grassLabel->raise();
        if (snowLabel) snowLabel->raise();
    }
}

void GameScreen::startSpawningItems() {
    bandageSpawnTimer->start(20000);
    medkitSpawnTimer->start(30000);
    adrenalineSpawnTimer->start(60000);
    knifeSpawnTimer->start(45000);
    ballSpawnTimer->start(60000);
    rifleSpawnTimer->start(70000);
    sniperSpawnTimer->start(90000);
    lightArmorSpawnTimer->start(55000);
    bulletproofVestSpawnTimer->start(65000);
}

void GameScreen::createPlatforms() {
    platforms.push_back(Platform(100, 450, 1000, 100, 0));
    platforms.push_back(Platform(210, 280, 210, 1, 1));
    platforms.push_back(Platform(775, 280, 210, 1, 2));
    platforms.push_back(Platform(500, 100, 200, 1, 0));
}

void GameScreen::spawnBandage() {
    int x = QRandomGenerator::global()->bounded(100, 1000);
    Item* bandage = new Item(Item::BANDAGE, gameArea);
    bandage->move(x, 0);
    bandage->show();
    bandage->raise();
    items.append(bandage);
}

// 其他道具生成函数...

void GameScreen::updateItems() {
    for (Item* item : items) {
        item->updatePosition(platforms);
    }
}

void GameScreen::updateHealthBar(int player, int health) {
    QLabel* bar = (player == 1) ? healthBar1 : healthBar2;
    QLabel* text = (player == 1) ? healthText1 : healthText2;

    int width = (health * 200) / 100;
    if (width < 0) width = 0;

    bar->setFixedWidth(width);
    text->setText(QString::number(health));

    if (health < 20) {
        bar->setStyleSheet("background-color: yellow;");
    } else {
        bar->setStyleSheet("background-color: red;");
    }
}

// 攻击检测
void GameScreen::checkAttack() {
    QRect charRect1(character1->getX(), character1->getY(),
                    character1->getWidth(), character1->getHeight());
    QRect charRect2(character2->getX(), character2->getY(),
                    character2->getWidth(), character2->getHeight());

    // 玩家1攻击检测
    if (character1->getAttackEffect()->isVisible() || character1->getKnifeEffect()->isVisible()) {
        QRect attackRange = character1->getAttackRange();
        if (attackRange.intersects(charRect2)) {
            bool shooterCrouching = character1->isCharacterCrouching();
            bool targetCrouching = character2->isCharacterCrouching();
            if (shooterCrouching || !targetCrouching) {
                int damage = (character1->getCurrentWeapon() == Character::FIST) ? 2 : 5;
                character2->takeDamage(damage, character1->getCurrentWeapon());
            }
        }
    }

    // 其他攻击检测...
}

void GameScreen::checkGameOver() {
    if (character1->getHealth() <= 0) {
        emit gameOver(2);
    } else if (character2->getHealth() <= 0) {
        emit gameOver(1);
    }
}

// 道具拾取检测
void GameScreen::checkItemPickup(Character* character) {
    int charX = character->getX();
    int charY = character->getY();
    int charWidth = character->getWidth();
    int charHeight = character->getHeight();
    int pickupX = charX + charWidth / 2;
    int pickupY = charY + charHeight - 10;

    for (int i = items.size() - 1; i >= 0; i--) {
        Item* item = items[i];
        if (item->contains(pickupX, pickupY)) {
            switch (item->getType()) {
            case Item::BANDAGE:
                character->heal(20);
                showHealEffect(character, "+20 HP");
                break;
            case Item::MEDKIT:
                character->fullHeal();
                showHealEffect(character, "+100 HP");
                break;
                // 其他道具处理...
            }
            item->deleteLater();
            items.removeAt(i);
            break;
        }
    }
}

// 绘制游戏界面
void GameScreen::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制平台
    painter.setPen(Qt::green);
    painter.setBrush(QBrush(QColor(100, 200, 100, 150)));
    for (const Platform& p : platforms) {
        painter.drawRect(p.x, p.y, p.width, p.height);
    }

    // 绘制调试信息
    if (character1->isCharacterCrouching()) {
        painter.setPen(Qt::red);
        painter.drawText(10, 70, "玩家1: 下蹲状态");
    }
    if (character2->isCharacterCrouching()) {
        painter.setPen(Qt::blue);
        painter.drawText(10, 90, "玩家2: 下蹲状态");
    }

    // 绘制攻击范围
    if (drawAttackRange) {
        painter.setPen(Qt::red);
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(character1->getAttackRange());
        painter.drawRect(character2->getAttackRange());
    }

    // 绘制状态提示
    if (character1->isInvincibleState()) {
        painter.setPen(Qt::red);
        painter.drawText(character1->getX(), character1->getY() - 20, "无敌");
    }
    if (character2->isInvincibleState()) {
        painter.setPen(Qt::red);
        painter.drawText(character2->getX(), character2->getY() - 20, "无敌");
    }

    // 绘制武器状态
    painter.setPen(Qt::white);
    if (character1->getCurrentWeapon() == Character::KNIFE) {
        painter.drawText(character1->getX(), character1->getY() - 60, "装备: 小刀");
    }
    // 其他状态绘制...
}

// 键盘事件处理
void GameScreen::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_A: character1->setMoveDirection(-1); break;
    case Qt::Key_D: character1->setMoveDirection(1); break;
    case Qt::Key_W: character1->jump(); break;
    case Qt::Key_S:
        character1->setCrouching(true);
        checkItemPickup(character1);
        break;
    case Qt::Key_F: character1->attack(); break;
    case Qt::Key_Left: character2->setMoveDirection(-1); break;
    case Qt::Key_Right: character2->setMoveDirection(1); break;
    case Qt::Key_Up: character2->jump(); break;
    case Qt::Key_Down:
        character2->setCrouching(true);
        checkItemPickup(character2);
        break;
    case Qt::Key_L: character2->attack(); break;
    case Qt::Key_R: drawAttackRange = !drawAttackRange; update(); break;
    default: QWidget::keyPressEvent(event);
    }
}

void GameScreen::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_A:
    case Qt::Key_D: character1->setMoveDirection(0); break;
    case Qt::Key_S: character1->setCrouching(false); break;
    case Qt::Key_Left:
    case Qt::Key_Right: character2->setMoveDirection(0); break;
    case Qt::Key_Down: character2->setCrouching(false); break;
    default: QWidget::keyReleaseEvent(event);
    }
}

void GameScreen::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (background) {
        background->setGeometry(0, 0, width(), height());
    }
}
