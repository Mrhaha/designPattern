//
// Created by luoxiaohu on 2020/9/24.
//

#pragma once

#include "logic_config_base.h"
#include "logic_game_cache_table.h"

// 通用事件参数数据类
class CLogicEventData
{
public:
    CLogicEventData() : m_eType(CLogicConfigDefine::ECET_Invalid), m_iTimes(1), m_iPara(0), m_iParaEx(0), m_iCount(0), m_iCmd(0) {}

    CLogicConfigDefine::EnumCommonEventType     m_eType;        // 事件类型
    int32_t     m_iTimes;       // 操作次数
    int32_t     m_iPara;        // 物品类型,  旧值,  各种ID
    int32_t     m_iParaEx;      // 物品ID,   新值
    int32_t     m_iCount;       // 数量相关
    int32_t     m_iCmd;
};

class CLogicCommonPublisher
{
public:
    typedef void (*observer_call_back_func)(user_data_table_ptr_type pUserData, const CLogicEventData& stData);

    CLogicCommonPublisher();

    void RegisterObserver(CLogicConfigDefine::EnumCommonEventType eType, observer_call_back_func pFunc)
    {
        if(CLogicConfigDefine::IsValidEnumCommonEventType(eType))
            m_stObservers[eType].push_back(pFunc);
    }

    void NotifyAllObservers(user_data_table_ptr_type pUserData, const CLogicEventData& stData)
    {
        auto stList = m_stObservers.find(stData.m_eType);
        if(stList == m_stObservers.end()) return;

        auto stBeg = stList->second.begin();
        auto stEnd = stList->second.end();
        for (; stBeg != stEnd; ++stBeg)
            (*stBeg)(pUserData, stData);
    }

private:
    std::map<CLogicConfigDefine::EnumCommonEventType, std::list<observer_call_back_func>> m_stObservers;
};

typedef taf::TC_Singleton<CLogicCommonPublisher> LOGIC_COMMON_PUBLISHER_INS_TYPE;

#define LOGIC_COMMON_PUBLISHER_INS LOGIC_COMMON_PUBLISHER_INS_TYPE::getInstance()
