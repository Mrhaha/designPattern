//
// Created by hzl on 2020/3/1.
//

#pragma once

#include <mutex>
#include "thread_safety.h"

namespace dobala
{
  class CAPABILITY("mutex") safety_mutex
  {
   public:
    constexpr void lock() ACQUIRE()
    {
      mutex_.lock();
    }

    constexpr void unlock() RELEASE()
    {
      mutex_.unlock();
    }

   private:
    std::mutex mutex_;
  };

}
