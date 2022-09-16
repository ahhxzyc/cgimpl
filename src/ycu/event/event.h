#pragma once

#include <tuple>
#include <vector>

#define YCU_EVENT_BEGIN namespace ycu::event {
#define YCU_EVENT_END }

YCU_EVENT_BEGIN

template<typename... TEvents>
class sender_t
{
private:
    template<typename TEvent>
    using receiver_set_t = std::unordered_set<receiver_t<TEvent> *>;

    std::tuple<receiver_set_t<TEvents>...>  sets_;

public:
    template<typename TEvent> 
    void send(TEvent& e);
};

template<typename TEvent>
class receiver_t
{
public:
    void handle(const TEvent& e);
};

template<typename... TEvents> 
template<typename TEvent>
void sender_t<TEvents...>::send(TEvent& e)
{
    auto &set = std::get<receiver_set_t<Event>>(sets_);
    for (auto r : set)
    {
        r->handle(e);
    }
}

YCU_EVENT_END