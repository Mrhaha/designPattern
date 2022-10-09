//
// Created by hzl on 2019/11/23.
//

#pragma once

namespace dobala
{
  class noncopyable
  {
   public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;

   protected:
    constexpr noncopyable() noexcept = default;
    ~noncopyable() noexcept = default;

    noncopyable(noncopyable&&) noexcept = default;
    noncopyable& operator=(noncopyable&&) noexcept  = default;
  };
}// namespace dobala





