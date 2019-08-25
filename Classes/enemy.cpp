#include "enemy.h"

bool Enemy::init()
{
    if (!Sprite::init())
        return false;
    
    return true;
}

void Enemy::initWithType(EnemyType enemy_type)
{
    m_type = enemy_type;
    
    // 根据纹理初始化不同的坦克
}
