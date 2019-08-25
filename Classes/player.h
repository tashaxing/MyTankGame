#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "cocos2d.h"
#include "item.h"
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
    void move();
//    Bullet* shootSingle(); // 射击一次，产生一颗子弹
//    Vector<Bullet*> shootDouble(); // 射击一次，产生双子弹
    void fetchItem(ItemType item_type); // 拾取道具
    void destroy(); // 玩家over
    
//    BulletType m_bullet_type; // 根据子弹类型改变子弹
};

#endif /* _PLAYER_H_ */
