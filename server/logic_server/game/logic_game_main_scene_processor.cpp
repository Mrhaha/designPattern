#include "common_datetime.h"
#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "server_user_protocol.h"
#include "server_award_protocol.h"
#include "server_item_protocol.h"
#include "server_fight_protocol.h"
#include "server_role_protocol.h"
#include "server_cross_protocol.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "logic_game_active_info.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"
#include "logic_game_multi_step_task.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_main_scene_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_global.h"
#include "logic_game_attribute.h"
#include "logic_game_active.h"
#include "logic_game_cache_table.h"
#include "server_guild_protocol.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_USER_INFO, CLogicMainSceneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_RECORD_USER_GUIDE, CLogicMainSceneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_REPORT_GUIDE_STEP, CLogicMainSceneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_RECORD_USER_SPECIAL_GUIDE, CLogicMainSceneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_FIRST_DEPOSIT_BONUS, CLogicMainSceneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_GET_ADDUP_DEPOSIT_BONUS, CLogicMainSceneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_DAILY_DEPOSIT_BONUS, CLogicMainSceneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_UPDATE_USER_AGE, CLogicMainSceneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_CARD_DIALOG, CLogicMainSceneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_WENWEN_NEWS, CLogicMainSceneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_DAILY_SIGN, CLogicMainSceneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_WENWEN_CHOOSE_REWARD, CLogicMainSceneProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_CONTROL_INFO, CLogicMainSceneProcessor)






CLogicMainSceneProcessor::CLogicMainSceneProcessor(uint16_t usCmd, const std::string& strCmdName)
	: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool 
CLogicMainSceneProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    if((int)m_usCmd == MSG_LOGIC_GET_USER_INFO)
    {
        CPackUserStatInfo stParam;

        for(auto i : CLogicConfig::GetZooLogicGameConfig().m_iSwitchControl)
        {
            PROCESSOR_LOG_DEBUG << "ControlInfo: " << i << std::endl;
        }


		if (DecodeMsgPack(stFormatData, stParam) == false) return false;

		auto antiChekErrorCode = CLogicGlobal::AntiAddCheckCanLogin(m_iUin, m_iGroupID, m_pUserInfo->m_stUserInfoTable.GetAge());
		if(antiChekErrorCode != SERVER_ERRCODE::SUCCESS)
        {
            PROCESSOR_LOG_DEBUG << "ANTI_ADDICTION:AntiAddCheckCanLogin Prevent Login! Code:" << antiChekErrorCode
                                << "|MSG:User Age:" << m_pUserInfo->m_stUserInfoTable.GetAge() << std::endl;

            if(antiChekErrorCode == SERVER_ERRCODE::ANTI_ADDICTION_LOGIN_TIME_LIMIT)
                return false;

            SEND_ERRCODE_AND_RET(antiChekErrorCode);
        }

        CLogicCacheCardOperation stCacheCardOP(m_pUserInfo);
        CLogicCacheCommon::AutoAddPhyPower(m_pUserInfo);
        CLogicCacheCommon::AutoAddVitality(m_pUserInfo);

        int iLastLoginTime = m_pUserInfo->m_stUserInfoTable.GetLastLogin();
        if (LOGIC_CHECK_DAY_REFRESH(iLastLoginTime))
        {
            m_pUserInfo->m_stUserInfoTable.AddAddUpLoginDay(1);

            CLogicActiveBase::Notify(m_pUserInfo, CLogicConfigDefine::ACTIVITY_TYPE_DEADLINE_LOGIN, 1);

			      CLogicActiveBase::Notify(m_pUserInfo, CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_INSURE, 1);

            CLogicActiveBase::Notify(m_pUserInfo, CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND, 1);

        }

		CLogicGlobal::AddAsyncNpcMail(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);

		CLogicDailyTaskSystem::NotifyTask(m_pUserInfo, CLogicDailyTaskSystem::LOGIN_TASK, 1, 1);
        
        m_pUserInfo->m_stUserInfoTable.SetLastLogin(CLogicCommon::GetSec());
		if (!m_pUserInfo->m_stFirstRechargeMap.Empty())
		{
			m_pUserInfo->m_stFirstRechargeMap.Begin()->second->SetLastLoginTime(CLogicCommon::GetSec());
		}

        CResponseGetUserData stProto;

        stProto.m_stUserBaseInfo.m_iUin = m_iUin;    
        stProto.m_stUserBaseInfo.m_iGroupID = m_iGroupID;
        stProto.m_stUserBaseInfo.m_strNick = m_pUserInfo->m_strNick;
        stProto.m_stUserBaseInfo.m_iVipLevel = m_pUserInfo->m_stUserInfoTable.GetVipLevel();
        stProto.m_stUserBaseInfo.m_iVipExp = m_pUserInfo->m_stUserInfoTable.GetVipExp();
        stProto.m_stUserBaseInfo.m_iMajorGuideStep = m_pUserInfo->m_stUserInfoTable.GetGuideMajorStep();
        stProto.m_stUserBaseInfo.m_iMinorGuideStep = m_pUserInfo->m_stUserInfoTable.GetGuideMinorStep();
        stProto.m_stUserBaseInfo.m_iLevel = m_pUserInfo->m_stUserInfoTable.GetLevel();
        stProto.m_stUserBaseInfo.m_iExp = m_pUserInfo->m_stUserInfoTable.GetExp();
        stProto.m_stUserBaseInfo.m_ulSpecialGuideStep = m_pUserInfo->m_stUserInfoTable.GetGuideSpecialStep();
        stProto.m_stUserBaseInfo.m_iAddupRmb = m_pUserInfo->m_stUserInfoTable.GetAddUpRmb();
        stProto.m_stUserBaseInfo.m_iLastLoginTime = m_pUserInfo->m_stUserInfoTable.GetLastLogin();
        stProto.m_stUserBaseInfo.m_iCreateRoleTime = m_pUserInfo->m_stUserInfoTable.GetCreateTime();
        stProto.m_stUserBaseInfo.m_iCurrentFightLevelID = m_pUserInfo->m_stUserInfoTable.GetFightLevelID();
        stProto.m_stUserBaseInfo.m_iLastGetScoreRankBonusTime = m_pUserInfo->m_stUserInfoTable.GetScoreRankBonusTime();
        stProto.m_stUserBaseInfo.m_iConstelExtraTimes = m_pUserInfo->m_stIBusyData.GetConstelAdvTimes();
        stProto.m_stUserBaseInfo.m_iLastGetCombatRankBonusTime = m_pUserInfo->m_stUserInfoTable.GetCombatRankBonusTime();
		stProto.m_stUserBaseInfo.m_iChangeNickCount = m_pUserInfo->m_stUserInfoTable.GetChangeNickCount();
        stProto.m_stUserBaseInfo.m_iThemeSkinID = m_pUserInfo->m_stUserInfoTable.GetThemeSkin();
        stProto.m_stUserBaseInfo.m_iRuneCardID = m_pUserInfo->m_stUserInfoTable.GetRuneCard();
        stProto.m_stUserBaseInfo.m_iLastSignTime = m_pUserInfo->m_stUserInfoTable.GetLastSignTime();
        stProto.m_stUserBaseInfo.m_iTotalSignTimes = m_pUserInfo->m_stUserInfoTable.GetTotalSign();

        stProto.m_stUserBaseInfoSection1.m_iAvatarBorderID = m_pUserInfo->m_stUserInfoTable.GetAvatarBorder();
        stProto.m_stUserBaseInfoSection1.m_iAvatarIconID = m_pUserInfo->m_stUserInfoTable.GetAvatarID();
        stProto.m_stUserBaseInfoSection1.m_iFirstDepositBonus = m_pUserInfo->m_stUserInfoTable.GetFirstDepositBonus();
        stProto.m_stUserBaseInfoSection1.m_strVipBagBitmap = int2bin(m_pUserInfo->m_stUserInfoTable.GetVipLevelBuyBonus());
        stProto.m_stUserBaseInfoSection1.m_iCrossFightResetTime = m_pUserInfo->m_stUserInfoTable.GetCrossFightResetTime();
        stProto.m_stUserBaseInfoSection1.m_iCrossFightLastRefreshResetTimesTime = m_pUserInfo->m_stUserInfoTable.GetCrossFightLastRefreshResetTimesTime();
        stProto.m_stUserBaseInfoSection1.m_strFightPowerRankStageBitmap = int2bin(m_pUserInfo->m_stUserInfoTable.GetFightPowerRankActive());
        stProto.m_stUserBaseInfoSection1.m_iAddUpLoginDay = m_pUserInfo->m_stUserInfoTable.GetAddUpLoginDay();
		stProto.m_stUserBaseInfoSection1.m_stCardDialogLimitAvyStat = m_pUserInfo->m_stIBusyData.GetCardDialogLimitAvyStat();
		stProto.m_stUserBaseInfoSection1.m_iCardDialogNextTime = m_pUserInfo->m_stIBusyData.GetCardDialogNextTime();
		stProto.m_stUserBaseInfoSection1.m_iCardDialogTimes = m_pUserInfo->m_stIBusyData.GetCardDialogTimes();
		stProto.m_stUserBaseInfoSection1.m_iAddUpDepositBonus = m_pUserInfo->m_stIBusyData.GetAddupDepositBonus();
		stProto.m_stUserBaseInfoSection1.m_iConstelExpPool = m_pUserInfo->m_stIBusyData.GetConstelExpPool();
        stProto.m_stUserBaseInfoSection1.m_stMainBgm = m_pUserInfo->m_stIBusyData.GetMainBgm();
        stProto.m_stUserBaseInfoSection1.m_ulOnceEventStat = m_pUserInfo->m_stIBusyData.GetOnceEventStat();

        if (LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stUserInfoTable.GetLastDepositTime()))
        {
            stProto.m_stUserBaseInfoSection1.m_iDailyDeposit = 0;
            stProto.m_stUserBaseInfoSection1.m_iDailyDepositBonus = 0;
        }
        else
        {
            stProto.m_stUserBaseInfoSection1.m_iDailyDeposit = m_pUserInfo->m_stUserInfoTable.GetDailyDeposit();
            stProto.m_stUserBaseInfoSection1.m_iDailyDepositBonus = m_pUserInfo->m_stUserInfoTable.GetDailyDepositBonus();
        }

        if (LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stUserInfoTable.GetCrossFightLastRefreshResetTimesTime()))
        {
            stProto.m_stUserBaseInfoSection1.m_iCrossFightRefreshResetTimesTimes = 0;
        }
        else
        {
            stProto.m_stUserBaseInfoSection1.m_iCrossFightRefreshResetTimesTimes = 
                m_pUserInfo->m_stUserInfoTable.GetCrossFightRefreshResetTimesTimes();
        }

        if (LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stUserInfoTable.GetCrossFightResetTime()))
        {
            stProto.m_stUserBaseInfoSection1.m_iCrossFightResetTimes = 0;
        }
        else
        {
            stProto.m_stUserBaseInfoSection1.m_iCrossFightResetTimes = 
                m_pUserInfo->m_stUserInfoTable.GetCrossFightResetTimes();
        }

        if(stProto.m_stUserBaseInfoSection1.m_iAvatarBorderID == 0)
            stProto.m_stUserBaseInfoSection1.m_iAvatarBorderID = CLogicConfig::GetAvatarConfig().m_iDefaultBorderID;
        if(stProto.m_stUserBaseInfoSection1.m_iAvatarIconID == 0)
            stProto.m_stUserBaseInfoSection1.m_iAvatarIconID = CLogicConfig::GetAvatarConfig().m_iDefaultIconID;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        stProto.m_stBuyResourceInfo.m_iBuyEnergyTimes = m_pUserInfo->m_stUserInfoTable.GetBuyEnergyTimes();
        stProto.m_stBuyResourceInfo.m_iBuyVitalityTimes = 0;
        stProto.m_stBuyResourceInfo.m_iBuyRefreshPVPTimes = 0;
        stProto.m_stBuyResourceInfo.m_iBuyGoldTimes = m_pUserInfo->m_stUserInfoTable.GetBuyGoldTimes();
        stProto.m_stBuyResourceInfo.m_iBuyGoldTotalTimes = m_pUserInfo->m_stUserInfoTable.GetBuyGoldTotalTimes();

        if (!LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stUserInfoTable.GetLastBuyVitalityTime()))
        {
            stProto.m_stBuyResourceInfo.m_iBuyVitalityTimes = m_pUserInfo->m_stUserInfoTable.GetBuyVitalityTimes();
        }
        if (!LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stUserInfoTable.GetLastRefreshPvpTime()))
        {
            stProto.m_stBuyResourceInfo.m_iBuyRefreshPVPTimes = m_pUserInfo->m_stUserInfoTable.GetRefreshPvpTimes();
        }

		stProto.m_iSerialPayAwarded = m_pUserInfo->m_stUserInfoTable.GetSerialPayAwarded();
		stProto.m_iOpenSvrTime = CLogicConfig::GetSystemConfig().GetOpenSvrTime();
		stProto.m_iMergeServerTime = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iMergeTime;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        stProto.m_stCurrencyInfo.m_iYuanBao = m_pUserInfo->m_stUserInfoTable.GetRmb();
        stProto.m_stCurrencyInfo.m_iGold = m_pUserInfo->m_stUserInfoTable.GetGold();
        stProto.m_stCurrencyInfo.m_iZhuHaiScore = m_pUserInfo->m_stUserInfoTable.GetZhuhaiScore();
        stProto.m_stCurrencyInfo.m_iEvilBrand = m_pUserInfo->m_stUserInfoTable.GetEvilBrand();
        stProto.m_stCurrencyInfo.m_iPayYuanBao = m_pUserInfo->m_stUserInfoTable.GetPayRmb();
        stProto.m_stCurrencyInfo.m_iExploreExp = m_pUserInfo->m_stIBusyData.GetExploreExp();
        stProto.m_stCurrencyInfo.m_iEnergy = m_pUserInfo->m_stUserInfoTable.GetPhyPower();
        stProto.m_stCurrencyInfo.m_iLastAutoAddEnergyTime = m_pUserInfo->m_stUserInfoTable.GetLastAutoAddTime();
        stProto.m_stCurrencyInfo.m_iEnergyStore = m_pUserInfo->m_stUserInfoTable.GetPhyStore();
        stProto.m_stCurrencyInfo.m_iVitality = m_pUserInfo->m_stUserInfoTable.GetVitality();
        stProto.m_stCurrencyInfo.m_iAddVitalityTime = m_pUserInfo->m_stUserInfoTable.GetAddVitalityTime();
		stProto.m_stCurrencyInfo.m_iGuildCoin = m_pUserInfo->m_stUserInfoTable.GetGuildCoin();
		stProto.m_stCurrencyInfo.m_iEntireWarContribution = m_pUserInfo->m_stUserInfoTable.GetEntireWarContribution();
		stProto.m_stCurrencyInfo.m_iEntireWarMilitarkLevel = m_pUserInfo->m_stUserInfoTable.GetEntireWarMilitarkLevel();
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		stProto.m_stClimbTowerInfo.m_iCurrentOrder = m_pUserInfo->m_stUserInfoTable.GetClimbTowerCurrentOrder();
		stProto.m_stClimbTowerInfo.m_iMaxOrder = m_pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder();
		if (!LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stUserInfoTable.GetLastClimbTowerResetTime()))
		{
			stProto.m_stClimbTowerInfo.m_iResetCount = m_pUserInfo->m_stUserInfoTable.GetClimbTowerResetCount();
		}
		stProto.m_stClimbTowerInfo.m_iSweepEndTime = m_pUserInfo->m_stUserInfoTable.GetClimbTowerSweepEndTime();
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for(int iType = CLogicConfigDefine::ETT_MainTeam; iType < CLogicConfigDefine::ETT_Max; ++iType)
        {
            stProto.m_stTeamVec.emplace_back(CLogicCacheCommon::GetTeam(m_pUserInfo, iType));
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for (auto stIter = m_pUserInfo->m_stItemMap.Begin(); stIter != m_pUserInfo->m_stItemMap.End(); ++stIter)
        {
            if(stIter->second->GetAmount() <= 0) continue;

            stProto.m_stItemNumMap.emplace(stIter->first, stIter->second->GetAmount());

            if (stIter->second->GetNextOpenTime() > 0 || stIter->second->GetOpenedCount() > 0)
            {
                CPackItemAttr attr;
                attr.m_iItemID = stIter->first;
                attr.m_iNextOpenTime = stIter->second->GetNextOpenTime();
                attr.m_iOpenedCount = stIter->second->GetOpenedCount();
                stProto.m_stItemAttrVec.push_back(attr);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for (auto stIter = m_pUserInfo->m_stHeroCardTableMap.Begin(); stIter != m_pUserInfo->m_stHeroCardTableMap.End(); ++stIter)
        {
            CPackHeroCard stHeroCardItem;
            stHeroCardItem.m_iHeroCardID = stIter->second->m_stData.m_iCardID;
            stHeroCardItem.m_iFashionID = stIter->second->GetFashionID();
            stHeroCardItem.m_iColor = stIter->second->GetCardColor();
            stHeroCardItem.m_iStar = stIter->second->GetCardStar();
            stHeroCardItem.m_iCreateTime = stIter->second->GetCardTime();
            stHeroCardItem.m_iLevel = stIter->second->GetCardLevel();
            stHeroCardItem.m_iExp = stIter->second->GetCardExp();

            stHeroCardItem.m_iTalentPoint1 = stIter->second->GetTalentPoint1();
            stHeroCardItem.m_iTalentPoint = stIter->second->GetTalentPoint();
            stHeroCardItem.m_iTalentTotalConsume = stIter->second->GetTalentTotalConsume();
            stHeroCardItem.m_stTalentMap = stIter->second->GetTalentMap();

            stHeroCardItem.m_iUpgradeColor1 = stIter->second->GetUpgradeColor1();
            stHeroCardItem.m_iUpgradeColor2 = stIter->second->GetUpgradeColor2();
            stHeroCardItem.m_iUpgradeColor3 = stIter->second->GetUpgradeColor3();
            stHeroCardItem.m_iUpgradeColor4 = stIter->second->GetUpgradeColor4();
            stHeroCardItem.m_iCrossFightCostHp = stIter->second->GetCrossFightCostHp();

			// 好感度
			stHeroCardItem.m_iHeartLevel    = stIter->second->GetHeartLevel();
			stHeroCardItem.m_iHeartExp      = stIter->second->GetHeartExp();
			stHeroCardItem.m_iDailyGiftNum  = stIter->second->GetDailyGiftNum();
			stHeroCardItem.m_iLastGiftTime  = stIter->second->GetLastGiftTime();
			stHeroCardItem.m_iGradeLevel    = stIter->second->GetGradeLevel();
			stHeroCardItem.m_stImpression   = stIter->second->GetImpression();

            // 星纹
            stHeroCardItem.m_iEquipConstellation1 = stIter->second->GetEquipConstellation1();
            stHeroCardItem.m_iEquipConstellation2 = stIter->second->GetEquipConstellation2();
            stHeroCardItem.m_iEquipConstellation3 = stIter->second->GetEquipConstellation3();
            stHeroCardItem.m_iEquipConstellation4 = stIter->second->GetEquipConstellation4();

            // 装备
            stHeroCardItem.m_iEquip1 = stIter->second->GetEquip1();
            stHeroCardItem.m_iEquip2 = stIter->second->GetEquip2();
            stHeroCardItem.m_iEquip3 = stIter->second->GetEquip3();
            stHeroCardItem.m_iEquip4 = stIter->second->GetEquip4();

            stProto.m_stHeroCardVec.emplace_back(stHeroCardItem);
        }

        /////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////

        CLogicGlobal::AddAsyncUserGlobalMail(m_iUin, m_iGroupID);
        
        int iNowSec = CLogicCommon::GetSec(); 
        auto stUserMailIterBeg = m_pUserInfo->m_stUserMailMap.Begin();
        auto stUserMailIterEnd = m_pUserInfo->m_stUserMailMap.End();
        for ( ; stUserMailIterBeg != stUserMailIterEnd; )
        {
            int iReadTime = stUserMailIterBeg->second->GetReadTime();

            //先处理过期邮件直接删除
            if (stUserMailIterBeg->second->GetExpiredTime() < iNowSec && stUserMailIterBeg->second->GetExpiredTime() > 0)
            {
                //删除设置过期时间的过期邮件
                m_pUserInfo->m_stUserMailMap.Erase(stUserMailIterBeg++);
                continue;
            }

            // 未读邮件不处理
            if(iReadTime == -1)
            {
                ++stProto.m_stUserBaseInfo.m_iUnReadMailAmount;
                ++stUserMailIterBeg;
                continue;
            }

            if((iReadTime + (CLogicConfigDefine::UNREAD_MAIL_MAX_REMAIN_TIME * 24 * 60 * 60)) < iNowSec)
            {
                // bill record
                if(!stUserMailIterBeg->second->GetAttachment().empty())
                {
                    CLogicBill::SaveMailBill(m_pUserInfo->m_iChannelID, CLogicBill::ESMBT_TIMEOUT_DELETE
                            , m_strCmdName, stUserMailIterBeg->second->m_stData, stUserMailIterBeg->second->GetAttachment());
                }

                m_pUserInfo->m_stUserMailMap.Erase(stUserMailIterBeg++);
            }
            else if ((iReadTime + (CLogicConfigDefine::READ_MAIL_SAVE_TIME * 24 * 60 * 60)) < iNowSec && stUserMailIterBeg->second->GetAttachment().empty())
            {
                // 没有附件的已读邮件，3天后删除
                m_pUserInfo->m_stUserMailMap.Erase(stUserMailIterBeg++);
            }
            else if ((iReadTime + (CLogicConfigDefine::READ_ITEM_MAIL_SAVE_TIME * 24 * 60 * 60)) < iNowSec && stUserMailIterBeg->second->GetIsReceived())
            {
                // 已经领取附件的已读邮件，3天后删除
                m_pUserInfo->m_stUserMailMap.Erase(stUserMailIterBeg++);
            }
            else
            {
                ++stUserMailIterBeg;
            }
        }
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int iCompletedTaskAmount = 0;
        iCompletedTaskAmount += CLogicMultiStepTaskBase::GetCompletedMutiTaskAmount(m_pUserInfo);

        stProto.m_stUserBaseInfo.m_iCompleteTaskNum = iCompletedTaskAmount;
        stProto.m_stUserBaseInfo.m_iTotalHeartLevel = m_pUserInfo->m_stIBusyData.GetTotalHeartLevel();
        stProto.m_stUserBaseInfo.m_iTotalHeartExp = m_pUserInfo->m_stIBusyData.GetTotalHeartExp();
        stProto.m_iTimestamp = CLogicCommon::GetSec();
        stProto.m_iOpenServerDay = CLogicConfig::GetSystemConfig().GetOpenSvrDay();
        stProto.m_iRefreshDayHour = 0;

		for (auto iter = m_pUserInfo->m_stFashionInfoMap.Begin(); iter != m_pUserInfo->m_stFashionInfoMap.End(); ++iter)
		{
			if (iter->second->GetExpiredTime() > 0 && iter->second->GetExpiredTime() < CLogicCommon::GetSec())
			{
                continue;
			}

			stProto.m_stFashionVec.emplace_back(CLogicCacheCommon::GetFashionInfo(iter->second->m_stData));
		}

		//称号
		for (auto iterHonor = m_pUserInfo->m_stUserHonorMap.Begin(); iterHonor != m_pUserInfo->m_stUserHonorMap.End(); ++iterHonor)
		{
			stProto.m_stHonorInfoVec.emplace_back(CLogicCacheCommon::GetHonorInfo(iterHonor->second->m_stData));
		}

        // 星纹
        for(auto iterConstell = m_pUserInfo->m_stConstelMap.Begin(); iterConstell != m_pUserInfo->m_stConstelMap.End(); ++iterConstell)
        {
            stProto.m_stConstellationVec.emplace_back(CLogicCacheCommon::GetConstellationInfo(iterConstell->second->m_stData));
        }

        // 装备
        for(auto iterEquip = m_pUserInfo->m_stEquipMap.Begin(); iterEquip != m_pUserInfo->m_stEquipMap.End(); ++iterEquip)
        {
            stProto.m_stEquipVec.emplace_back(CLogicCacheCommon::GetEquipInfo(iterEquip->second->m_stData));
        }

        //每月充值金额
        stProto.m_iMonthAddUp = m_pUserInfo->m_stUserInfoTable.GetMonthAddUpRmb();
        stProto.m_iAgeLimitIndex = m_pUserInfo->m_stUserInfoTable.GetMonthAgeLimit();
        stProto.m_iLastChangeAgeLimit = m_pUserInfo->m_stUserInfoTable.GetLastMonthAgeChange();

		CLogicCacheCommon::ResultMatchPVPScore(m_pUserInfo, false);
		CLogicCacheCommon::ResultFairPVPScore(m_pUserInfo, false);

        m_stUserConnectorData.m_stStatInfo.m_strAccount = stParam.m_strAccount;
        m_stUserConnectorData.m_stStatInfo.m_strDeviceCarrier = stParam.m_strDeviceCarrier;
        m_stUserConnectorData.m_stStatInfo.m_strDeviceID = stParam.m_strDeviceID;
        m_stUserConnectorData.m_stStatInfo.m_strDeviceModel = stParam.m_strDeviceModel;
        m_stUserConnectorData.m_stStatInfo.m_strDeviceNet = stParam.m_strDeviceNet;
        m_stUserConnectorData.m_stStatInfo.m_strDeviceOS = stParam.m_strDeviceOS;
        m_stUserConnectorData.m_stStatInfo.m_strDeviceRatio = stParam.m_strDeviceRatio;
        m_stUserConnectorData.m_stStatInfo.m_strPlatformID = stParam.m_strPlatformID;
        m_stUserConnectorData.m_stStatInfo.m_strPlatformSign = stParam.m_strPlatformSign;
		m_stUserConnectorData.m_iResVersion = stParam.m_iResVer;
        m_stUserConnectorData.m_iChannelID = m_iChannelID;

        CLogicUser::GetInstance().SetUserInfo(m_iUin, m_stUserConnectorData);

		CLogicDailyTaskSystem::SetProgress(m_pUserInfo, CLogicDailyTaskSystem::TEAM_LEVEL, 0, m_pUserInfo->m_stUserInfoTable.GetLevel());

        m_pUserInfo->m_strPid = stParam.m_strAccount;
        m_pUserInfo->m_strDeviceID = stParam.m_strDeviceID;

        SendToClient(stProto, SERVER_ERRCODE::SUCCESS);

		//////////////////////////////////////////////////////////////////////////////////////////////////
		/////通知客户端玩家其他信息
		CLogicUserNotifySystem::NotifyUserDataToClient(m_pUserInfo);
        // 不重要的数据，只在上线时通知一次
        CLogicUserNotifySystem::NotifyUserOtherData(m_pUserInfo);
        // 任务数据
        CLogicUserNotifySystem::NotifyQuestData(m_pUserInfo, {});
        // 徽章数据
        CLogicUserNotifySystem::NotifyBadgeData(m_pUserInfo, {});

        // 检查问卷调查
        CLogicGlobal::UpdateQuestionnaire(m_pUserInfo, true);

		/////////////////////////////////////////////////////////////////////////////////////////////////
		////更新匹配库数据
		if (m_pUserInfo->m_stUserInfoTable.GetLevel() >= CLogicConfig::GetMatchPvpConfig().m_iMatchMinLevel)
		{
			CSRequestUpdateMatchUser stCrossBody;
			stCrossBody.m_stUserVec.emplace_back(CLogicCacheCommon::GetUserCrossInfo(m_pUserInfo));
			SendToCrossServer(enmCmdType_request, MSG_CROSS_UPDATE_MATCH_USER, stCrossBody);
		}

        ////拉取跨服队伍ID
        std::string strEmpty;
        SendToCrossServer(enmCmdType_request, MSG_CROSS_BANQUET_UPDATE_LOGIN, strEmpty);

        ////更新限时boss状态
        std::string strEmptyBoss;
        SendToCrossServer(enmCmdType_request, MSG_CROSS_NEW_LIMIT_BOSS_GET_INFO, strEmptyBoss);

        ////获取最近一次公会战排名
        CRequestGuildWarSelfRankInfo stReq;
        stReq.selfRankInfo = m_pUserInfo->m_stIBusyData.GetGuildWarCalInfo();
        if (stReq.selfRankInfo.m_iRankID != 0)
        {
            SendToCrossServer(enmCmdType_request, MSG_CROSS_GUILD_WAR_GET_SELF_RANK, stReq);
        }

        ////检测数据转换
        CLogicUserNotifySystem::RefreshDataChange(m_pUserInfo);
		// 防沉迷
        CLogicGlobal::AntiAddNotifyUpdateUserAge(m_iUin, m_iGroupID);
		CLogicGlobal::AntiAddCheckUser(m_iUin, m_iGroupID, m_pUserInfo->m_stUserInfoTable.GetAge());

        PROCESSOR_LOG_DEBUG << "GROUP_ID:" << m_iGroupID 
                            << "|UIN:" << m_iUin 
                            << "|REMOTE_HOST:" << m_stUserConnectorData.m_strClientIP 
                            << ":" << m_stUserConnectorData.m_usClientPort 
                            << "|ACCOUNT:" << m_stUserConnectorData.m_stStatInfo.m_strAccount
                            << "|DEVICE_ID:" << m_stUserConnectorData.m_stStatInfo.m_strDeviceID 
                            << "|DEVICE_MODEL:" << m_stUserConnectorData.m_stStatInfo.m_strDeviceModel
                            << "|DEVICE_OS:" << m_stUserConnectorData.m_stStatInfo.m_strDeviceOS 
                            << "|DEVICE_RATIO:" << m_stUserConnectorData.m_stStatInfo.m_strDeviceRatio
                            << "|DEVICE_NET:" << m_stUserConnectorData.m_stStatInfo.m_strDeviceNet 
                            << "|DEVICE_CARRIER:" << m_stUserConnectorData.m_stStatInfo.m_strDeviceCarrier
                            << "|PLATFORM_ID:" << m_stUserConnectorData.m_stStatInfo.m_strPlatformID 
                            << "|ROLE_LEVEL:" << m_pUserInfo->m_stUserInfoTable.GetLevel()
                            << "|NICK:" << m_pUserInfo->m_strNick
                            << "|LOGIN SUCCESS";
    }
	else if((int)m_usCmd == MSG_LOGIC_RECORD_USER_GUIDE)
    {
        CResponseRecordUserGuide stRsp;
        CRequestRecordUserGuide stParam;

        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);

		if (stParam.m_iMajorStep >= m_pUserInfo->m_stUserInfoTable.GetGuideMajorStep())
		{
			m_pUserInfo->m_stUserInfoTable.SetGuideMajorStep(stParam.m_iMajorStep);
			m_pUserInfo->m_stUserInfoTable.SetGuideMinorStep(stParam.m_iMinorStep);
		}

        stRsp.m_iMajorStep = stParam.m_iMajorStep;
        stRsp.m_iMinorStep = stParam.m_iMinorStep;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|MAJOR_STEP:" << stParam.m_iMajorStep << "|MINOR_STEP:"
                            << stParam.m_iMinorStep  << std::endl;
    }
    else if((int)m_usCmd == MSG_LOGIC_GET_FIRST_DEPOSIT_BONUS)
    {
        if (m_pUserInfo->m_stUserInfoTable.GetAddUpRmb() > 0)
        {
            if (m_pUserInfo->m_stUserInfoTable.GetFirstDepositBonus() == 0)
            {
                CResponseGetFirstDepositBonus stRsp;

                stRsp.m_stBonusVec = CLogicConfig::GetFirstDepositConfig().m_stBonusVec;

                static std::string strBonusItem;

                CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stBonusVec, strBonusItem, m_usCmd);

                m_pUserInfo->m_stUserInfoTable.SetFirstDepositBonus(1);
                m_pUserInfo->m_stUserInfoTable.SetDailyDepositBonus(1);

                SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

                PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|BONUS_ITEM:" << strBonusItem << std::endl;

                return true;
            }
            else
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << HAS_RECEIVED_FIRST_DEPOSTI_BONUS 
                                    << "|MSG:" << GET_ERRMSG_BY_ERRCODE(HAS_RECEIVED_FIRST_DEPOSTI_BONUS)
                                    << std::endl;

                SEND_ERRCODE_AND_RET(HAS_RECEIVED_FIRST_DEPOSTI_BONUS);
            }
        }
        else
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_ADD_UP_RMB
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_ADD_UP_RMB)
                                << std::endl;

            SEND_ERRCODE_AND_RET(NOT_ENOUGH_ADD_UP_RMB);
        }
    }
    else if((int)m_usCmd == MSG_LOGIC_TOUHOU_GET_ADDUP_DEPOSIT_BONUS)
    {
        if(m_pUserInfo->m_stIBusyData.GetAddupDepositBonus() >= (int)CLogicConfig::GetFirstDepositConfig().m_stAddUpDepositBonus.size())
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << AWARD_HAVE_RECEIVED << "|MSG:"
                                << GET_ERRMSG_BY_ERRCODE(AWARD_HAVE_RECEIVED) << std::endl;

            SEND_ERRCODE_AND_RET(AWARD_HAVE_RECEIVED);
        }

        const auto& config = CLogicConfig::GetFirstDepositConfig().m_stAddUpDepositBonus[m_pUserInfo->m_stIBusyData.GetAddupDepositBonus()];
        if(config.m_iDepositNum > m_pUserInfo->m_stUserInfoTable.GetAddUpRmb())
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_ADD_UP_RMB << "|MSG:"
                                << GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_ADD_UP_RMB) << std::endl;

            SEND_ERRCODE_AND_RET(NOT_ENOUGH_ADD_UP_RMB);
        }

        CResponseGetAddUpDepositBonus stRsp;
        stRsp.m_stBonusVec = config.m_stBonus;

        static std::string strBonusItem;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stBonusVec, strBonusItem, m_usCmd);

        m_pUserInfo->m_stIBusyData.AddAddupDepositBonus(1);

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|BONUS_ITEM:" << strBonusItem << std::endl;

        return true;
    }
    else if((int)m_usCmd == MSG_LOGIC_GET_DAILY_DEPOSIT_BONUS)
    {
        if (m_pUserInfo->m_stUserInfoTable.GetDailyDepositBonus() == 0)
        {
            int iDailyDeposit = m_pUserInfo->m_stUserInfoTable.GetDailyDeposit();
            int iLastDepositTime = m_pUserInfo->m_stUserInfoTable.GetLastDepositTime();
            if (LOGIC_CHECK_DAY_REFRESH(iLastDepositTime)) iDailyDeposit = 0;

            if (iDailyDeposit > 0)
            {
                CResponseGetFirstDepositBonus stRsp;

                stRsp.m_stBonusVec = CLogicConfig::GetDailyDepositConfig().m_stBonusVec;

                static std::string strBonusItem;

                CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stBonusVec, strBonusItem, m_usCmd);

                m_pUserInfo->m_stUserInfoTable.SetDailyDepositBonus(1);

                SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

                PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|BONUS_ITEM:" << strBonusItem << std::endl;

                return true;
            }
            else
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_ADD_UP_RMB 
                                    << "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_ADD_UP_RMB)
                                    << std::endl;

                SEND_ERRCODE_AND_RET(NOT_ENOUGH_ADD_UP_RMB);
            }
        }
        else
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << DAILY_DEPOSIT_BONUS_HAS_RECEIVED 
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(DAILY_DEPOSIT_BONUS_HAS_RECEIVED)
                                << std::endl;

            SEND_ERRCODE_AND_RET(DAILY_DEPOSIT_BONUS_HAS_RECEIVED);
        }
    }
    else if(m_usCmd == MSG_LOGIC_DAILY_SIGN)
    {
        CResponseDailySign stRsp;

        if(!(LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stUserInfoTable.GetLastSignTime())))
        {
            stRsp.m_stDailySignItem = m_pUserInfo->m_stUserInfoTable.GetDailySignReward();
            stRsp.m_iFlag = m_pUserInfo->m_stUserInfoTable.GetDailySignFlag();
            stRsp.m_stAtlasSignItems = m_pUserInfo->m_stUserInfoTable.GetCompleteSignReward();
            SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
            return true;
        }

        auto& stConfigVec = CLogicConfig::GetSignConfig().m_stDailySignVec;
        if(stConfigVec.empty())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
        }

        //签到随机
        auto totalWeight = 1;
        auto curWeight = 0;
        auto signReward = stConfigVec[0];
        for(auto iterCardWeight : CLogicConfig::GetSignConfig().m_stDailySignVec)
        {
            PROCESSOR_LOG_DEBUG << iterCardWeight.m_iWeight << std::endl;
            totalWeight += iterCardWeight.m_iWeight;
        }
        auto randNum = CLogicCommon::GetRandNum() % totalWeight;
        for(auto iterCardWeight : CLogicConfig::GetSignConfig().m_stDailySignVec)
        {
            curWeight += iterCardWeight.m_iWeight;
            if(curWeight > randNum)
            {
                signReward = iterCardWeight;
                break;
            }
        }

        auto stRandVec = CLogicConfig::GetCardbagConfig().GetRandBonusByName(signReward.m_strCardBag);
        if(stRandVec.empty())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR)
        }
        //检查是否是全图鉴
        bool isComplete = false;
        if (CLogicActive::IsAvyOpen(CLogicConfig::GetCompleteAtlasConfig().m_iAvyID, m_pUserInfo->m_stUserInfoTable.GetCreateTime()))
        {
            for(auto &heroCard: CLogicConfig::GetCompleteAtlasConfig().m_vecCompleteCard)
            {
                auto iterHeroCard = m_pUserInfo->m_stHeroCardTableMap.Find(heroCard);
                if (m_pUserInfo->m_stHeroCardTableMap.End() == iterHeroCard)
                {
                    break;
                }
                isComplete = true;
            }
        }

        //添加全图鉴随机奖励
        if (isComplete)
        {
            auto stAtlasVec = CLogicConfig::GetCardbagConfig().GetRandBonusByName(CLogicConfig::GetCompleteAtlasConfig().m_strCardBag);
            auto& stAtlasRand = stAtlasVec[0];
            stRsp.m_stAtlasSignItems.m_iItemType = stAtlasRand.m_iItemType;
            stRsp.m_stAtlasSignItems.m_iCardID = stAtlasRand.m_iCardID;
            stRsp.m_stAtlasSignItems.m_iNum = stAtlasRand.m_iNum;
        }

        auto& stRand = stRandVec[0];
        stRsp.m_stDailySignItem.m_iItemType = stRand.m_iItemType;
        stRsp.m_stDailySignItem.m_iCardID = stRand.m_iCardID;
        stRsp.m_stDailySignItem.m_iNum = stRand.m_iNum;
        stRsp.m_iFlag = signReward.m_iNotify;
        stRsp.m_bSignSuccess = true;

        std::vector<CPackGameItem> stRewards;

        stRewards.push_back(stRsp.m_stDailySignItem);
        stRewards.push_back(stRsp.m_stAtlasSignItems);

        m_pUserInfo->m_stUserInfoTable.AddTotalSign(1);
        m_pUserInfo->m_stUserInfoTable.SetLastSignTime(CLogicCommon::GetSec());

        auto pTotalReward = CLogicConfig::GetSignConfig().GetTotalSignReward(m_pUserInfo->m_stUserInfoTable.GetTotalSign());
        if(pTotalReward)
        {
            stRsp.m_stTotalSignItems = pTotalReward->m_stRewards;
            stRewards.insert(stRewards.end(), pTotalReward->m_stRewards.begin(), pTotalReward->m_stRewards.end());
        }

        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRewards, m_usCmd);

        stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;

        m_pUserInfo->m_stUserInfoTable.SetDailySignReward(stRsp.m_stDailySignItem);
        m_pUserInfo->m_stUserInfoTable.SetDailySignFlag(stRsp.m_iFlag);
        m_pUserInfo->m_stUserInfoTable.SetCompleteSignReward(stRsp.m_stAtlasSignItems);

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return true;
    }
    else if((int)m_usCmd == MSG_LOGIC_UPDATE_USER_AGE)
    {
        CRequestUpdateUserAge stReq;

        if (DecodeMsgPack(stFormatData, stReq) == false) return (false);

        if(stReq.m_iAge < 0 || stReq.m_iAge > 999 || stReq.m_iAge < m_pUserInfo->m_stUserInfoTable.GetAge())
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << PARAMETER_ERROR
                                << "|MSG:Update User Age Invalid Age:" << stReq.m_iAge << std::endl;

            SEND_ERRCODE_AND_RET(PARAMETER_ERROR);
        }

        if(stReq.m_iAge == m_pUserInfo->m_stUserInfoTable.GetAge())
        {
            return true;
        }

        m_pUserInfo->m_stUserInfoTable.SetAge(stReq.m_iAge);
        CLogicGlobal::AntiAddCheckUser(m_iUin, m_iGroupID, stReq.m_iAge);
    }
    else if((int)m_usCmd == MSG_LOGIC_TOUHOU_CARD_DIALOG)
    {
        CRequestCardDialog stReq;
        CResponseCardDialog stRsp;

        if (DecodeMsgPack(stFormatData, stReq) == false) return (false);

        // 优先判断限时活动类互动
        if(stReq.m_bIsLimitAvy)
        {
            // 已领取过
            const auto& limitAvyStat = m_pUserInfo->m_stIBusyData.GetCardDialogLimitAvyStat();
            if(std::find(limitAvyStat.begin(), limitAvyStat.end(), stReq.m_iCardID) != limitAvyStat.end())
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << AWARD_HAVE_RECEIVED << "|MSG:" << GET_ERRMSG_BY_ERRCODE(AWARD_HAVE_RECEIVED)
                                    << "|CardID:" << stReq.m_iCardID << std::endl;

                SEND_ERRCODE_AND_RET(AWARD_HAVE_RECEIVED);
            }

            auto iterBegin = CLogicConfig::GetCardDialogConfig().m_stLimitAvyRewardMap.lower_bound(stReq.m_iCardID);
            auto iterEnd = CLogicConfig::GetCardDialogConfig().m_stLimitAvyRewardMap.upper_bound(stReq.m_iCardID);
            if(iterBegin == iterEnd)
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << PARAMETER_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                                    << "|CardID:" << stReq.m_iCardID << std::endl;

                SEND_ERRCODE_AND_RET(PARAMETER_ERROR);
            }

            for(; iterBegin != iterEnd; ++iterBegin)
            {
                auto iterRef = CLogicConfig::GetActivityConfig().m_stActivityRef.find(iterBegin->second.m_iAvyID);
                if (iterRef == CLogicConfig::GetActivityConfig().m_stActivityRef.end())
                {
                    using namespace SERVER_ERRCODE;
                    PROCESSOR_LOG_ERROR << "ERRCODE:" << INTERN_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                                        << "|AvyID:" << iterBegin->second.m_iAvyID << std::endl;

                    SEND_ERRCODE_AND_RET(INTERN_ERROR);
                }

                if (CLogicActive::IsWithinActivityTime(m_pUserInfo, iterRef->second.m_iType, CLogicCommon::GetSec()
                        , iterRef->second.m_iTimeType, iterRef->second.m_iStartDay, iterRef->second.m_iEndDay
                        , iterRef->second.m_iServerDay, iterRef->second.m_iStartTime, iterRef->second.m_iEndTime))
                {
                    break;
                }
            }

            if(iterBegin == iterEnd)
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << ACTIVE_IS_NOT_OPEN << "|MSG:" << GET_ERRMSG_BY_ERRCODE(ACTIVE_IS_NOT_OPEN)
                                    << "|CardID:" << stReq.m_iCardID << std::endl;

                SEND_ERRCODE_AND_RET(ACTIVE_IS_NOT_OPEN);
            }

            stRsp.m_iCardDialogNextTime = m_pUserInfo->m_stIBusyData.GetCardDialogNextTime();
            stRsp.m_iCardDialogTimes = m_pUserInfo->m_stIBusyData.GetCardDialogTimes();
            stRsp.m_stCardDialogLimitAvyStat = m_pUserInfo->m_stIBusyData.GetCardDialogLimitAvyStat();
            stRsp.m_stBonusVec = iterBegin->second.m_stBonusList;

            std::string strBonus;
            CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stBonusVec, strBonus, m_usCmd);

            stRsp.m_stCardDialogLimitAvyStat.push_back(stReq.m_iCardID);
            m_pUserInfo->m_stIBusyData.SetCardDialogLimitAvyStat(stRsp.m_stCardDialogLimitAvyStat);

            SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

            PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|CardDialog LimitAvy Award" << "|BONUS_ITEM:" << strBonus << std::endl;
            return (true);
        }
        else
        {
            // 普遍互动
            if(CLogicCommon::GetSec() < m_pUserInfo->m_stIBusyData.GetCardDialogNextTime())
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << TALK_TOO_FAST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(TALK_TOO_FAST)
                                    << "|CardID:" << stReq.m_iCardID << std::endl;

                SEND_ERRCODE_AND_RET(TALK_TOO_FAST);
            }

            if((int)CLogicConfig::GetCardDialogConfig().m_stTimesCDList.size() <= m_pUserInfo->m_stIBusyData.GetCardDialogTimes())
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << COUNT_NOT_ENOUGH << "|MSG:" << GET_ERRMSG_BY_ERRCODE(COUNT_NOT_ENOUGH)
                                    << "|CardID:" << stReq.m_iCardID << std::endl;

                SEND_ERRCODE_AND_RET(COUNT_NOT_ENOUGH);
            }

            if(stReq.m_iOption <= 0)
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << PARAMETER_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                                    << "|CardID:" << stReq.m_iCardID << std::endl;

                SEND_ERRCODE_AND_RET(PARAMETER_ERROR);
            }

            int32_t iRewardListIndex = 0;
            auto iterCardIndex = CLogicConfig::GetCardDialogConfig().m_stCardIndexMap.find(stReq.m_iCardID);
            if(iterCardIndex != CLogicConfig::GetCardDialogConfig().m_stCardIndexMap.end())
            {
                iRewardListIndex = iterCardIndex->second;
            }

            if(iRewardListIndex >= (int32_t)CLogicConfig::GetCardDialogConfig().m_stDialogRewardList.size())
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << PARAMETER_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                                    << "|CardID:" << stReq.m_iCardID << std::endl;

                SEND_ERRCODE_AND_RET(PARAMETER_ERROR);
            }

            const auto& stRewardListConfig = CLogicConfig::GetCardDialogConfig().m_stDialogRewardList[iRewardListIndex];
            if(stRewardListConfig.empty())
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << COUNT_NOT_ENOUGH << "|MSG:" << GET_ERRMSG_BY_ERRCODE(COUNT_NOT_ENOUGH)
                                    << "|CardID:" << stReq.m_iCardID << std::endl;

                SEND_ERRCODE_AND_RET(COUNT_NOT_ENOUGH);
            }

            // 先计算额外奖励
            auto iterBegin = CLogicConfig::GetCardDialogConfig().m_stOtherAvyRewardList.begin();
            auto iterEnd = CLogicConfig::GetCardDialogConfig().m_stOtherAvyRewardList.end();
            for(; iterBegin != iterEnd; ++iterBegin)
            {
                auto& stAvyTime = iterBegin->second;
                if(stAvyTime.m_iStartTime != 0)
                {
                    if(CLogicActive::IsWithinRangeTime(CLogicCommon::GetSec(), stAvyTime.m_iStartTime, stAvyTime.m_iEndTime))
                        break;
                }
                else if(stAvyTime.m_iStartDay != 0)
                {
                    if(CLogicActive::IsWithinRangeDay(m_pUserInfo, CLogicConfigDefine::TIME_TYPE_SERVER, stAvyTime.m_iStartDay, stAvyTime.m_iEndDay, 0))
                        break;
                }
                else
                {
                    using namespace SERVER_ERRCODE;
                    PROCESSOR_LOG_ERROR << "ERRCODE:" << INTERN_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                                        << "|CardID:" << stReq.m_iCardID << std::endl;

                    SEND_ERRCODE_AND_RET(INTERN_ERROR);
                }
            }

            if(iterBegin != iterEnd)
            {
                auto bonusList = CLogicConfig::GetCardDialogConfig().GetBonusByBagName(m_pUserInfo->m_stUserInfoTable.GetLevel(), iterBegin->first);
                if(bonusList.size() > 0)
                {
                    stRsp.m_stBonusVec = bonusList;
                }
            }

            // 普通奖励
            int32_t iRewardOptIndex = m_pUserInfo->m_stIBusyData.GetCardDialogTimes();
            if((int)stRewardListConfig.size() <= iRewardOptIndex)
            {
                iRewardOptIndex = 0;
            }

            auto& bagList = stRewardListConfig[iRewardOptIndex];
            if(bagList.size() < (std::size_t)stReq.m_iOption)
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << COUNT_NOT_ENOUGH << "|MSG:" << GET_ERRMSG_BY_ERRCODE(COUNT_NOT_ENOUGH)
                                    << "|CardID:" << stReq.m_iCardID << std::endl;

                SEND_ERRCODE_AND_RET(COUNT_NOT_ENOUGH);
            }

            auto bonusList = CLogicConfig::GetCardDialogConfig().GetBonusByBagName(m_pUserInfo->m_stUserInfoTable.GetLevel(), bagList[stReq.m_iOption-1]);
            if(bonusList.size() > 0)
            {
                stRsp.m_stBonusVec.insert(stRsp.m_stBonusVec.end(), bonusList.begin(), bonusList.end());
            }

            std::string strBonus;
            CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stBonusVec, strBonus, m_usCmd);

            m_pUserInfo->m_stIBusyData.AddCardDialogTimes(1);
            if((int)CLogicConfig::GetCardDialogConfig().m_stTimesCDList.size() > m_pUserInfo->m_stIBusyData.GetCardDialogTimes())
            {
                const auto iter = CLogicConfig::GetCardDialogConfig().m_stTimesCDList[m_pUserInfo->m_stIBusyData.GetCardDialogTimes()];
                m_pUserInfo->m_stIBusyData.SetCardDialogNextTime(CLogicCommon::GetSec() + CLogicCommon::GetRandNum(iter.first, iter.second));
            }

            stRsp.m_iCardDialogNextTime = m_pUserInfo->m_stIBusyData.GetCardDialogNextTime();
            stRsp.m_iCardDialogTimes = m_pUserInfo->m_stIBusyData.GetCardDialogTimes();
            stRsp.m_stCardDialogLimitAvyStat = m_pUserInfo->m_stIBusyData.GetCardDialogLimitAvyStat();

            SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

            PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|CardDialog OtherDialog Award" << "|BONUS_ITEM:" << strBonus << std::endl;
            return (true);
        }
    }
    else if((int)m_usCmd == MSG_LOGIC_TOUHOU_WENWEN_NEWS)
    {
        CResponseWenWenNews stRsp;
        auto& serverData = CLogicGlobal::GetGlobalCache()->m_stBasicTable.GetNewsData().m_stDataMap;
        for(auto iterNewsType: CLogicConfig::GetWenWenNewsConfig().m_stNewsDynamicMap)
        {
            auto iterBegin = serverData.lower_bound(iterNewsType.first);
            auto iterEnd = serverData.upper_bound(iterNewsType.first);
            if(iterBegin == iterEnd)
                continue;

            for(; iterBegin != iterEnd; ++iterBegin)
            {
                CPackWenWenNewsElem stData;
                stData.m_iType = iterBegin->second.m_iType;
                stData.m_iID = iterBegin->second.m_iID;
                stData.m_iPara = iterBegin->second.m_iPara;
                stData.m_iIndex = iterBegin->second.m_iIndexID;
                stRsp.m_stThemeData.emplace_back(stData);
            }
        }

        stRsp.m_cChoose = m_pUserInfo->m_stIBusyData.GetWenwenChoose();
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return (true);
    }
    else if ((int)m_usCmd == MSG_LOGIC_WENWEN_CHOOSE_REWARD)
    {
        CRequestChooseWenWenReward stReq;
        if(DecodeMsgPack(stFormatData, stReq) == false) return (false);

        auto newsChooseConfig = CLogicConfig::GetWenWenNewsConfig().m_stNewsChooseMap;
        if (stReq.m_iChoose > (int32_t)newsChooseConfig.size() || stReq.m_iChoose <= 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        m_pUserInfo->m_stIBusyData.SetWenwenChoose(stReq.m_iChoose);
        CResponseChooseWenWenReward stRsp;
        stRsp.m_iChoose = stReq.m_iChoose;
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
    else if ((int)m_usCmd == MSG_LOGIC_RECORD_USER_SPECIAL_GUIDE)
    {
        CResponseRecordUserSpecialGuide stRsp;
        CRequestRecordUserSpecialGuide stParam;

        if(!DecodeMsgPack(stFormatData, stParam)) return (false);

        if (stParam.m_ilSpecialStep >= 1 && stParam.m_ilSpecialStep <= 64)
        {
            uint64_t ulOffsetValue = (uint64_t)1<<(stParam.m_ilSpecialStep-1);
            uint64_t ulRealValue = m_pUserInfo->m_stUserInfoTable.GetGuideSpecialStep()|ulOffsetValue;
            m_pUserInfo->m_stUserInfoTable.SetGuideSpecialStep(ulRealValue);
        } else {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << PARAMETER_ERROR
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                                << "|SPECIAL_STEP:" << stParam.m_ilSpecialStep << std::endl;

            SEND_ERRCODE_AND_RET(PARAMETER_ERROR);
        }

        stRsp.m_ilSpecialStep = stParam.m_ilSpecialStep;
        stRsp.m_ilRecordSpecialStep = m_pUserInfo->m_stUserInfoTable.GetGuideSpecialStep();
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|SPECIAL_STEP:" << stParam.m_ilSpecialStep
                            << "RECORD_SPECIAL_STEP:" << stRsp.m_ilRecordSpecialStep << std::endl;
    }
    else if ((int)m_usCmd == MSG_LOGIC_GET_CONTROL_INFO)
    {
        CResponseGetControlInfo stRsp;
        stRsp.m_iSwitchControl = CLogicConfig::GetZooLogicGameConfig().m_iSwitchControl;
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }

    return (true);
}

bool CLogicMainSceneProcessor::GetWenWenNewsOneServerData(const TLogicNewsDynamicElem& stConfig, CPackWenWenNewsDynamicData& stData)
{
//    auto& serverData = CLogicGlobal::GetGlobalCache()->m_stBasicTable.GetNewsData().m_stDataMap;
//    if(stConfig.m_iSubType == EWWNST_Rank)
//    {
//        uint64_t lKey = 0;
//        int32_t iValue = 0;
//        switch (stConfig.m_iType)
//        {
//            case CLogicConfigDefine::EWWNT_Power:
//                if(LOGIC_FIGHT_POWER_RANK_INS.begin() != LOGIC_FIGHT_POWER_RANK_INS.end())
//                {
//                    lKey = LOGIC_FIGHT_POWER_RANK_INS.begin()->first;
//                    iValue = LOGIC_FIGHT_POWER_RANK_INS.begin()->second;
//                }
//                break;
//            case CLogicConfigDefine::EWWNT_PVP_Rank:
//                if(LOGIC_PVP_RANK_INS.begin() != LOGIC_PVP_RANK_INS.end())
//                {
//                    lKey = LOGIC_PVP_RANK_INS.begin()->first;
//                    iValue = LOGIC_PVP_RANK_INS.begin()->second;
//                }
//                break;
//            case CLogicConfigDefine::EWWNT_Tower:
//                if(LOGIC_CLIMB_TOWER_RANK_INS.begin() != LOGIC_CLIMB_TOWER_RANK_INS.end())
//                {
//                    lKey = LOGIC_CLIMB_TOWER_RANK_INS.begin()->first;
//                    iValue = LOGIC_CLIMB_TOWER_RANK_INS.begin()->second;
//                }
//                break;
//            case CLogicConfigDefine::EWWNT_Charm:
//                if(LOGIC_CHARM_RANK_INS.begin() != LOGIC_CHARM_RANK_INS.end())
//                {
//                    lKey = LOGIC_CHARM_RANK_INS.begin()->first;
//                    iValue = LOGIC_CHARM_RANK_INS.begin()->second;
//                }
//                break;
//            case CLogicConfigDefine::EWWNT_Home:
//                if(LOGIC_HOME_RANK_INS.begin() != LOGIC_HOME_RANK_INS.end())
//                {
//                    lKey = LOGIC_HOME_RANK_INS.begin()->first;
//                    iValue = LOGIC_HOME_RANK_INS.begin()->second;
//                }
//                break;
//            case CLogicConfigDefine::EWWNT_Guild:
//                if(LOGIC_GUILD_RANK_INS.begin() != LOGIC_GUILD_RANK_INS.end())
//                {
//                    lKey = LOGIC_GUILD_RANK_INS.begin()->first;
//                    iValue = LOGIC_GUILD_RANK_INS.begin()->second;
//                }
//                break;
//            case CLogicConfigDefine::EWWNT_Match_PVP:
//                if(LOGIC_MATCH_PVP_RANK_INS.begin() != LOGIC_MATCH_PVP_RANK_INS.end())
//                {
//                    lKey = LOGIC_MATCH_PVP_RANK_INS.begin()->first;
//                    iValue = LOGIC_MATCH_PVP_RANK_INS.begin()->second;
//                }
//                break;
//            case CLogicConfigDefine::EWWNT_Military_Level:
//                if(LOGIC_MILITARY_LEVEL_RANK_INS.begin() != LOGIC_MILITARY_LEVEL_RANK_INS.end())
//                {
//                    lKey = LOGIC_MILITARY_LEVEL_RANK_INS.begin()->first;
//                    iValue = LOGIC_MILITARY_LEVEL_RANK_INS.begin()->second;
//                }
//                break;
//        }
//
//        if(lKey <= 0) return false;
//
//        stData.m_iType = stConfig.m_iType;
//        stData.m_iID = 1;
//        stData.m_iPara = iValue;
//        if(stConfig.m_iType == CLogicConfigDefine::EWWNT_Guild)
//        {
            /*
            auto* pstGuild = CLogicGuild::GetGuild(lKey);
            if (pstGuild)
            {
                stData.m_strName = pstGuild->GetGuildName();
            }
             */
        //}
//        else
//        {
//            const user_info_key_t stKey(lKey);
//            if(stKey.m_stRoleIDDetail.m_iGroupID > 0)
//            {
//                const auto pUserInfo = CLogicCacheCommon::LoadUserData(stKey.m_stRoleIDDetail.m_iGroupID, stKey.m_stRoleIDDetail.m_iUin);
//                if(pUserInfo)
//                {
//                    stData.m_iUid = pUserInfo->m_iUin;
//                    stData.m_iGroupID = pUserInfo->m_iGroupID;
//                    stData.m_strName = pUserInfo->m_strNick;
//                    stData.m_iLevel = pUserInfo->m_stUserInfoTable.GetLevel();
//                    stData.m_iAvatarIcon = pUserInfo->m_stUserInfoTable.GetAvatarID();
//                    stData.m_iAvatarBorder = pUserInfo->m_stUserInfoTable.GetAvatarBorder();
//                    stData.m_iVipLevel = pUserInfo->m_stUserInfoTable.GetVipLevel();
//                }
//            }
//            else
//            {
//                const auto iterRobot = CLogicConfig::GetPVPRobotConfig().m_stRobotList.find(stKey.m_stRoleIDDetail.m_iUin);
//                if (iterRobot != CLogicConfig::GetPVPRobotConfig().m_stRobotList.end())
//                {
//                    stData.m_iUid = stKey.m_stRoleIDDetail.m_iUin;
//                    stData.m_iGroupID = stKey.m_stRoleIDDetail.m_iGroupID;
//                    stData.m_strName = iterRobot->second.m_strNick;
//                    stData.m_iLevel = iterRobot->second.m_iLevel;
//                    stData.m_iAvatarIcon = iterRobot->second.m_iAvatarID;
//                    stData.m_iAvatarBorder = iterRobot->second.m_iAvatarBorder;
//                    stData.m_iVipLevel = 0;
//                }
//            }
//        }
//    }
//    else
//    {
//        auto iterBegin = serverData.lower_bound(stConfig.m_iType);
//        auto iterEnd = serverData.upper_bound(stConfig.m_iType);
//        if(iterBegin == iterEnd)
//            return false;
//
//        std::vector<CPackWenWenNewsServerElem> vecPack;
//        for(; iterBegin != iterEnd; ++iterBegin)
//            vecPack.push_back(iterBegin->second);
//
//        auto& data = vecPack[CLogicCommon::GetRandNum(0, vecPack.size())];
//        stData.m_iType = stConfig.m_iType;
//        stData.m_iID = data.m_iID;
//        stData.m_iPara = data.m_iPara;
//        const auto pUserInfo = CLogicCacheCommon::LoadUserData(data.m_iGroupID, data.m_iUin);
//        if(pUserInfo)
//        {
//            stData.m_iUid = pUserInfo->m_iUin;
//            stData.m_iGroupID = pUserInfo->m_iGroupID;
//            stData.m_strName = pUserInfo->m_strNick;
//            stData.m_iLevel = pUserInfo->m_stUserInfoTable.GetLevel();
//            stData.m_iAvatarIcon = pUserInfo->m_stUserInfoTable.GetAvatarID();
//            stData.m_iAvatarBorder = pUserInfo->m_stUserInfoTable.GetAvatarBorder();
//            stData.m_iVipLevel = pUserInfo->m_stUserInfoTable.GetVipLevel();
//        }
//    }

    return true;
}
