//
// Created by hzl on 2020/3/1.
//

#pragma once

#include "safety_mutex.hpp"

namespace dobala
{
  class SCOPED_CAPABILITY safety_lock
  {
   public:
    constexpr explicit safety_lock(safety_mutex& mutex) ACQUIRE(mutex) : mutex_(mutex)
    {
      mutex_.lock();
    }

    ~safety_lock() RELEASE()
    {
      mutex_.unlock();
    }

   private:
    safety_mutex& mutex_;
  };

}

// Prevent misuse like:
// SafetyLock(mutex_);
// A temporary object doesn't hold the lock for long!
#define SafetyLock(x) error "Missing lock object name"
