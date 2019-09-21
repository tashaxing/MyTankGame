#ifndef _BATTLE_FIELD_H_
#define _BATTLE_FIELD_H_

#include "cocos2d.h"
#include "joypad.h"
#include "bullet.h"

using namespace cocos2d;

enum TileType
{
    BLANK,
    GRASS,
    STEEL,
    WALL,
    RIVER,
    EAGLE,
    DEAD_EAGLE
};

class BattleField : public TMXTiledMap
{
public:
    virtual bool init();

    void initWithLevel(int round);
    
    CREATE_FUNC(BattleField);
    
    bool isBulletCollide(Rect bounding_box, BulletType bullet_type); // 子弹的碰撞
    bool isTankCollide(Rect bounding_box, JoyDirection direction); // 带方向坦克的碰撞
    bool isEagleHurt(Rect bounding_box);
    
    void protectEagle(); // 用钢板把老鹰围起来
    void unprotectEagle(); // 老鹰恢复为土砖围起来
};

#endif /* _BATTLE_FIELD_H_ */
