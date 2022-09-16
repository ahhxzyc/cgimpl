#pragma once

#define YCU_EVENT_BEGIN namespace ycu::event {
#define YCU_EVENT_END }

YCU_EVENT_BEGIN

typename<typename... Events>
class sender_t
{
private:
    template<typename Event>
    using receiver_set = std::unordered_set<receiver_t<Event> *>;

    std::tuple<receiver_set<Events>...>  m_Sets;

public:
    template<typename Event> void Send(Event& e);
};


class receiver_t
{

}

template<typename Event> 
void sender::Send(Event& e)
{
    auto &set = std::get<receiver_set<Event>>(m_Sets);
    for (auto rec : set)
    {
        rec.Handle(e);
    }
}

YCU_EVENT_END