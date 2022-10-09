//
// Created by hzl on 2019/4/30.
//

#pragma once

#include "common_base.h"

class CMsgHeadAdmin
{
public:
    static constexpr auto SIZE = (sizeof(int32_t) + sizeof(uint32_t) +  sizeof(int16_t) + sizeof(uint16_t));

    CMsgHeadAdmin();
    ~CMsgHeadAdmin();

    int32_t encode(char* pszOut, int32_t& iOutLength);
    int32_t decode(const char* pszIn, int32_t iInLength);

    uint32_t m_iPackageLen;
    uint16_t m_usCmd;
    int32_t m_iUin;
    int16_t m_nGroupID;
};

