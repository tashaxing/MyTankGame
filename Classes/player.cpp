#include <SimpleAudioEngine.h>
#include "player.h"
#include "game_scene.h"

using namespace CocosDenshion;

const float kFrameUpdateInterval = 0.02;

const float kBulletInterval = 0.2;
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
    
    // 初始状态是无敌
    m_status = SHIELD;
    
    // 初始火力
    m_weapon = SINGLE_GUN;
    
    // 初始子弹
    m_bullet_interval = kBulletInterval;
    
    // 初始不在移动状态
    m_moving = false;
    
    // 播放出生动画
    Animate* player_born_anim = Animate::create(AnimationCache::getInstance()->getAnimation("player_born_animation"));
    runAction(Sequence::create(Repeat::create(player_born_anim, 20), CallFunc::create([&](){
        m_status = SIMPLE; // 出生无敌时间过后就回归正常
        
        switch (m_head_direction)
        {
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
        setContentSize(m_size); // 重设尺寸（由于是异步，此时m_size已经被设置过）
    }), NULL) );
}

void Player::setGameScene(GameScene* game_scene)
{
    m_game_scene = game_scene;
}

void Player::setSize(Size size)
{
    m_size = size;
    setContentSize(m_size);
}

void Player::setDirection(JoyDirection direction)
{
    // 改变方向，如果摇杆归位了则不改变方向
    if (direction != NONE)
        m_head_direction = direction;
    
    // 根据不同朝向切换不同纹理，无敌状态不切换
    if (m_status == SIMPLE)
    {
        switch (m_head_direction)
        {
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
    }
    
    // 启停坦克位移的调度器
    if (direction != NONE)
        schedule(schedule_selector(Player::move), kFrameUpdateInterval);
    else
        unschedule(schedule_selector(Player::move));
}

void Player::move(float tm)
{
    if (!m_moving)
        return;
    
    // 间隔固定时间发出移动音效
    static int step_count = 0;
    step_count++;
    if (step_count % 8 == 0)
        SimpleAudioEngine::getInstance()->playEffect("sound/moving.wav");
    
    // 四个方向移动
    switch (m_head_direction)
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
            break;
        default:
            break;
    }
}

Bullet* Player::shootSingle()
{
    // 发射子弹音效
    SimpleAudioEngine::getInstance()->playEffect("sound/shoot.wav");
    
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
Vector<Bullet*> Player::shootDouble()
{
    // 发射子弹音效
    SimpleAudioEngine::getInstance()->playEffect("sound/shoot.wav");
    
    Vector<Bullet*> double_bullets;
    for (int i = 0; i < 2; i++)
    {
        // 子弹从坦克头部打出来
        Bullet* bullet = Bullet::create();
        
        BulletType bullet_type = BASE;
        // 第一发火力增强
        if (i == 0)
            bullet_type = POWER;
        bullet->initWithDirection(m_head_direction, bullet_type);
        switch (m_head_direction)
        {
            case UP:
                bullet->setPosition(getPositionX(),
                                    getPositionY() + getContentSize().height / 2);
                break;
            case DOWN:
                bullet->setPosition(getPositionX(),
                                    getPositionY() - getContentSize().height / 2);
                break;
            case LEFT:
                bullet->setPosition(getPositionX() - getContentSize().width / 2,
                                    getPositionY());
                break;
            case RIGHT:
                bullet->setPosition(getPositionX() + getContentSize().width / 2,
                                    getPositionY());
                break;
            default:
                break;
        }
        
        double_bullets.pushBack(bullet);
    }
    
    return double_bullets;
}

void Player::fetchItem(ItemType item_type)
{
    SimpleAudioEngine::getInstance()->playEffect("sound/bonus.wav");
    // 拾取不同道具，产生不同影响，有些需要调用游戏主场景回调函数
    switch (item_type)
    {
        case ACTIVE:
        {
            // 进入无敌模式
            m_status = SHIELD;
            Animate* player_born_anim = Animate::create(AnimationCache::getInstance()->getAnimation("player_born_animation"));
            runAction(Sequence::create(Repeat::create(player_born_anim, 20), CallFunc::create([&](){
                m_status = SIMPLE; // 无敌时间过后就回归正常
                
                switch (m_head_direction)
                {
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
                setContentSize(m_size); // 重设尺寸（由于是异步，此时m_size已经被设置过）
            }), NULL) );
        }
            break;
        case STAR:
            // 进入双子弹状态
            m_weapon = DOUBLE_GUN;
            break;
        case BOMB:
            if (m_game_scene)
                m_game_scene->onBomb();
            break;
        case SHOVEL:
            if (m_game_scene)
                m_game_scene->onShovel();
            break;
        case CLOCK:
            if (m_game_scene)
                m_game_scene->onClock();
            break;
        case MINITANK:
            if (m_game_scene)
                m_game_scene->onMiniTank();
            break;
    }
}

void Player::destroy()
{
    // 被摧毁音效
    SimpleAudioEngine::getInstance()->playEffect("sound/fexplosion.wav");
    
    // 删除坦克
    removeFromParent();
}
