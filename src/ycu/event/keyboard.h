#pragma once

#include "event.h"
#include "keycode.h"
#include <unordered_map>

YCU_EVENT_BEGIN

struct KeyDownEvent {KeyCode key;};
struct KeyUpEvent {KeyCode key;};
struct KeyHoldEvent {KeyCode key;};

class Keyboard : 
    public ycu::event::receiver_t<KeyDownEvent>, 
    public ycu::event::receiver_t<KeyUpEvent>, 
    public ycu::event::receiver_t<KeyHoldEvent>
{
public:
    Keyboard() = default;
    bool is_down(KeyCode key);
    void handle(const KeyDownEvent& e) override;
    void handle(const KeyUpEvent & e) override;
    void handle(const KeyHoldEvent & e) override;
public:
    std::unordered_map<int, bool> map_;
};

YCU_EVENT_END