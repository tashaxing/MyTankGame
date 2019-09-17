#include "item.h"

bool Item::init()
{
    if (!Sprite::init())
        return false;
    
    return true;
}

void Item::initWithType(ItemType item_type)
{
    m_type = item_type;
    
    // 根据纹理初始化不同的坦克
    switch (item_type)
    {
        case ACTIVE:
            setTexture("img/item/active.png"); // 设置纹理必须用单个图片而不是帧缓存
            break;
        case STAR:
            setTexture("img/item/star.png");
            break;
        case BOMB:
            setTexture("img/item/bomb.png");
            break;
        case SHOVEL:
            setTexture("img/item/shovel.png");
            break;
        case TIMER:
            setTexture("img/item/timer.png");
            break;
        case MINITANK:
            setTexture("img/item/mintank.png");
            break;
        default:
            break;
    }
    
    // 闪烁动画
    Blink* blink = Blink::create(1.0, 9999); // 每1秒闪动一次，无限次
    runAction(blink);
}
