#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_tactics_protocol.h"
#include "logic_link_error.h"
#include "logic_game_talk_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_config.h"
#include "logic_game_global_frame.h"
#include "logic_game_cache_common.h"
#include "logic_game_rune_processor.h"
#include "server_rune_protocol.h"
#include "server_common_protocol.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_SET_RUNE_CARD, CLogicRuneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_RUNE_INFO, CLogicRuneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_UNLOCK_RUNE, CLogicRuneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_RUNE_LEVEL_UP, CLogicRuneProcessor)


CLogicRuneProcessor::CLogicRuneProcessor(uint16_t usCmd, const std::string &strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicRuneProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    bool bRet = false;

    switch (m_usCmd)
    {
    case MSG_LOGIC_SET_RUNE_CARD:
        bRet = DoUserRunSetRune(stFormatData);
        break;
    case MSG_LOGIC_RUNE_INFO:
        bRet = DoUserRunGetRuneInfo(stFormatData);
        break;
    case MSG_LOGIC_UNLOCK_RUNE:
        bRet = DoUserRunUnlockRune(stFormatData);
        break;
    case MSG_LOGIC_RUNE_LEVEL_UP:
        bRet = DoUserRunRuneLevelUp(stFormatData);
        break;
    default:
        break;
    }

    return bRet;
}

int32_t CLogicRuneProcessor::DoUserRunSetRune(const msgpack::unpacked& stFormatData)
{
    CRequestSetThemeSkin stReq;
    CResponseSetThemeSkin stRsp;
    if (DecodeMsgPack(stFormatData, stReq) == false) return (false);

    stRsp.m_iSkinID = stReq.m_iSkinID;
    if(m_pUserInfo->m_stUserInfoTable.GetRuneCard() == stReq.m_iSkinID)
    {
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return true;
    }

    m_pUserInfo->m_stUserInfoTable.SetRuneCard(stReq.m_iSkinID);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

int32_t CLogicRuneProcessor::DoUserRunGetRuneInfo(const msgpack::unpacked& stFormatData)
{
    //每次获取灵符信息检查各个升级条件，但是只检查已经解锁的。
    auto runeInfo = m_pUserInfo->m_stIBusyData.GetRuneInfo();
    for (auto& iterRune: runeInfo)
    {
        auto runeConfig = CLogicConfig::GetRuneConfig().GetRuneElemByID(iterRune.first);
        if (nullptr == runeConfig)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
        }

        auto runeLeveUpConfig = CLogicConfig::GetRuneConfig().GetRuneUpgradeByID(runeConfig->m_iUpgradeID);
        if (nullptr == runeLeveUpConfig)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
        }

        //检查各个等级是否能够解锁
        for (auto iterUnlockPara: runeLeveUpConfig->m_stUnlock)
        {
            switch (iterUnlockPara.second.m_iUnlockType)
            {
            case EnumRuneUnLockType::RuneUnLockTypeLeveUp:
            {
                auto stIter = m_pUserInfo->m_stHeroCardTableMap.Find(iterUnlockPara.second.m_iUnlockPara[0]);
                if (stIter == m_pUserInfo->m_stHeroCardTableMap.End() || stIter->second->GetCardLevel() <= 0)
                {
                    continue;
                }

                if (iterUnlockPara.second.m_iUnlockPara[1] <= stIter->second->GetCardLevel())
                {
                    iterRune.second.m_stState[iterUnlockPara.first] = true;
                }
                break;
            }
            case EnumRuneUnLockType::RuneUnLockTypeUpgrade:
            {
                auto stIter = m_pUserInfo->m_stHeroCardTableMap.Find(iterUnlockPara.second.m_iUnlockPara[0]);
                if (stIter == m_pUserInfo->m_stHeroCardTableMap.End())
                {
                    continue;
                }

                auto pCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(iterUnlockPara.second.m_iUnlockPara[0]);
                if (pCardConfig == nullptr)
                {
                    continue;
                }

                if (iterUnlockPara.second.m_iUnlockPara[1] <= stIter->second->GetCardColor())
                {
                    iterRune.second.m_stState[iterUnlockPara.first] = true;
                }
                break;
            }
            case EnumRuneUnLockType::RuneUnLockTypeStarUp:
            {
                auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(iterUnlockPara.second.m_iUnlockPara[0]);
                if (iterCard == m_pUserInfo->m_stHeroCardTableMap.End())
                {
                    continue;
                }

                if (iterUnlockPara.second.m_iUnlockPara[1] <= iterCard->second->GetCardStar())
                {
                    iterRune.second.m_stState[iterUnlockPara.first] = true;
                }
                break;
            }
            case EnumRuneUnLockType::RuneUnLockTypeUseSkill:
            {
                auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(iterUnlockPara.second.m_iUnlockPara[0]);
                if (iterCard == m_pUserInfo->m_stHeroCardTableMap.End())
                {
                    continue;
                }

                if (iterUnlockPara.second.m_iUnlockPara[1] <= iterCard->second->GetUseSkillCount())
                {
                    iterRune.second.m_stState[iterUnlockPara.first] = true;
                }
                break;
            }
            case EnumRuneUnLockType::RuneUnLockTypePassLevel:
            {
                auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(iterUnlockPara.second.m_iUnlockPara[0]);
                if (iterCard == m_pUserInfo->m_stHeroCardTableMap.End())
                {
                    continue;
                }

                for (auto unLockLevel : iterUnlockPara.second.m_iUnlockParaEx)
                {
                    if (iterCard->second->GetPassLevel().find(unLockLevel) != iterCard->second->GetPassLevel().end())
                    {
                        iterRune.second.m_stState[iterUnlockPara.first] = true;
                        break;
                    }
                }
                break;
            }
            }
        }
    }

    m_pUserInfo->m_stIBusyData.SetRuneInfo(runeInfo);
    CResponseGetRuneInfo stRsp;
    stRsp.m_stRuneInfo = runeInfo;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

int32_t CLogicRuneProcessor::DoUserRunUnlockRune(const msgpack::unpacked& stFormatData)
{
    CRequestUnlockRune stReq;
    if (DecodeMsgPack(stFormatData, stReq) == false) return (false);

    auto pRuneConfig = CLogicConfig::GetRuneConfig().GetRuneElemByID(stReq.m_iRuneID);
    if (nullptr == pRuneConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
    }

    auto pItemConfig = CLogicConfig::GetItemConfig().GetConfig(pRuneConfig->m_iLockItemID);
    if (nullptr == pItemConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
    }

    auto curRuneInfo = m_pUserInfo->m_stIBusyData.GetRuneInfo();
    auto runeIter = curRuneInfo.find(stReq.m_iRuneID);
    if (runeIter != curRuneInfo.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::RUNE_HAS_UNLOCK)
    }

    if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo,pItemConfig->m_iSubType,pItemConfig->m_iItemID,1,m_usCmd))
    {
        PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                            << "|MSG:" << CLogicError::GetErrMsg()
                            << std::endl;

        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
    }

    CResponseUnlockRune stRsp;

    CPackRuneInfo runeInfo;
    runeInfo.m_iRuneID = stReq.m_iRuneID;
    runeInfo.m_iLevel = 1;
    curRuneInfo.insert(std::make_pair(stReq.m_iRuneID,runeInfo));

    m_pUserInfo->m_stIBusyData.SetRuneInfo(curRuneInfo);
    stRsp.m_iRuneID = stReq.m_iRuneID;
    stRsp.m_iUnlockItemID = pItemConfig->m_iItemID;

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}


int32_t CLogicRuneProcessor::DoUserRunRuneLevelUp(const msgpack::unpacked& stFormatData)
{
    CRequestRuneLevelUp stReq;
    if (DecodeMsgPack(stFormatData, stReq) == false) return (false);

    auto stRuneInfo = m_pUserInfo->m_stIBusyData.GetRuneInfo();
    auto iterRune = stRuneInfo.find(stReq.m_iRuneID);
    if (iterRune == stRuneInfo.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::RUNE_ID_NOT_FOUND);
    }

    if (!iterRune->second.m_stState[iterRune->second.m_iLevel])
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::RUNE_CAN_NOT_LEVEL_UP);
    }

    iterRune->second.m_iLevel++;
    m_pUserInfo->m_stIBusyData.SetRuneInfo(stRuneInfo);

    CResponseRuneLevelUp stRsp;
    stRsp.m_iRuneID = stReq.m_iRuneID;

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}
