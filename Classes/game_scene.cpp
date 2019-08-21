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
    
    Point visible_origin = Director::getInstance()->getVisibleOrigin();
    Size visible_size = Director::getInstance()->getVisibleSize();
    
    // 播放开场音乐
    SimpleAudioEngine::getInstance()->playBackgroundMusic("sound/levelstarting.wav", false);
    
    // 关卡显示
    Sprite* level_splash = Sprite::create("img/menu/level.png");
    level_splash->setContentSize(visible_size);
    level_splash->setPosition(visible_origin.x + visible_size.width / 2, visible_origin.y + visible_size.height / 2);
    addChild(level_splash);
    
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
