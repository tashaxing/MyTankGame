#include "game_scene.h"

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
