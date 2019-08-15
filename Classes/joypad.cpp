#include "joypad.h"

bool Joypad::init()
{
    if (!Layer::init())
        return false;
    
    return true;
}
