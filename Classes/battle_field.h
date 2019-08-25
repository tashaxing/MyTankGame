#ifndef _BATTLE_FIELD_H_
#define _BATTLE_FIELD_H_

#include "cocos2d.h"
using namespace cocos2d;

enum TileType
{
    BLANK,
    GRASS,
    STEEL,
    WALL,
    RIVER,
    EAGLE
};

class BattleField : public TMXTiledMap
{
public:
    virtual bool init();

    void initWithLevel(int round);
    
    CREATE_FUNC(BattleField);
    
    bool isCollide(Rect bounding_box);
};

#endif /* _BATTLE_FIELD_H_ */
