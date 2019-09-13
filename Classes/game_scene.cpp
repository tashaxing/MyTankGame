#include <SimpleAudioEngine.h>
#include "game_scene.h"

using namespace CocosDenshion;

// 全局定义
const int kMapZorder = 0;
const int kItemZorder = 1;
const int kJoypadZorder = 3;
const int kLevelSplashZorder = 5;

const float kBulletGenerateInterval = 2.0;

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
    
    Size map_size = m_battle_field->getContentSize();
    Size map_array = m_battle_field->getMapSize();
    // tmx地图的方格必须用行列值重新计算，getTileSize()拿到的值应该是大方块的尺寸，不准确的
    Size tile_size = Size(map_size.width / map_array.width, map_size.height / map_array.height);
    
    CCLOG("tile size, width %f, height %f", tile_size.width, tile_size.height);
    CCLOG("map array, row %f, col %f", map_array.height, map_array.width);
    CCLOG("map size, width %f, height %f", map_size.width, map_size.height);
    
    // 加载玩家坦克
    m_player1 = Player::create();
    m_player1->initWithType(P1);
    m_player1->setSize(tile_size * 2 * kTankSizeFactor); // should fit tile size
    m_player1->setPosition(m_battle_field->getPositionX() + map_size.width / 2 - tile_size.width * 4,
                           m_battle_field->getPositionY() + tile_size.height); // 注意一个大方块由4个小方块拼成
    addChild(m_player1, kMapZorder);
    
    // 加载敌方坦克, 每批增加六个
    for (int i = 0; i < 6; i++)
        generateEnemy();
    
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
            if (m_player_bullets.empty())
            {
                Bullet* bullet = m_player1->shootSingle();
                addChild(bullet, kMapZorder);
                m_player_bullets.pushBack(bullet);
            }
            
            schedule(schedule_selector(GameScene::emitPlayerBullet), m_player1->m_bullet_interval);
        }
        else
            unschedule(schedule_selector(GameScene::emitPlayerBullet));
            
        pre_press_status = is_pressed;
    }
}

void GameScene::emitPlayerBullet(float tm)
{
    // 小技巧，如果子弹消失了则快速发射一颗，所以子弹发射频率会随着碰撞调整
    if (m_player_bullets.empty())
    {
        Bullet* bullet = m_player1->shootSingle();
        addChild(bullet, kMapZorder);
        m_player_bullets.pushBack(bullet);
    }
}

void GameScene::generateEnemy()
{
    Size map_size = m_battle_field->getContentSize();
    Size map_array = m_battle_field->getMapSize();
    // tmx地图的方格必须用行列值重新计算，getTileSize()是不准确的
    Size tile_size = Size(map_size.width / map_array.width, map_size.height / map_array.height);
    
    // 根据概率生成敌方坦克
    float tank_type_factor = CCRANDOM_0_1();
    EnemyType enemy_type;
    if (tank_type_factor < 0.6)
        enemy_type = NORMAL;
    else if (tank_type_factor >= 0.6 && tank_type_factor < 0.9)
        enemy_type = ARMOR;
    else
        enemy_type = SPEED;
    
    Enemy* enemy = Enemy::create();
    enemy->initWithType(enemy_type);
    enemy->setSize(tile_size * 2 * kTankSizeFactor);
    
    // 随机生成位置，顶部三个空位
    float tank_pos_factor = CCRANDOM_0_1();
    if (tank_pos_factor <= 1.0 / 3)
        enemy->setPosition(m_battle_field->getPositionX() + tile_size.width / 2,
                           m_battle_field->getPositionY() + map_size.height - tile_size.height / 2);
    else if (tank_pos_factor >= 1.0 / 3 && tank_pos_factor < 2.0 / 3)
        enemy->setPosition(m_battle_field->getPositionX() + map_size.width / 2,
                           m_battle_field->getPositionY() + map_size.height - tile_size.height / 2);
    else
        enemy->setPosition(m_battle_field->getPositionX() + tile_size.width - tile_size.width / 2,
                           m_battle_field->getPositionY() + map_size.height - tile_size.height / 2);
    
    // 随机改变方向，一直移动中
    float tank_direction_factor = CCRANDOM_0_1();
    if (tank_direction_factor < 0.25)
        enemy->setMoveDirection(UP);
    else if (tank_direction_factor >= 0.25 && tank_direction_factor < 0.5)
        enemy->setMoveDirection(DOWN);
    else if (tank_direction_factor >= 0.5 && tank_direction_factor < 0.75)
        enemy->setMoveDirection(LEFT);
    else
        enemy->setMoveDirection(RIGHT);
    
    addChild(enemy, kMapZorder);
}

void GameScene::update(float dt)
{
//    CCLOG("update delta: %f", dt);
    
    // --- 碰撞检测 ---
    // 子弹击中
    for (Bullet* bullet : m_player_bullets)
    {
        if (m_battle_field->isBulletCollide(bullet->getBoundingBox(), bullet->m_type))
        {
            m_player_bullets.eraseObject(bullet);
            bullet->removeFromParent();
        }
    }
    
    // 玩家移动障碍
    if (m_battle_field->isTankCollide(m_player1->getBoundingBox(), m_player1->m_head_direction))
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
