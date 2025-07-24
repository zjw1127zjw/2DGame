#include "HelpScreen.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

HelpScreen::HelpScreen(QWidget *parent) : QWidget(parent) {
    // 设置背景
    bgLabel = new QLabel(this);
    originalBg = QPixmap(":/new/prefix1/res/background.png");
    if (!originalBg.isNull()) {
        bgLabel->setPixmap(originalBg);
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

    // 操作说明
    QLabel *controlsTitle = new QLabel("操作说明", contentWidget);
    controlsTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: red;");
    gridLayout->addWidget(controlsTitle, 0, 0, 1, 2, Qt::AlignCenter);

    QLabel *player1Label = new QLabel("玩家1 (蓝色角色):", contentWidget);
    player1Label->setStyleSheet("font-size: 18px; font-weight: bold; color: red;");
    gridLayout->addWidget(player1Label, 1, 0, Qt::AlignLeft);

    QLabel *player1Controls = new QLabel(
        "W - 跳跃\n"
        "A - 向左移动\n"
        "D - 向右移动\n"
        "S - 下蹲/隐身\n"
        "F - 攻击", contentWidget);
    player1Controls->setStyleSheet("font-size: 16px; color: yellow;");
    gridLayout->addWidget(player1Controls, 2, 0, Qt::AlignLeft);

    QLabel *player2Label = new QLabel("玩家2 (红色角色):", contentWidget);
    player2Label->setStyleSheet("font-size: 18px; font-weight: bold; color: red;");
    gridLayout->addWidget(player2Label, 3, 0, Qt::AlignLeft);

    QLabel *player2Controls = new QLabel(
        "↑ - 跳跃\n"
        "← - 向左移动\n"
        "→ - 向右移动\n"
        "↓ - 下蹲/隐身\n"
        "L - 攻击", contentWidget);
    player2Controls->setStyleSheet("font-size: 16px; color: yellow;");
    gridLayout->addWidget(player2Controls, 4, 0, Qt::AlignLeft);

    // 道具说明
    QLabel *itemsTitle = new QLabel("道具介绍", contentWidget);
    itemsTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: red;");
    gridLayout->addWidget(itemsTitle, 1, 1, Qt::AlignLeft);

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

    // 地形效果
    QLabel *terrainTitle = new QLabel("地形效果", contentWidget);
    terrainTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: red;");
    gridLayout->addWidget(terrainTitle, 3, 1, Qt::AlignLeft);

    QLabel *terrainDesc = new QLabel(
        "草地 - 下蹲时隐身\n"
        "冰面 - 增加移动速度", contentWidget);
    terrainDesc->setStyleSheet("font-size: 16px; color: yellow;");
    gridLayout->addWidget(terrainDesc, 4, 1, Qt::AlignLeft);

    mainLayout->addWidget(contentWidget);

    // 返回按钮
    returnButton = new QPushButton("返回", this);
    returnButton->setFixedSize(200, 50);
    returnButton->setStyleSheet("font-size: 24px; font-weight: bold;");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(returnButton);
    mainLayout->addLayout(buttonLayout);

    // 连接按钮信号
    connect(returnButton, &QPushButton::clicked, this, &HelpScreen::returnToStart);
}

void HelpScreen::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    bgLabel->setGeometry(0, 0, width(), height());
    if (!originalBg.isNull()) {
        QPixmap scaled = originalBg.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        bgLabel->setPixmap(scaled);
    }
}

void HelpScreen::returnToStart() {
    emit returnToStartRequested();
}
