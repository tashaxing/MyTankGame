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

class Enemy : public Sprite
{
public:
    virtual bool init();
    
    CREATE_FUNC(Enemy);
    void pauseAll();
    void resumeAll();
    
    void initWithType(EnemyType enemy_type);
    void setSize(Size size);
    void move(float tm);
    Bullet* shoot();
    void hit(int reduce_hp);
    void die();
    
    int m_hp;
    EnemyType m_type;
    
private:
    Size m_size;
    float m_speed;
    JoyDirection m_head_direction;
    JoyDirection m_move_direction; // 坦克行进方向
};

#endif /* _ENEMY_H_ */
