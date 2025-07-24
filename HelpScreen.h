#ifndef HELP_SCREEN_H
#define HELP_SCREEN_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QGridLayout>

// 修改后的帮助界面类
class HelpScreen : public QWidget {
    Q_OBJECT
public:
    HelpScreen(QWidget *parent = nullptr);

    // 重写resizeEvent确保背景正确调整大小
    void resizeEvent(QResizeEvent *event) override;

signals:
    void returnToStartRequested();

private slots:
    void returnToStart();

private:
    QPixmap originalBg; // 保存原始背景图片
    QLabel *bgLabel;
    QPushButton *returnButton;
};

#endif // HELP_SCREEN_H
