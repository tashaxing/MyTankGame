#ifndef __BULLET_H__
#define __BULLET_H__

#include "cocos2d.h"
#include "joypad.h"

enum BulletType
{
    BASE, // 单子弹
    POWER // 双子单
};

class Bullet : public cocos2d::Sprite
{
public:
    virtual bool init();
    void initWithDirection(JoyDirection direction); // 初始化方向和纹理
    CREATE_FUNC(Bullet);
    
public:
    BulletType m_type; // 子弹类型
    int m_kill_hp; // 子弹的杀伤力
    bool m_hit_flag; // 标记子弹是否已击中
    
private:
    void move(float tm);
    JoyDirection m_direction;
    float m_speed;
};

#endif /* __BULLET_H__ */
