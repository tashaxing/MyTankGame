#ifndef _JOYPAD_H_
#define _JOYPAD_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"


using namespace cocos2d;
using namespace ui; // button namespace

// 摇杆类型，四方向、八方向、任意方向
enum JoystickType
{
	KEY4,
	KEY8,
	KEYANY
};

enum JoyDirection
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	RIGHT_UP,
	LEFT_UP,
	LEFT_DOWN,
	RIGHT_DOWN,
    NONE
};

// use pre declare instead of include to avoid compile error
class GameScene;

// 虚拟手柄控制层
class Joypad : public Layer
{
public:
    virtual bool init();
    CREATE_FUNC(Joypad);
    
    virtual bool onTouchBegan(Touch *touch, Event *event);
    virtual void onTouchMoved(Touch *touch, Event *event);
    virtual void onTouchEnded(Touch *touch, Event *event);
    
    void setJoystickType(JoystickType joystick_type);
    void setGameScene(GameScene* game_scene);

private:
	JoystickType m_type;
    float calcRad(Point p1, Point p2);
    Vec2 getAnglePosition(float R, float rad);
    Sprite* m_wheel;
    Sprite* m_stick;
    Button* m_attack;
    bool m_can_move;

    bool m_direction_status; // 方向键按下
    bool m_fire_status; // 开火键按下
    GameScene* m_game_scene = nullptr; // like callback
};

#endif /* _JOYPAD_H_ */
