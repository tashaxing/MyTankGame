#include "player.h"

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
        case P2:
            setTexture("img/tank/player2/player2U.png");
            break;
        default:
            break;
    }
    
    // 播放出生动画
    
}

void Player::fetchItem(ItemType item_type)
{
    // 拾取不同道具，产生不同影响
    
}
