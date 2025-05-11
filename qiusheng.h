#ifndef QIUSHENG_H
#define QIUSHENG_H
#include<QPixmap>
#include<QRect>
class qiusheng
{

public:

    qiusheng();
    //更新坐标
    void updatePosition();
    //资源对象数组，方便加载不同图片
    QPixmap m_enemy[30];
    //位置4
    int m_X;
    int m_Y;
    //矩形边框，用于碰撞检测
    QRect m_Rect;
    //状态
    bool m_Free;
    //速度
    int m_Speed;

};

#endif // QIUSHENG_H
