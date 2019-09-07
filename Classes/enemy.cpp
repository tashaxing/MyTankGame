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
            setTexture("img/tank/normal/normalD.png"); // 设置纹理必须用单个图片而不是帧缓存
            m_speed = kMoveSpeedNormal;
            break;
        case ARMOR:
            setTexture("img/tank/armor/armor3D.png");
            m_speed = kMoveSpeedArmor;
            break;
        case SPEED:
            setTexture("img/tank/speed/speedD.png");
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

void Enemy::setMoveDirection(JoyDirection direction)
{
    m_head_direction = direction;
    m_move_direction = direction;
    
    // 切换方向和纹理
    switch (m_type)
    {
        case NORMAL:
        {
            switch (m_head_direction)
            {
                case UP:
                    setTexture("img/tank/normal/normalU.png");
                    break;
                case DOWN:
                    setTexture("img/tank/normal/normalD.png");
                    break;
                case LEFT:
                    setTexture("img/tank/normal/normalL.png");
                    break;
                case RIGHT:
                    setTexture("img/tank/normal/normalR.png");
                    break;
                default:
                    break;
            }
            setContentSize(m_size); // 重设尺寸
        }
            break;
        case ARMOR:
        {
            switch (m_head_direction)
            {
                case UP:
                    setTexture("img/tank/armor/armor3U.png");
                    break;
                case DOWN:
                    setTexture("img/tank/armor/armor3D.png");
                    break;
                case LEFT:
                    setTexture("img/tank/armor/armor3L.png");
                    break;
                case RIGHT:
                    setTexture("img/tank/armor/armor3R.png");
                    break;
                default:
                    break;
            }
            setContentSize(m_size); // 重设尺寸
        }
            break;
        case SPEED:
        {
            switch (m_head_direction)
            {
                case UP:
                    setTexture("img/tank/speed/speedU.png");
                    break;
                case DOWN:
                    setTexture("img/tank/speed/speedD.png");
                    break;
                case LEFT:
                    setTexture("img/tank/speed/speedL.png");
                    break;
                case RIGHT:
                    setTexture("img/tank/speed/speedR.png");
                    break;
                default:
                    break;
            }
            setContentSize(m_size); // 重设尺寸
        }
            break;
        default:
            break;
    }
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
