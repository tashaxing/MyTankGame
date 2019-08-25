#ifndef _ITEM_H_
#define _ITEM_H_

#include "cocos2d.h"
USING_NS_CC;

enum ItemType
{
    ACTIVE, // 帽子
    START,  // 火力星
    BOMB,   // 炸弹
    SHOVEL,  // 铲子
    TIMER,  // 定时
    MINTANK // 命
};

class Item : public Sprite
{
public:
    virtual bool init();
    
    CREATE_FUNC(Item);
    void initWithType(ItemType Item_type);
    
    ItemType m_type;
};

#endif /* _ITEM_H_ */
