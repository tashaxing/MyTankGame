#include "joypad.h"
#include "game_scene.h"

const double PI = 3.141592654;

bool Joypad::init()
{
    if (!Layer::init())
        return false;
    
    Point visible_origin = Director::getInstance()->getVisibleOrigin();
    Size visible_size = Director::getInstance()->getVisibleSize();
    
	// 此处是4方向
	m_type = KEY4;

    // 锚点在圆心
    // 摇杆
    m_wheel = Sprite::create("img/joypad/wheel.png");
    m_wheel->setPosition(visible_origin.x + 70, visible_origin.y + 70);
    addChild(m_wheel);
    
    m_stick = Sprite::create("img/joypad/stick.png");
    m_stick->setPosition(m_wheel->getPosition());
    addChild(m_stick);
    
    m_can_move = true;
    
    // 开火键(用按钮来做会比较精确,其实也可以用精灵在触摸的回调里面做)
    m_attack = Button::create("img/joypad/attack.png");
    m_attack->setContentSize(Size(60, 60));
    m_attack->setPosition(Vec2(visible_origin.x + visible_size.width - 70,
                          visible_origin.y + 70));
    m_attack->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type){
        // 按钮点击事件
        switch (type)
        {
            case ui::Widget::TouchEventType::BEGAN:
            {
                // 开火
                // callback
                if (m_game_scene)
                    m_game_scene->onFireBtn(true);
            }
                break;
            case ui::Widget::TouchEventType::ENDED:
            {
                // 停止开火
                // callback
                if (m_game_scene)
                    m_game_scene->onFireBtn(false);
            }
                break;
            default:
                break;
        }
    });
    addChild(m_attack);
    
    // 触摸监听
    auto touch_listener = EventListenerTouchOneByOne::create();
    touch_listener->onTouchBegan = CC_CALLBACK_2(Joypad::onTouchBegan, this);
    touch_listener->onTouchMoved = CC_CALLBACK_2(Joypad::onTouchMoved, this);
    touch_listener->onTouchEnded = CC_CALLBACK_2(Joypad::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);
    
    return true;
}

void Joypad::setJoystickType(JoystickType joystick_type)
{
    m_type = joystick_type;
}

// 获取以p1为圆心，p2p1与x轴正方向的弧度值
float Joypad::calcRad(Point p1, Point p2)
{
    float xx = p2.x - p1.x;
    float yy = p2.y - p1.y;
    
    // 斜边
    float xie = sqrt(pow(xx, 2) + pow(yy, 2));
    // yy >= 0 弧度在于 0 到 π 之间。(0~180°)
    // yy < 0 弧度在于 π 到 2π 之间。(180°~360°)
    float rad = yy >= 0 ? (acos(xx / xie)) : (PI * 2 - acos(xx / xie));
    return rad;
}

// 得到与角度对应的半径为R的圆上一坐标点, 相对值
Vec2 Joypad::getAnglePosition(float R, float rad)
{
    return Point(R * cos(rad), R * sin(rad));
}

bool Joypad::onTouchBegan(Touch *touch, Event *event)
{
    CCLOG("onTouchBegan");
    
    Point point = touch->getLocation();
    
    // 触点在中心圈内才能移动
    if (m_stick->getBoundingBox().containsPoint(point))
        m_can_move = true;
    
    return true;
}

void Joypad::onTouchMoved(Touch *touch, Event *event)
{
    CCLOG("onTouchMoved");
    if (!m_can_move)
        return;
    
    Point point = touch->getLocation();
    Point wheel_center = m_wheel->getPosition();
    float wheel_radius = m_wheel->getContentSize().width / 2;
    float stick_radius = m_stick->getContentSize().width / 2;
    
    // 判断两个圆心的距离是否大于外圈半径
    float distance = sqrt(pow(point.x - wheel_center.x, 2) + pow(point.y - wheel_center.y, 2));
	float rad = calcRad(wheel_center, point);
	if (distance >= wheel_radius)
    {
        // 摇杆中心不超出外圈范围
        m_stick->setPosition(wheel_center + getAnglePosition(wheel_radius, rad));
    }
    else
        m_stick->setPosition(point); // 摇杆跟随触点

	// 换算成角度，根据键类型确定方向，将方向控制信号传给游戏场景
	float angle = rad * 180.0 / PI;
	if (m_type == KEY4)
	{
        JoyDirection direction;
        // 靠近轴90度范围
		if ((angle >= 0 && angle < 45) || (angle >= 315 && angle < 360))
            direction = RIGHT;  // 右
		else if (angle >= 45 && angle < 135)
            direction = UP;    // 上
		else if (angle >= 135 && angle < 225)
            direction = LEFT;  // 左
		else if (angle >= 225 && angle < 315)
            direction = DOWN;  // 下
        
        // callback
        if (m_game_scene)
            m_game_scene->onEnumDirection(direction);
	}
	else if (m_type == KEY8)
	{
        JoyDirection direction;
        // 靠近轴45度范围
		if ((angle >= 0 && angle < 22.5) || (angle >= 337.5 && angle < 360))
            direction = RIGHT; // 右
		else if (angle >= 22.5 && angle < 67.5)
            direction = RIGHT_UP; // 右上
		else if (angle >= 67.5 && angle < 112.5)
            direction = UP; // 上
		else if (angle >= 112.5 && angle < 157.5)
            direction = LEFT_UP; // 左上
		else if (angle >= 157.5 && angle < 202.5)
            direction = LEFT; // 左
		else if (angle >= 202.5 && angle < 247.5)
            direction = LEFT_DOWN; // 左下
		else if (angle >= 247.5 && angle < 292.5)
            direction = DOWN; // 下
		else if (angle >= 292.5 && angle < 337.5)
            direction = RIGHT_DOWN; // 右下
        
        // callback
        if (m_game_scene)
            m_game_scene->onEnumDirection(direction);
	}
    else if (m_type == KEYANY)
    {
        // callback
        if (m_game_scene)
            m_game_scene->onAngleDirection(angle);
    }
}

void Joypad::onTouchEnded(Touch *touch, Event *event)
{
    CCLOG("onTouchEnded");
    
    // 恢复到圆心
    Point wheel_center = m_wheel->getPosition();
    m_stick->stopAllActions();
    m_stick->runAction(MoveTo::create(0.08, wheel_center));
    
    m_can_move = false;

    // 停止对游戏场景的控制
	// callback
    if (m_type == KEY4 || m_type == KEY8)
    {
        if (m_game_scene)
            m_game_scene->onEnumDirection(NONE);
    }
    else if (m_type == KEYANY)
    {
        if (m_game_scene)
        {
            m_game_scene->onAngleDirection(0.0);
        }
    }
}

void Joypad::setGameScene(GameScene* game_scene)
{
    m_game_scene = game_scene;
}
