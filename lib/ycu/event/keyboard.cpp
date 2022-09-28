#include "keyboard.h"
#include "../log/log.h"

YCU_EVENT_BEGIN


bool Keyboard::is_down(KeyCode key)
{
    return map_[key];
}

void Keyboard::handle(const KeyDownEvent &e)
{
    map_[e.key] = true;
}
void Keyboard::handle(const KeyUpEvent &e)
{
    map_[e.key] = false;
}
void Keyboard::handle(const KeyHoldEvent &e)
{
    ;
}

YCU_EVENT_END