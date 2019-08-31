#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "cocos2d.h"
#include "item.h"
#include "bullet.h"
#include "joypad.h"

USING_NS_CC;

enum PlayerType
{
    P1,
    P2
};

class Player : public Sprite
{
public:
    virtual bool init();
    
    CREATE_FUNC(Player);
    
public:
    void initWithType(PlayerType player_type);
    void move(float tm);
    Bullet* shootSingle(); // 射击一次，产生一颗子弹
    Vector<Bullet*> shootDouble(); // 射击一次，产生双子弹
    void fetchItem(ItemType item_type); // 拾取道具
    void destroy(); // 玩家over
    
public:
    void setSize(Size size);
    void setMoveDirection(JoyDirection direction);
    float m_bullet_interval;
    
private:
    Size m_size;
    JoyDirection m_head_direction; // 坦克朝向
    JoyDirection m_move_direction; // 坦克行进方向
    BulletType m_bullet_type; // 根据子弹类型改变子弹
};

#endif /* _PLAYER_H_ */
