#pragma once

#include <tuple>
//#include <unordered_set>
#include <functional>
#include <vector>

#define YCU_EVENT_BEGIN namespace ycu::event {
#define YCU_EVENT_END }

YCU_EVENT_BEGIN

template<typename TEvent>
class receiver_t
{
public:
    using handler_type = std::function<void(const TEvent &)>;
public:
    virtual ~receiver_t() = default;
    virtual void handle(const TEvent &) = 0;
private:
    handler_type f_;
};

template<typename... TEvents>
class sender_t
{
public:
    template<typename TEvent>
    using container_type = std::vector<receiver_t<TEvent>*>;
public:
    template<typename TEvent>
    void attach(receiver_t<TEvent>* r)
    {
        auto &v = assure<TEvent>();
        v.push_back(r);
    }
    template<typename TEvent>
    void send(const TEvent &e)
    {
        auto &v = assure<TEvent>();
        for (auto r : v)
        {
            if (r)
                r->handle(e);
        }
    }
private:
    template<typename TEvent>
    auto &assure()
    {
        return std::get<container_type<TEvent>>(receivers_);
    }
private:
    std::tuple<container_type<TEvents>...> receivers_;
};

YCU_EVENT_END