#include "joypad.h"

bool Joypad::init()
{
    if (!Layer::init())
        return false;
    
    Point visible_origin = Director::getInstance()->getVisibleOrigin();
    Size visible_size = Director::getInstance()->getVisibleSize();
    
    // 锚点在圆心
    // 摇杆
    m_wheel = Sprite::create("img/joypad/wheel.png");
    m_wheel->setPosition(visible_origin.x + 70, visible_origin.y + 70);
    addChild(m_wheel);
    
    m_stick = Sprite::create("img/joypad/stick.png");
    m_stick->setPosition(m_wheel->getPosition());
    addChild(m_stick);
    
    is_can_move = true;
    
    // 开火键
    m_attack = Sprite::create("img/joypad/attack.png");
    m_attack->setContentSize(Size(40, 40));
    m_attack->setPosition(visible_origin.x + visible_size.width - 70,
                        visible_origin.y + 70);
    addChild(m_attack);
    
    // 触摸监听
    auto touch_listener = EventListenerTouchOneByOne::create();
    touch_listener->onTouchBegan = CC_CALLBACK_2(Joypad::onTouchBegan, this);
    touch_listener->onTouchMoved = CC_CALLBACK_2(Joypad::onTouchMoved, this);
    touch_listener->onTouchEnded = CC_CALLBACK_2(Joypad::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);
    
    return true;
}

// 获取以p1为圆心，p2p1与x轴正方向的弧度值
float Joypad::calcRad(Point pos1, Point pos2)
{
    float rad;
    return rad;
}

bool Joypad::onTouchBegan(Touch *touch, Event *event)
{
    CCLOG("onTouchBegan");
    
    Point point = touch->getLocation();
    
    // 触点在中心圈内才能移动
    if (m_stick->getBoundingBox().containsPoint(point))
        is_can_move = true;
    
    return true;
}

void Joypad::onTouchMoved(Touch *touch, Event *event)
{
    CCLOG("onTouchMoved");
    if (!is_can_move)
        return;
    
    Point point = touch->getLocation();
    Point wheel_center = m_wheel->getPosition();
    float wheel_radius = m_wheel->getContentSize().width / 2;
    
    // 判断两个圆心的距离是否大于外圈半径
    float distance = sqrt(pow(point.x - wheel_center.x, 2) + pow(point.y - wheel_center.y, 2));
    if (distance >= wheel_radius)
        
}

void Joypad::onTouchEnded(Touch *touch, Event *event)
{
    CCLOG("onTouchEnded");
}
