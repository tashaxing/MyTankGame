#include "bullet.h"

USING_NS_CC;

const float kLowSpeed = 3.0;
const float kHighSpeed = 5.0;

const int kLowKillHp = 10;

const float kFrameUpdateInterval = 0.02;

bool Bullet::init()
{
    if (!Sprite::init())
        return false;
    
    return true;
}

void Bullet::initWithDirection(JoyDirection direction)
{
    m_direction = direction;
    
    // 初始子弹未击中
    m_hit_flag = false;
    
    // 设置纹理和子弹速度
    setTexture("img/tank/bullet.png");
    m_speed = kLowSpeed;
    m_kill_hp = kLowKillHp;
    
    // 调度子弹的移动，固定间隔移动固定单位
    schedule(schedule_selector(Bullet::move), kFrameUpdateInterval);
}

void Bullet::move(float tm)
{
    // 每帧更新，沿初始方向移动
    switch (m_direction)
    {
        case UP:
            setPositionY(getPositionY() + kLowSpeed);
            break;
        case DOWN:
            setPositionY(getPositionY() - kLowSpeed);
            break;
        case LEFT:
            setPositionX(getPositionX() - kLowSpeed);
            break;
        case RIGHT:
            setPositionX(getPositionX() + kLowSpeed);
        default:
            break;
    }
}
