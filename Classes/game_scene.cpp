#include "game_scene.h"
#include <SimpleAudioEngine.h>

using namespace CocosDenshion;

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
    
    // 播放开场音乐
    SimpleAudioEngine::getInstance()->playBackgroundMusic("sound/levelstarting.wav", false);
    
    // 关卡显示，加关卡数字
    Sprite* level_splash = Sprite::create("img/menu/level.png");
    level_splash->setContentSize(visible_size);
    
    Label* level_numer = Label::createWithTTF(std::to_string(21),
                                              "fonts/arial.ttf",
                                              14);
    level_numer->setColor(Color3B::BLACK);
    level_numer->setPosition(level_splash->getPositionX() + visible_size.width / 2 + 25,
                             level_splash->getPositionY() + visible_size.height / 2 + 11);
    level_splash->addChild(level_numer);
    
    level_splash->setPosition(visible_origin.x + visible_size.width / 2,
                              visible_origin.y + visible_size.height / 2);
    addChild(level_splash);
    
    // 播放动画，关卡显示消失
    
    // 加载游戏地图（tile地图必须保证tmx文件跟图片资源相对目录）
    std::string map_name = "img/map/Round" + std::to_string(m_round) + ".tmx";
    TMXTiledMap* tile_map = TMXTiledMap::create(map_name);
    tile_map->setAnchorPoint(Point(0.5, 0.5));
    tile_map->setPosition(visible_origin.x + visible_size.width / 2,
                          visible_origin.y + visible_size.height / 2);
    addChild(tile_map);
    
    return true;
}

void GameScene::update(float dt)
{
    
}

void GameScene::onEnter()
{
    Layer::onEnter();
    CCLOG("GameScene onEnter");
}

void GameScene::onExit()
{
    Layer::onExit();
    CCLOG("GameScene onExit");
}
