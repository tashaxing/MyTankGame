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
    
    // 初始生命
    switch (enemy_type)
    {
        case NORMAL:
            m_life = 1;
            break;
        case ARMOR:
            m_life = 3;
            break;
        case SPEED:
            m_life = 2;
            break;
        default:
            break;
    }
    
    // 初始方向
    m_head_direction = DOWN;
    
    // 初始不可移动
    m_moving = false;
    
    // 初始状态
    m_status = ESHIELD;
    
    // 播放出生动画
    Animate* enemy_born_animation = Animate::create(AnimationCache::getInstance()->getAnimation("enemy_born_animation"));
    runAction(Sequence::create(enemy_born_animation, CallFunc::create([&](){
        m_status = ESIMPLE; // 动画完毕才能接收伤害
        m_moving = true;
        changeDirection(); // 根据方向切换纹理
    }), NULL) );
    
    // 调度坦克移动
    schedule(schedule_selector(Enemy::move), kFrameUpdateInterval);
}

void Enemy::setSize(Size size)
{
    m_size = size;
    setContentSize(m_size);
}

void Enemy::setDirection(JoyDirection direction)
{
    // 敌机不会停，只有四个方向
    m_head_direction = direction;
    
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
    if (!m_moving)
        return;
    
    // 间隔固定时间发出移动音效
    static int step_count = 0;
    step_count++;
    if (step_count % 8 == 0 && step_count < 100)
        SimpleAudioEngine::getInstance()->playEffect("sound/nmoving.wav");
    
    // 四个方向移动
    switch (m_head_direction)
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

void Enemy::changeDirection()
{
    // 随机变换方向
    if (m_status == ESHIELD)
        return;
        
    float tank_direction_factor = CCRANDOM_0_1();
    if (tank_direction_factor < 0.25)
        setDirection(UP);
    else if (tank_direction_factor >= 0.25 && tank_direction_factor < 0.5)
        setDirection(DOWN);
    else if (tank_direction_factor >= 0.5 && tank_direction_factor < 0.75)
        setDirection(LEFT);
    else
        setDirection(RIGHT);
}

Bullet* Enemy::shoot()
{
    // 子弹从坦克头部打出来
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

void Enemy::hit()
{
    SimpleAudioEngine::getInstance()->playEffect("sound/shieldhit.wav");
    m_life--;
}

void Enemy::die()
{
    SimpleAudioEngine::getInstance()->playEffect("sound/eexplosion.wav");
    
    // TODO: enemy die animation
}
