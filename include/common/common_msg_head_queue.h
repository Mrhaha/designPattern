//
// Created by lxhbroser on 2019/4/24.
//
#pragma once

#include "stdint.h"

class CMsgHeadQueue
{
public:
    CMsgHeadQueue();
    ~CMsgHeadQueue();

    static constexpr auto SIZE = (sizeof(uint32_t) + sizeof(uint16_t));

    int32_t     encode(char* pszOut, int32_t& iOutLength);
    int32_t     decode(const char* pszIn, int32_t iInLength);

    // 头部数据
    uint32_t    m_iPackageLen;         // 整个包的长度
    uint16_t    m_usCmd;
};
