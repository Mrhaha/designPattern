//
// Created by hzl on 2020/3/1.
//

#pragma once

#include <chrono>
#include <condition_variable>
#include "safety_mutex.hpp"

namespace dobala
{
  class safety_condition
  {
   public:
    explicit safety_condition(safety_mutex& mutex) : mutex_(mutex), cond_()
    {

    }

    void wait()
    {
      cond_.wait(mutex_);
    }

    template <typename _Predicate>
    void wait(_Predicate predicate)
    {
      cond_.wait(mutex_, std::move(predicate));
    }

    //returns true if time out, false otherwise.
    template<typename _Rep, typename _Period>
    bool wait_for(const std::chrono::duration<_Rep, _Period>& duration)
    {
      return cond_.wait_for(mutex_, duration) == std::cv_status::timeout;
    }

    void notify_one()
    {
      cond_.notify_one();
    }

    void notify_all()
    {
      cond_.notify_all();
    }

   private:
    safety_mutex& mutex_;
    std::condition_variable_any cond_;
  };
}
