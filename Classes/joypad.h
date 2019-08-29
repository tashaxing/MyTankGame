#ifndef _JOYPAD_H_
#define _JOYPAD_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "game_scene.h"

using namespace cocos2d;
using namespace ui; // button namespace

// 摇杆类型，四方向、八方向、任意方向
enum JoystickType
{
	KEY4,
	KEY8,
	KEYANY
};

enum MoveDirection
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	RIGHT_UP,
	LEFT_UP,
	LEFT_DOWN,
	RIGHT_DOWN
};

// 虚拟手柄控制层
class Joypad : public Layer
{
public:
    virtual bool init();
    CREATE_FUNC(Joypad);
    
    virtual bool onTouchBegan(Touch *touch, Event *event);
    virtual void onTouchMoved(Touch *touch, Event *event);
    virtual void onTouchEnded(Touch *touch, Event *event);
    
	void setGameScene(GameScene* game_scene);

private:
	JoystickType m_type;
    float calcRad(Point p1, Point p2);
    Vec2 getAnglePosition(float R, float rad);
    Sprite* m_wheel;
    Sprite* m_stick;
    Button* m_attack;
    bool m_can_move;

	GameScene* m_game_scene; // like callback
};

#endif /* _JOYPAD_H_ */
