//
// Created by hzl on 2019/12/19.
//

#pragma once

#include <deque>
#include <cassert>
#include <optional>
#include "noncopyable.hpp"
#include "safety_mutex.hpp"
#include "safety_condition.hpp"
#include "safety_lock.hpp"

namespace dobala
{
template <typename T>
class blocking_queue : public noncopyable
{
  public:
    blocking_queue() : mutex_(), not_empty_(mutex_), is_stop_(false), queue_()
    {
    }

    inline void push_back(T&& x)
    {
        safety_lock lock(mutex_);
        queue_.push_back(std::move(x));
        not_empty_.notify_one();
    }

    inline void push_back(const T& x)
    {
        safety_lock lock(mutex_);
        queue_.push_back(x);
        not_empty_.notify_one();
    }

    template <typename ...Args>
    inline void emplace_back(Args&& ... args)
    {
        safety_lock lock(mutex_);
        queue_.emplace_back(std::forward<Args>(args)...);
        not_empty_.notify_one();
    }

    inline void push_front(T&& x)
    {
        safety_lock lock(mutex_);
        queue_.push_front(std::move(x));
        not_empty_.notify_one();
    }

    inline void push_front(const T& x)
    {
        safety_lock lock(mutex_);
        queue_.push_front(x);
        not_empty_.notify_one();
    }

    template <typename ...Args>
    inline void emplace_front(Args&& ... args)
    {
        safety_lock lock(mutex_);
        queue_.emplace_front(std::forward<Args>(args)...);
        not_empty_.notify_one();
    }

    inline std::optional<T> pop_front()
    {
        safety_lock lock(mutex_);
        not_empty_.wait([this]() REQUIRES(mutex_) { return !mutex_empty() || mutex_is_stop(); });

        if (!mutex_empty())
        {
            T front(std::move(queue_.front()));
            queue_.pop_front();
            return std::move(front);
        }

        return std::nullopt;
    }

    inline std::optional<T> pop_back()
    {
        safety_lock lock(mutex_);
        not_empty_.wait([this]() REQUIRES(mutex_) { return !mutex_empty() || mutex_is_stop(); });

        if (!mutex_empty())
        {
            T back(std::move(queue_.back()));
            queue_.pop_back();
            return std::move(back);
        }

        return std::nullopt;
    }

    inline void stop()
    {
        safety_lock lock(mutex_);
        is_stop_ = true;
        not_empty_.notify_all();
    }

    inline size_t size() const
    {
        safety_lock lock(mutex_);
        return mutex_size();
    }

  private:
    inline bool mutex_is_stop() const REQUIRES(mutex_)
    {
        return is_stop_;
    }

    inline bool mutex_empty() const REQUIRES(mutex_)
    {
        return queue_.empty();
    }

    inline size_t mutex_size() const REQUIRES(mutex_)
    {
        return queue_.size();
    }

  private:
    mutable safety_mutex mutex_;
    safety_condition not_empty_ GUARDED_BY(mutex_);
    bool is_stop_ GUARDED_BY(mutex_);
    std::deque<T> queue_ GUARDED_BY(mutex_);
};
}