#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "cocos2d.h"
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
    void move(float tm);
    void shoot();
    void hit(int reduce_hp);
    void die();
    
    int m_hp;
    EnemyType m_type;
    
private:
    float m_speed;
};

#endif /* _ENEMY_H_ */
