#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "server_errcode.h"
#include "server_card_protocol.h"
#include "server_fight_protocol.h"
#include "server_cross_protocol.h"
#include "logic_game_item.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_card_operation_processor.h"
#include "logic_game_publisher.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_item_factory.h"
#include "logic_game_global.h"
#include "logic_game_attribute.h"
#include "logic_game_active_info.h"
#include "logic_game_common_publisher.h"
#include "server_data_report_protocol.h"
#include "server_cmd_data_report_define.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_UPGRADE_CARD_LEVEL, CLogicCardOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_COMBINE_CARD_SOUL, CLogicCardOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_UPGRADE_CARD_COLOR, CLogicCardOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_UPGRADE_CARD_STAR, CLogicCardOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_UPGRADE_CARD_COLOR_MATERIAL, CLogicCardOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_CHANGE_TEAM, CLogicCardOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_SET_TEAM_PRESET, CLogicCardOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_CHANGE_TEAM_PRESET_NAME, CLogicCardOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_HERO_HEART_GIFTS, CLogicCardOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CARD_SOUL_EXCHANGE, CLogicCardOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_UPGRADE_CARD_TALENT, CLogicCardOperationProcessor)

CLogicCardOperationProcessor::CLogicCardOperationProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicCardOperationProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    CLogicCacheCardOperation stCacheCardOP(m_pUserInfo);
    
    // bool bIsNeedReCalcCombat = true;

    if (m_usCmd == MSG_LOGIC_UPGRADE_CARD_LEVEL)
    {
        CRequestUpgradeHeroCardLevel stParam;

        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);
        
        do
        {
            auto stIter = m_pUserInfo->m_stHeroCardTableMap.Find(stParam.m_iHeroCardID);
            if (stIter == m_pUserInfo->m_stHeroCardTableMap.End() || stIter->second->GetCardLevel() <= 0)
            {
                using namespace SERVER_ERRCODE;
                SET_ERR_INFO(HERO_CARD_IS_NOT_OWNED);
                break;
            }

            int iBeforeLevelUpCardLevel = stIter->second->GetCardLevel();
            int iBeforeLevelUpCardExp = stIter->second->GetCardExp();

            int iCurrentTeamLevel = m_pUserInfo->m_stUserInfoTable.GetLevel();
            int iCurrentCardLevel = iBeforeLevelUpCardLevel;

            if (iCurrentTeamLevel < 0 || (unsigned int) iCurrentTeamLevel >= CLogicConfig::GetUserExpConfig().m_stUserExpConfig.size())
            {
                using namespace SERVER_ERRCODE;
                SET_ERR_INFO(ROLE_LEVEL_EXP_OUT_OF_CONFIG);
                break;
            }

            int iCardLevelLimit = CLogicConfig::GetUserExpConfig().m_stUserExpConfig[iCurrentTeamLevel].m_iCardLevelLimit;
            if (iCurrentCardLevel > iCardLevelLimit)
            {
                using namespace SERVER_ERRCODE;
                SET_ERR_INFO(TEAM_LEVEL_LIMIT_CARD_LEVEL);
                break;
            }
            
            if(stParam.m_stExpCardMap.empty())
            {
                using namespace SERVER_ERRCODE;
                SET_ERR_INFO(NUMBER_INVALID);
                break;
            }
            
            std::string strExpCardLog = "{";
            char szBuffer[128];
            
            bool bParamValid = true;
            int iTotalExpAmount = 0;
            std::vector<CLogicGameItemFactory::CRkLogicGameItemBasePtr> stExpCardPtrList;
            for(auto stIterExpMap = stParam.m_stExpCardMap.begin(); stIterExpMap != stParam.m_stExpCardMap.end(); ++ stIterExpMap)
            {
                if(stIterExpMap->second <= 0)
                {
                    using namespace SERVER_ERRCODE;
                    CLogicError::SetErrCode(NUMBER_INVALID, GET_ERRMSG_BY_ERRCODE(NUMBER_INVALID));
                    bParamValid = false;
                    break;
                }
                
                auto pExpDrinkConfig = CLogicConfig::GetItemConfig().GetConfig(stIterExpMap->first);
                if(nullptr == pExpDrinkConfig || pExpDrinkConfig->m_iSubType != CLogicConfigDefine::EIST_Drink)
                {
                    using namespace SERVER_ERRCODE;
                    CLogicError::SetErrCode(PARAMETER_ERROR, GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR));
                    bParamValid = false;
                    break;
                }
                iTotalExpAmount += (pExpDrinkConfig->m_iTypePara1 * stIterExpMap->second);
                
                auto stExpCardPtr = LOGIC_GAME_ITEM_FACTORY_INS->CreateGameItem(CLogicConfigDefine::EXCHANGE_ITEM,
                      stIterExpMap->first, stIterExpMap->second, m_usCmd, m_pUserInfo, {
                          {"id", std::to_string(stParam.m_iHeroCardID)},
                          {"before_level", std::to_string(iBeforeLevelUpCardLevel)},
                          {"before_exp", std::to_string(iBeforeLevelUpCardExp)}
                    });
                if(stExpCardPtr.get() == nullptr)
                {
                    using namespace SERVER_ERRCODE;
                    CLogicError::SetErrCode(EXP_CARD_IS_NOT_CONFIGED, GET_ERRMSG_BY_ERRCODE(EXP_CARD_IS_NOT_CONFIGED));
                    bParamValid = false;
                    break;
                }
                
                stExpCardPtrList.push_back(stExpCardPtr);
                
                snprintf(szBuffer, sizeof(szBuffer), "[exp_drink_id:%d,exp_drink_amount:%d]", stIterExpMap->first, stIterExpMap->second);
                strExpCardLog += szBuffer;
            }
            
            strExpCardLog += "}";
            
            if(!bParamValid) break;

            bool bIsExpCardEnough = true;
            for(auto stIterExpCardOtr = stExpCardPtrList.begin(); stIterExpCardOtr != stExpCardPtrList.end(); ++ stIterExpCardOtr)
            {
                if((*stIterExpCardOtr)->CanConsume() == false)
                {
                    bIsExpCardEnough = false;
                    break;
                }
            }
            
            if(!bIsExpCardEnough) break;
            
            if(stCacheCardOP.UpgradeCardLevel(stParam.m_iHeroCardID, iTotalExpAmount) == false) break;
            
            for(auto stIterExpCardPtr = stExpCardPtrList.begin(); stIterExpCardPtr != stExpCardPtrList.end(); ++ stIterExpCardPtr)
            {
                (*stIterExpCardPtr)->Consume();
            }
            
            CResponseUpgradeHeroCardLevel stRsp;
            for(auto stIterExpCardMap = stParam.m_stExpCardMap.begin(); stIterExpCardMap != stParam.m_stExpCardMap.end(); ++ stIterExpCardMap)
            {
                auto stExpDrinkIT = m_pUserInfo->m_stItemMap.Find(stIterExpCardMap->first);
                if(stExpDrinkIT != m_pUserInfo->m_stItemMap.End())
                {
                    stRsp.m_stExpCardMap[stIterExpCardMap->first] = stExpDrinkIT->second->m_stData.m_iAmount;
                }
                else
                {
                    stRsp.m_stExpCardMap[stIterExpCardMap->first] = 0;
                }
            }
            
            stRsp.m_iHeroCardID = stParam.m_iHeroCardID;
            stRsp.m_iHeroCardLevel = stIter->second->GetCardLevel();
            stRsp.m_iHeroCardExp = stIter->second->GetCardExp();
            
            SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

            CLogicGameCardChannel::data_type stData;
            stData.m_iCardOption = CLogicGameChannelEnum::UPGRADE_CARD_USE_EXP_DRINK;
            LOGIC_PUBLISHER_INS->PublishGameCardInfo(m_pUserInfo, stData);

            // 通用事件
            CLogicEventData stEventData;
            stEventData.m_eType = CLogicConfigDefine::ECET_UpgradeCardTimes;
            stEventData.m_iCmd = m_usCmd;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

            PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|HERO_CARD_ID:" << stParam.m_iHeroCardID << "|EXP_DRINK_LIST:" 
                                << strExpCardLog << "|ADD_EXP_AMOUNT:" << iTotalExpAmount << "|BEFORE_HERO_CARD_LEVEL:"
                                << iBeforeLevelUpCardLevel << "|BEFORE_HERO_CARD_EXP:" << iBeforeLevelUpCardExp << "|AFTER_HERO_CARD_LEVEL:" 
                                << stRsp.m_iHeroCardLevel << "|AFTER_HERO_CARD_EXP:" << stRsp.m_iHeroCardExp << std::endl;
            
            return (true);
        } while(0);
        
        PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                            << "|MSG:" << CLogicError::GetErrMsg()
                            << "|HERO_CARD_ID:" << stParam.m_iHeroCardID << std::endl;

        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
    }
    else if(m_usCmd == MSG_LOGIC_CARD_SOUL_EXCHANGE)
    {
        CRequestCardSoulExchange stParam;
        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);

        if(stParam.m_iSoulNum <= 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        auto iterData = m_pUserInfo->m_stHeroCardTableMap.Find(stParam.m_iCardID);
        if(iterData == m_pUserInfo->m_stHeroCardTableMap.End())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED)
        }

        auto& stConfig = CLogicConfig::GetUpgradeHeroCardStarConfig();
        if(stConfig.UpgradeStarNeedSoul(iterData->second->GetCardStar()) > 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_STAR_NOT_MAX)
        }

        auto pConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stParam.m_iCardID);
        if(nullptr == pConfig)
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        int iExchangeNum = stConfig.GetSoulExchangeNum(iterData->second->GetGradeLevel());
        if(iExchangeNum <= 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_CANNOT_EXCHANGE)
        }

        iExchangeNum *= stParam.m_iSoulNum;

        if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, pConfig->m_iSoulID, stParam.m_iSoulNum, m_usCmd))
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, CLogicConfig::GetUpgradeHeroCardStarConfig().m_iSoulExchangeItem, iExchangeNum, m_usCmd);

        SendSuccessCacheItemToClient();
        return true;
    }
    else if(m_usCmd == MSG_LOGIC_UPGRADE_CARD_COLOR_MATERIAL)
    {
        CResponseUpgradeHeroCardColorMaterial stRsp;
        CRequestUpgradeHeroCardColorMaterial stParam;

        if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

        auto stCardIter = m_pUserInfo->m_stHeroCardTableMap.Find(stParam.m_iCardID);
        if (stCardIter == m_pUserInfo->m_stHeroCardTableMap.End() || stCardIter->second->GetCardLevel() <= 0)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << HERO_CARD_IS_NOT_OWNED
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(HERO_CARD_IS_NOT_OWNED)
                                << "|CARD_ID:" << stParam.m_iCardID << std::endl;

            SEND_ERRCODE_AND_RET(HERO_CARD_IS_NOT_OWNED)
        }

        auto pCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stParam.m_iCardID);
        if (pCardConfig == nullptr)
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|CARD_ID:" << stParam.m_iCardID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        auto pUpgradeConfig = CLogicConfig::GetUpgradeHeroCardColorConfig().GetConfig(pCardConfig->m_iCardType, stCardIter->second->GetCardColor());
        if (pUpgradeConfig == nullptr)
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|CARD_ID:" << stParam.m_iCardID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        if(stParam.m_stPosVec.empty())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        RECORD_ARRAY_MEM_HANDLE4(int32_t, THeroCardInfoTable, Color, UpgradeColor1, UpgradeColor2, UpgradeColor3, UpgradeColor4);

        for (auto& iPos : stParam.m_stPosVec)
        {
            if(iPos <= 0 || iPos > CLogicConfigDefine::ESD_CardColorUpPosNum)
                continue;

            if (stGetColorFuncVec[iPos](stCardIter->second) != 0)
                continue;

            CPackGameItem stColorItem(CLogicConfigDefine::EXCHANGE_ITEM, pUpgradeConfig->m_stColorItemVec[iPos].first, pUpgradeConfig->m_stColorItemVec[iPos].second);

            auto stConsume = LOGIC_GAME_ITEM_FACTORY_INS->CreateGameItem(
                    (CLogicConfigDefine::GAME_ITEM_COLLECTION)stColorItem.m_iItemType,
                    stColorItem.m_iCardID,
                    stColorItem.m_iNum,
                    m_usCmd, m_pUserInfo);

            if (!stConsume.get() || !stConsume->CanConsume())
            {
                // 尝试一键合成
                std::map<int, std::pair<int, int>> stConsumeVec;
                int iCurNum = CLogicCacheCommon::GetItemNum(m_pUserInfo, stColorItem.m_iItemType, stColorItem.m_iCardID);
                if(iCurNum > 0)
                {
                    stConsumeVec[stColorItem.m_iCardID] = std::make_pair(stColorItem.m_iItemType, iCurNum);
                    stColorItem.m_iNum -= iCurNum;
                }

                if(stColorItem.m_iNum <= 0 || !CLogicCacheCommon::CombineOneKey(m_pUserInfo, stColorItem.m_iCardID, stColorItem.m_iNum, stConsumeVec))
                {
                    continue;
                }

                std::string strConsumeList = "{";
                std::vector<CPackGameItem> stItemVec;
                for(auto& iter : stConsumeVec)
                {
                    stItemVec.emplace_back(iter.second.first, iter.first, iter.second.second);

                    static char szBuffer[128];
                    snprintf(szBuffer, sizeof(szBuffer), "[%d, %d, %d],", iter.second.first, iter.first, iter.second.second);

                    strConsumeList += szBuffer;
                }
                strConsumeList[strConsumeList.length() - 1] = '}';

                if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stItemVec, m_usCmd))
                {
                    continue;
                }

                stRsp.m_stConsumeItemVec.insert(stRsp.m_stConsumeItemVec.end(), stItemVec.begin(), stItemVec.end());
            }
            else
            {
                stConsume->Consume();

                stRsp.m_stConsumeItemVec.push_back(stColorItem);
            }

            stSetColorFuncVec[iPos](stCardIter->second, stColorItem.m_iCardID);
            stRsp.m_stPos.push_back(iPos);
        }

        if(stRsp.m_stPos.empty())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM);
        }

        stRsp.m_iCardID = stParam.m_iCardID;
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|CARD_ID:" << stParam.m_iCardID << std::endl;
    }
	else if(m_usCmd == MSG_LOGIC_TOUHOU_CHANGE_TEAM)
	{
        CRequestChangeTeam stReq;
        CResponseChangeTeam stRsp;
		if (!DecodeMsgPack(stFormatData, stReq)) return (false);

        // 竹海阵容
        if(stReq.m_iTeamType == CLogicConfigDefine::ETT_ZhuHaiTeam)
        {
            if(m_pUserInfo->m_stIBusyData.GetZhuHaiGress() > 0)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ZHUHAI_IN_FIGHTING)
            }
        }

        auto strMembers = vector_join(stReq.m_stTeam, '-');
		if (!CLogicCacheCommon::SetTeam(m_pUserInfo, stReq.m_iTeamType, stReq.m_stTeam))
		{
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< "|TEAM_TYPE:" << stReq.m_iTeamType
				<< "|TEAM_MEMBER1:" << strMembers
                << "|PARA:" << stReq.m_iPara
				<< std::endl;

			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		stRsp.m_iTeamType = stReq.m_iTeamType;
		stRsp.m_stTeam = stReq.m_stTeam;
		stRsp.m_iPara = stReq.m_iPara;

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

		PROCESSOR_LOG_DEBUG << "TEAM_TYPE:" << stReq.m_iTeamType
		    << "|TEAM_MEMBER1:" << strMembers
            << "|PARA:" << stReq.m_iPara
			<< std::endl;

		return (true);
	}
    else if(m_usCmd == MSG_LOGIC_TOUHOU_SET_TEAM_PRESET)
    {
        CRequestChangeTeamPreset stReq;
        CResponseChangeTeamPreset stRsp;
        if(DecodeMsgPack(stFormatData, stReq) == false) return false;

        if(stReq.m_iTeamIndex <= 0 || stReq.m_iTeamIndex > CLogicConfig::GetGlobalConfig().m_iTeamPresetNum)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        if (!CLogicCacheCommon::CheckTeamMemberValid(m_pUserInfo, stReq.m_stTeam))
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        if(stReq.m_stTeam.size() < CLogicConfigDefine::ESD_TeamMaxCardNum)
        {
            stReq.m_stTeam.resize(CLogicConfigDefine::ESD_TeamMaxCardNum);
        }

        auto iterData = m_pUserInfo->m_stUserTeamPresetMap.Find(stReq.m_iTeamIndex);
        if(iterData == m_pUserInfo->m_stUserTeamPresetMap.End())
        {
            TUserTeamPresetInfoTableValueType stValue;
            stValue.m_iUid = m_iUin;
            stValue.m_iGroupID = m_iGroupID;
            stValue.m_iTeamIndex = stReq.m_iTeamIndex;
            stValue.m_strName = "";
            stValue.m_stTeam = stReq.m_stTeam;
            auto stRet = m_pUserInfo->m_stUserTeamPresetMap.Insert(std::make_pair(stValue.m_iTeamIndex, stValue));
            if (stRet.second == false)
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERROR:" << INTERN_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                                    << "|TEAM_INDEX:" << stReq.m_iTeamIndex << std::endl;

                SEND_ERRCODE_AND_RET(INTERN_ERROR)
            }
        }
        else
        {
            iterData->second->SetTeam(stReq.m_stTeam);
        }

        stRsp.m_iTeamIndex = stReq.m_iTeamIndex;
        stRsp.m_stTeam = stReq.m_stTeam;
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return (true);
    }
    else if(m_usCmd == MSG_LOGIC_TOUHOU_CHANGE_TEAM_PRESET_NAME)
    {
        CRequestChangeTeamPresetName stReq;
        if (!DecodeMsgPack(stFormatData, stReq)) return false;

        if(stReq.m_iTeamIndex <= 0 || stReq.m_iTeamIndex > CLogicConfig::GetGlobalConfig().m_iTeamPresetNum)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        auto iErrorCode = CLogicCacheCommon::CheckAliasNickValid(stReq.m_strName);
        if(iErrorCode != SERVER_ERRCODE::SUCCESS)
        {
            SEND_ERRCODE_AND_RET(iErrorCode)
        }

        auto iterData = m_pUserInfo->m_stUserTeamPresetMap.Find(stReq.m_iTeamIndex);
        if(iterData == m_pUserInfo->m_stUserTeamPresetMap.End())
        {
            TUserTeamPresetInfoTableValueType stValue;
            stValue.m_iUid = m_iUin;
            stValue.m_iGroupID = m_iGroupID;
            stValue.m_iTeamIndex = stReq.m_iTeamIndex;
            stValue.m_strName = stReq.m_strName;
            stValue.m_stTeam = {};
            auto stRet = m_pUserInfo->m_stUserTeamPresetMap.Insert(std::make_pair(stValue.m_iTeamIndex, stValue));
            if (stRet.second == false)
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERROR:" << INTERN_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                                    << "|TEAM_INDEX:" << stReq.m_iTeamIndex << std::endl;

                SEND_ERRCODE_AND_RET(INTERN_ERROR)
            }
        }
        else
        {
            iterData->second->SetName(stReq.m_strName);
        }

        SendToClient(SERVER_ERRCODE::SUCCESS);
        return true;
    }
	else if(m_usCmd == MSG_LOGIC_TOUHOU_HERO_HEART_GIFTS)
    {// 少女好感度升级
        CRequestHeroHeartGifts stReq;
        CResponseHeroHeartGifts stRsp;
        if(DecodeMsgPack(stFormatData, stReq) == false) return false;

        auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(stReq.m_iHeroCardID);
        if(iterCard == m_pUserInfo->m_stHeroCardTableMap.End() || !stCacheCardOP.CheckHeroCardExist(iterCard->second))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED);
        }

        int32_t iCurLevel = iterCard->second->GetHeartLevel();
        int32_t iCurExp = iterCard->second->GetHeartExp();
        int32_t iGiftLimitNum = CLogicConfig::GetHeroHeartConfig().GetHeartGiftLimitNumByHeartLevel(iCurLevel);

        if(iCurLevel > int(CLogicConfig::GetHeroHeartConfig().m_stLevelList.size() - 1))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HAVE_MAX_LEVEL);
        }

        // 先更新每日送礼物次数
        if(!is_same_day(CLogicCommon::GetSec(), iterCard->second->GetLastGiftTime()))
        {
            iterCard->second->SetLastGiftTime(CLogicCommon::GetSec());
            iterCard->second->SetDailyGiftNum(0);
        }

        if(iterCard->second->GetDailyGiftNum() >= iGiftLimitNum || stReq.m_stGiftsMap.empty())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NUMBER_INVALID);
        }

        const auto pCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stReq.m_iHeroCardID);
        if (!pCardConfig)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
        }

        int32_t iLastGiftNum = iGiftLimitNum - iterCard->second->GetDailyGiftNum();
        int32_t iTotoleAddExp = 0;

        std::string strExpCardLog = "{";
        char szBuffer[128];
        std::vector<CPackGameItem> stItemList;
        int32_t iGiftTotalNum = 0;
        for(auto item : stReq.m_stGiftsMap)
        {
            if(item.second <= 0 || item.second > iLastGiftNum || iGiftTotalNum > 100)
            {
                break;
            }

            iLastGiftNum -= item.second;
            iGiftTotalNum += item.second;
            stRsp.m_stGiftsMap.insert(std::make_pair(item.first, item.second));
            auto pGiftConfig = CLogicConfig::GetHeroHeartConfig().GetGiftItemInfo(item.first);
            if(!pGiftConfig)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
            }

            for(int i = 0; i < item.second; ++i)
            {
                int32_t iAddExp = pGiftConfig->m_iHeartExp;
                int32_t iCriVal = 1;
                stRsp.m_stExpMap.emplace(iAddExp, iCriVal);
                iTotoleAddExp += iAddExp;
            }

            CPackGameItem stItem;
            stItem.m_iItemType = CLogicConfigDefine::EXCHANGE_ITEM;
            stItem.m_iCardID = item.first;
            stItem.m_iNum = item.second;
            stItemList.push_back(stItem);

            snprintf(szBuffer, sizeof(szBuffer), "[heart_item_id:%d,heart_item_amount:%d]", item.first, item.second);
            strExpCardLog += szBuffer;
        }

        strExpCardLog += "}";

        // 消耗物品
        if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stItemList, m_usCmd))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        // 升级
        int32_t iNewLevel = iCurLevel;
        int32_t iNewExp = iCurExp + iTotoleAddExp;
        CLogicConfig::GetHeroHeartConfig().ReCalcHeartLevelExp(iNewLevel, iNewExp);
        iterCard->second->SetHeartLevel(iNewLevel);
        iterCard->second->SetHeartExp(iNewExp);
        iterCard->second->SetDailyGiftNum(iGiftLimitNum - iLastGiftNum);

        // 好感度升级奖励
        for(int i = iterCard->second->GetHeartGiftLevel() + 1; i <= iNewLevel; ++i)
        {
            auto iterLevelUpAwardConfig = CLogicConfig::GetHeroHeartConfig().m_stLevelAward.find(i);
            if(iterLevelUpAwardConfig == CLogicConfig::GetHeroHeartConfig().m_stLevelAward.end())
                continue;

            auto& stAward = iterLevelUpAwardConfig->second.m_stLevelUpAward;
            if(!stAward.empty())
            {
                stRsp.m_stReward.insert(stRsp.m_stReward.end(), stAward.begin(), stAward.end());
            }
        }
        iterCard->second->SetHeartGiftLevel(iNewLevel);

        // 总好感度等级
        int32_t iTotalCurLevel = m_pUserInfo->m_stIBusyData.GetTotalHeartLevel();
        int32_t iTotalCurExp = m_pUserInfo->m_stIBusyData.GetTotalHeartExp();
        iTotalCurExp += iTotoleAddExp;
        CLogicConfig::GetHeroHeartConfig().ReCalcMainCardHeartLevelExp(iTotalCurLevel, iTotalCurExp);
        m_pUserInfo->m_stIBusyData.SetTotalHeartLevel(iTotalCurLevel);
        m_pUserInfo->m_stIBusyData.SetTotalHeartExp(iTotalCurExp);

        std::string strAddItems;
        if(!stRsp.m_stReward.empty())
        {
            CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stReward, strAddItems, m_usCmd);
        }

        stRsp.m_iHeroCardID = stReq.m_iHeroCardID;
        stRsp.m_iHeartLevel = iNewLevel;
        stRsp.m_iHeartExp   = iNewExp;
        stRsp.m_iLastGiftTime   = iterCard->second->GetLastGiftTime();
        stRsp.m_iDailyGiftNum   = iterCard->second->GetDailyGiftNum();
        stRsp.m_iTotalHeartLevel = iTotalCurLevel;
        stRsp.m_iTotalHeartExp   = iTotalCurExp;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        if(iCurLevel != iNewLevel)
        {
            // 更新伙伴传记
            CLogicGlobal::UpdateHeroHistory(m_pUserInfo, stReq.m_iHeroCardID);

            CLogicDailyTaskSystem::SetProgress(m_pUserInfo, CLogicDailyTaskSystem::HERO_HEART_LEVEL, stReq.m_iHeroCardID, iNewLevel);
            CLogicDailyTaskSystem::NotifyTask(m_pUserInfo, CLogicDailyTaskSystem::UPGRADE_CARD_HEART_LEVEL, 0, iNewLevel - iCurLevel);

            // 好感度基金
            CLogicActiveBase::Notify(m_pUserInfo, CLogicConfigDefine::ACTIVITY_TYPE_HEART_INSURE, 1);

            // 通用事件 好感度等级
            CLogicEventData stEventHeartData;
            stEventHeartData.m_eType = CLogicConfigDefine::ECET_CardHeartLevel;
            stEventHeartData.m_iPara = iCurLevel;
            stEventHeartData.m_iParaEx = iNewLevel;
            stEventHeartData.m_iCmd = m_usCmd;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventHeartData);
        }

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|HERO_CARD_ID:" << stReq.m_iHeroCardID << "|HEART_GIFT_LIST:" << strExpCardLog << "|ADD_EXP_AMOUNT:" << iTotoleAddExp
                            << "|BEFORE_HERO_HEART_LEVEL:" << iCurLevel << "|BEFORE_HERO_HEART_EXP:" << iCurExp << "|AFTER_HERO_HEART_LEVEL:" << iNewLevel
                            << "|AFTER_HERO_HEART_EXP:" << iNewExp << "|REWARD:" << strAddItems << std::endl;
        return true;
    }
    else if(m_usCmd == MSG_LOGIC_UPGRADE_CARD_TALENT)
    {
        CRequestUpgradeCardTalent stReq;
        CResponseUpgradeCardTalent stRsp;
        if(!DecodeMsgPack(stFormatData, stReq)) return (false);

        if((stReq.m_iSelfPointNum < 0 && stReq.m_iSelfPointNum1 < 0)|| stReq.m_iCommonPointNum < 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(stReq.m_iCardID);
        if(iterCard == m_pUserInfo->m_stHeroCardTableMap.End())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED)
        }

        auto pConfig = CLogicConfig::GetHeroCardConfig().GetTalentConfig(stReq.m_iCardID, stReq.m_iTalentID);
        if(nullptr == pConfig)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
        }

        if((stReq.m_iSelfPointNum + stReq.m_iCommonPointNum != pConfig->m_iConsumePoint)
            && (stReq.m_iSelfPointNum1 + stReq.m_iCommonPointNum != pConfig->m_iConsumePoint1))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        if(stReq.m_iSelfPointNum > iterCard->second->GetTalentPoint() || stReq.m_iSelfPointNum1 > iterCard->second->GetTalentPoint1())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TALENT_SELF_POINT_NOT_ENOUGH)
        }

        if(stReq.m_iCommonPointNum > 0 && ((iterCard->second->GetTalentPoint() > stReq.m_iSelfPointNum && stReq.m_iSelfPointNum != 0)
            || (iterCard->second->GetTalentPoint1() > stReq.m_iSelfPointNum1 && stReq.m_iSelfPointNum1 != 0)))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        auto stData = iterCard->second->GetTalentMap();
        if(stData[stReq.m_iTalentID] <= 0)
        {
            // 判断当前投入
            if(pConfig->m_iNeedPoint > iterCard->second->GetTalentTotalConsume())
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TALENT_NEED_POINT_NOT_ENOUGH)
            }

            // 前置天赋判断
            if(pConfig->m_iPreID1 > 0 && stData[pConfig->m_iPreID1] <= 0)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TALENT_PRE_NOT_OPEN)
            }

            if(pConfig->m_iPreID2 > 0 && stData[pConfig->m_iPreID2] <= 0)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TALENT_PRE_NOT_OPEN)
            }
        }
        else
        {
            if(stData[stReq.m_iTalentID] >= pConfig->m_iMaxLevel)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MAX_LEVEL_ERROR)
            }
        }

        if(stReq.m_iCommonPointNum > 0)
        {
            auto stItem = CLogicConfig::GetUpgradeHeroCardColorConfig().m_stCommonTalent;
            stItem.m_iNum = stReq.m_iCommonPointNum;
            if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stItem, m_usCmd))
            {
                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }
        }

        iterCard->second->AddTalentPoint(-stReq.m_iSelfPointNum);
        iterCard->second->AddTalentPoint1(-stReq.m_iSelfPointNum1);

        ++(stData[stReq.m_iTalentID]);
        iterCard->second->SetTalentMap(stData);
        iterCard->second->AddTalentTotalConsume(pConfig->m_iConsumePoint + pConfig->m_iConsumePoint1);

        stRsp.m_iCardID = stReq.m_iCardID;
        stRsp.m_iTalentID = stReq.m_iTalentID;
        stRsp.m_iSelfPointNum = stReq.m_iSelfPointNum;
        stRsp.m_iSelfPointNum1 = stReq.m_iSelfPointNum1;
        stRsp.m_iCommonPointNum = stReq.m_iCommonPointNum;

        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_UseTalentPoint;
        stEventData.m_iCount = pConfig->m_iConsumePoint + pConfig->m_iConsumePoint1;
        stEventData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

        SendToClient(stRsp,SERVER_ERRCODE::SUCCESS);
        return true;
    }
	else
    {
        CRequestHeroCardOp stParam;
        if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

        if (stParam.m_iCardOpCount < 1) stParam.m_iCardOpCount = 1;
        if (stParam.m_iCardOpCount > 99) stParam.m_iCardOpCount = 99;

        if((int)m_usCmd == MSG_LOGIC_COMBINE_CARD_SOUL)
        {
            if (stCacheCardOP.CombineCard(stParam.m_iHeroCardID, m_usCmd))
            {
                SendSuccessCacheItemToClient();

				//公告
				{
					auto pConf = CLogicConfig::GetHeroCardConfig().GetConfig(stParam.m_iHeroCardID);
					if (pConf != nullptr)
					{
						CLogicGlobal::PublishBroadcast("combine_card", stParam.m_iHeroCardID, {
								{ "nick", m_pUserInfo->m_strNick },
								{ "star", CLogicConfig::GetUpgradeHeroCardStarConfig().GetGradeName(pConf->m_iInitGrade)},
								{ "card_name", pConf->m_strHeroCardName },
						});
					}
				}
                
                PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|HERO_CARD_ID:" << stParam.m_iHeroCardID << std::endl;
            }
            else
            {
                PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                    << "|MSG:" << CLogicError::GetErrMsg()
                                    << "|HERO_CARD_ID:" << stParam.m_iHeroCardID << std::endl;
        
                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }
        }
        else if((int)m_usCmd == MSG_LOGIC_UPGRADE_CARD_COLOR)
        {
            if(stCacheCardOP.UpgradeCardColor(stParam.m_iHeroCardID, m_usCmd))
            {
                auto stIter = m_pUserInfo->m_stHeroCardTableMap.Find(stParam.m_iHeroCardID);
                CResponseUpgradeHeroCardColor stRsp;
                stRsp.m_iHeroCardID = stParam.m_iHeroCardID;
                stRsp.m_iHeroCardColor = stIter->second != nullptr ? stIter->second->GetCardColor() : 0;
                stRsp.m_iHeroTalentPoint = stIter->second != nullptr ? stIter->second->GetTalentPoint() : 0;
                stRsp.m_iHeroTalentPoint1 = stIter->second != nullptr ? stIter->second->GetTalentPoint1(): 0;
                SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
                
                PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|HERO_CARD_ID:" << stParam.m_iHeroCardID 
                                    << "|BEFORE_HERO_CARD_COLOR:" << stRsp.m_iHeroCardColor - 1 
                                    << "|AFTER_HERO_CARD_COLOR:" << stRsp.m_iHeroCardColor 
                                    << std::endl;
            }
            else
            {
                PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                    << "|MSG:" << CLogicError::GetErrMsg()
                                    << "|HERO_CARD_ID:" << stParam.m_iHeroCardID << std::endl;
        
                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }
        }
        else if((int)m_usCmd == MSG_LOGIC_UPGRADE_CARD_STAR)
        {
            const auto* pstCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stParam.m_iHeroCardID);
            if (nullptr == pstCardConfig)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED);
            }

            auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(stParam.m_iHeroCardID);
            if(iterCard == m_pUserInfo->m_stHeroCardTableMap.End())
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED);
            }

            if(stCacheCardOP.UpgradeCardStar(stParam.m_iHeroCardID, m_usCmd))
            {
                CResponseUpgradeHeroCardStar stRsp;
                stRsp.m_iHeroCardID = stParam.m_iHeroCardID;
                stRsp.m_iHeroCardStar = iterCard->second->GetCardStar();
                stRsp.m_stCacheItem = m_pUserInfo->m_stCacheBonusData;

				CLogicGlobal::PublishBroadcast("upgrade_card_star", stRsp.m_iHeroCardStar, {
					{ "nick", m_pUserInfo->m_strNick },
					{ "card_id", std::to_string(stRsp.m_iHeroCardID) },
					{ "card_name", CLogicConfig::GetItemName(CLogicConfigDefine::HERO_CARD, stRsp.m_iHeroCardID) },
					{ "card_star", std::to_string(stRsp.m_iHeroCardStar) }
				});

				CLogicDailyTaskSystem::SetProgress(m_pUserInfo, CLogicDailyTaskSystem::HERO_STAR, stParam.m_iHeroCardID, stRsp.m_iHeroCardStar);

                SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

                // 更新伙伴传记
                CLogicGlobal::UpdateHeroHistory(m_pUserInfo, stParam.m_iHeroCardID);
                
                PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|HERO_CARD_ID:" << stParam.m_iHeroCardID 
                                    << "|BEFORE_HERO_CARD_STAR:" << stRsp.m_iHeroCardStar - 1 
                                    << "|AFTER_HERO_CARD_STAR:" << stRsp.m_iHeroCardStar
                                    << std::endl;

                // 通用事件
                CLogicEventData stEventData;
                stEventData.m_eType = CLogicConfigDefine::ECET_UpgradeCardStar;
                stEventData.m_iPara = iterCard->second->GetCardStar();
                stEventData.m_iCmd = m_usCmd;
                LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

                // 通用事件
                CLogicEventData stTargetEventData;
                stTargetEventData.m_eType = CLogicConfigDefine::ECET_UpStarsHeroCard;
                stTargetEventData.m_iPara = stParam.m_iHeroCardID ;
                stTargetEventData.m_iParaEx = iterCard->second->GetCardStar();
                stTargetEventData.m_iCmd = m_usCmd;
                LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stTargetEventData);
            }
            else
            {
                PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                    << "|MSG:" << CLogicError::GetErrMsg()
                                    << "|HERO_CARD_ID:" << stParam.m_iHeroCardID << std::endl;
        
                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }
        }
    }

    return (true);
}

