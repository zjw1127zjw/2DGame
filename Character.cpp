#include "Character.h"
#include "AttackEffect.h"
#include "KnifeAttackEffect.h"
#include <QPainter>
#include <QDebug>

Character::Character(const QString& spritePath, bool isPlayer1, QWidget *parent)
    : QWidget(parent), player1(isPlayer1) {
    // 加载角色精灵图
    spriteSheet = QPixmap(spritePath);
    if (spriteSheet.isNull()) {
        qDebug() << "角色精灵图加载失败:" << spritePath;
    } else {
        frameWidth = spriteSheet.width() / 4;
        frameHeight = spriteSheet.height() / 4;
        setFixedSize(frameWidth, frameHeight);
    }

    // 初始化属性
    health = 100;
    currentWeapon = FIST;

    // 动画定时器
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, [this]() { updateFrame(); });

    // 水平移动定时器
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, [this]() {
        if (moveDirection != 0 && !isCrouching) {
            int newX = characterX + moveDirection * moveSpeed;
            bool collision = false;
            if (platforms) {
                for (const Platform& p : *platforms) {
                    bool onPlatform = (characterY + frameHeight >= p.y) &&
                                      (characterY + frameHeight <= p.y + 5) &&
                                      (newX + frameWidth > p.x) &&
                                      (newX < p.x + p.width);
                    bool sideCollision = p.intersects(newX, characterY, frameWidth, frameHeight);
                    if (!onPlatform && sideCollision) {
                        collision = true;
                        break;
                    }
                }
            }
            if (!collision) {
                characterX = newX;
                move(characterX, characterY);
                updateArmorPosition();
            }
        }
    });

    // 重力定时器
    gravityTimer = new QTimer(this);
    connect(gravityTimer, &QTimer::timeout, this, [this]() { applyGravity(); });
    gravityTimer->start(16);

    // 创建攻击特效
    attackEffect = new AttackEffect(parentWidget());
    attackEffect->hide();
    knifeEffect = new KnifeAttackEffect(parentWidget());
    knifeEffect->hide();

    // 无敌帧定时器
    invincibleTimer = new QTimer(this);
    invincibleTimer->setSingleShot(true);
    connect(invincibleTimer, &QTimer::timeout, this, [this]() {
        isInvincible = false;
        armorDamageEffect = false;
        update();
    });

    // 地形效果检测定时器
    terrainEffectTimer = new QTimer(this);
    connect(terrainEffectTimer, &QTimer::timeout, this, [this]() { checkTerrainEffects(); });
    terrainEffectTimer->start(100);

    // 肾上腺素效果定时器
    adrenalineTimer = new QTimer(this);
    connect(adrenalineTimer, &QTimer::timeout, this, [this]() {
        if (isAdrenalineActive) {
            heal(1);
            adrenalineRemainingTime -= 250;
            if (adrenalineRemainingTime <= 0) {
                isAdrenalineActive = false;
                moveSpeed = baseMoveSpeed;
                checkTerrainEffects();
                update();
            }
        }
    });

    // 加载武器图片
    knifeRightPixmap = QPixmap(":/new/prefix1/res/knife.png");
    knifeLeftPixmap = QPixmap(":/new/prefix1/res/knife2.png");
    if (!knifeRightPixmap.isNull()) {
        knifeRightPixmap = knifeRightPixmap.scaled(frameWidth, frameHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    if (!knifeLeftPixmap.isNull()) {
        knifeLeftPixmap = knifeLeftPixmap.scaled(frameWidth, frameHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    ballPixmap = QPixmap(":/new/prefix1/res/ball.png");
    if (!ballPixmap.isNull()) {
        int size = qMin(frameWidth, frameHeight) * 0.5;
        ballPixmap = ballPixmap.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    rifleRightPixmap = QPixmap(":/new/prefix1/res/AKM.png");
    rifleLeftPixmap = QPixmap(":/new/prefix1/res/AKM2.png");
    if (!rifleRightPixmap.isNull()) {
        int width = frameWidth * 0.8;
        int height = frameHeight * 0.8;
        rifleRightPixmap = rifleRightPixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    if (!rifleLeftPixmap.isNull()) {
        int width = frameWidth * 0.8;
        int height = frameHeight * 0.8;
        rifleLeftPixmap = rifleLeftPixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    sniperRightPixmap = QPixmap(":/new/prefix1/res/juji.png");
    sniperLeftPixmap = QPixmap(":/new/prefix1/res/juji2.png");
    if (!sniperRightPixmap.isNull()) {
        int width = frameWidth * 0.8;
        int height = frameHeight * 0.8;
        sniperRightPixmap = sniperRightPixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    if (!sniperLeftPixmap.isNull()) {
        int width = frameWidth * 0.8;
        int height = frameHeight * 0.8;
        sniperLeftPixmap = sniperLeftPixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // 射击间隔定时器
    rifleShootTimer = new QTimer(this);
    rifleShootTimer->setSingleShot(true);
    sniperShootTimer = new QTimer(this);
    sniperShootTimer->setSingleShot(true);

    // 初始化护甲标签
    armorLabel = new QLabel(parentWidget());
    armorLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    armorLabel->setScaledContents(true);
    armorLabel->hide();

    vestLabel = new QLabel(parentWidget());
    vestLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    vestLabel->setScaledContents(true);
    vestLabel->hide();
}

// 位置设置函数
void Character::setPos(int x, int y) {
    characterX = x;
    characterY = y;
    move(characterX, characterY);
    updateArmorPosition();
}

void Character::setPlatforms(std::vector<Platform>* p) {
    platforms = p;
}

// 移动方向设置
void Character::setMoveDirection(int direction) {
    if (isCrouching) return;

    moveDirection = direction;
    if (direction != 0) {
        currentRow = (direction < 0) ? 1 : 2;
        if (!animationTimer->isActive()) {
            animationTimer->start(animationSpeed);
            currentFrame = 0;
        }
        if (!moveTimer->isActive()) {
            moveTimer->start(30);
        }
    } else {
        moveTimer->stop();
        currentFrame = 0;
        update();
    }
}

// 下蹲状态设置
void Character::setCrouching(bool crouch) {
    if (isCrouching == crouch) return;

    isCrouching = crouch;
    if (isCrouching) {
        lastDirectionRow = currentRow;
        currentRow = 0;
        animationTimer->stop();
        update();
        if (isOnGrass) {
            setVisible(false);
        }
    } else {
        currentRow = lastDirectionRow;
        if (moveDirection != 0) {
            animationTimer->start(animationSpeed);
        } else {
            currentFrame = 0;
            update();
        }
        setVisible(true);
    }
}

// 攻击方法
void Character::attack() {
    switch (currentWeapon) {
    case FIST: punch(); break;
    case KNIFE: knifeAttack(); break;
    case BALL: throwBall(); break;
    case RIFLE: rifleAttack(); break;
    case SNIPER: sniperAttack(); break;
    }
}

void Character::knifeAttack() {
    bool attackRight = isFacingRight();
    knifeEffect->startAttack(attackRight, characterX, characterY, frameWidth, frameHeight);
    knifeEffect->raise();
}

void Character::punch() {
    bool attackRight = isFacingRight();
    attackEffect->startAttack(attackRight, characterX, characterY, frameWidth, frameHeight);
    attackEffect->raise();
}

void Character::throwBall() {
    ballUses--;
    bool throwRight = isFacingRight();
    emit ballThrown(characterX, characterY, throwRight, this);
    if (ballUses <= 0) {
        equipFist();
    }
}

void Character::rifleAttack() {
    if (rifleShootTimer->isActive() || rifleAmmo <= 0) return;

    rifleAmmo--;
    bool shootRight = isFacingRight();
    int startX = characterX;
    int startY = characterY;

    if (shootRight) {
        startX += frameWidth * 0.4;
    } else {
        startX -= frameWidth * 0.1;
    }

    startY += frameHeight * 0.5;
    emit bulletShot(startX, startY, shootRight, this);
    rifleShootTimer->start(500);

    if (rifleAmmo <= 0) {
        equipFist();
    }
}

void Character::sniperAttack() {
    if (sniperShootTimer->isActive() || sniperAmmo <= 0) return;

    sniperAmmo--;
    bool shootRight = isFacingRight();
    int startX = characterX;
    int startY = characterY;

    if (shootRight) {
        startX += frameWidth * 0.4;
    } else {
        startX -= frameWidth * 0.1;
    }

    startY += frameHeight * 0.5;
    emit sniperBulletShot(startX, startY, shootRight, this);
    sniperShootTimer->start(2000);

    if (sniperAmmo <= 0) {
        equipFist();
    }
}

// 装备武器方法
void Character::equipKnife() { currentWeapon = KNIFE; }
void Character::equipFist() { currentWeapon = FIST; }

void Character::equipBall() {
    currentWeapon = BALL;
    ballUses = 3;
}

void Character::equipRifle() {
    currentWeapon = RIFLE;
    rifleAmmo = 20;
}

void Character::equipSniper() {
    currentWeapon = SNIPER;
    sniperAmmo = 5;
}

void Character::equipLightArmor() {
    if (bulletproofVestEquipped) {
        bulletproofVestEquipped = false;
        vestLabel->hide();
    }
    lightArmorEquipped = true;

    QPixmap armorPix(":/new/prefix1/res/dun.png");
    if (!armorPix.isNull()) {
        int size = qMax(frameWidth, frameHeight) *0.5;
        armorLabel->setFixedSize(size*2, size);
        armorPix = armorPix.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        armorLabel->setPixmap(armorPix);
    }
    armorLabel->show();
    armorLabel->raise();
    updateArmorPosition();
}

void Character::equipBulletproofVest() {
    if (lightArmorEquipped) {
        lightArmorEquipped = false;
        armorLabel->hide();
    }
    bulletproofVestEquipped = true;
    vestDurability = 100;

    QPixmap vestPix(":/new/prefix1/res/dun2.png");
    if (!vestPix.isNull()) {
        int size = qMax(frameWidth, frameHeight) *0.5;
        vestLabel->setFixedSize(size, size);
        vestPix = vestPix.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        vestLabel->setPixmap(vestPix);
    }
    vestLabel->show();
    vestLabel->raise();
    updateArmorPosition();
}

// 方向判断
bool Character::isFacingRight() const {
    if (isCrouching) {
        return lastDirectionRow == 2;
    }
    return currentRow == 2;
}

// 跳跃方法
void Character::jump() {
    if (isCrouching) return;

    if (canJump) {
        verticalVelocity = JUMP_VELOCITY;
        canJump = false;
        if (isInAir) {
            doubleJumpUsed = true;
        }
        isInAir = true;
    }
}

// 受伤害处理
void Character::takeDamage(int damage, Weapon damageSource) {
    if (isInvincible) return;

    // 护甲伤害调整
    if (lightArmorEquipped) {
        if (damageSource == FIST) damage = 0;
        else if (damageSource == KNIFE) damage = 2;
    } else if (bulletproofVestEquipped) {
        if (damageSource == RIFLE) {
            damage = 2;
            vestDurability -= 10;
        } else if (damageSource == SNIPER) {
            damage = 10;
            vestDurability -= 40;
        }
        if (vestDurability <= 0) {
            bulletproofVestEquipped = false;
            vestLabel->hide();
        }
    }

    // 受击效果
    if (lightArmorEquipped && (damageSource == FIST || damageSource == KNIFE)) {
        damageColor = Qt::yellow;
    } else if (bulletproofVestEquipped && (damageSource == RIFLE || damageSource == SNIPER)) {
        damageColor = Qt::yellow;
    } else if (damage > 0) {
        damageColor = Qt::red;
    }

    health -= damage;
    if (health < 0) health = 0;
    emit healthChanged(health);

    // 无敌状态
    isInvincible = true;
    invincibleTimer->start(300);
}

// 治疗处理
void Character::heal(int amount) {
    health += amount;
    if (health > 100) health = 100;
    emit healthChanged(health);
}

void Character::fullHeal() {
    health = 100;
    emit healthChanged(health);
}

// 地形效果检测
void Character::checkTerrainEffects() {
    if (!platforms) return;

    isOnGrass = false;
    isOnIce = false;

    for (const Platform& p : *platforms) {
        bool onPlatform = (characterY + frameHeight >= p.y) &&
                          (characterY + frameHeight <= p.y + 5) &&
                          (characterX + frameWidth > p.x) &&
                          (characterX < p.x + p.width);

        if (onPlatform) {
            if (p.type == 1) isOnGrass = true;
            else if (p.type == 2) isOnIce = true;
        }
    }

    // 草地隐身效果
    if (isOnGrass && isCrouching) {
        setVisible(false);
    } else if (isOnGrass && !isCrouching) {
        setVisible(true);
    }

    // 冰面加速效果
    if (isOnIce) {
        moveSpeed = isAdrenalineActive ? baseMoveSpeed * 2.0 : baseMoveSpeed * 1.5;
    } else {
        moveSpeed = isAdrenalineActive ? baseMoveSpeed * 1.5 : baseMoveSpeed;
    }
}

// 肾上腺素效果
void Character::activateAdrenaline() {
    if (isAdrenalineActive) {
        adrenalineRemainingTime = ADRENALINE_DURATION;
        return;
    }

    isAdrenalineActive = true;
    adrenalineRemainingTime = ADRENALINE_DURATION;
    moveSpeed = baseMoveSpeed * 1.5;

    if (!adrenalineTimer->isActive()) {
        adrenalineTimer->start(250);
    }
    checkTerrainEffects();
    update();
}

// 绘制角色
void Character::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    if (spriteSheet.isNull()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(0, 0, spriteSheet,
                       currentFrame * frameWidth,
                       currentRow * frameHeight,
                       frameWidth, frameHeight);

    bool facingRight = isFacingRight();

    // 绘制装备的武器
    if (currentWeapon == KNIFE) {
        if (facingRight && !knifeRightPixmap.isNull()) {
            painter.drawPixmap(0, 20, knifeRightPixmap);
        } else if (!facingRight && !knifeLeftPixmap.isNull()) {
            painter.drawPixmap(0, 20, knifeLeftPixmap);
        }
    } else if (currentWeapon == BALL && !ballPixmap.isNull()) {
        int offsetX, offsetY;
        if (facingRight) {
            offsetX = frameWidth * 0.48;
        } else {
            offsetX = -ballPixmap.width() * 0.11;
        }
        offsetY = (frameHeight - ballPixmap.height()) / 2 + frameHeight * 0.1 + 15;
        painter.drawPixmap(offsetX, offsetY, ballPixmap);
    } else if (currentWeapon == RIFLE) {
        QPixmap* riflePixmap = facingRight ? &rifleRightPixmap : &rifleLeftPixmap;
        if (!riflePixmap->isNull()) {
            int offsetX = facingRight ? frameWidth * 0.2 : -riflePixmap->width() * 0.05;
            int offsetY = (frameHeight - riflePixmap->height()) / 2 + 25;
            painter.drawPixmap(offsetX, offsetY, *riflePixmap);
        }
    } else if (currentWeapon == SNIPER) {
        QPixmap* sniperPixmap = facingRight ? &sniperRightPixmap : &sniperLeftPixmap;
        if (!sniperPixmap->isNull()) {
            int offsetX = facingRight ? frameWidth * 0.2 : -sniperPixmap->width() * 0.05;
            int offsetY = (frameHeight - sniperPixmap->height()) / 2 + 15;
            painter.drawPixmap(offsetX, offsetY, *sniperPixmap);
        }
    }

    // 绘制状态效果
    if (isInvincible) {
        painter.fillRect(rect(), QColor(damageColor.red(), damageColor.green(), damageColor.blue(), 100));
    }
    if (isAdrenalineActive) {
        painter.fillRect(rect(), QColor(0, 100, 255, 100));
    }
}

// 动画帧更新
void Character::updateFrame() {
    currentFrame = (currentFrame + 1) % 4;
    update();
    if (moveDirection == 0 && currentFrame == 0) {
        animationTimer->stop();
    }
}

// 重力应用
void Character::applyGravity() {
    verticalVelocity += GRAVITY;
    int newY = characterY + verticalVelocity;
    bool onPlatform = false;

    if (platforms) {
        for (const Platform& p : *platforms) {
            if (newY + frameHeight >= p.top() &&
                characterY + frameHeight <= p.top() + 5 &&
                characterX + frameWidth > p.x &&
                characterX < p.x + p.width &&
                verticalVelocity >= 0) {
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

    if (!onPlatform) {
        characterY = newY;
        if (characterY > 800) {
            characterY = 100;
            characterX = 600;
            verticalVelocity = 0;
        }
        if (verticalVelocity != 0) {
            isInAir = true;
        }
    }

    move(characterX, characterY);
    updateArmorPosition();
}

// 护甲位置更新
void Character::updateArmorPosition() {
    if (armorLabel && armorLabel->isVisible()) {
        int armorX = characterX - (armorLabel->width() - frameWidth) / 2;
        int armorY = characterY - armorLabel->height() + frameHeight * 0.5;
        armorLabel->move(armorX, armorY);
    }
    if (vestLabel && vestLabel->isVisible()) {
        int vestX = characterX - (vestLabel->width() - frameWidth) / 2;
        int vestY = characterY - vestLabel->height() + frameHeight * 0.5;
        vestLabel->move(vestX, vestY);
    }
}
