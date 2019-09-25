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
    static Scene* createScene(int round = 1);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    
    virtual void update(float dt);
    
    CREATE_FUNC(GameScene);
private:
    void initWithRound(int round = 1);
    
public:
	void onEnumDirection(JoyDirection direction); // 控制摇杆方向回调，枚举值
	void onAngleDirection(float angle); // 控制摇杆方向回调，任意角度
	void onFireBtn(bool is_pressed);
    
    void onBomb(); // 玩家拾取到炸弹，当前所有敌人消灭
    void onClock(); // 玩家拾取到定时，所有敌人坦克无法移动和开火
    void onShovel(); // 玩家拾取到铲子，老鹰碉堡搞成钢板围起来
    void onMiniTank(); // 玩家拾取到命，增加一条命
    
    int m_round = 1; // 第几关，默认第一关
    int m_enemy_count = 20; // 每关20个
    
private:
    void gameWin();
    void gameOver();
    bool m_is_over;
    bool m_is_clock;
    int m_player1_life = 2; // 初始生命值
    int m_player2_life = 2;

private:
    void emitPlayerBullet(float tm);
    Vector<Bullet*> m_player_bullets; // 场景中玩家的子弹
    void emitEnemyBullet(float tm);
    Vector<Bullet*> m_enemy_bullets; // 场景中敌人的子弹
    
    Vector<Enemy*> m_enemies; // 场景中的敌人
    void generateEnemy(float tm);
    
    Vector<Item*> m_items; // 场景中的道具
    void generateItem();
    
    BattleField* m_battle_field;
    int m_total_enemy_count = 20; // 默认每局敌人数量
    Player* m_player1;
    Joypad* m_joypad;
};

#endif /* _GAME_SCENE_H_ */
