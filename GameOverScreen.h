#ifndef GAME_OVER_SCREEN_H
#define GAME_OVER_SCREEN_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPixmap>

// 游戏结束界面
class GameOverScreen : public QWidget {
    Q_OBJECT
public:
    GameOverScreen(QWidget *parent = nullptr);

    // 设置胜利者
    void setWinner(int winner);

    // 重写resizeEvent确保背景正确调整大小
    void resizeEvent(QResizeEvent *event) override;

signals:
    void returnToStartRequested();

private slots:
    void returnToStart();

private:
    QPixmap originalBg; // 保存原始背景图片
    QLabel *bgLabel;
    QLabel *winnerLabel;
    QLabel *gameOverLabel;
    QPushButton *returnButton;
};

#endif // GAME_OVER_SCREEN_H
