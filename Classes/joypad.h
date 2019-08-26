#ifndef _JOYPAD_H_
#define _JOYPAD_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;
using namespace ui; // button namespace

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
    float calcRad(Point p1, Point p2);
    Vec2 getAnglePosition(float R, float rad);
    Sprite* m_wheel;
    Sprite* m_stick;
    Button* m_attack;
    bool m_can_move;
};

#endif /* _JOYPAD_H_ */
