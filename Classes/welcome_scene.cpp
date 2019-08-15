#include "welcome_scene.h"
#include "game_scene.h"

Scene* WelcomeScene::createScene()
{
    return WelcomeScene::create();
}

bool WelcomeScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
        return false;
    
    return true;
}
