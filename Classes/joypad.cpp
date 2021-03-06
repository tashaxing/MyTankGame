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
    
    // 开火键
    m_attack = Sprite::create("img/joypad/attack.png");
    m_attack->setScale(1.5);
    m_attack->setPosition(Vec2(visible_origin.x + visible_size.width - 70,
                          visible_origin.y + 70));
    addChild(m_attack);
    
    // 触摸监听,多点触控
    auto touch_listener = EventListenerTouchAllAtOnce::create();
    touch_listener->onTouchesBegan = CC_CALLBACK_2(Joypad::onTouchesBegan, this);
    touch_listener->onTouchesMoved = CC_CALLBACK_2(Joypad::onTouchesMoved, this);
    touch_listener->onTouchesEnded = CC_CALLBACK_2(Joypad::onTouchesEnded, this);
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

void Joypad::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
{
//    CCLOG("onTouchBegan");
    // 某一时刻可能是一个或两个触点
    Point point1 = touches.front()->getLocation();
    Point point2 = touches.back()->getLocation();
    
    // 区分左右触点，可能是同一个
    Point left_point;
    Point right_point;
    if (point1.x < point2.x)
    {
        left_point = point1;
        right_point = point2;
    }
    else
    {
        left_point = point2;
        right_point = point1;
    }
    
    // 左触点在圈内才能移动
    if (m_wheel->getBoundingBox().containsPoint(left_point))
        m_can_move = true;
    
    // 右触点开火
    if (m_attack->getBoundingBox().containsPoint(right_point))
    {
        // 开火
        // callback
        if (m_game_scene)
            m_game_scene->onFireBtn(true);
    }
}

void Joypad::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
//    CCLOG("onTouchMoved");
    
    if (!m_can_move)
        return;
    
    Point visible_origin = Director::getInstance()->getVisibleOrigin();
    Size visible_size = Director::getInstance()->getVisibleSize();
    
    // 某一时刻可能是一个或两个触点
    Point point1 = touches.front()->getLocation();
    Point point2 = touches.back()->getLocation();
    
    Point wheel_center = m_wheel->getPosition();
    float wheel_radius = m_wheel->getContentSize().width / 2;
//    float stick_radius = m_stick->getContentSize().width / 2;
    
    // 区分左右触点，可能是同一个
    Point left_point;
    Point right_point;
    if (point1.x < point2.x)
    {
        left_point = point1;
        right_point = point2;
    }
    else
    {
        left_point = point2;
        right_point = point1;
    }
    
    // 只有左触点在左半边屏才判断摇杆
    if (left_point.x < visible_origin.x + visible_size.width / 2)
    {
        Point point = left_point;
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
            // 加入控制死区，只有圆心偏移距离够长才换方向
            JoyDirection direction;
            if (distance >= wheel_radius / 5)
            {
                // 靠近轴90度范围
                if ((angle >= 0 && angle < 45) || (angle >= 315 && angle < 360))
                    direction = RIGHT;  // 右
                else if (angle >= 45 && angle < 135)
                    direction = UP;    // 上
                else if (angle >= 135 && angle < 225)
                    direction = LEFT;  // 左
                else if (angle >= 225 && angle < 315)
                    direction = DOWN;  // 下
            }
            else
                direction = NONE;
            
            // callback
            if (m_game_scene)
                m_game_scene->onEnumDirection(direction);
        }
        else if (m_type == KEY8)
        {
            // 加入控制死区，只有圆心偏移距离够长才换方向
            JoyDirection direction;
            if (distance >= wheel_radius / 5)
            {
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
            }
            else
                direction = NONE;
            
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
}

void Joypad::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
//    CCLOG("onTouchEnded");
    
    Point visible_origin = Director::getInstance()->getVisibleOrigin();
    Size visible_size = Director::getInstance()->getVisibleSize();
    
    // 某一时刻可能是一个或两个触点
    Point point1 = touches.front()->getLocation();
    Point point2 = touches.back()->getLocation();
    
    // 区分左右触点，可能是同一个
    Point left_point;
    Point right_point;
    if (point1.x < point2.x)
    {
        left_point = point1;
        right_point = point2;
    }
    else
    {
        left_point = point2;
        right_point = point1;
    }
    
    // 左触点松开
    if (left_point.x < visible_origin.x + visible_size.width / 2)
    {
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
    
    // 右触点松开
    if (right_point.x > visible_origin.x + visible_size.width / 2)
    {
        // 停止开火
        // callback
        if (m_game_scene)
            m_game_scene->onFireBtn(false);
    }
}

void Joypad::setGameScene(GameScene* game_scene)
{
    m_game_scene = game_scene;
}
