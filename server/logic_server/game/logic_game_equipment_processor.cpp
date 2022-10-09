#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_game_item_protocol.h"
#include "server_card_protocol.h"
#include "logic_link_common.h"
#include "logic_config.h"
#include "logic_game_item.h"
#include "logic_game_cache_common.h"
#include "logic_game_equipment_processor.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_publisher.h"
#include "logic_game_global.h"
#include "logic_game_item_factory.h"
#include "logic_game_bill.h"
#include "logic_game_currency_check.h"
#include "logic_game_common_publisher.h"
#include "server_cmd_data_report_define.h"
#include "server_data_report_protocol.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_COMBINE_GAME_ITEM, CLogicEquipmentProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_CONSTEL_DIV, CLogicEquipmentProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_CONSTEL_LEVEL_UP, CLogicEquipmentProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_CONSTEL_EQUIP, CLogicEquipmentProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_CONSTEL_ONEKEY_UNLOAD, CLogicEquipmentProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_CONSTEL_LOCK_OR_DISCARD, CLogicEquipmentProcessor)

REGISTER_CMD_FACTORY(MSG_LOGIC_CONSTEL_PRESET_SAVE, CLogicEquipmentProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CONSTEL_PRESET_DEL, CLogicEquipmentProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CONSTEL_PRESET_EQUIP, CLogicEquipmentProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CONSTEL_PRESET_LIST, CLogicEquipmentProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_EQUIPMENT_EQUIP, CLogicEquipmentProcessor)

REGISTER_CMD_FACTORY(MSG_LOGIC_EQUIPMENT_DIV, CLogicEquipmentProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_EQUIPMENT_LOCK, CLogicEquipmentProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_EQUIPMENT_ADD_EXP, CLogicEquipmentProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_EQUIPMENT_UPGRADE, CLogicEquipmentProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_EQUIPMENT_REFINE, CLogicEquipmentProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_EQUIPMENT_REFINE_OPT, CLogicEquipmentProcessor)





CLogicEquipmentProcessor::CLogicEquipmentProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicEquipmentProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    bool bRet = false;
    switch(m_usCmd)
    {
        case MSG_LOGIC_COMBINE_GAME_ITEM:
            bRet = DoCombineGameItem(stFormatData);
            break;

        case MSG_LOGIC_TOUHOU_CONSTEL_DIV:
            bRet = DoConstelDiv(stFormatData);
            break;

        case MSG_LOGIC_TOUHOU_CONSTEL_LEVEL_UP:
            bRet = DoConstelLevelUp(stFormatData);
            break;

        case MSG_LOGIC_TOUHOU_CONSTEL_EQUIP:
            bRet = DoConstelEquip(stFormatData);
            break;

        case MSG_LOGIC_TOUHOU_CONSTEL_ONEKEY_UNLOAD:
            bRet = DoConstelOnekeyUnload(stFormatData);
            break;

        case MSG_LOGIC_TOUHOU_CONSTEL_LOCK_OR_DISCARD:
            bRet = DoConstelLockOrDiscard(stFormatData);
            break;

        case MSG_LOGIC_CONSTEL_PRESET_SAVE:
            bRet = DoConstelPresetSave(stFormatData);
            break;

        case MSG_LOGIC_CONSTEL_PRESET_DEL:
            bRet = DoConstelPresetDel(stFormatData);
            break;

        case MSG_LOGIC_CONSTEL_PRESET_EQUIP:
            bRet = DoConstelPresetEquip(stFormatData);
            break;

        case MSG_LOGIC_CONSTEL_PRESET_LIST:
            bRet = DoConstelPresetList(stFormatData);
            break;

        case MSG_LOGIC_EQUIPMENT_EQUIP:
            bRet = DoEquipmentEquip(stFormatData);
            break;

        case MSG_LOGIC_EQUIPMENT_DIV:
            bRet = DoEquipmentDiv(stFormatData);
            break;

        case MSG_LOGIC_EQUIPMENT_LOCK:
            bRet = DoEquipmentLock(stFormatData);
            break;

        case MSG_LOGIC_EQUIPMENT_ADD_EXP:
            bRet = DoEquipmentAddExp(stFormatData);
            break;

        case MSG_LOGIC_EQUIPMENT_UPGRADE:
            bRet = DoEquipmentUpgrade(stFormatData);
            break;

        case MSG_LOGIC_EQUIPMENT_REFINE:
            bRet = DoEquipmentRefine(stFormatData);
            break;

        case MSG_LOGIC_EQUIPMENT_REFINE_OPT:
            bRet = DoEquipmentRefineOp(stFormatData);
            break;
    }

    return bRet;
}

bool CLogicEquipmentProcessor::DoCombineGameItem(const msgpack::unpacked& stFormatData)
{
    CResponseCombineGameItem stRsp;
    CRequestCombineGameItem stParam;

    if(DecodeMsgPack(stFormatData, stParam) == false) return (false);

    if(stParam.m_iTimes <= 0 || stParam.m_iTimes > 999)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    auto pCombineConfig = CLogicConfig::GetFragmentCombineConfig().GetConfig(stParam.m_iItemID);
    if (pCombineConfig == nullptr)
    {
        PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                            << "|MSG:" << CLogicError::GetErrMsg()
                            << "|CONSUME_ITEM_ID:" << stParam.m_iItemID
                            << std::endl;

        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
    }

    std::map<int, std::pair<int, int>> stConsume;
    if(!CLogicCacheCommon::CombineOneKey(m_pUserInfo, stParam.m_iItemID, stParam.m_iTimes, stConsume))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM);
    }

    stRsp.m_stCombineItem.m_iItemType = pCombineConfig->m_iItemType;
    stRsp.m_stCombineItem.m_iCardID = stParam.m_iItemID;
    stRsp.m_stCombineItem.m_iNum = stParam.m_iTimes;

    std::string strConsumeList = "{";
    for(auto& iter : stConsume)
    {
        stRsp.m_stConsumeItems.emplace_back(iter.second.first, iter.first, iter.second.second);

        static char szBuffer[128];
        snprintf(szBuffer, sizeof(szBuffer), "[%d, %d, %d],",
                 iter.second.first,
                 iter.first,
                 iter.second.second);

        strConsumeList += szBuffer;
    }
    strConsumeList[strConsumeList.length() - 1] = '}';

    if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRsp.m_stConsumeItems, m_usCmd))
    {
        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
    }

    static std::string strCombineItem;
    CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stCombineItem, strCombineItem, m_usCmd);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|COMBINE_TIMES:" << stParam.m_iTimes
                        << "|CONSUME_ITEM:" << strConsumeList
                        << "|COMBINE_TIME:" << strCombineItem
                        << std::endl;

    CLogicGlobal::PublishBroadcast("combine_game_item", stRsp.m_stCombineItem.m_iItemType, {
            {"nick", m_pUserInfo->m_strNick},
            {"item_name", CLogicConfig::GetItemName(stRsp.m_stCombineItem.m_iItemType, stRsp.m_stCombineItem.m_iCardID)}
    });

    return true;
}

bool CLogicEquipmentProcessor::DoConstelDiv(const msgpack::unpacked& stFormatData)
{
    CRequestConstelCommonOpt stReq;
    CResponseConstelDiv stRsp;
    if(!DecodeMsgPack(stFormatData, stReq)) return false;

    if(stReq.m_stUniqueIDs.size() >= CLogicConfigConstelAttrParser::E_MaxBagSize)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HANDLE_NUM_TOO_MANY)
    }

    std::string strDivIDs = "[";
    int iTotalExp = 0;
    std::vector<CPackGameItem> stRewards;
    for(auto id : stReq.m_stUniqueIDs)
    {
        auto iter = m_pUserInfo->m_stConstelMap.Find(id);
        if(iter == m_pUserInfo->m_stConstelMap.End())
            continue;

        if(iter->second->GetIsLocked() > 0)
            continue;

        if(iter->second->GetEquipCardID() > 0)
            continue;

        if(iter->second->GetTotalExp() > 0)
            continue;

        auto pConfig = CLogicConfig::GetConstelAttrConfig().GetGradeResolveConfig(iter->second->GetGrade());
        if(nullptr == pConfig)
            continue;

        // 分解
        if(stReq.m_bDiv)
        {
            // 不可分解
            if(pConfig->second.empty())
                continue;

            CLogicConfig::GetLevelDropConfig().GetRandBonusByName(pConfig->second, stRewards);
        }
        else
        {
            // 吞噬
            // 不可吞噬
            if(pConfig->first <= 0)
                continue;

            iTotalExp += pConfig->first;
        }

        // bill 记录
        CLogicBill::SaveConstelBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, -1, iter->second->m_stData);

        m_pUserInfo->m_stConstelMap.Erase(iter);

        stRsp.m_stUniqueIDs.push_back(id);

        strDivIDs += std::to_string(id);
        strDivIDs += ",";
    }

    strDivIDs[strDivIDs.length() - 1] = ']';

    if(stRsp.m_stUniqueIDs.empty())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }


    if(iTotalExp > 0)
        stRewards.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_ConstelExpID, iTotalExp);

    static std::string strBonus;
    CLogicCacheCommon::AddGameItem(m_pUserInfo, stRewards, strBonus, m_usCmd);

    stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|UNIQUE_IDS:" << strDivIDs << "|IS_DIV:" << stReq.m_bDiv << "|BONUS_ITEM:" << strBonus << std::endl;

    // 通用事件
    CLogicEventData stEventData;
    stEventData.m_eType = CLogicConfigDefine::ECET_DivConstelTimes;
    stEventData.m_iTimes = stRsp.m_stUniqueIDs.size();
    stEventData.m_iCmd = m_usCmd;
    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

    return true;
}

bool CLogicEquipmentProcessor::DoConstelLevelUp(const msgpack::unpacked& stFormatData)
{
    CRequestConstelLevelUp stReq;
    CResponseConstelLevelUp stRsp;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    auto iterLevelUp = m_pUserInfo->m_stConstelMap.Find(stReq.m_iLevelUpUniqueID);
    if(iterLevelUp == m_pUserInfo->m_stConstelMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    /*if(iterLevelUp->second->GetIsDiscard())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::IS_DISCARD);
    }*/

    if(stReq.m_iConsumeExpPool < 0 || stReq.m_iConsumeExpPool > m_pUserInfo->m_stIBusyData.GetConstelExpPool())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    if(stReq.m_iConsumeExpPool == 0 && stReq.m_stExpUniqueIDs.empty())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    auto pLevelUpConfig = CLogicConfig::GetConstelAttrConfig().GetGradeConfig(iterLevelUp->second->GetGrade());
    if(nullptr == pLevelUpConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
    }

    int32_t iBaseExp = stReq.m_iConsumeExpPool;
    int32_t iInheritExp = 0;
    for(auto id : stReq.m_stExpUniqueIDs)
    {
        if(id == stReq.m_iLevelUpUniqueID)
            continue;

        auto iter = m_pUserInfo->m_stConstelMap.Find(id);
        if(iter == m_pUserInfo->m_stConstelMap.End())
            continue;

        if(iter->second->GetIsLocked() > 0)
            continue;

        if(iter->second->GetEquipCardID() > 0)
            continue;

        auto pConfig = CLogicConfig::GetConstelAttrConfig().GetGradeConfig(iter->second->GetGrade());
        if(nullptr == pConfig)
            continue;

        iBaseExp += pConfig->m_iBaseExp;
        iInheritExp += iter->second->GetTotalExp();

        stRsp.m_stExpUniqueIDs.push_back(id);
    }

    if(iBaseExp + iInheritExp <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    int32_t iTotalAddExp = iBaseExp + std::ceil(iInheritExp * CLogicConfig::GetConstelAttrConfig().m_dExpInheritPct);
    int32_t iConsumeGold = std::ceil(iBaseExp * CLogicConfig::GetConstelAttrConfig().m_dConsumeGoldPct);
    if (!CLogicCurrencyCheck::CheckEnoughGold(m_pUserInfo, iConsumeGold))
    {
        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
    }

    int32_t iOldLevel = iterLevelUp->second->GetLevel();
    int32_t iNewLevel = iOldLevel;
    int32_t iNewExp = iterLevelUp->second->GetExp() + iTotalAddExp;
    pLevelUpConfig->ReCalcLevelExp(iNewLevel, iNewExp);

    // 计算新副属性
    const auto& stAddAttrLevel = CLogicConfig::GetConstelAttrConfig().m_stLevelAddAttr;
    auto stExAttrData = iterLevelUp->second->GetExAttr();
    bool bSave = false;
    for(int i = iterLevelUp->second->GetLevel() + 1; i <= iNewLevel; ++i)
    {
        if(std::find(stAddAttrLevel.begin(), stAddAttrLevel.end(), i) == stAddAttrLevel.end())
            continue;

        bSave = true;
        std::map<int32_t, int32_t> stWeight;
        int32_t iTotalWeight = 0;
        if(stExAttrData.size() >= CLogicConfigConstelAttrParser::E_ExAttrMaxNum)
        {
            for(auto& attr : stExAttrData)
            {
                if (attr.second.second == 1)
                {
                    attr.second.second = CONSTELL_BASE_PRO;
                }
                stWeight[attr.first] = attr.second.second;
                iTotalWeight += attr.second.second;
            }
        }
        else
        {
            for(auto& attr : pLevelUpConfig->m_stExAttr)
            {
                if(stExAttrData.find(attr.first) != stExAttrData.end())
                    continue;

                stWeight[attr.first] = attr.second.m_iWeight;
                iTotalWeight += attr.second.m_iWeight;
            }
        }

        // 随机副属性
        int32_t iAttrType = CLogicCacheCommon::RandStlCont(stWeight, iTotalWeight);
        if(iAttrType < 0)
        {
            PROCESSOR_LOG_ERROR << "UniqueID:" << stReq.m_iLevelUpUniqueID << "|OldLevel:" << iterLevelUp->second->GetLevel()
                                << "|OldExp:" << iterLevelUp->second->GetExp() << "|NewLevel:" << iNewLevel << "|NewExp:"
                                << iNewExp << std::endl;

            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
        }

        auto iterExAttrConfig = pLevelUpConfig->m_stExAttr.find(iAttrType);
        if(iterExAttrConfig == pLevelUpConfig->m_stExAttr.end())
        {
            PROCESSOR_LOG_ERROR << "UniqueID:" << stReq.m_iLevelUpUniqueID << "|OldLevel:" << iterLevelUp->second->GetLevel()
                                << "|OldExp:" << iterLevelUp->second->GetExp() << "|NewLevel:" << iNewLevel << "|NewExp:"
                                << iNewExp << std::endl;

            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
        }

        int32_t iAttrValue = CLogicCommon::GetRandNum(iterExAttrConfig->second.m_iValueMin, iterExAttrConfig->second.m_iValueMax, true);
        stExAttrData[iAttrType].first += iAttrValue;
        stExAttrData[iAttrType].second += CONSTELL_BASE_PRO;
    }

    iterLevelUp->second->SetLevel(iNewLevel);
    iterLevelUp->second->SetExp(iNewExp);
    iterLevelUp->second->AddTotalExp(iTotalAddExp);
    if(bSave)
    {
        iterLevelUp->second->SetExAttr(stExAttrData);
        // bill 记录
        CLogicBill::SaveConstelBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, 0, iterLevelUp->second->m_stData);
    }

    for(auto id : stRsp.m_stExpUniqueIDs)
    {
        auto iter = m_pUserInfo->m_stConstelMap.Find(id);
        if(iter == m_pUserInfo->m_stConstelMap.End())
            continue;

        // bill 记录
        CLogicBill::SaveConstelBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, -1, iter->second->m_stData);
        m_pUserInfo->m_stConstelMap.Erase(iter);
    }

    std::vector<CPackGameItem> stConsumes;
    stConsumes.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_GoldID, iConsumeGold);
    stConsumes.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_ConstelExpID, stReq.m_iConsumeExpPool);
    CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stConsumes, m_usCmd);

    stRsp.m_iExpPool = m_pUserInfo->m_stIBusyData.GetConstelExpPool();
    stRsp.m_stLevelUpConstel = CLogicCacheCommon::GetConstellationInfo(iterLevelUp->second->m_stData);
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    if(iOldLevel != iNewLevel)
    {
        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_ConstelLevelUp;
        stEventData.m_iPara = iOldLevel;
        stEventData.m_iParaEx = iNewLevel;
        stEventData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
    }

    // 通用事件
    CLogicEventData stEventDataMainAtr;
    stEventDataMainAtr.m_eType = CLogicConfigDefine::ECET_UpLevelConstelMainAtr;
    stEventDataMainAtr.m_iPara = iterLevelUp->second->GetMainAttrType();
    stEventDataMainAtr.m_iParaEx = iNewLevel;
    stEventDataMainAtr.m_iCmd = m_usCmd;
    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventDataMainAtr);
    return true;
}

bool CLogicEquipmentProcessor::DoConstelEquip(const msgpack::unpacked& stFormatData)
{
    CRequestConstelEquip stReq;
    CResponseConstelEquip stRsp;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    stRsp.m_iCardID = stReq.m_iCardID;
    stRsp.m_iConstelID = stReq.m_iConstelID;

    int rtCode = ConstelEquip(stReq.m_iCardID, stReq.m_iConstelID);
    if(rtCode != SERVER_ERRCODE::SUCCESS)
    {
        SEND_ERRCODE_AND_RET(rtCode);
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);


    return true;
}

bool CLogicEquipmentProcessor::DoConstelOnekeyUnload(const msgpack::unpacked& stFormatData)
{
    CRequestConstelOnekeyUnload stReq;
    CResponseConstelOnekeyUnload stRsp;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    stRsp.m_iCardID = stReq.m_iCardID;

    int iRetCode = ConstelOnekeyUnload(stReq.m_iCardID);
    if(iRetCode != SERVER_ERRCODE::SUCCESS)
    {
        SEND_ERRCODE_AND_RET(iRetCode);
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    return true;
}

bool CLogicEquipmentProcessor::DoConstelLockOrDiscard(const msgpack::unpacked& stFormatData)
{
    CRequestConstelLockOrDiscard stReq;
    CResponseConstelLockOrDiscard stRsp;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    stRsp.m_iUniqueID = stReq.m_iUniqueID;
    stRsp.m_bIsLock = stReq.m_bIsLock;
    stRsp.m_bIsSign = stReq.m_bIsSign;

    auto iter = m_pUserInfo->m_stConstelMap.Find(stReq.m_iUniqueID);
    if(iter == m_pUserInfo->m_stConstelMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    if(stReq.m_bIsLock)
    {
        if(stReq.m_bIsSign)
        {
            if(iter->second->GetIsDiscard())
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::IS_DISCARD);
            }

            iter->second->SetIsLocked(1);
        }
        else
        {
            iter->second->SetIsLocked(0);
        }
    }
    else
    {
        if(stReq.m_bIsSign)
        {
            if(iter->second->GetIsLocked())
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::IS_LOCKED);
            }

            iter->second->SetIsDiscard(1);
        }
        else
        {
            iter->second->SetIsDiscard(0);
        }
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicEquipmentProcessor::DoConstelPresetSave(const msgpack::unpacked& stFormatData)
{
    CRequestConstelPresetSave stReq;
    CResponseConstelPresetSave stRsp;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    auto iErrorCode = CLogicCacheCommon::CheckAliasNickValid(stReq.m_strName);
    if(iErrorCode != SERVER_ERRCODE::SUCCESS)
    {
        SEND_ERRCODE_AND_RET(iErrorCode);
    }

    if(stReq.m_stConstelID.size() != CLogicConfigDefine::EHET_Max - 1)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    // 检查礼装
    for(int i = CLogicConfigDefine::EHET_Invalid; i < CLogicConfigDefine::EHET_Max - 1; ++i)
    {
        if(stReq.m_stConstelID[i] == 0) continue;

        auto iter = m_pUserInfo->m_stConstelMap.Find(stReq.m_stConstelID[i]);
        if(iter == m_pUserInfo->m_stConstelMap.End() || iter->second->GetEquipmentTag() != i + 1)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        /*if(iter->second->GetIsDiscard())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::IS_DISCARD);
        }*/
    }

    auto iterPreset = m_pUserInfo->m_stConstelPresetMap.End();
    // 新建
    if(stReq.m_iPresetID == 0)
    {
        TUserConstelPresetTableValueType stVal;
        stVal.m_iGroupID = m_iGroupID;
        stVal.m_iUid = m_iUin;
        stVal.m_iUniqueID = CLogicCommon::GetSec();

        auto stRet = m_pUserInfo->m_stConstelPresetMap.Insert(std::make_pair(stVal.m_iUniqueID, stVal));
        if (stRet.second == false)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
        }

        iterPreset = stRet.first;
    }
    else
    {
        // 修改
        iterPreset = m_pUserInfo->m_stConstelPresetMap.Find(stReq.m_iPresetID);
    }

    if(iterPreset == m_pUserInfo->m_stConstelPresetMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    RECORD_ARRAY_MEM_HANDLE4(int, TUserConstelPresetTable, Preset, ConstelId1, ConstelId2, ConstelId3, ConstelId4);
    for(int i = CLogicConfigDefine::EHET_Invalid; i < CLogicConfigDefine::EHET_Max - 1; ++i)
    {
        stSetPresetFuncVec[i + 1](iterPreset->second, stReq.m_stConstelID[i]);
    }

    iterPreset->second->SetName(stReq.m_strName);

    stRsp.m_stPresetInfo.m_iPresetID = iterPreset->first;
    stRsp.m_stPresetInfo.m_strName = iterPreset->second->GetName();
    stRsp.m_stPresetInfo.m_stConstelIDs.push_back(iterPreset->second->GetConstelId1());
    stRsp.m_stPresetInfo.m_stConstelIDs.push_back(iterPreset->second->GetConstelId2());
    stRsp.m_stPresetInfo.m_stConstelIDs.push_back(iterPreset->second->GetConstelId3());
    stRsp.m_stPresetInfo.m_stConstelIDs.push_back(iterPreset->second->GetConstelId4());

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicEquipmentProcessor::DoConstelPresetDel(const msgpack::unpacked& stFormatData)
{
    CRequestConstelPresetOp stReq;
    CResponseConstelPresetOp stRsp;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    m_pUserInfo->m_stConstelPresetMap.Erase(stReq.m_iPresetID);

    stRsp.m_iPresetID = stReq.m_iPresetID;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicEquipmentProcessor::DoConstelPresetEquip(const msgpack::unpacked& stFormatData)
{
    CRequestConstelPresetOp stReq;
    CResponseConstelPresetOp stRsp;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    auto iterPreset = m_pUserInfo->m_stConstelPresetMap.Find(stReq.m_iPresetID);
    if(iterPreset == m_pUserInfo->m_stConstelPresetMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(stReq.m_iCardID);
    if(iterCard == m_pUserInfo->m_stHeroCardTableMap.End() || iterCard->second->GetCardLevel() <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED);
    }

    // 先一键卸下
    int iRetCode = ConstelOnekeyUnload(stReq.m_iCardID);
    if(iRetCode != SERVER_ERRCODE::SUCCESS)
    {
        SEND_ERRCODE_AND_RET(iRetCode);
    }

    ConstelEquip(stReq.m_iCardID, iterPreset->second->GetConstelId1());
    ConstelEquip(stReq.m_iCardID, iterPreset->second->GetConstelId2());
    ConstelEquip(stReq.m_iCardID, iterPreset->second->GetConstelId3());
    ConstelEquip(stReq.m_iCardID, iterPreset->second->GetConstelId4());

    stRsp.m_stEquipConstel[CLogicConfigDefine::EHET_Weapon] = iterCard->second->GetEquipConstellation1();
    stRsp.m_stEquipConstel[CLogicConfigDefine::EHET_Clothes] = iterCard->second->GetEquipConstellation2();
    stRsp.m_stEquipConstel[CLogicConfigDefine::EHET_Helmet] = iterCard->second->GetEquipConstellation3();
    stRsp.m_stEquipConstel[CLogicConfigDefine::EHET_Decoration] = iterCard->second->GetEquipConstellation4();

    stRsp.m_iPresetID = stReq.m_iPresetID;
    stRsp.m_iCardID = stReq.m_iCardID;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicEquipmentProcessor::DoConstelPresetList(const msgpack::unpacked& stFormatData)
{
    CResponseConstelPresetList stRsp;

    for(auto iter = m_pUserInfo->m_stConstelPresetMap.Begin(); iter != m_pUserInfo->m_stConstelPresetMap.End(); ++iter)
    {
        CPackConstelPresetInfo stInfo;
        stInfo.m_iPresetID = iter->first;
        stInfo.m_strName = iter->second->GetName();
        stInfo.m_stConstelIDs.push_back(iter->second->GetConstelId1());
        stInfo.m_stConstelIDs.push_back(iter->second->GetConstelId2());
        stInfo.m_stConstelIDs.push_back(iter->second->GetConstelId3());
        stInfo.m_stConstelIDs.push_back(iter->second->GetConstelId4());

        stRsp.m_stPresetList.push_back(stInfo);
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicEquipmentProcessor::DoEquipmentEquip(const msgpack::unpacked& stFormatData)
{
    CRequestEquipmentEquip stReq;
    CResponseEquipmentEquip stRsp;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(stReq.m_iCardID);
    if(iterCard == m_pUserInfo->m_stHeroCardTableMap.End() || iterCard->second->GetCardLevel() < 1)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED);
    }

    if(!CLogicConfigDefine::IsValidEnumHeroEquipType(stReq.m_iEquipTagIndex) || stReq.m_iEquipUniqueID < 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    RECORD_ARRAY_MEM_HANDLE4(int32_t, THeroCardInfoTable, CardEquip, Equip1, Equip2, Equip3, Equip4);

    if(stReq.m_iEquipUniqueID > 0)
    {
        auto iterEquip = m_pUserInfo->m_stEquipMap.Find(stReq.m_iEquipUniqueID);
        if(iterEquip == m_pUserInfo->m_stEquipMap.End() || iterEquip->second->GetEquipmentTag() != stReq.m_iEquipTagIndex)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        // 如果已装备到其他伙伴，则卸下
        for(auto iter = m_pUserInfo->m_stHeroCardTableMap.Begin(); iter != m_pUserInfo->m_stHeroCardTableMap.End(); ++iter)
        {
            if(stGetCardEquipFuncVec[stReq.m_iEquipTagIndex](iter->second) == stReq.m_iEquipUniqueID)
            {
                stSetCardEquipFuncVec[stReq.m_iEquipTagIndex](iter->second, 0);
                break;
            }
        }
    }

    stSetCardEquipFuncVec[stReq.m_iEquipTagIndex](iterCard->second, stReq.m_iEquipUniqueID);

    stRsp.m_iCardID = stReq.m_iCardID;
    stRsp.m_iEquipTagIndex = stReq.m_iEquipTagIndex;
    stRsp.m_iEquipUniqueID = stReq.m_iEquipUniqueID;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicEquipmentProcessor::DoEquipmentDiv(const msgpack::unpacked& stFormatData)
{
    CRequestEquipmentDiv stReq;
    CResponseEquipmentDiv stRsp;
    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    stRsp.m_stRewardItem.m_iItemType = CLogicConfig::GetEquipAttrConfig().m_iTokenItemType;
    stRsp.m_stRewardItem.m_iCardID = CLogicConfig::GetEquipAttrConfig().m_iTokenItemID;
    stRsp.m_stRewardItem.m_iNum = 0;

    std::set<int32_t> stEquipUniqueIDs;
    for(auto iter = m_pUserInfo->m_stHeroCardTableMap.Begin(); iter != m_pUserInfo->m_stHeroCardTableMap.End(); ++iter)
    {
        stEquipUniqueIDs.insert(iter->second->GetEquip1());
        stEquipUniqueIDs.insert(iter->second->GetEquip2());
        stEquipUniqueIDs.insert(iter->second->GetEquip3());
        stEquipUniqueIDs.insert(iter->second->GetEquip4());
    }

    for(auto id : stReq.m_stUniqueIDs)
    {
        if(stEquipUniqueIDs.find(id) != stEquipUniqueIDs.end())
            continue;

        auto iter = m_pUserInfo->m_stEquipMap.Find(id);
        if(iter == m_pUserInfo->m_stEquipMap.End())
            continue;

        if(iter->second->GetIsLocked() > 0)
            continue;

        auto pConfig = CLogicConfig::GetEquipAttrConfig().GetGradeResolveConfig(iter->second->GetGrade());
        if(nullptr == pConfig)
            continue;

        // bill 记录
        CLogicBill::SaveEquipBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, -1, iter->second->m_stData);

        stRsp.m_iRewardGold += pConfig->m_iGold;

        int32_t iRewardTokenNum = CLogicCacheCommon::RandStlCont(pConfig->m_stNumWeight, pConfig->m_iTotalWeight);
        if(iRewardTokenNum > 0)
            stRsp.m_stRewardItem.m_iNum += iRewardTokenNum;

        m_pUserInfo->m_stEquipMap.Erase(iter);

        stRsp.m_stUniqueIDs.push_back(id);
    }

    if(stRsp.m_stUniqueIDs.empty())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    std::vector<CPackGameItem> stReward;
    if(stRsp.m_iRewardGold > 0) stReward.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_GoldID, stRsp.m_iRewardGold);
    if(stRsp.m_stRewardItem.m_iNum > 0) stReward.push_back(stRsp.m_stRewardItem);
    if(!stReward.empty()) CLogicCacheCommon::AddGameItem(m_pUserInfo, stReward, m_usCmd);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicEquipmentProcessor::DoEquipmentLock(const msgpack::unpacked& stFormatData)
{
    CRequestEquipmentLock stReq;
    CResponseEquipmentLock stRsp;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    stRsp.m_iUniqueID = stReq.m_iUniqueID;
    stRsp.m_bIsLock = stReq.m_bIsLock;

    auto iter = m_pUserInfo->m_stEquipMap.Find(stReq.m_iUniqueID);
    if(iter == m_pUserInfo->m_stEquipMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    if(stReq.m_bIsLock)
    {
        iter->second->SetIsLocked(1);
    }
    else
    {
        iter->second->SetIsLocked(0);
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}


bool CLogicEquipmentProcessor::DoEquipmentAddExp(const msgpack::unpacked& stFormatData)
{
    CRequestEquipmentAddExp stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    auto equipIter = m_pUserInfo->m_stEquipMap.Find(stReq.m_iUniqueID);
    if(equipIter == m_pUserInfo->m_stEquipMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    for(auto &consumeItem : stReq.m_stConsumeItem)
    {
        auto itemConfig = CLogicConfig::GetItemConfig().GetConfig(consumeItem.m_iCardID);
        if (nullptr == itemConfig)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        if (consumeItem.m_iNum <= 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        auto ret = CLogicCacheCommon::HasEnoughItem(m_pUserInfo,consumeItem);
        if (!ret)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }
    }

    int32_t iTotalAddExp = 0;
    for(auto &consumeItem : stReq.m_stConsumeItem)
    {
        auto itemConfig = CLogicConfig::GetItemConfig().GetConfig(consumeItem.m_iCardID);
        if (nullptr == itemConfig)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        iTotalAddExp += itemConfig->m_iTypePara1 * consumeItem.m_iNum;

        CLogicCacheCommon::ConsumeGameItem(m_pUserInfo,consumeItem,m_usCmd);
    }

    equipIter->second->AddEquipmentExp(iTotalAddExp);

    CResponseEquipmentAddExp stRsp;
    stRsp.m_iUniqueID = stReq.m_iUniqueID;
    stRsp.m_stConsumeItem = m_pUserInfo->m_stCacheBonusData;
    stRsp.m_iTotalExp = iTotalAddExp;
    SendToClient(stRsp,SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicEquipmentProcessor::DoEquipmentUpgrade(const msgpack::unpacked& stFormatData)
{
    CRequestEquipmentUpgrade stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;
    //都是存在的
    auto equipUpgradeIter = m_pUserInfo->m_stEquipMap.Find(stReq.m_iUniqueID);
    if(equipUpgradeIter == m_pUserInfo->m_stEquipMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    //检查装备等级和目前的阶数
    auto UpgradeConfig = CLogicConfig::GetEquipAttrConfig().m_stEquipUpGrade.find(equipUpgradeIter->second->GetEquipmentRank()+1);
    if (CLogicConfig::GetEquipAttrConfig().m_stEquipUpGrade.end() == UpgradeConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    if (equipUpgradeIter->second->GetEquipmentExp() < UpgradeConfig->second.m_iNeedLevel)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::EQUIPMENT_UPGRADE_LEVEL_NOT_MATCH);
    }

    if (stReq.m_iUpgradeType == CLogicConfigDefine::USE_EQUIPMENT)
    {
        //不是同一件
        if (stReq.m_iUniqueID == stReq.m_iUpgradeID)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        auto equipIter = m_pUserInfo->m_stEquipMap.Find(stReq.m_iUpgradeID);
        if(equipIter == m_pUserInfo->m_stEquipMap.End())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        //检查是否是同名装备
        auto targetMainAttr = CLogicConfig::GetEquipAttrConfig().GetAttrConfig(equipIter->second->GetAttrIndex());
        auto UpgradeMainAttr = CLogicConfig::GetEquipAttrConfig().GetAttrConfig(equipIter->second->GetAttrIndex());

        if (targetMainAttr == nullptr || UpgradeMainAttr == nullptr)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        if (targetMainAttr->m_iMainSkill != UpgradeMainAttr->m_iMainSkill)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        //消耗同名装备和物品
        m_pUserInfo->m_stEquipMap.Erase(equipUpgradeIter);
        CPackGameItem costGold;
        costGold.m_iNum = UpgradeConfig->second.m_iCostGold;
        costGold.m_iCardID = CLogicConfigDefine::ECIT_GoldID;
        costGold.m_iItemType = CLogicConfigDefine::CURRENCY;
        auto ret = CLogicCacheCommon::ConsumeGameItem(m_pUserInfo,costGold,m_usCmd);
        if (!ret)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM);
        }

    }
    else if (stReq.m_iUpgradeType == CLogicConfigDefine::USE_ITEM)
    {
        auto ret = CLogicCacheCommon::ConsumeGameItem(m_pUserInfo,UpgradeConfig->second.m_stCostItem,m_usCmd);
        if (!ret)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM);
        }
    }

    //装备进阶
    equipUpgradeIter->second->AddEquipmentRank(1);
    CResponseEquipmentUpgrade stRsp;
    stRsp.m_iUniqueID = stReq.m_iUniqueID;
    stRsp.m_iUpgradeID = stReq.m_iUpgradeID;
    stRsp.m_iTotalItem = m_pUserInfo->m_stCacheBonusData;

    SendToClient(stRsp,SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicEquipmentProcessor::DoEquipmentRefine(const msgpack::unpacked& stFormatData)
{
    CRequestEquipmentRefine stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    //检查目标装备存在
    auto equipIter = m_pUserInfo->m_stEquipMap.Find(stReq.m_iUniqueID);
    if(equipIter == m_pUserInfo->m_stEquipMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    //扣除消耗道具
    auto ret = CLogicCacheCommon::ConsumeGameItem(m_pUserInfo,CLogicConfig::GetEquipAttrConfig().m_iRefineItem,m_usCmd);
    if (!ret)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM);
    }

    //随机词条
    auto targetMainAttr = CLogicConfig::GetEquipAttrConfig().GetAttrConfig(equipIter->second->GetAttrIndex());
    auto iterRandRefine = CLogicConfig::GetEquipAttrConfig().m_stRandRefine.find(targetMainAttr->m_iRefineIndex);
    if (iterRandRefine == CLogicConfig::GetEquipAttrConfig().m_stRandRefine.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    //构建权重对
    std::vector<std::pair<int32_t,int32_t>> pairWeight;
    auto iterWeightBegin = iterRandRefine->second.m_iWeight.begin();
    auto iterRandBegin = iterRandRefine->second.m_iRandom.begin();
    for(; iterWeightBegin != iterRandRefine->second.m_iWeight.end() && iterRandBegin != iterRandRefine->second.m_iRandom.end();
         iterWeightBegin++,iterRandBegin++)
    {
        pairWeight.emplace_back(std::make_pair(*iterWeightBegin,*iterRandBegin));
    }

    int32_t chooseAttr = CLogicCacheCommon::RandStlCont(pairWeight);

    //记录在内存中
    m_pUserInfo->m_stOnlyInCacheData.m_IndexAttr.insert(std::make_pair(stReq.m_iUniqueID,chooseAttr));

    CResponseEquipmentRefine stRsp;
    stRsp.m_iUniqueID = stReq.m_iUniqueID;
    stRsp.m_iCostItem = m_pUserInfo->m_stCacheBonusData;
    stRsp.m_iAttrIndex = chooseAttr;

    SendToClient(stRsp,SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicEquipmentProcessor::DoEquipmentRefineOp(const msgpack::unpacked& stFormatData)
{
    CRequestEquipmentRefineOp stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    //检查目标装备存在
    auto equipIter = m_pUserInfo->m_stEquipMap.Find(stReq.m_iUniqueID);
    if(equipIter == m_pUserInfo->m_stEquipMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    auto upGradeItem = m_pUserInfo->m_stOnlyInCacheData.m_IndexAttr.find(stReq.m_iUniqueID);
    if (upGradeItem == m_pUserInfo->m_stOnlyInCacheData.m_IndexAttr.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    if (stReq.m_bChoose)
    {
        equipIter->second->SetEquipmentExIndex(upGradeItem->second);
    }

    CResponseEquipmentRefineOp stRsp;
    stRsp.m_iUniqueID = stReq.m_iUniqueID;
    stRsp.m_iAttrIndex = upGradeItem->second;

    m_pUserInfo->m_stOnlyInCacheData.m_IndexAttr.erase(upGradeItem);
    SendToClient(stRsp,SERVER_ERRCODE::SUCCESS);
    return true;
}



int CLogicEquipmentProcessor::ConstelOnekeyUnload(int iCardID)
{
    auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(iCardID);
    if(iterCard == m_pUserInfo->m_stHeroCardTableMap.End() || iterCard->second->GetCardLevel() <= 0)
    {
        return SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED;
    }

    std::vector<int32_t> stUniqueIDs = {
            iterCard->second->GetEquipConstellation1(),
            iterCard->second->GetEquipConstellation2(),
            iterCard->second->GetEquipConstellation3(),
            iterCard->second->GetEquipConstellation4()
    };

    iterCard->second->SetEquipConstellation1(0);
    iterCard->second->SetEquipConstellation2(0);
    iterCard->second->SetEquipConstellation3(0);
    iterCard->second->SetEquipConstellation4(0);

    for(auto id : stUniqueIDs)
    {
        if(id <= 0)
            continue;

        auto iter = m_pUserInfo->m_stConstelMap.Find(id);
        if(iter == m_pUserInfo->m_stConstelMap.End())
            continue;

        iter->second->SetEquipCardID(0);
    }

    return SERVER_ERRCODE::SUCCESS;
}

int CLogicEquipmentProcessor::ConstelEquip(int iCardID, int iConstelID)
{
    if(iCardID < 0) return SERVER_ERRCODE::PARAMETER_ERROR;

    RECORD_ARRAY_MEM_HANDLE4(int, THeroCardInfoTable, Constel, EquipConstellation1, EquipConstellation2, EquipConstellation3, EquipConstellation4);

    auto iterEquipConstel = m_pUserInfo->m_stConstelMap.End();
    auto iterUnequipConstel = m_pUserInfo->m_stConstelMap.End();
    auto iterEquipCard = m_pUserInfo->m_stHeroCardTableMap.End();
    auto iterUnequipCard = m_pUserInfo->m_stHeroCardTableMap.End();

    iterEquipConstel = m_pUserInfo->m_stConstelMap.Find(iConstelID);
    if(iterEquipConstel == m_pUserInfo->m_stConstelMap.End())
    {
        return SERVER_ERRCODE::PARAMETER_ERROR;
    }

    const int32_t iEquipTag = iterEquipConstel->second->GetEquipmentTag();
    if(false == CLogicConfigDefine::IsValidEnumHeroEquipType(iEquipTag))
    {
        return SERVER_ERRCODE::INTERN_ERROR;
    }

    if(iCardID > 0)
    {
        if(iterEquipConstel->second->GetEquipCardID() == iCardID)
        {
            return SERVER_ERRCODE::SUCCESS;
        }

        /*if(iterEquipConstel->second->GetIsDiscard())
        {
            return SERVER_ERRCODE::IS_DISCARD;
        }*/

        iterEquipCard = m_pUserInfo->m_stHeroCardTableMap.Find(iCardID);
        if(iterEquipCard == m_pUserInfo->m_stHeroCardTableMap.End() || iterEquipCard->second->GetCardLevel() <= 0)
        {
            return SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED;
        }

        // 判断是否有需要卸下的星纹
        int32_t iUnequipConstelID = stGetConstelFuncVec[iEquipTag](iterEquipCard->second);
        if(iUnequipConstelID > 0)
        {
            iterUnequipConstel = m_pUserInfo->m_stConstelMap.Find(iUnequipConstelID);
            if(iterUnequipConstel == m_pUserInfo->m_stConstelMap.End())
            {
                return SERVER_ERRCODE::INTERN_ERROR;
            }
        }
    }

    // 判断星纹是否需要先卸下
    int32_t iUnequipCardID = iterEquipConstel->second->GetEquipCardID();
    if(iUnequipCardID > 0)
    {
        iterUnequipCard = m_pUserInfo->m_stHeroCardTableMap.Find(iUnequipCardID);
        if(iterUnequipCard == m_pUserInfo->m_stHeroCardTableMap.End() || iterUnequipCard->second->GetCardLevel() <= 0)
        {
            return SERVER_ERRCODE::INTERN_ERROR;
        }
    }

    iterEquipConstel->second->SetEquipCardID(iCardID);

    if(iterEquipCard != m_pUserInfo->m_stHeroCardTableMap.End())
        stSetConstelFuncVec[iEquipTag](iterEquipCard->second, iConstelID);

    if(iterUnequipConstel != m_pUserInfo->m_stConstelMap.End())
        iterUnequipConstel->second->SetEquipCardID(0);

    if(iterUnequipCard != m_pUserInfo->m_stHeroCardTableMap.End())
        stSetConstelFuncVec[iEquipTag](iterUnequipCard->second, 0);

    //记录套装数
    auto stPutInfo = m_pUserInfo->m_stIBusyData.GetConstelPutInfo();
    PROCESSOR_LOG_DEBUG << iterEquipConstel->second->GetSuit() << std::endl;
    auto iter = stPutInfo.find(iterEquipConstel->second->GetSuit());
    if (iter == stPutInfo.end())
    {
        PROCESSOR_LOG_DEBUG << iterEquipConstel->second->GetEquipmentTag() << std::endl;
        std::map<int,char> mapInfo;
        mapInfo.insert(std::make_pair(iterEquipConstel->second->GetEquipmentTag(),1));
        stPutInfo[iterEquipConstel->second->GetSuit()] = mapInfo;
    } else
    {
        PROCESSOR_LOG_DEBUG << iterEquipConstel->second->GetEquipmentTag() << std::endl;
        auto iterPutInfo = iter->second.find(iterEquipConstel->second->GetEquipmentTag());
        if (iterPutInfo == iter->second.end())
        {
            PROCESSOR_LOG_DEBUG << iterEquipConstel->second->GetEquipmentTag() << std::endl;
            iter->second[iterEquipConstel->second->GetEquipmentTag()] = 1;
        }
    }
    m_pUserInfo->m_stIBusyData.SetConstelPutInfo(stPutInfo);


    // 通用事件
    CLogicEventData stEventData;
    stEventData.m_eType = CLogicConfigDefine::ECET_PutOnConstelSuit;
    stEventData.m_iPara = iterEquipConstel->second->GetSuit();
    stEventData.m_iParaEx = iterEquipConstel->second->GetEquipmentTag();
    stEventData.m_iCmd = m_usCmd;
    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

    return SERVER_ERRCODE::SUCCESS;
}
