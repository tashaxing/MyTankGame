#include <SimpleAudioEngine.h>
#include "game_scene.h"

using namespace CocosDenshion;

const int kMapZorder = 0;
const int kItemZorder = 1;
const int kJoypadZorder = 3;
const int kLevelSplashZorder = 5;

const float kTankSizeFactor = 0.8;

Scene* GameScene::createScene()
{
    Scene *game_scene = Scene::create();
    Layer *game_layer = GameScene::create();
    game_scene->addChild(game_layer);
    return game_scene;
}

bool GameScene::init()
{
    if (!Layer::init())
        return false;
    
    CCLOG("=== start level %d ===", m_round);
    
    Point visible_origin = Director::getInstance()->getVisibleOrigin();
    Size visible_size = Director::getInstance()->getVisibleSize();
    
    // 加载关卡地图
    m_battle_field = BattleField::create();
    m_battle_field->initWithLevel(m_round);
    addChild(m_battle_field, kMapZorder);
    
    Size tile_size = m_battle_field->getTileSize();
    Size map_array = m_battle_field->getMapSize();
    Size map_size = m_battle_field->getContentSize();
    
    // print map info
    CCLOG("tile size, width %f, height %f", tile_size.width, tile_size.height);
    CCLOG("map array, row %f, col %f", map_array.height, map_array.width);
    CCLOG("map size, width %f, height %f", map_size.width, map_size.height);
    
    // 加载玩家坦克
    m_player1 = Player::create();
    m_player1->initWithType(P1);
    m_player1->setSize(tile_size * kTankSizeFactor); // should fit tile size
    m_player1->setPosition(m_battle_field->getPositionX() + map_size.width / 2 - tile_size.width * 2,
                           m_battle_field->getPositionY() + tile_size.height / 2);
    addChild(m_player1, kMapZorder);
    
    // 加载敌方坦克
    Enemy* enemy = Enemy::create();
    enemy->initWithType(NORMAL);
    enemy->setSize(tile_size * kTankSizeFactor);
    enemy->setPosition(m_battle_field->getPositionX() + tile_size.width / 2,
                       m_battle_field->getPositionY() + map_size.height - tile_size.height / 2);
    addChild(enemy, kMapZorder);
    
    // 加载摇杆控制
    m_joypad = Joypad::create();
    m_joypad->setJoystickType(KEY4); // 四个方向键位
    m_joypad->setGameScene(this); // 绑定该游戏场景被摇杆控制
    addChild(m_joypad, kJoypadZorder);
    
    // 播放开场音乐
    SimpleAudioEngine::getInstance()->playBackgroundMusic("sound/levelstarting.wav", false);
    
    // 关卡显示，加关卡数字
    Sprite* level_splash = Sprite::create("img/menu/level.png");
    level_splash->setContentSize(visible_size);
    
    Label* level_numer = Label::createWithTTF(std::to_string(m_round),
                                              "fonts/arial.ttf",
                                              14);
    level_numer->setColor(Color3B::BLACK);
    level_numer->setPosition(level_splash->getPositionX() + visible_size.width / 2 + 25,
                             level_splash->getPositionY() + visible_size.height / 2 + 11); // adjust position xy
    level_splash->addChild(level_numer);
    level_splash->setPosition(visible_origin.x + visible_size.width / 2,
                              visible_origin.y + visible_size.height / 2);
    addChild(level_splash, kLevelSplashZorder);
    
    // 播放动画，关卡显示消失
    auto move_by = MoveBy::create(0.3, Vec2(0, visible_size.height));
    level_splash->runAction(Sequence::create(DelayTime::create(1.0), move_by, NULL)); // FIXME: remove in callback
    
    // 默认渲染更新
    scheduleUpdate();
    
    return true;
}

void GameScene::onEnumDirection(JoyDirection direction)
{
//    CCLOG("GameScene onEnumDirection: %d", direction);
    
    static JoyDirection pre_direction = NONE;
    // 只有方向改变时才给玩家坦克发控制指令
    if (direction != pre_direction)
    {
        m_player1->setMoveDirection(direction);
        pre_direction = direction;
    }
}

void GameScene::onAngleDirection(float angle)
{
//    CCLOG("GameScene onAngleDirection: %f", angle);
    // no need in this game
}

void GameScene::onFireBtn(bool is_pressed)
{
//    CCLOG("GameScene onFireBtn: %s", is_pressed ? "yes" : "no");
    
    static bool pre_press_status = false;
    if (is_pressed != pre_press_status)
    {
        // 调度子弹，同时考虑单发和i连发
        if (is_pressed)
        {
            // TODO: every moment make sure only limit number bullet in the screen
            // FIXME: timer would wait at least one delay interval, here shoot a bullet at one
            Bullet* bullet = m_player1->shootSingle();
            addChild(bullet, kMapZorder);
            
            schedule(schedule_selector(GameScene::emitPlayerBullet), m_player1->m_bullet_interval);
        }
        else
            unschedule(schedule_selector(GameScene::emitPlayerBullet));
            
        pre_press_status = is_pressed;
    }
}

void GameScene::emitPlayerBullet(float tm)
{
    Bullet* bullet = m_player1->shootSingle();
    addChild(bullet, kMapZorder);
}

void GameScene::generateEnemey()
{
    // 在适当时候生成敌方坦克
    
    // 随机改变方向，一直移动中
}

void GameScene::update(float dt)
{
//    CCLOG("update delta: %f", dt);
    
    // --- 碰撞检测 ---
    // 玩家移动障碍
    if (m_battle_field->isCollide(m_player1->getBoundingBox(), m_player1->m_head_direction))
    {
        CCLOG("player can not move");
        m_player1->m_move_enable = false;
    }
    else
    {
        CCLOG("player can move");
        m_player1->m_move_enable = true;
    }
    
    
    
    // --- 场景管理 ---
}

void GameScene::onEnter()
{
    Layer::onEnter();
//    CCLOG("GameScene onEnter");
}

void GameScene::onExit()
{
    Layer::onExit();
//    CCLOG("GameScene onExit");
}
