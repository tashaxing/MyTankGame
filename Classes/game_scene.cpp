#include <SimpleAudioEngine.h>
#include "game_scene.h"

using namespace CocosDenshion;

// 全局定义
const int kTotalRound = 20;

const int kMapZorder = 0;
const int kItemZorder = 1;
const int kJoypadZorder = 3;
const int kLevelSplashZorder = 5;

const float kEnemyBulletInterval = 1.0;

const float kTankSizeFactor = 0.8;
const int kEnemyBatchTankCount = 4;
const float kEnemyGenerateInterval = 5.0;

Scene* GameScene::createScene(int round)
{
    Scene* game_scene = Scene::create();
    GameScene* game_layer = GameScene::create();
    game_layer->initWithRound(round);
    
    game_scene->addChild(game_layer);
    return game_scene;
}

bool GameScene::init()
{
    if (!Layer::init())
        return false;
    
    return true;
}

void GameScene::initWithRound(int round)
{
    m_round = round;
    CCLOG("=== start level %d ===", m_round);
    
    // 初始化游戏标志
    m_is_over = false;
    
    // 初始化坦克定时标志
    m_is_clock = false;
    
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
    m_player1->setGameScene(this);
    m_player1->setSize(tile_size * 2 * kTankSizeFactor); // should fit tile size
    m_player1->setPosition(m_battle_field->getPositionX() + map_size.width / 2 - tile_size.width * 4,
                           m_battle_field->getPositionY() + tile_size.height); // 注意一个大方块由4个小方块拼成
    addChild(m_player1, kMapZorder);
    
    // 初始加载4个敌方坦克
    for (int i = 0; i < kEnemyBatchTankCount; i++)
        generateEnemy(0);
    schedule(schedule_selector(GameScene::generateEnemy), kEnemyGenerateInterval);
    schedule(schedule_selector(GameScene::emitEnemyBullet), kEnemyBulletInterval);
    
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
}

void GameScene::onEnumDirection(JoyDirection direction)
{
//    CCLOG("GameScene onEnumDirection: %d", direction);
    
    static JoyDirection pre_direction = NONE;
    // 只有方向改变时才给玩家坦克发控制指令
    if (direction != pre_direction)
    {
        m_player1->setDirection(direction);
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
            // TODO: every moment make sure only limit number bullets in the screen
            // FIXME: timer would wait at least one delay interval, here shoot a bullet at one btn click
            if (m_player_bullets.empty())
            {
                if (m_player1->m_weapon == SINGLE_GUN)
                {
                    Bullet* bullet = m_player1->shootSingle();
                    addChild(bullet, kMapZorder);
                    m_player_bullets.pushBack(bullet);
                }
                else if (m_player1->m_weapon == DOUBLE_GUN)
                {
                    Vector<Bullet*> double_bullets = m_player1->shootDouble();
                    for (Bullet* bullet : double_bullets)
                    {
                        addChild(bullet, kMapZorder);
                        m_player_bullets.pushBack(bullet);
                    }
                }
            }
            
            schedule(schedule_selector(GameScene::emitPlayerBullet), m_player1->m_bullet_interval);
        }
        else
            unschedule(schedule_selector(GameScene::emitPlayerBullet));
            
        pre_press_status = is_pressed;
    }
}

void GameScene::onBomb()
{
    SimpleAudioEngine::getInstance()->playEffect("sound/eexplosion.wav");
    for (Enemy* enemy : m_enemies)
    {
//        enemy->die();
        m_enemy_count--; // 敌人坦克减少
        enemy->removeFromParent();
    }
    m_enemies.clear(); // 这里统一清除
}

void GameScene::onClock()
{
    m_is_clock = true;
    
    for (Enemy* enemy : m_enemies)
        enemy->m_moving = false;
}

void GameScene::onShovel()
{
    m_battle_field->protectEagle();
}

void GameScene::onMiniTank()
{
    m_player1_life++;
}

void GameScene::generateEnemy(float tm)
{
    if (m_enemies.size() >= kEnemyBatchTankCount)
        return;
    
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
        enemy->setPosition(m_battle_field->getPositionX() + tile_size.width,
                           m_battle_field->getPositionY() + map_size.height - tile_size.height);
    else if (tank_pos_factor >= 1.0 / 3 && tank_pos_factor < 2.0 / 3)
        enemy->setPosition(m_battle_field->getPositionX() + map_size.width / 2,
                           m_battle_field->getPositionY() + map_size.height - tile_size.height);
    else
        enemy->setPosition(m_battle_field->getPositionX() + map_size.width - tile_size.width,
                           m_battle_field->getPositionY() + map_size.height - tile_size.height);
    
    // 随机生成方向
    float tank_direction_factor = CCRANDOM_0_1();
    if (tank_direction_factor < 0.25)
        enemy->setDirection(UP);
    else if (tank_direction_factor >= 0.25 && tank_direction_factor < 0.5)
        enemy->setDirection(DOWN);
    else if (tank_direction_factor >= 0.5 && tank_direction_factor < 0.75)
        enemy->setDirection(LEFT);
    else
        enemy->setDirection(RIGHT);
    
    addChild(enemy, kMapZorder);
    m_enemies.pushBack(enemy);
}

void GameScene::emitPlayerBullet(float tm)
{
    // 小技巧，如果子弹消失了则快速发射一颗，所以子弹发射频率会随着碰撞调整
    if (m_player_bullets.empty())
    {
        if (m_player1->m_weapon == SINGLE_GUN)
        {
            Bullet* bullet = m_player1->shootSingle();
            addChild(bullet, kMapZorder);
            m_player_bullets.pushBack(bullet);
        }
        else if (m_player1->m_weapon == DOUBLE_GUN)
        {
            Vector<Bullet*> double_bullets = m_player1->shootDouble();
            for (Bullet* bullet : double_bullets)
            {
                addChild(bullet, kMapZorder);
                m_player_bullets.pushBack(bullet);
            }
        }
    }
}

void GameScene::emitEnemyBullet(float tm)
{
    for (Enemy* enemy : m_enemies)
    {
        // 不同的敌人坦克根据类型，发射子弹频率不同
        float enemy_shoot_factor = CCRANDOM_0_1();
        if (enemy->m_type == NORMAL && enemy_shoot_factor >= 0.5
            || enemy->m_type == ARMOR && enemy_shoot_factor >= 0.2
            || enemy->m_type == SPEED && enemy_shoot_factor >= 0.7)
        {
            Bullet* bullet = enemy->shoot();
            addChild(bullet, kMapZorder);
            m_enemy_bullets.pushBack(bullet);
        }
    }
}

void GameScene::generateItem()
{
    SimpleAudioEngine::getInstance()->playEffect("sound/tbonushit.wav");
    
    Size map_size = m_battle_field->getContentSize();
    Size map_array = m_battle_field->getMapSize();
    // tmx地图的方格必须用行列值重新计算，getTileSize()是不准确的
    Size tile_size = Size(map_size.width / map_array.width, map_size.height / map_array.height);
    
    // 随机类型
    ItemType item_type;
    float item_type_factor = CCRANDOM_0_1();
    if (item_type_factor < 1.0 / 6)
        item_type = ACTIVE;
    else if (item_type_factor >= 1.0 / 6 && item_type_factor < 2.0 / 6)
        item_type = STAR;
    else if (item_type_factor >= 2.0 / 6 && item_type_factor < 3.0 / 6)
        item_type = BOMB;
    else if (item_type_factor >= 3.0 / 6 && item_type_factor < 4.0 / 6)
        item_type = SHOVEL;
    else if (item_type_factor >= 4.0 / 6 && item_type_factor < 5.0 / 6)
        item_type = CLOCK;
    else
        item_type = MINITANK;
    
    Item* item = Item::create();
    item->initWithType(item_type);
    
    // 随机位置
    float item_posx_factor = CCRANDOM_0_1();
    float pos_x = m_battle_field->getPositionX() + (map_size.width - tile_size.width * 2) * item_posx_factor;
    float item_posy_factor = CCRANDOM_0_1();
    float pos_y = m_battle_field->getPositionY() + (map_size.height - tile_size.height * 2) * item_posy_factor;
    item->setPosition(pos_x, pos_y);
    
    addChild(item, kItemZorder);
    m_items.pushBack(item);
}

void GameScene::gameWin()
{
    CCLOG("game win");
    
    SimpleAudioEngine::getInstance()->playBackgroundMusic("sound/gamewin.wav", false);
    
    // 延时一会儿再切换关卡,通关后从第一关开始
    int next_round = m_round + 1; // 关卡提升
    if (next_round > kTotalRound)
        next_round = 1;
    
    Scene* scene = GameScene::createScene(next_round);
    TransitionScene* transition_scene = TransitionFade::create(0.5, scene);
    Director::getInstance()->replaceScene(transition_scene);
}

void GameScene::gameOver()
{
    CCLOG("game over");
    SimpleAudioEngine::getInstance()->playEffect("sound/gameover.wav", false);
    m_is_over = true;
    
    // 游戏结束的标签
    Point visible_origin = Director::getInstance()->getVisibleOrigin();
    Size visible_size = Director::getInstance()->getVisibleSize();
    
    Label *gameover_label = Label::createWithTTF("game over", "fonts/Marker Felt.ttf", 24);
    gameover_label->setColor(Color3B::WHITE);
    gameover_label->setPosition(visible_origin.x + visible_size.width / 2,
                                visible_origin.y - 30);
    addChild(gameover_label, kLevelSplashZorder); // 分数浮层在最上方
    
    // 播放动画，飞入画面
    auto move_to = MoveTo::create(1.0, Point(visible_origin.x + visible_size.width / 2,
                                             visible_origin.y + visible_size.height / 2));
    gameover_label->runAction(move_to);
}

void GameScene::update(float dt)
{
//    CCLOG("update delta: %f", dt);
    
    // 判断游戏胜利
    if (m_enemy_count <= 0)
        gameWin();
    
    // ==== 碰撞检测 ====
    // --- 老鹰总部被子弹击中 ---
    if (!m_is_over)
    {
        for (Bullet* bullet : m_enemy_bullets)
        {
            if (m_battle_field->isEagleHurt(bullet->getBoundingBox()))
            {
                SimpleAudioEngine::getInstance()->playEffect("sound/fexplosion.wav");
                gameOver();
                m_player1->m_moving = false;
                return;
            }
        }
        
        for (Bullet* bullet : m_player_bullets)
        {
            if (m_battle_field->isEagleHurt(bullet->getBoundingBox()))
            {
                SimpleAudioEngine::getInstance()->playEffect("sound/fexplosion.wav");
                gameOver();
                m_player1->m_moving = false;
                return;
            }
        }
    }
    
    // --- 玩家被子弹击中 ---
    if (!m_is_over)
    {
        for (Bullet* bullet : m_enemy_bullets)
        {
            if (m_player1->getBoundingBox().intersectsRect(bullet->getBoundingBox()))
            {
                // 消除子弹
                m_enemy_bullets.eraseObject(bullet);
                bullet->removeFromParent();
                
                // 只有普通模式会生效，无敌模式不管
                if (m_player1->m_status == SIMPLE)
                {
                    m_player1->destroy(); // 此处玩家坦克已销毁，后续不再判断其碰撞
                    
                    if (m_player1_life == 0)
                    {
                        gameOver();
                        return;
                    }
                    else
                    {
                        // 加载玩家坦克
                        Size map_size = m_battle_field->getContentSize();
                        Size map_array = m_battle_field->getMapSize();
                        // tmx地图的方格必须用行列值重新计算，getTileSize()拿到的值应该是大方块的尺寸，不准确的
                        Size tile_size = Size(map_size.width / map_array.width, map_size.height / map_array.height);
                        
                        m_player1 = Player::create();
                        m_player1->initWithType(P1);
                        m_player1->setGameScene(this);
                        m_player1->setSize(tile_size * 2 * kTankSizeFactor); // should fit tile size
                        m_player1->setPosition(m_battle_field->getPositionX() + map_size.width / 2 - tile_size.width * 4,
                                               m_battle_field->getPositionY() + tile_size.height); // 注意一个大方块由4个小方块拼成
                        addChild(m_player1, kMapZorder);
                        
                        m_player1_life--;
                    }
                }
            }
        }
    }

    // --- 敌人坦克被子弹击中 ---
    Vector<Bullet*> hit_bullets;
    for (Bullet* bullet : m_player_bullets)
    {
        for (Enemy* enemy : m_enemies)
        {
            if (!bullet->m_hit_flag
                && enemy->m_life > 0
                && bullet->getBoundingBox().intersectsRect(enemy->getBoundingBox()))
            {
                enemy->hit();
                
                // 击中敌人爆装备
                if (enemy->m_type == ARMOR)
                    generateItem();
                
                // 敌人生命值减到0则死亡
                if (enemy->m_life == 0)
                {
                    enemy->die();
                    m_enemies.eraseObject(enemy);
                    m_enemy_count--; // 敌人坦克减少
                    enemy->removeFromParent();
                }
                
                bullet->m_hit_flag = true;
                hit_bullets.pushBack(bullet);
            }
        }
    }
    for (Bullet* bullet : hit_bullets)
    {
        m_player_bullets.eraseObject(bullet);
        bullet->removeFromParent();
    }
    hit_bullets.clear();
    
    // --- 子弹击中砖块 ---
    for (Bullet* bullet : m_player_bullets)
    {
        if (m_battle_field->isBulletCollide(bullet->getBoundingBox(), bullet->m_type))
        {
            m_player_bullets.eraseObject(bullet);
            bullet->removeFromParent(); // 标记起来统一清除，避免指针问题
        }
    }
    
    for (Bullet* bullet : m_enemy_bullets)
    {
        if (m_battle_field->isBulletCollide(bullet->getBoundingBox(), bullet->m_type))
            hit_bullets.pushBack(bullet); // 标记起来统一清除，避免指针问题
    }
    
    for (Bullet* bullet : hit_bullets)
    {
        m_enemy_bullets.eraseObject(bullet);
        bullet->removeFromParent();
    }
    hit_bullets.clear();
    
    // --- 玩家子弹跟敌人子弹抵消 ---
    for (Bullet* player_bullet : m_player_bullets)
    {
        for (Bullet* enemy_bullet : m_enemy_bullets)
        {
            if (player_bullet->getBoundingBox().intersectsRect(enemy_bullet->getBoundingBox()))
            {
                hit_bullets.pushBack(enemy_bullet);
                hit_bullets.pushBack(player_bullet);
                break;
            }
        }
    }
    for (Bullet* bullet : hit_bullets)
    {
        if (m_enemy_bullets.contains(bullet))
        {
            m_enemy_bullets.eraseObject(bullet);
            bullet->removeFromParent();
        }
        
        if (m_player_bullets.contains(bullet))
        {
            m_player_bullets.eraseObject(bullet);
            bullet->removeFromParent();
        }
    }
    hit_bullets.clear();
    
    // --- 敌人坦克遇到障碍时则变换方向 ---
    for (Enemy* enemy : m_enemies)
    {
        if (m_battle_field->isTankCollide(enemy->getBoundingBox(), enemy->m_head_direction))
        {
            CCLOG("enemy can not move");
            enemy->m_moving = false;
            enemy->changeDirection();
        }
        else
        {
            CCLOG("enemy can move");
            enemy->m_moving = true;
        }
    }
    
    // --- 玩家遇到砖块时停止移动 ---
    if (!m_is_over)
    {
        if (m_battle_field->isTankCollide(m_player1->getBoundingBox(), m_player1->m_head_direction))
        {
            CCLOG("player can not move");
            m_player1->m_moving = false;
        }
        else
        {
            CCLOG("player can move");
            m_player1->m_moving = true;
        }
    }
    
    // --- 玩家碰到敌人坦克时停止移动 ---
    if (!m_is_over)
    {
        for (Enemy* enemy : m_enemies)
        {
            if (m_player1->getBoundingBox().intersectsRect(enemy->getBoundingBox()))
            {
                CCLOG("player can not move");
                m_player1->m_moving = false;
                break;
            }
        }
    }
    
    // --- 玩家坦克拾取道具 ---
    if (!m_is_over)
    {
        for (Item* item : m_items)
        {
            if (m_player1->getBoundingBox().intersectsRect(item->getBoundingBox()))
            {
                m_player1->fetchItem(item->m_type);
                m_items.eraseObject(item);
                item->removeFromParent();
            }
        }
    }
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
