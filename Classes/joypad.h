#ifndef _JOYPAD_H_
#define _JOYPAD_H_

#include "cocos2d.h"
using namespace cocos2d;

// 虚拟手柄控制层
class Joypad : public Layer
{
public:
    virtual bool init();
    CREATE_FUNC(Joypad);
    
    virtual bool onTouchBegan(Touch *touch, Event *event);
    virtual void onTouchMoved(Touch *touch, Event *event);
    virtual void onTouchEnded(Touch *touch, Event *event);
    
private:
    float calcRad(Point pos1, Point pos2);
    Sprite* m_wheel;
    Sprite* m_stick;
    Sprite* m_attack;
    bool is_can_move;
};

#endif /* _JOYPAD_H_ */
