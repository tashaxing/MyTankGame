#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "cocos2d.h"
#include "item.h"
#include "bullet.h"
#include "joypad.h"

USING_NS_CC;

enum PlayerType
{
    P1, // 玩家1
    P2  // 玩家2
};

enum PlayerStatus
{
    SIMPLE, // 普通
    SHIELD  // 无敌
};

enum PlayerWeapon
{
    SINGLE_GUN,
    DOUBLE_GUN
};

class GameScene;

class Player : public Sprite
{
public:
    virtual bool init();
    
    CREATE_FUNC(Player);
    
public:
    void initWithType(PlayerType player_type);
    void setGameScene(GameScene* game_scene);
    void move(float tm);
    Bullet* shootSingle(); // 射击一次，产生一颗子弹
    Vector<Bullet*> shootDouble(); // 射击一次，产生双子弹
    void fetchItem(ItemType item_type); // 拾取道具
    void destroy(); // 玩家over
    
public:
    void setSize(Size size);
    void setDirection(JoyDirection direction);
    JoyDirection m_head_direction; // 坦克朝向
    float m_bullet_interval;
    bool m_moving;
    PlayerStatus m_status;
    PlayerWeapon m_weapon;
    
private:
    GameScene* m_game_scene = nullptr; // like callback
    Size m_size;
};

#endif /* _PLAYER_H_ */
