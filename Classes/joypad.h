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
};

#endif /* _JOYPAD_H_ */
