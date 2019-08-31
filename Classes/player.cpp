#include <SimpleAudioEngine.h>
#include "player.h"

using namespace CocosDenshion;

const float kFrameUpdateInterval = 0.02;
const float kBulletBaseInterval = 1.0;
const float kBulletPowerInterval = 0.5;
const float kMoveSpeed = 1.0;

bool Player::init()
{
    if (!Sprite::init())
        return false;
    
    return true;
}

void Player::initWithType(PlayerType player_type)
{
    // 根据玩家类型设置纹理
    switch (player_type)
    {
        case P1:
            setTexture("img/tank/player1/player1U.png");
            break;
//        case P2:
//            setTexture("img/tank/player2/player2U.png");
//            break;
        default:
            break;
    }
    
    // 初始朝向
    m_head_direction = UP;
    
    // 初始子弹
    m_bullet_type = BASE;
    m_bullet_interval = kBulletBaseInterval;
    
    // 初始方向无
    m_move_direction = NONE;
    
    // 播放出生动画
    
}

void Player::setSize(Size size)
{
    m_size = size;
    setContentSize(m_size);
}

void Player::setMoveDirection(JoyDirection direction)
{
    // 方便区分朝向和移动方向（因为停止移动也是一种方向）
    if (direction != NONE)
        m_head_direction = direction;
    
    m_move_direction = direction;
    
    // 根据不同方向切换不同纹理
    switch (m_move_direction) {
        case UP:
            setTexture("img/tank/player1/player1U.png");
            break;
        case DOWN:
            setTexture("img/tank/player1/player1D.png");
            break;
        case LEFT:
            setTexture("img/tank/player1/player1L.png");
            break;
        case RIGHT:
            setTexture("img/tank/player1/player1R.png");
            break;
        default:
            break;
    }
    setContentSize(m_size); // 重设尺寸
    
    // 启停坦克位移的调度器
    if (direction != NONE)
        schedule(schedule_selector(Player::move), kFrameUpdateInterval);
    else
        unschedule(schedule_selector(Player::move));
}

void Player::move(float tm)
{
    // 间隔固定时间发出移动音效
    static int step_count = 0;
    step_count++;
    if (step_count % 8 == 0)
    {
        if (m_move_direction != NONE)
            SimpleAudioEngine::getInstance()->playEffect("sound/moving.wav");
    }
    
    // 四个方向移动
    switch (m_move_direction)
    {
        case UP:
            setPositionY(getPositionY() + kMoveSpeed);
            break;
        case DOWN:
            setPositionY(getPositionY() - kMoveSpeed);
            break;
        case LEFT:
            setPositionX(getPositionX() - kMoveSpeed);
            break;
        case RIGHT:
            setPositionX(getPositionX() + kMoveSpeed);
        default:
            break;
    }
}

Bullet* Player::shootSingle()
{
    // 发射子弹音效
    SimpleAudioEngine::getInstance()->playEffect("sound/shoot.wav");
    
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
Vector<Bullet*> Player::shootDouble()
{
    return Vector<Bullet*>();
}

void Player::fetchItem(ItemType item_type)
{
    // 拾取不同道具，产生不同影响
    
}
