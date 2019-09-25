#include <SimpleAudioEngine.h>
#include "battle_field.h"

using namespace CocosDenshion;

// tmx地图方块gid与方块类型的映射
const TileType kTileHash[] = {
    BLANK, // must keep the 0-th as BLANK
    BLANK, BLANK, GRASS, GRASS, STEEL, STEEL,
    BLANK, BLANK, GRASS, GRASS, STEEL, STEEL,
    WALL, WALL, RIVER, RIVER, EAGLE, EAGLE,
    WALL, WALL, RIVER, RIVER, EAGLE, EAGLE,
    DEAD_EAGLE, DEAD_EAGLE, BLANK, BLANK, BLANK, BLANK, // 25, 26
    DEAD_EAGLE, DEAD_EAGLE, BLANK, BLANK, BLANK, BLANK  // 31, 32
};

// 老鹰周围砖块坐标
const Vec2 kEagleFence[] = {
    Vec2(11, 25),
    Vec2(11, 24),
    Vec2(11, 23),
    Vec2(12, 23),
    Vec2(13, 23),
    Vec2(14, 23),
    Vec2(14, 24),
    Vec2(14, 25)
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
    //    tile_map->setScale(kScaleFactor); // this works to make map looks bigger
    
    // print map info
    Size map_size = getContentSize();
    Size map_array = getMapSize();
    // tmx地图的方格必须用行列值重新计算，getTileSize()是不准确的
    Size tile_size = Size(map_size.width / map_array.width, map_size.height / map_array.height);
    
    CCLOG("tile size, width %f, height %f", tile_size.width, tile_size.height);
    CCLOG("map array, row %f, col %f", map_array.height, map_array.width);
    CCLOG("map size, width %f, height %f", map_size.width, map_size.height);
    
    // 画外框线，地图边缘外扩留点余量，局部坐标系，以左下角为原点
    DrawNode* map_frame = DrawNode::create();
    Point rect[4] = {
        Point(-1.0, -1.0),
        Point(getContentSize().width + 1.0, -1.0),
        Point(getContentSize().width + 1.0, getContentSize().height + 1.0),
        Point(-1.0, getContentSize().height + 1.0)
    };
    map_frame->drawPolygon(rect, 4, Color4F(0, 0, 0, 1), 0.6, Color4F::YELLOW);
    addChild(map_frame, -1); // 置于底层
    
    setAnchorPoint(Point::ZERO); // 地图整体左下角作为锚点方便全局定位
    setPosition(visible_origin.x + visible_size.width / 2 - getContentSize().width / 2,
                          visible_origin.y + visible_size.height / 2 - getContentSize().height / 2);
}

bool BattleField::isBulletCollide(Rect bounding_box, BulletType bullet_type)
{
    // 这里的包围盒坐标要转换成相对地图左下角原点的坐标
    bounding_box = Rect(bounding_box.origin.x - getPositionX(), bounding_box.origin.y - getPositionY(),
                        bounding_box.size.width, bounding_box.size.height);
    
    Size map_size = getContentSize();
    Size map_array = getMapSize();
    // tmx地图的方格必须用行列值重新计算，getTileSize()是不准确的
    Size tile_size = Size(map_size.width / map_array.width, map_size.height / map_array.height);
    
    // 不能出界
    if (bounding_box.getMinX() <= 0.1 || bounding_box.getMaxX() >= map_size.width - 0.1
        || bounding_box.getMinY() <= 0.1 || bounding_box.getMaxY() >= map_size.height - 0.1)
    {
        CCLOG("bullet hit border");
        return true;
    }
    
    TMXLayer* layer0 = getLayer("layer_0");
//    TMXLayer* layer1 = getLayer("layer_1");
    
    // 根据包围盒四个角的坐标判断gid对应的tile类别
    // tmx地图从左上角算(0, 0)原点，要做坐标转换
    float MinX = bounding_box.getMinX();
    float MaxX = bounding_box.getMaxX();
    float MinY = map_size.height - bounding_box.getMinY();
    float MaxY = map_size.height - bounding_box.getMaxY();
    
    // 小幅度修正一下边界值，避免地图标号超范围
    if (MaxX >= map_size.width)
        MaxX = map_size.width - 0.1;
    if (MinY >= map_size.height)
        MinY = map_size.height - 0.1;
    
    int left_down_gid = layer0->getTileGIDAt(Vec2(int(MinX / tile_size.width),
                                                  int(MinY / tile_size.height)));
    int left_up_gid = layer0->getTileGIDAt(Vec2(int(MinX / tile_size.width),
                                                int(MaxY / tile_size.height)));
    int right_down_gid = layer0->getTileGIDAt(Vec2(int(MaxX / tile_size.width),
                                                   int(MinY / tile_size.height)));
    int right_up_gid = layer0->getTileGIDAt(Vec2(int(MaxX / tile_size.width),
                                                 int(MaxY / tile_size.height)));

    // 子弹击中，消除对应的砖块
    // 普通子弹只能打土砖
    bool hit_flag = false;
    if (bullet_type == BASE)
    {
        // 可能一次打两个小块
        if (kTileHash[left_down_gid] == WALL)
        {
            SimpleAudioEngine::getInstance()->playEffect("sound/brickhit.wav");
            layer0->removeTileAt(Vec2(int(MinX / tile_size.width),
                                      int(MinY / tile_size.height)));
            hit_flag = true;
        }
        
        if (kTileHash[left_up_gid] == WALL)
        {
            SimpleAudioEngine::getInstance()->playEffect("sound/brickhit.wav");
            layer0->removeTileAt(Vec2(int(MinX / tile_size.width),
                                      int(MaxY / tile_size.height)));
            hit_flag = true;
        }
            
        if (kTileHash[right_down_gid] == WALL)
        {
            SimpleAudioEngine::getInstance()->playEffect("sound/brickhit.wav");
            layer0->removeTileAt(Vec2(int(MaxX / tile_size.width),
                                      int(MinY / tile_size.height)));
            hit_flag = true;
        }
            
        if (kTileHash[right_up_gid] == WALL)
        {
            SimpleAudioEngine::getInstance()->playEffect("sound/brickhit.wav");
            layer0->removeTileAt(Vec2(int(MaxX / tile_size.width),
                                      int(MaxY / tile_size.height)));
            hit_flag = true;
        }
        
        // 如果遇到钢板则直接算碰撞，打不了砖块
        if (kTileHash[left_down_gid] == STEEL
            || kTileHash[left_up_gid] == STEEL
            || kTileHash[right_down_gid] == STEEL
            || kTileHash[right_up_gid] == STEEL)
        {
            SimpleAudioEngine::getInstance()->playEffect("sound/steelhit.wav");
            hit_flag = true;
        }
    }
    else if (bullet_type == POWER)
    {
        // 火力子弹可以打钢板
        if (kTileHash[left_down_gid] == WALL || kTileHash[left_down_gid] == STEEL)
        {
            SimpleAudioEngine::getInstance()->playEffect("sound/brickhit.wav");
            layer0->removeTileAt(Vec2(int(MinX / tile_size.width),
                                      int(MinY / tile_size.height)));
            hit_flag = true;
        }
        
        if (kTileHash[left_up_gid] == WALL || kTileHash[left_up_gid] == STEEL)
        {
            SimpleAudioEngine::getInstance()->playEffect("sound/brickhit.wav");
            layer0->removeTileAt(Vec2(int(MinX / tile_size.width),
                                      int(MaxY / tile_size.height)));
            hit_flag = true;
        }
        
        if (kTileHash[right_down_gid] == WALL || kTileHash[right_down_gid] == STEEL)
        {
            SimpleAudioEngine::getInstance()->playEffect("sound/brickhit.wav");
            layer0->removeTileAt(Vec2(int(MaxX / tile_size.width),
                                      int(MinY / tile_size.height)));
            hit_flag = true;
        }
        
        if (kTileHash[right_up_gid] == WALL || kTileHash[right_up_gid] == STEEL)
        {
            SimpleAudioEngine::getInstance()->playEffect("sound/brickhit.wav");
            layer0->removeTileAt(Vec2(int(MaxX / tile_size.width),
                                      int(MaxY / tile_size.height)));
            hit_flag = true;
        }
    }
    
    return hit_flag;
}

bool BattleField::isTankCollide(Rect bounding_box, JoyDirection direction)
{
    // 这里的包围盒坐标要转换成相对地图左下角原点的坐标，包围盒本身也是左下角为原点
    bounding_box = Rect(bounding_box.origin.x - getPositionX(), bounding_box.origin.y - getPositionY(),
                        bounding_box.size.width, bounding_box.size.height);
    
    Size map_size = getContentSize();
    Size map_array = getMapSize();
    // tmx地图的方格必须用行列值重新计算，getTileSize()是不准确的
    Size tile_size = Size(map_size.width / map_array.width, map_size.height / map_array.height);
    
    // 不能出界
    if (bounding_box.getMinX() <= 0.1 && direction == LEFT
        || bounding_box.getMaxX() >= map_size.width - 0.1 && direction == RIGHT
        || bounding_box.getMinY() <= 0.1 && direction == DOWN
        || bounding_box.getMaxY() >= map_size.height - 0.1 && direction == UP)
    {
        CCLOG("nearly out of border");
        return true;
    }
    
    TMXLayer* layer0 = getLayer("layer_0");
    //    TMXLayer* layer1 = getLayer("layer_1");
    
    // 根据包围盒四个角的行列坐标判断gid对应的tile类别
    // tmx地图从左上角算(0, 0)行列值的原点，要做坐标转换
    // FIXME: 优化点，确保坦克能平滑过空白，又不卡进墙里
    float MinX = bounding_box.getMinX();
    float MaxX = bounding_box.getMaxX();
    float MinY = map_size.height - bounding_box.getMinY();
    float MaxY = map_size.height - bounding_box.getMaxY();
    
    // 小幅度修正一下边界值，避免地图标号超范围
    if (MaxX >= map_size.width)
        MaxX = map_size.width - 0.1;
    if (MinY >= map_size.height)
        MinY = map_size.height - 0.1;
    
    int left_down_gid = layer0->getTileGIDAt(Vec2(int(MinX / tile_size.width),
                                                  int(MinY / tile_size.height)));
    int left_up_gid = layer0->getTileGIDAt(Vec2(int(MinX / tile_size.width),
                                                int(MaxY / tile_size.height)));
    int right_down_gid = layer0->getTileGIDAt(Vec2(int(MaxX / tile_size.width),
                                                   int(MinY / tile_size.height)));
    int right_up_gid = layer0->getTileGIDAt(Vec2(int(MaxX / tile_size.width),
                                                 int(MaxY / tile_size.height)));
    
    // 若有一个顶点所在范围不是空白或草坪就算碰撞（其实如果考虑有船过水的话要复杂一些）
    // 分方向判断才能保证坦克不会被卡住，换了方向后就能继续移动
    if (direction == UP)
    {
        if (kTileHash[left_up_gid] != BLANK && kTileHash[left_up_gid] != GRASS
            || kTileHash[right_up_gid] != BLANK && kTileHash[right_up_gid] != GRASS)
            return true;
    }
    else if (direction == DOWN)
    {
        if (kTileHash[left_down_gid] != BLANK && kTileHash[left_down_gid] != GRASS
            || kTileHash[right_down_gid] != BLANK && kTileHash[right_down_gid] != GRASS)
            return true;
    }
    else if (direction == LEFT)
    {
        if (kTileHash[left_down_gid] != BLANK && kTileHash[left_down_gid] != GRASS
            || kTileHash[left_up_gid] != BLANK && kTileHash[left_up_gid] != GRASS)
            return true;
    }
    else if (direction == RIGHT)
    {
        if (kTileHash[right_down_gid] != BLANK && kTileHash[right_down_gid] != GRASS
            || kTileHash[right_up_gid] != BLANK && kTileHash[right_up_gid] != GRASS)
            return true;
    }

    return false;
}

bool BattleField::isEagleHurt(Rect bounding_box)
{
    // 这里的包围盒坐标要转换成相对地图左下角原点的坐标，包围盒本身也是左下角为原点
    bounding_box = Rect(bounding_box.origin.x - getPositionX(), bounding_box.origin.y - getPositionY(),
                        bounding_box.size.width, bounding_box.size.height);
    
    Size map_size = getContentSize();
    Size map_array = getMapSize();
    // tmx地图的方格必须用行列值重新计算，getTileSize()是不准确的
    Size tile_size = Size(map_size.width / map_array.width, map_size.height / map_array.height);
    
    // 不能出界
    if (bounding_box.getMinX() <= 0.1 || bounding_box.getMaxX() >= map_size.width - 0.1
        || bounding_box.getMinY() <= 0.1 || bounding_box.getMaxY() >= map_size.height - 0.1)
    {
        CCLOG("bullet hit border");
        return false;
    }
    
    TMXLayer* layer0 = getLayer("layer_0");
    
    // 根据包围盒四个角的行列坐标判断gid对应的tile类别
    // tmx地图从左上角算(0, 0)行列值的原点，要做坐标转换
    // FIXME: 优化点，确保坦克能平滑过空白，又不卡进墙里
    float MinX = bounding_box.getMinX();
    float MaxX = bounding_box.getMaxX();
    float MinY = map_size.height - bounding_box.getMinY();
    float MaxY = map_size.height - bounding_box.getMaxY();
    
    // 小幅度修正一下边界值，避免地图标号超范围
    if (MaxX >= map_size.width)
        MaxX = map_size.width - 0.1;
    if (MinY >= map_size.height)
        MinY = map_size.height - 0.1;
    
    int left_down_gid = layer0->getTileGIDAt(Vec2(int(MinX / tile_size.width),
                                                  int(MinY / tile_size.height)));
    int left_up_gid = layer0->getTileGIDAt(Vec2(int(MinX / tile_size.width),
                                                int(MaxY / tile_size.height)));
    int right_down_gid = layer0->getTileGIDAt(Vec2(int(MaxX / tile_size.width),
                                                   int(MinY / tile_size.height)));
    int right_up_gid = layer0->getTileGIDAt(Vec2(int(MaxX / tile_size.width),
                                                 int(MaxY / tile_size.height)));
    
    if (kTileHash[left_up_gid] == EAGLE
        || kTileHash[left_up_gid] == EAGLE
        || kTileHash[right_down_gid] == EAGLE
        || kTileHash[right_up_gid] == EAGLE)
    {
        // 老鹰塌掉
        layer0->setTileGID(25, Vec2(12, 24));
        layer0->setTileGID(26, Vec2(13, 24));
        layer0->setTileGID(31, Vec2(12, 25));
        layer0->setTileGID(32, Vec2(13, 25));
        
        return true;
    }
    

    return false;
}

void BattleField::protectEagle()
{
    TMXLayer* layer0 = getLayer("layer_0");
    
    // 根据坐标改变砖块为钢板
    for (Vec2 cordinate : kEagleFence)
        layer0->setTileGID(5, cordinate);
}

void BattleField::unprotectEagle()
{
    TMXLayer* layer0 = getLayer("layer_0");
    
    // 根据坐标改变砖块为土砖
    for (Vec2 cordinate : kEagleFence)
        layer0->setTileGID(13, cordinate);
}
