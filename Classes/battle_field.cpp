#include "battle_field.h"

const TileType kTileHash[] = {
    BLANK,
    BLANK, BLANK, GRASS, GRASS, STEEL, STEEL,
    BLANK, BLANK, GRASS, GRASS, STEEL, STEEL,
    WALL, WALL, RIVER, RIVER, EAGLE, EAGLE,
    WALL, WALL, RIVER, RIVER, EAGLE, EAGLE,
    EAGLE, EAGLE, BLANK, BLANK, BLANK, BLANK,
    EAGLE, EAGLE, BLANK, BLANK, BLANK, BLANK
};

bool BattleField::init()
{
    if (!TMXTiledMap::init())
        return false;
    
    return true;
}

void BattleField::initWithLevel(int round)
{
    Point visible_origin = Director::getInstance()->getVisibleOrigin();
    Size visible_size = Director::getInstance()->getVisibleSize();
    
    // 加载游戏地图（tile地图必须保证tmx文件跟图片资源相对目录）
    std::string map_name = "img/map/Round" + std::to_string(round) + ".tmx";
    initWithTMXFile(map_name);
    //    tile_map->setScale(kScaleFactor); // this works
    setAnchorPoint(Point::ZERO); // 地图左下角作为锚点方便定位
    setPosition(visible_origin.x + visible_size.width / 2 - getContentSize().width / 2,
                          visible_origin.y + visible_size.height / 2 - getContentSize().height / 2);
}

bool BattleField::isCollide(Rect bounding_box)
{
    // 这里的包围盒坐标要转换成相对地图左下角原点的坐标
    bounding_box = Rect(bounding_box.origin.x - getPositionX(), bounding_box.origin.y - getPositionY(),
                        bounding_box.size.width, bounding_box.size.height);
    
    Size map_size = getContentSize();
    Size tile_size = getTileSize();
    
//    if (bounding_box.getMinX() <= 0.1 || bounding_box.getMaxX() >= map_size.width
//        || bounding_box.getMinY() <= 0.1 || bounding_box.getMaxY() >= map_size.height)
//        return true;
    
    TMXLayer* layer0 = getLayer("layer_0");
//    TMXLayer* layer1 = getLayer("layer_1");
    
    // 根据包围盒四个角的坐标判断gid对应的tile类别
    int left_down_gid = layer0->getTileGIDAt(Vec2((int)bounding_box.getMinX() / tile_size.width,
                                                  (int)bounding_box.getMinY() / tile_size.height));
    int left_up_gid = layer0->getTileGIDAt(Vec2((int)bounding_box.getMinX() / tile_size.width,
                                                (int)bounding_box.getMaxY() / tile_size.height));
    int right_down_gid = layer0->getTileGIDAt(Vec2((int)bounding_box.getMaxX() / tile_size.width,
                                                (int)bounding_box.getMinY() / tile_size.height));
    int right_up_gid = layer0->getTileGIDAt(Vec2((int)bounding_box.getMaxX() / tile_size.width,
                                                (int)bounding_box.getMaxY() / tile_size.height));
    CCLOG("gid left_down: %d, left_up: %d, right_down: %d, right_up: %d",
          left_down_gid, left_up_gid, right_down_gid, right_up_gid);
    
    // 若有一个定点所在范围不是空白或草坪就算碰撞（其实如果考虑有船过水的话要复杂一些）
    if (kTileHash[left_down_gid] != BLANK && kTileHash[left_down_gid] != GRASS
        || kTileHash[left_up_gid] != BLANK && kTileHash[left_up_gid] != GRASS
        || kTileHash[right_down_gid] != BLANK && kTileHash[right_down_gid] != GRASS
        || kTileHash[right_up_gid] != BLANK && kTileHash[right_up_gid] != GRASS)
        return true;
    
    return false;
}