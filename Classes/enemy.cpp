#include <SimpleAudioEngine.h>
#include "enemy.h"

using namespace CocosDenshion;

const float kFrameUpdateInterval = 0.02;

const float kMoveSpeedNormal = 0.5;
const float kMoveSpeedArmor = 0.8;
const float kMoveSpeedFast = 1.2;

bool Enemy::init()
{
    if (!Sprite::init())
        return false;
    
    return true;
}

void Enemy::initWithType(EnemyType enemy_type)
{
    m_type = enemy_type;
    
    // 根据纹理初始化不同的坦克
    switch (enemy_type)
    {
        case NORMAL:
            setTexture("img/tank/normal/normalD.png"); // 设置纹理必须用单给图片
            m_speed = kMoveSpeedNormal;
            break;
        case ARMOR:
            setTexture("img/tank/normal/armor3D.png");
            m_speed = kMoveSpeedArmor;
            break;
        case SPEED:
            setTexture("img/tank/normal/speedD.png");
            m_speed = kMoveSpeedFast;
            break;
        default:
            break;
    }
    
    // 初始方向
    m_head_direction = DOWN;
    m_move_direction = DOWN;
    
    // 调度坦克移动
    schedule(schedule_selector(Enemy::move), kFrameUpdateInterval);
}

void Enemy::setSize(Size size)
{
    m_size = size;
    setContentSize(m_size);
}

void Enemy::move(float tm)
{
    // 间隔固定时间发出移动音效
    static int step_count = 0;
    step_count++;
    if (step_count % 8 == 0 && step_count < 100)
    {
        if (m_move_direction != NONE)
            SimpleAudioEngine::getInstance()->playEffect("sound/nmoving.wav");
    }
    
    // 四个方向移动
    switch (m_move_direction)
    {
        case UP:
            setPositionY(getPositionY() + m_speed);
            break;
        case DOWN:
            setPositionY(getPositionY() - m_speed);
            break;
        case LEFT:
            setPositionX(getPositionX() - m_speed);
            break;
        case RIGHT:
            setPositionX(getPositionX() + m_speed);
        default:
            break;
    }
}

Bullet* Enemy::shoot()
{
    // 子弹从飞机头部打出来
    Bullet* bullet = Bullet::create();
    bullet->initWithDirection(m_head_direction);
    switch (m_head_direction)
    {
        case UP:
            bullet->setPosition(getPositionX(), getPositionY() + getContentSize().height / 2);
            break;
        case DOWN:
            bullet->setPosition(getPositionX(), getPositionY() - getContentSize().height / 2);
            break;
        case LEFT:
            bullet->setPosition(getPositionX() - getContentSize().width / 2, getPositionY());
            break;
        case RIGHT:
            bullet->setPosition(getPositionX() + getContentSize().width / 2, getPositionY());
            break;
        default:
            break;
    }
    
    return bullet;
}
