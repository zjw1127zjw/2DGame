#ifndef PLATFORM_H
#define PLATFORM_H

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

#endif // PLATFORM_H
