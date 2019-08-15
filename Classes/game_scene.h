#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include "cocos2d.h"

using namespace cocos2d;

// 游戏场景默认绑定到一个层上面
class GameScene : public Layer
{
public:
    static Scene* createScene();
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    
    virtual void update(float dt);
    
    CREATE_FUNC(GameScene);

};

#endif /* _GAME_SCENE_H_ */
