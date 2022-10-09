//
// Created by lxhbroser on 2019/7/2.
//

#include "logic_game_monitor.h"
#include "logic_game_processor_base.h"
#include "logic_config.h"
#include "logic_link_common.h"
#include "server_cmd_extend_define.h"

void CLogicMonitor::ProtoMonitor(user_data_table_ptr_type pUserData, uint16_t usCmd, std::string strCmdName)
{
    // 检查协议开关
    if(!ProtoSwitch(CMD_EXTEND_PROTO_MONITOR))
    {
        return;
    }

    auto config = CLogicConfig::GetProtocolMonitorConfig().GetConfig(usCmd);
    if(!config) return;

    LOGIC_LOG_TRACE_DEBUG << "Protocol Monitor|" << "USER UIN:" << pUserData->m_iUin << " GROUP_ID:" << pUserData->m_iGroupID
                          << "|CMD:" << usCmd << "|CMD_NAME:"<< strCmdName << std::endl;

    auto protoMonitor = pUserData->m_stProtoMonitorMap.Find(usCmd);
    if(protoMonitor == pUserData->m_stProtoMonitorMap.End())
    {
        TProtoMonitorTableValueType valueType;
        valueType.m_iUid = pUserData->m_iUin;
        valueType.m_iGroupID = pUserData->m_iGroupID;
        valueType.m_iCmd = usCmd;
        valueType.m_iCurNum = 1;
        valueType.m_iLastRecordTime = CLogicCommon::GetSec();

        pUserData->m_stProtoMonitorMap.Insert(std::make_pair(valueType.m_iCmd, valueType));
        return;
    }

    auto value = protoMonitor->second;
    switch (config->m_iTimeType)
    {
        case CLogicConfigDefine::EMTT_Daily:
            if(LOGIC_CHECK_DAY_REFRESH(value->GetLastRecordTime())) value->SetCurNum(0);
            break;

        case CLogicConfigDefine::EMTT_Weekly:
            if(LOGIC_CHECK_WEEK_REFRESH(value->GetLastRecordTime())) value->SetCurNum(0);
            break;

        case CLogicConfigDefine::EMTT_Monthly:
            if(LOGIC_CHECK_MONTH_REFRESH(value->GetLastRecordTime())) value->SetCurNum(0);
            break;

        case CLogicConfigDefine::EMTT_Total:
            break;

        default:
            break;
    }

    value->AddCurNum(1);
    value->SetLastRecordTime(CLogicCommon::GetSec());

    // 暂时只报警一次，避免重复日志
    if(value->GetCurNum() == config->m_iMaxNum + 1)
    {
        // 报警
        LOGIC_LOG_TRACE_ERROR << "Protocol Monitor ERROR!!!|MSG: CurNum > MaxNum|Uin:" << pUserData->m_iUin
                              << " Group ID:" << pUserData->m_iGroupID << "|CMD:" << usCmd
                              << "|CMD_NAME:"<< strCmdName << "|CurNum:" << value->GetCurNum()
                              << " MaxNum:" << config->m_iMaxNum << std::endl;

        LOGIC_SERVICE_API_INS->RequestServiceAlert("proto_monitor", {
                { "uin" , std::to_string(pUserData->m_iUin)},
                { "group_id", std::to_string(pUserData->m_iGroupID)},
                { "cmd", std::to_string(usCmd)},
                { "cmd_name", strCmdName},
                { "max_num", std::to_string(config->m_iMaxNum)},
        });
    }
}

void CLogicMonitor::ItemMonitor(user_data_table_ptr_type pUserData, int32_t iItemType, int32_t iItemID, int32_t iNum, uint16_t usCmd)
{
    // 检查协议开关
    if(!ProtoSwitch(CMD_EXTEND_ITEM_MONITOR))
    {
        return;
    }

    auto config = CLogicConfig::GetItemMonitorConfig().GetConfig(iItemType, iItemID);
    if(!config) return;

    LOGIC_LOG_TRACE_DEBUG << "Item Monitor|" << "USER UIN:" << pUserData->m_iUin << " GROUP_ID:" << pUserData->m_iGroupID
                          << "|CMD:" << usCmd << "|ItemType:"<< iItemType << " ItemID:" << iItemID << " Num:" << iNum << std::endl;

    auto key = std::make_pair(iItemType, iItemID);
    auto itemMonitor = pUserData->m_stItemMonitorMap.Find(key);
    if(itemMonitor == pUserData->m_stItemMonitorMap.End())
    {
        TItemMonitorTableValueType valueType;
        valueType.m_iUid = pUserData->m_iUin;
        valueType.m_iGroupID = pUserData->m_iGroupID;
        valueType.m_iItemType = iItemType;
        valueType.m_iItemID = iItemID;
        valueType.m_iCurNum = 0;
        valueType.m_iLastRecordTime = CLogicCommon::GetSec();

        auto stRet = pUserData->m_stItemMonitorMap.Insert(std::make_pair(key, valueType));
        if(!stRet.second)
        {
            return;
        }

        itemMonitor = stRet.first;
    }

    auto value = itemMonitor->second;
    switch (config->m_iTimeType)
    {
        case CLogicConfigDefine::EMTT_Daily:
            if(LOGIC_CHECK_DAY_REFRESH(value->GetLastRecordTime())) value->SetCurNum(0);
            break;

        case CLogicConfigDefine::EMTT_Weekly:
            if(LOGIC_CHECK_WEEK_REFRESH(value->GetLastRecordTime())) value->SetCurNum(0);
            break;

        case CLogicConfigDefine::EMTT_Monthly:
            if(LOGIC_CHECK_MONTH_REFRESH(value->GetLastRecordTime())) value->SetCurNum(0);
            break;

        case CLogicConfigDefine::EMTT_Total:
            break;

        default:
            break;
    }

    int iOldNum = value->GetCurNum();
    value->AddCurNum(iNum);
    value->SetLastRecordTime(CLogicCommon::GetSec());

    // 暂时只报警一次，避免重复日志
    if(iOldNum < config->m_iMaxNum && config->m_iMaxNum <= value->GetCurNum())
    {
        // 报警
        LOGIC_LOG_TRACE_ERROR << "Item Monitor ERROR!!!|MSG: CurNum > MaxNum|Uin:" << pUserData->m_iUin
                              << " Group ID:" << pUserData->m_iGroupID << "|CMD:" << usCmd
                              << "|ItemType:"<< iItemType << " ItemID:" << iItemID << " Num:" << iNum
                              << "|CurNum:" << value->GetCurNum() << " MaxNum:" << config->m_iMaxNum << std::endl;

        LOGIC_SERVICE_API_INS->RequestServiceAlert("item_monitor", {
                { "uin" , std::to_string(pUserData->m_iUin)},
                { "group_id", std::to_string(pUserData->m_iGroupID)},
                { "cmd", std::to_string(usCmd)},
                { "item_type", std::to_string(iItemType)},
                { "item_id", std::to_string(iItemID)},
                { "num", std::to_string(iNum)},
                { "max_num", std::to_string(config->m_iMaxNum)},
        });
    }
}

bool CLogicMonitor::ProtoSwitch(int32_t iCmd)
{
    int32_t iDateTime = CLogicConfig::GetProtoSwitchConfig().GetConfig(iCmd);
    if(iDateTime <= 0 || CLogicCommon::GetSec() >= iDateTime)
        return false;

    return true;
}
