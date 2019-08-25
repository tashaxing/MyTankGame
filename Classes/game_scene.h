#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include "cocos2d.h"
#include "battle_field.h"
#include "player.h"
#include "enemy.h"
#include "item.h"
#include "joypad.h"

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
    
    int m_round = 1; // 第几关，默认第一关
    
private:
    BattleField* m_battle_field;
    Player* m_player1;
    Joypad* m_joypad;
};

#endif /* _GAME_SCENE_H_ */
