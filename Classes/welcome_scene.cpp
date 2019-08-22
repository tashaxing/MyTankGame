#include "welcome_scene.h"
#include "game_scene.h"
#include "ui/CocosGUI.h"

using namespace ui; // button namespace

Scene* WelcomeScene::createScene()
{
    return WelcomeScene::create();
}

bool WelcomeScene::init()
{
    // super init first
    if ( !Scene::init() )
        return false;
    
    Point visible_origin = Director::getInstance()->getVisibleOrigin();
    Size visible_size = Director::getInstance()->getVisibleSize();
    
    // NOTICE: 在ios或者mac项目中必须把资源添加到工程，否则会找不到文件
    // 添加欢迎页背景
    Sprite* background = Sprite::create("img/menu/welcome.png");
    background->setContentSize(visible_size);
    background->setPosition(visible_origin.x + visible_size.width / 2,
                            visible_origin.y + visible_size.height / 2);
    addChild(background);
    
    // 添加开始游戏按钮
    Button* playgame_btn = Button::create("img/menu/playgame_normal.png", "img/menu/playgame_pressed.png");
    playgame_btn->setScale(1.5); // 尺寸适当调整
    playgame_btn->setPosition(Vec2(visible_origin.x + visible_size.width / 2,
                                visible_origin.y + visible_size.height / 2 - 20));
    playgame_btn->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type){
        // 按钮点击事件
        switch (type)
        {
            case ui::Widget::TouchEventType::BEGAN:
                break;
            case ui::Widget::TouchEventType::ENDED:
            {
                // 切换到主游戏场景
                auto game_scene = GameScene::createScene();
                TransitionScene* transition_scene = TransitionFade::create(0.5, game_scene);
                Director::getInstance()->replaceScene(transition_scene);
            }
                break;
            default:
                break;
        }
    });
    addChild(playgame_btn);
    
    return true;
}
