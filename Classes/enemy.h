#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "cocos2d.h"
#include "bullet.h"
#include "joypad.h"

USING_NS_CC;

enum EnemyType
{
    NORMAL,  // 普通坦克
    ARMOR,   // 装甲车
    SPEED    // 迅捷坦克
};

enum EnemyStatus
{
    ESIMPLE, // 普通
    ESHIELD  // 无敌
};

class Enemy : public Sprite
{
public:
    virtual bool init();
    
    CREATE_FUNC(Enemy);
    
    void initWithType(EnemyType enemy_type);
    void setSize(Size size);
    
public:
    void setDirection(JoyDirection direction);
    JoyDirection m_head_direction;
    void move(float tm);
    void changeDirection();
    Bullet* shoot();
    void hit();
    void die();
    int m_life;
    
    EnemyType m_type;
    bool m_moving;
    EnemyStatus m_status;
    
private:
    Size m_size;
    
    float m_speed;
};

#endif /* _ENEMY_H_ */
