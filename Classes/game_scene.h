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
    
public:
	void onEnumDirection(JoyDirection direction); // 控制摇杆方向回调，枚举值
	void onAngleDirection(float angle); // 控制摇杆方向回调，任意角度
	void onFireBtn(bool is_pressed);
    int m_round = 2; // 第几关，默认第一关

private:
    void emitPlayerBullet(float tm);
    Vector<Bullet*> m_player_bullets;
    
    Vector<Enemy*> m_enemies;
    Vector<Bullet*> m_enemy_bullets;
    void generateEnemy(float tm);
    
    BattleField* m_battle_field;
    int m_total_enemy_count = 20; // 默认每局敌人数量
    Player* m_player1;
    Joypad* m_joypad;
};

#endif /* _GAME_SCENE_H_ */
