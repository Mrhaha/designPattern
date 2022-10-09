//
// Created by luoxiaohu on 2020/9/25.
//

#pragma once

#include "logic_game_cache_table.h"
#include "logic_game_common_publisher.h"

class CLogicCommonObserver
{
public:
    // 世界任务
    static void NotifyWorldQuestEvent(user_data_table_ptr_type pUserInfo, const CLogicEventData& stData);

    // 新红包
    static void NotifyNewRedBagEvent(user_data_table_ptr_type pUserInfo, const CLogicEventData& stData);

    // 调查问卷部分事件改实时
    static void NotifyQuestionnaireEvent(user_data_table_ptr_type pUserInfo, const CLogicEventData& stData);

    // 任务系统
    static void NotifyQuestEvent(user_data_table_ptr_type pUserInfo, const CLogicEventData& stData);

    // 家园经验
    static void NotifyHomeLevelChangeEvent(user_data_table_ptr_type pUserInfo, const CLogicEventData& stData);
};
