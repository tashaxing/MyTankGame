#ifndef _WELCOME_SCENE_H_
#define _WELCOME_SCENE_H_

#include "cocos2d.h"
using namespace cocos2d;

// 欢迎场景，场景简单，没有绑定层
class WelcomeScene : public Scene
{
public:
    static Scene* createScene();
    
    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(WelcomeScene);
};

#endif /* _WELCOME_SCENE_H_ */
