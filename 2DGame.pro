QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AttackEffect.cpp \
    BallProjectile.cpp \
    Bullet.cpp \
    Character.cpp \
    GameOverScreen.cpp \
    GameScreen.cpp \
    HelpScreen.cpp \
    Item.cpp \
    KnifeAttackEffect.cpp \
    main.cpp

HEADERS += \
    AttackEffect.h \
    BallProjectile.h \
    Bullet.h \
    Character.h \
    GameOverScreen.h \
    GameScreen.h \
    HelpScreen.h \
    Item.h \
    KnifeAttackEffect.h \
    Platform.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
