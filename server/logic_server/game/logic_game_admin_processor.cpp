#include "logic_game_admin_processor.h"
#include "common_mysql.h"
#include "common_msg_head_admin.h"
#include "server_errcode.h"
#include "server_cmd_admin_define.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "server_admin_protocol.h"
#include "server_cross_protocol.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "logic_config.h"
#include "logic_game_active_info.h"
#include "logic_game_user.h"
#include "logic_game_global.h"
#include "logic_game_rank_list.h"
#include "logic_game_cache_level_operation.h"
#include "logic_game_user_stat.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_item.h"
#include "logic_game_bill.h"
#include "logic_game_day_refresh_system.h"
#include "logic_game_global_frame.h"
#include "logic_game_item_factory.h"
#include "logic_game_global.h"
#include "logic_game_monitor.h"
#include "logic_game_buff.h"
#include "server_cmd_data_report_define.h"
#include "server_data_report_protocol.h"

using namespace SERVER_ERRCODE;

struct TPayInfo
{
    std::string	m_strPayCash;
    int32_t		m_iActivePayCash;//活动计算的充值金额，国服为远，繁体为美分
    int32_t		m_iPayGameCoin;
    int32_t		m_iPayItemType;
    int32_t		m_iPayItemID;
    int32_t     m_iPayVipExp;
    bool		m_bIsTestOrder;
};

#define ASSIGN_PROCESSOR(CMD, FUNC) { CMD, {GET_MACRO_STRING(CMD), std::bind(&FUNC, std::placeholders::_1, std::placeholders::_2, CMD, GET_MACRO_STRING(CMD), std::placeholders::_3) } }

CLogicAdminProcessor::ADMIN_PROCESSOR_MAP_TYPE CLogicAdminProcessor::ADMIN_PROCESSOR_MAP =
{
        ASSIGN_PROCESSOR(MSG_ADMIN_RECHARGE, CLogicAdminProcessor::ProcessRecharge),
        ASSIGN_PROCESSOR(MSG_ADMIN_MODIFY_ITEM, CLogicAdminProcessor::ProcessModifyItem),
        ASSIGN_PROCESSOR(MSG_ADMIN_ADD_SYSTEM_MAIL, CLogicAdminProcessor::ProcessAddSystemMail),
        ASSIGN_PROCESSOR(MSG_ADMIN_SET_USER_BAN, CLogicAdminProcessor::ProcessSetUserBan),
        ASSIGN_PROCESSOR(MSG_ADMIN_DELETE_SYSTEM_MAIL, CLogicAdminProcessor::ProcessDeleteSystemMail),
        ASSIGN_PROCESSOR(MSG_ADMIN_GIFT_CARD_AWARD, CLogicAdminProcessor::ProcessGiftCardAward),
        ASSIGN_PROCESSOR(MSG_ADMIN_SEND_ACTIVE_MAIL, CLogicAdminProcessor::ProcessSendActiveMail),
        ASSIGN_PROCESSOR(MSG_ADMIN_MODIFY_CHAPTER, CLogicAdminProcessor::ProcessModifyChapter),
        ASSIGN_PROCESSOR(MSG_ADMIN_MODIFY_HERO_CARD, CLogicAdminProcessor::ProcessModifyHeroCard),
        ASSIGN_PROCESSOR(MSG_ADMIN_MODIFY_CREATE_TIME, CLogicAdminProcessor::ProcessModifyCreateTime),
        ASSIGN_PROCESSOR(MSG_ADMIN_MODIFY_GUIDE, CLogicAdminProcessor::ProcessModifyGuide),
        ASSIGN_PROCESSOR(MSG_ADMIN_MODIFY_GUILD, CLogicAdminProcessor::ProcessModifyGuild),
        ASSIGN_PROCESSOR(MSG_ADMIN_DELETE_HERO, CLogicAdminProcessor::ProcessDeleteHero),
        ASSIGN_PROCESSOR(MSG_ADMIN_DELETE_RANK, CLogicAdminProcessor::ProcessDeleteRank),
        ASSIGN_PROCESSOR(MSG_ADMIN_CLEAR_CACHE, CLogicAdminProcessor::ProcessClearCache),
        ASSIGN_PROCESSOR(MSG_ADMIN_SET_GUILD_BULLETIN, CLogicAdminProcessor::ProcessSetGuildBulletin),
        ASSIGN_PROCESSOR(MSG_ADMIN_JUBAO_AWARD, CLogicAdminProcessor::ProcessJubaoAward),
        ASSIGN_PROCESSOR(MSG_ADMIN_USER_RANK, CLogicAdminProcessor::ProcessUserRank),
        ASSIGN_PROCESSOR(MSG_ADMIN_CROSS_DAY, CLogicAdminProcessor::ProcessUserCrossDay),
        ASSIGN_PROCESSOR(MSG_ADMIN_ADD_FISH, CLogicAdminProcessor::ProcessAddFish),
        ASSIGN_PROCESSOR(MSG_ADMIN_ADD_EQUIP, CLogicAdminProcessor::ProcessAddEquip),
        ASSIGN_PROCESSOR(MSG_ADMIN_DISMISS_GUILD, CLogicAdminProcessor::ProcessDismissGuild),
        ASSIGN_PROCESSOR(MSG_ADMIN_SET_GUILD_CHIEF, CLogicAdminProcessor::ProcessSetGuildChief),
        ASSIGN_PROCESSOR(MSG_ADMIN_GET_CAN_DEPOSIT, CLogicAdminProcessor::ProcessGetCanDeposit),
        ASSIGN_PROCESSOR(MSG_ADMIN_SET_ENTIRE_WAR_PASS, CLogicAdminProcessor::ProcessSetEntireWarPass),
        ASSIGN_PROCESSOR(MSG_ADMIN_CHANGE_GUILD_NAME, CLogicAdminProcessor::ProcessChangeGuildName),
        ASSIGN_PROCESSOR(MSG_ADMIN_COPY_USER, CLogicAdminProcessor::ProcessCopyUser),
        ASSIGN_PROCESSOR(MSG_ADMIN_ADD_CONSTEL, CLogicAdminProcessor::ProcessAddConstel),
        ASSIGN_PROCESSOR(MSG_ADMIN_DEL_CONSTEL, CLogicAdminProcessor::ProcessDelConstel),
        ASSIGN_PROCESSOR(MSG_ADMIN_CHANGE_AGE_LIMIT, CLogicAdminProcessor::ProcessChangeAgeLimit),

};

///////////////////////////////////////////////////////////
//Process Function

bool CLogicAdminProcessor::ProcessMessage(int32_t iUin, int16_t nGroupID, uint16_t usCmd, stLogicSoAdminClient &stClient)
{
    const auto iter = ADMIN_PROCESSOR_MAP.find(usCmd);
    if (iter == ADMIN_PROCESSOR_MAP.end())
    {
        LOGIC_LOG_TRACE_ERROR << "ADMIN CLIENT|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                        << "|CMD IS INVALID|CMD:" << usCmd << "|UIN:" << iUin << "|GROUP:" << nGroupID << std::endl;

        stClient.Clear();

        return false;
    }

    // 协议开关判断
    if(CLogicMonitor::ProtoSwitch(usCmd))
    {
        LOGIC_LOG_TRACE_DEBUG << "ADMIN CLIENT|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                              << "|PROTOCOL SWITCH LIMIT|RETURN|CMD:" << usCmd << "|UIN:" << iUin << "|GROUP:" << nGroupID << std::endl;

        using namespace SERVER_ERRCODE;

        CGMResponseBody stRsp;
        stRsp.m_iCode = PROTO_SWITCH_LIMIT;
        ResponseMessage(stClient.m_iFd, usCmd, stRsp);

        return (false);
    }

    CLogicBill::RefreshBillRation(usCmd, iter->second.first);
    auto result = iter->second.second(iUin, nGroupID, stClient);

    LOGIC_RECORD_CACHE_INS->ExecSQL(nGroupID, iUin, iter->second.first);

    return result;
}

bool CLogicAdminProcessor::ProcessRecharge(int32_t iUin, int16_t nGroupID, uint16_t usCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestRecharge stReqBody;
    CGMResponseRecharge stRspBody;

    stRspBody.m_bIsNew = 0;

    if(!DecodeClient(stClient, pProtoName, stReqBody))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                       << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    const auto pstDeposit = CLogicConfig::GetDepositConfig().GetDepositConfig(stReqBody.m_iDepositID);
    if (nullptr == pstDeposit || stReqBody.m_iProductID <= 0 || pstDeposit->m_iType == CLogicConfigDefine::DEPOSIT_TYPE_OTHER_DEPOSIT)
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:|MSG:DEPOSIT_ID_NOT_FOUND"
                       << "|GROUP_ID:" << nGroupID
                       << "|UIN:" << iUin << "|ORDER_ID:" << stReqBody.m_strOrderID
                       << "|DEPOSIT_ID:" << stReqBody.m_iDepositID
                       << "|PRODUCT_ID:" << stReqBody.m_iProductID
                       << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::DEPOSIT_ID_NOT_FOUND;

        LOGIC_SERVICE_API_INS->RequestServiceAlert("request_pay_error", {
                { "uin", std::to_string(iUin) },
                { "group_id", std::to_string(nGroupID) },
                { "deposit", std::to_string(stReqBody.m_iDepositID) },
                { "error", GET_MACRO_STRING(SERVER_ERRCODE::DEPOSIT_ID_NOT_FOUND)}
        });

        ResponseMessage(stClient.m_iFd, usCmd, stRspBody);
        return false;
    }

    if (stReqBody.m_iProductID != pstDeposit->m_iProductID && stReqBody.m_iProductID != stReqBody.m_iDepositID)
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:|MSG:DEPOSIT_PRICE_ERROR"
                       << "|GROUP_ID:" << nGroupID
                       << "|UIN:" << iUin << "|ORDER_ID:" << stReqBody.m_strOrderID
                       << "|DEPOSIT_ID:" << stReqBody.m_iDepositID
                       << "|PRODUCT_ID:" << stReqBody.m_iProductID
                       << "|CONFIG_PRODUCT_ID:" << pstDeposit->m_iProductID
                       << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::DEPOSIT_PRICE_ERROR;

        LOGIC_SERVICE_API_INS->RequestServiceAlert("request_pay_error", {
                { "uin", std::to_string(iUin) },
                { "group_id", std::to_string(nGroupID) },
                { "deposit", std::to_string(stReqBody.m_iDepositID) },
                { "error", GET_MACRO_STRING(SERVER_ERRCODE::DEPOSIT_PRICE_ERROR)}
        });

        ResponseMessage(stClient.m_iFd, usCmd, stRspBody);
        return false;
    }

    double dParamPrice = 0.0;
    if (!try_str2num(stReqBody.m_strPayPrice, dParamPrice))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:|MSG:DEPOSIT_PRICE_ERROR"
                       << "|GROUP_ID:" << nGroupID
                       << "|UIN:" << iUin << "|ORDER_ID:" << stReqBody.m_strOrderID
                       << "|DEPOSIT_ID:" << stReqBody.m_iDepositID
                       << "|PRODUCT_ID:" << stReqBody.m_iProductID
                       << "|PRICE:" << stReqBody.m_strPayPrice
                       << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::DEPOSIT_PRICE_ERROR;

        LOGIC_SERVICE_API_INS->RequestServiceAlert("request_pay_error", {
                { "uin", std::to_string(iUin) },
                { "group_id", std::to_string(nGroupID) },
                { "deposit", std::to_string(stReqBody.m_iDepositID) },
                { "error", GET_MACRO_STRING(SERVER_ERRCODE::DEPOSIT_PRICE_ERROR)}
        });

        ResponseMessage(stClient.m_iFd, usCmd, stRspBody);
        return false;
    }

    //价格不一致只打印日志
    if (fabs(pstDeposit->m_dPrice - dParamPrice) >= 1e-6)
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:|MSG:DEPOSIT_PRICE_ERROR"
                       << "|GROUP_ID:" << nGroupID
                       << "|UIN:" << iUin << "|ORDER_ID:" << stReqBody.m_strOrderID
                       << "|DEPOSIT_ID:" << stReqBody.m_iDepositID
                       << "|PRODUCT_ID:" << stReqBody.m_iProductID
                       << "|PRICE:" << stReqBody.m_strPayPrice
                       << "|CAST_PRICE:" << dParamPrice
                       << "|CONFIG_PRICE:" << pstDeposit->m_dPrice
                       << std::endl;

//        stRspBody.m_iCode = SERVER_ERRCODE::DEPOSIT_PRICE_ERROR;
//
//        LOGIC_SERVICE_API_INS->RequestServiceAlert("request_pay_error", {
//                { "uin", std::to_string(iUin) },
//                { "group_id", std::to_string(nGroupID) },
//                { "deposit", std::to_string(stReqBody.m_iDepositID) },
//                { "error", GET_MACRO_STRING(SERVER_ERRCODE::DEPOSIT_PRICE_ERROR)}
//        });
//
//        ResponseMessage(stClient.m_iFd, usCmd, stRspBody);
//        return false;
    }

    try
    {
        const std::string escape_order = CLogicSQLTableRecordBase::GetMysqlConn()->escapeString(stReqBody.m_strOrderID);
        if (CLogicSQLTableRecordBase::GetMysqlConn()->existRecord("select * from bill_recharge_t where order_id='" + escape_order + "'"))
        {
            stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

            ResponseMessage(stClient.m_iFd, usCmd, stRspBody);
            return true;
        }
    }
    catch (const taf::TC_Mysql_Exception& stEx)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:|MSG:DUPLICATE_ORDER"
                       << "|QUERY ORDER FAIL|" << stEx.what() << "|GROUP_ID:" << nGroupID
                       << "|UIN:" << iUin << "|ORDER_ID:" << stReqBody.m_strOrderID
                       << "|DEPOSIT_ID:" << stReqBody.m_iDepositID << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::INTERN_ERROR;

        LOGIC_SERVICE_API_INS->RequestServiceAlert("request_pay_error", {
                { "uin", std::to_string(iUin) },
                { "group_id", std::to_string(nGroupID) },
                { "deposit", std::to_string(stReqBody.m_iDepositID) },
                { "error", GET_MACRO_STRING(SERVER_ERRCODE::INTERN_ERROR)}
        });

        ResponseMessage(stClient.m_iFd, usCmd, stRspBody);
        return false;
    }

    TPayInfo stPayInfo = {};
    stPayInfo.m_strPayCash = pstDeposit->m_strPrice;
    stPayInfo.m_iActivePayCash = static_cast<int32_t>(round(pstDeposit->m_dPrice * CLogicConfig::GetZooLogicPlatformConfig().m_dPayCashMultiple));
    stPayInfo.m_iPayGameCoin = 0;
    stPayInfo.m_iPayItemType = 0;
    stPayInfo.m_iPayItemID = 0;
    stPayInfo.m_iPayVipExp = pstDeposit->m_iVipExp;
    stPayInfo.m_bIsTestOrder = stReqBody.m_bIsTestOrder;

    auto pstUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
    if (pstUserInfo == nullptr)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
                       << "|DEPOSIT_ID:" << stReqBody.m_iDepositID << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin
                       << "|ORDER_ID:" << stReqBody.m_strOrderID << "|PRICE:" << stPayInfo.m_strPayCash << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

        LOGIC_SERVICE_API_INS->RequestServiceAlert("request_pay_error", {
                { "uin", std::to_string(iUin) },
                { "group_id", std::to_string(nGroupID) },
                { "deposit", std::to_string(stReqBody.m_iDepositID) },
                { "error", GET_MACRO_STRING(SERVER_ERRCODE::USER_ROLE_NOT_EXIST)}
        });

        ResponseMessage(stClient.m_iFd, usCmd, stRspBody);
        return false;
    }

    // 清空缓存道具
    pstUserInfo->m_stCacheBonusData = {};

    auto stDepositIter = pstUserInfo->m_stUserDepositMap.Find(stReqBody.m_iDepositID);
    if (stDepositIter == pstUserInfo->m_stUserDepositMap.End())
    {
        TUserDepositTableValueType stVal;

        stVal.m_iOrderID = stReqBody.m_iDepositID;
        stVal.m_iUid = iUin;
        stVal.m_iGroupID = nGroupID;
        stVal.m_iBuyTimes = 0;
        stVal.m_iTodayBuyTimes = 0;
        stVal.m_iWeekBuyTimes = 0;
        stVal.m_iMonthBuyTimes = 0;
        stVal.m_iDayIntervalBuyTimes = 0;
        stVal.m_iDoubleTimes = 0;
        stVal.m_iLastDepositTime = 0;
        const auto stRet = pstUserInfo->m_stUserDepositMap.Insert(std::make_pair(stReqBody.m_iDepositID, stVal));
        if (!stRet.second)
        {
            using namespace SERVER_ERRCODE;
            LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << INTERN_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                           << "|INSERT_INTO_CACHE_FAIL|DEPOSIT_ID:" << stReqBody.m_iDepositID << "|GROUP_ID:" << nGroupID
                           << "|UIN:" << iUin << "|ORDER_ID:" << stReqBody.m_strOrderID
                           << "|PRICE:" << stPayInfo.m_strPayCash << std::endl;

            stRspBody.m_iCode = SERVER_ERRCODE::INTERN_ERROR;

            LOGIC_SERVICE_API_INS->RequestServiceAlert("request_pay_error", {
                    { "uin", std::to_string(iUin) },
                    { "group_id", std::to_string(nGroupID) },
                    { "deposit", std::to_string(stReqBody.m_iDepositID) },
                    { "error", GET_MACRO_STRING(SERVER_ERRCODE::INTERN_ERROR)}
            });

            ResponseMessage(stClient.m_iFd, usCmd, stRspBody);
            return false;
        }
        stDepositIter = stRet.first;
    }

    const auto iLeftDoubleCount = CLogicUserStat::GetLeftRechargeDoubleCount(pstUserInfo, stReqBody.m_iDepositID);
    stPayInfo.m_iPayGameCoin = pstDeposit->m_iNormalBuyAmount;
    stPayInfo.m_iPayItemType = pstDeposit->m_iBuyItemType;
    stPayInfo.m_iPayItemID = pstDeposit->m_iBuyItemID;

    const int32_t iLeftRechargeCount = CLogicUserStat::GetLeftRechargeBuyCount(pstUserInfo, stReqBody.m_iDepositID);
    if (iLeftRechargeCount <= 0)
    {
        if (stPayInfo.m_iPayVipExp <= 0)
        {//不加vip  是领取奖励  赠送物品
            stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;
            ResponseMessage(stClient.m_iFd, usCmd, stRspBody);
            return true;
        }

        stPayInfo.m_iPayGameCoin = stPayInfo.m_iPayVipExp;
        stPayInfo.m_iPayItemType = 0;
        stPayInfo.m_iPayItemID = 0;
    }

    try
    {
        auto escape_func = [&](const auto& s) { return CLogicSQLTableRecordBase::GetMysqlConn()->escapeString(s);};
        auto sql = build_insert_escape_sql("bill_recharge_t", {
                { "stat_time",    CLogicCommon::GetSec() },
                { "uid",          iUin },
                { "group_id",     nGroupID },
                { "bill_tag",     CLogicBill::GetBillRation()},
                { "order_id",     stReqBody.m_strOrderID },
                { "server_id",    CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID },
                { "channel_id",   stReqBody.m_iChannelID },
                { "nick",         pstUserInfo->m_strNick } ,
                { "pay_cash",     stPayInfo.m_strPayCash },
                { "pay_gamecoin", stPayInfo.m_iPayGameCoin },
                { "pay_type",     stPayInfo.m_iPayItemType },
                { "pay_item",     stPayInfo.m_iPayItemID },
                { "pay_time",     stReqBody.m_iPayTime },
                { "pay_method",   stReqBody.m_strPayMethod },
                { "is_test_order", int32_t(stPayInfo.m_bIsTestOrder)},
                { "additional",    stReqBody.m_strAdditional },
        }, escape_func);

        CLogicSQLTableRecordBase::GetMysqlConn()->execute(sql);
    }
    catch (const taf::TC_Mysql_Exception& stEx)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:|MSG:INTERN_ERROR"
                       << "|INSERT_INTO_BILL_FAIL|" << stEx.what() << "|GROUP_ID:" << nGroupID
                       << "|UIN:" << iUin << "|ORDER_ID:" << stReqBody.m_strOrderID
                       << "|DEPOSIT_ID:" << stReqBody.m_iDepositID << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::INTERN_ERROR;

        LOGIC_SERVICE_API_INS->RequestServiceAlert("request_pay_error", {
                { "uin", std::to_string(iUin) },
                { "group_id", std::to_string(nGroupID) },
                { "deposit", std::to_string(stReqBody.m_iDepositID) },
                { "error", GET_MACRO_STRING(SERVER_ERRCODE::INTERN_ERROR)}
        });

        ResponseMessage(stClient.m_iFd, usCmd, stRspBody);
        return true;
    }

    if (iLeftRechargeCount > 0)
    {
        stDepositIter->second->AddBuyTimes(1);
        if (iLeftDoubleCount > 0)
        {
            stDepositIter->second->AddDoubleTimes(1);
        }
        stDepositIter->second->AddTodayBuyTimes(1);
        stDepositIter->second->AddWeekBuyTimes(1);
        stDepositIter->second->AddMonthBuyTimes(1);
        stDepositIter->second->AddDayIntervalBuyTimes(1);
        if(pstDeposit->m_iDayIntervalNum <= 0 || day_diff(stDepositIter->second->GetLastDepositTime(), CLogicCommon::GetSec()) >= pstDeposit->m_iDayIntervalNum)
        {
            stDepositIter->second->SetLastDepositTime(CLogicCommon::GetSec());
        }
    }

    std::vector<CPackGameItem> stVector;
    if(stPayInfo.m_iPayItemType > 0) stVector.emplace_back(stPayInfo.m_iPayItemType, stPayInfo.m_iPayItemID, 1);
    stVector.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_PayYuanBaoID, stPayInfo.m_iPayGameCoin);
    stVector.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_VIPExpID, stPayInfo.m_iPayVipExp);
    //添加购买立即获得
    stVector.insert(stVector.end(),pstDeposit->m_stFirstReward.begin(),pstDeposit->m_stFirstReward.end());

    //月卡邮件奖励
    if(!CLogicBuff::IsHaveMonthCard(pstUserInfo) && stReqBody.m_iDepositID == VIP_MONTHLY_DEPOSIT_ID)
    {
        auto& stConfig = CLogicConfig::GetAwardConfig().m_stMonthCardConfig;
        if(stConfig.m_stAwardItems.size() > 0)
        {
            CLogicGlobal::AddAsyncUserMail(pstUserInfo->m_iUin, pstUserInfo->m_iGroupID,
                                           stConfig.m_strSender, stConfig.m_strTitle, stConfig.m_strContent,
                                           stConfig.m_stAwardItems,
                                           "SEND_MONTH_CARD_DAILY_AWARD",1,false,stConfig.m_iExpiredDay);
        }
    }

    //体力月卡邮件奖励
    if(!CLogicBuff::IsHaveFatMonthCard(pstUserInfo) && stReqBody.m_iDepositID == FAT_MONTHLY_DEPOSIT_ID)
    {
        auto& stConfig = CLogicConfig::GetAwardConfig().m_stFatMonthCardConfig;
        if(stConfig.m_stAwardItems.size() > 0)
        {
            CLogicGlobal::AddAsyncUserMail(pstUserInfo->m_iUin, pstUserInfo->m_iGroupID,
                                           stConfig.m_strSender, stConfig.m_strTitle, stConfig.m_strContent,
                                           stConfig.m_stAwardItems,
                                           "SEND_FAT_MONTH_CARD_DAILY_AWARD",1,false,stConfig.m_iExpiredDay);
        }
    }

    CLogicCacheCommon::AddGameItem(pstUserInfo, stVector, usCmd);

    CLogicUserStat::OnRecharge(pstUserInfo, stPayInfo.m_iActivePayCash);

    pstUserInfo->m_stUserInfoTable.AddAddUpRmb(stPayInfo.m_iActivePayCash);

    //首次充值
    bool bFirstRecharge = false;
    if (pstUserInfo->m_stFirstRechargeMap.Empty() && !stPayInfo.m_bIsTestOrder)
    {
        TFirstRechargeTableValueType stVal;
        stVal.m_iUid = iUin;
        stVal.m_iGroupID = nGroupID;
        stVal.m_iLevel = pstUserInfo->m_stUserInfoTable.GetLevel();
        stVal.m_strPayCash = stPayInfo.m_strPayCash;
        stVal.m_iPayGamecoin = stPayInfo.m_iPayGameCoin;
        stVal.m_iPayType = stPayInfo.m_iPayItemType;
        stVal.m_iPayItem = stPayInfo.m_iPayItemID;
        stVal.m_strStatDate = CLogicCommon::GetDateString();
        stVal.m_iCreateRoleTime =pstUserInfo->m_stUserInfoTable.GetCreateTime();
        stVal.m_iLastLoginTime = pstUserInfo->m_stUserInfoTable.GetLastLogin();

        pstUserInfo->m_stFirstRechargeMap.Insert(std::make_pair(stVal.m_iUid, stVal));
        bFirstRecharge = true;
    }

    // ZERO Daily RECHARGE
    const int iLastDepositTime = pstUserInfo->m_stUserInfoTable.GetLastDepositTime();
    if (LOGIC_CHECK_DAY_REFRESH(iLastDepositTime))
    {
        pstUserInfo->m_stUserInfoTable.SetDailyDeposit(stPayInfo.m_iActivePayCash);
        pstUserInfo->m_stUserInfoTable.SetDailyDepositBonus(0);
        pstUserInfo->m_stUserInfoTable.SetLastDepositTime(CLogicCommon::GetSec());
    }
    else
    {
        pstUserInfo->m_stUserInfoTable.AddDailyDeposit(stPayInfo.m_iActivePayCash);
        pstUserInfo->m_stUserInfoTable.SetLastDepositTime(CLogicCommon::GetSec());
    }

    // 每月累计充值
    if(LOGIC_CHECK_MONTH_REFRESH(iLastDepositTime))
    {
        pstUserInfo->m_stUserInfoTable.SetMonthAddUpRmb(stPayInfo.m_iActivePayCash);
    }
    else
    {
        pstUserInfo->m_stUserInfoTable.AddMonthAddUpRmb(stPayInfo.m_iActivePayCash);
    }

    if (!stPayInfo.m_bIsTestOrder)
    {
        LOGIC_STATISTICS_INS->AddPlayerDailyPayCash(iUin, nGroupID, stPayInfo.m_iActivePayCash, pstUserInfo->m_stUserInfoTable.GetAddUpRmb());
    }

    stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;
    /////////////////////////////////////////////////////////////////////
    ///////////////// Notify Deposit
    CGMNotifyRecharge notify;
    notify.m_iDepositID = iLeftRechargeCount > 0 ? stReqBody.m_iDepositID : 0;
    notify.m_iPrice = stPayInfo.m_iActivePayCash;
    notify.m_iTotalAddupRmb = pstUserInfo->m_stUserInfoTable.GetAddUpRmb();
    notify.m_strOrderID = stReqBody.m_strOrderID;
    notify.m_iPayGamecoin = stPayInfo.m_iPayGameCoin;
    notify.m_stItemData = pstUserInfo->m_stCacheBonusData;

    std::string STR_NOTIFY_FLAG = "|NOTIFY_USER_FAILED";

    if (LOGIC_CONNECTOR_SENDER->NotifyClientUser(nGroupID, iUin, MSG_LOGIC_USER_RECHARGE_NOTIFY, notify))
    {
        STR_NOTIFY_FLAG = "|NOTIFY_USER_SUCCESS";
    }

    logic_game_user_action_channel_data_t stActionData;
    stActionData.m_iTimes = stPayInfo.m_iActivePayCash;
    stActionData.m_iAction = CLogicGameChannelEnum::ACTION_RECHARGE;
    LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(pstUserInfo, stActionData);

    /////////////////////////////////////////////////////////////////////
    /////////////////
    CLogicGameDepositChannel::data_type stData;
    stData.m_iNum = stPayInfo.m_iActivePayCash;
    LOGIC_PUBLISHER_INS->PublishGameDepositInfo(pstUserInfo, stData);

    /////////////////////////////////////////////////////////////////////
    ///////////////// Activity Delegate
    LOGIC_ACTIVITY_CHANGE_LIST_INS->NotifyClientUser(nGroupID, iUin);

    stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;
    stRspBody.m_bIsNew = 1;

    LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|DEPOSIT_ID" << stReqBody.m_iDepositID << STR_NOTIFY_FLAG << "|GROUP_ID:" << nGroupID
                   << "|UIN:" << iUin << "|ORDER_ID:" << stReqBody.m_strOrderID
                   << "|COMMODITY_ID:" << notify.m_iDepositID << "|PRICE:" << notify.m_iPrice << std::endl;

    ResponseMessage(stClient.m_iFd, usCmd, stRspBody);

    // 公告
    CLogicGlobal::PublishDepositBroadcast(pstUserInfo->m_strNick, notify.m_iDepositID);
    // 记录付费钻石订单剩余信息
    auto payDiamondLeft = pstUserInfo->m_stUserPayDiamondTable.GetPayDiamondLeft();
    auto payDiamondRationConfig = CLogicConfig::GetDepositConfig().GetPayDiamondRatioConfig(stReqBody.m_iDepositID);
    if (nullptr != payDiamondRationConfig)
    {
        LOGIC_LOG_TRACE_DEBUG << "PROCESS" << payDiamondRationConfig->m_iDiamondNum << "|" << payDiamondRationConfig->m_iPriceNum
                              << std::endl;
        CPackPayDiamond payDiamondRatio;
        payDiamondRatio.m_iNum = payDiamondRationConfig->m_iDiamondNum;
        payDiamondRatio.m_iRatio = payDiamondRationConfig->m_iPriceNum / payDiamondRationConfig->m_iDiamondNum;
        payDiamondLeft.emplace_back(payDiamondRatio);
        pstUserInfo->m_stUserPayDiamondTable.SetPayDiamondLeft(payDiamondLeft);
    }

    //数据打点上报
    CDataReportPay params;
    params.m_iDepositID = stReqBody.m_iDepositID;
    params.m_iProductID = stReqBody.m_iProductID;
    params.m_iChannelID = stReqBody.m_iChannelID;
    params.m_strOrderID = stReqBody.m_strOrderID;
    params.m_iPayTime = stReqBody.m_iPayTime;
    params.m_strPayPrice = stReqBody.m_strPayPrice;
    params.m_strPayMethod = stReqBody.m_strPayMethod;
    params.m_bIsGM = stReqBody.m_bIsTestOrder;
    params.m_bFirst = bFirstRecharge;

    params.m_stCommon.m_strEven = "tga_pay";
    params.m_stCommon.m_strAccount = pstUserInfo->m_strPid;
    params.m_stCommon.m_strUdid = pstUserInfo->m_strDeviceID;
    params.m_stCommon.m_strNickName = pstUserInfo->m_strNick;


    CMsgHeadCross stMsgHead;
    stMsgHead.m_byCmdType = enmCmdType_request;
    stMsgHead.m_usCmd = SERVER_CMD_DATA_REPORT_DEFINE::MSG_REPORT_PAY;
    stMsgHead.m_iUin = pstUserInfo->m_iUin;
    stMsgHead.m_nGroupID = pstUserInfo->m_iGroupID;
    LOGIC_SERVICE_API_INS->GetCrossConnection()->SendToServer(stMsgHead, params);
    return true;
}

bool CLogicAdminProcessor::ProcessModifyItem(int32_t iUin, int16_t nGroupID, uint16_t usCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestModifyItem stReqBody;
    CGMResponseModifyItem stRspBody;

    if (!DecodeClient(stClient, pProtoName, stReqBody))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                       << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    auto pstUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
    if (!pstUserInfo)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
                       << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

        ResponseMessage(stClient.m_iFd, usCmd, stRspBody);

        return false;
    }

    pstUserInfo->m_stCacheBonusData = {};

    stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;
    for (const auto& item : stReqBody.m_stItems)
    {
        bool ret = false;
        if (item.m_iNum >= 0)
        {
            ret = CLogicCacheCommon::AddGameItem(pstUserInfo, item.m_iItemType, item.m_iCardID, item.m_iNum, usCmd);
        }
        else
        {
            ret = CLogicCacheCommon::ConsumeGameItem(pstUserInfo, item.m_iItemType, item.m_iCardID, -item.m_iNum, usCmd);
        }

        if (ret)
        {
            stRspBody.m_stSuccess.push_back(item.m_iCardID);
        }
        else
        {
            stRspBody.m_stFail.push_back(item.m_iCardID);
        }
    }

    stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

    LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":"
                   << stClient.m_usClientPort << "|" << pProtoName
                   << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

    CGMNotifyModifyItem notify;
    notify.m_stItems = pstUserInfo->m_stCacheBonusData;
    LOGIC_CONNECTOR_SENDER->NotifyClientUser(nGroupID, iUin, MSG_LOGIC_NOTIFY_MODIFY_ITEM_EXTERNAL, notify);

    ResponseMessage(stClient.m_iFd, usCmd, stRspBody);

    return true;
}

bool CLogicAdminProcessor::ProcessAddSystemMail(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestAddSystemMail stReqBody;
    CGMResponseAddSystemMail stRspBody;

    if(!DecodeClient(stClient, pProtoName, stReqBody))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                        << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    for (const auto& item : stReqBody.m_stAttachment)
    {
        if (!LOGIC_GAME_ITEM_FACTORY_INS->CheckGameItem((CLogicConfigDefine::GAME_ITEM_COLLECTION)item.m_iItemType, item.m_iCardID))
        {
            using namespace SERVER_ERRCODE;
            LOGIC_LOG_TRACE_ERROR << "PROCESS ADD SYSTEM MAIL ERROR:" << UNKNOW_GAME_ITEM_NOT_IN_CONFIG
                           << "|MSG:" << GET_ERRMSG_BY_ERRCODE(UNKNOW_GAME_ITEM_NOT_IN_CONFIG)
                           << "|INSERT_INTO_SYSTEM_MAIL_FAILD|" << "|GROUP_ID:" << nGroupID
                           << "|BEGIN_TIME:" << stReqBody.m_iBeginTime << "|END_TIME:" << stReqBody.m_iEndTime
                           << "|TITLE:" << stReqBody.m_strTitle << "|CONTENT:" << stReqBody.m_strContent
                           << "|ITEM_TYPE:" << item.m_iItemType << "|ITEM_ID:" << item.m_iCardID
                           << std::endl;

            stRspBody.m_iCode = SERVER_ERRCODE::UNKNOW_GAME_ITEM_NOT_IN_CONFIG;

            ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
            return false;
        }
    }

    if(stReqBody.m_stChannelVec.find(0) != stReqBody.m_stChannelVec.end())
    {
        stReqBody.m_stChannelVec.clear();
    }

	if (!CLogicGlobal::AddGlobalMail(stReqBody.m_iBeginTime, stReqBody.m_iEndTime, stReqBody.m_iVipLevel, stReqBody.m_strSender,
                                     stReqBody.m_strTitle, stReqBody.m_strContent, stReqBody.m_stAttachment, stReqBody.m_stChannelVec))
	{
		stRspBody.m_iCode = SERVER_ERRCODE::INTERN_ERROR;

		ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
		return false;
	}

    stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

    ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

    return true;
}

bool CLogicAdminProcessor::ProcessSetUserBan(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestSetUserBan stReqBody;
    if(!DecodeClient(stClient, pProtoName, stReqBody))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                        << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    CGMResponseSetUserBan stRspBody;
    stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;
    if(!CLogicConfigDefine::IsValidEnumUserBanType(stReqBody.m_iBanType))
    {
        stRspBody.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;
        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
        return true;
    }

    auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
    if(stUserInfo)
    {
        switch (stReqBody.m_iBanType)
        {
        case CLogicConfigDefine::EUBT_BanUser:
        {
            if(CLogicCacheCommon::IsBanDelUser(stUserInfo))
            {
                stRspBody.m_iCode = SERVER_ERRCODE::USER_IS_IN_BAN_TIME;
                ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
                return true;
            }

            break;
        }
        case CLogicConfigDefine::EUBT_BanTalk:
        {
            //被封的账号不能禁言
            if(CLogicCacheCommon::IsBanAccount(stUserInfo))
            {
                stRspBody.m_iCode = SERVER_ERRCODE::USER_IS_IN_BAN_TIME;
                ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
                return true;
            }

            break;
        }
        }

		stUserInfo->m_stUserInfoTable.SetBanType(stReqBody.m_iBanType);
        stUserInfo->m_stUserInfoTable.SetBanBeginTime(CLogicCommon::GetSec());
        stUserInfo->m_stUserInfoTable.SetBanEndTime(CLogicCommon::GetSec() + stReqBody.m_iDuration);
		stUserInfo->m_stUserInfoTable.SetBanComment(stReqBody.m_strComment);

		if (CLogicCacheCommon::IsBanAccount(stUserInfo))
		{
			CLogicRankList::RemoveUserRank(iUin, nGroupID);
		}

		LOGIC_GLOBAL_FRAME_INS->RemoveNickWorldTalk(stUserInfo->m_strNick);

        stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

        LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort << "|"
                        << pProtoName << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << "|SET_USER_BAN_DURATION:" << stReqBody.m_iDuration
    					<< "SET USER BAN COMMNET:" << stReqBody.m_strComment << std::endl;
    }
    else
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort << "|"
                        << pProtoName << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << "|USER_ROLE_NOT_EXIST"
						<< "SET USER BAN COMMNET:" << stReqBody.m_strComment << std::endl;
    }

    ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

    return true;
}

bool CLogicAdminProcessor::ProcessDeleteSystemMail(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestDeleteSystemMail stReqBody;
    if(!DecodeClient(stClient, pProtoName, stReqBody))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                        << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    CGMResponseDeleteSystemMail stRspBody;
    stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

    CLogicGlobal::DelGlobalMail(stReqBody.m_iBeginTime, stReqBody.m_iMailID);


    LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":"
                    << stClient.m_usClientPort << "|" << pProtoName << "|GROUP_ID:"
                    << nGroupID << "|MAIL_ID:" << stReqBody.m_iMailID
                    << "|BEGIN_TIME:" << stReqBody.m_iBeginTime << std::endl;

    ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

    return true;
}

bool CLogicAdminProcessor::ProcessGiftCardAward(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestGiftCardAward stReqBody;
    CGMResponseGiftCardAward stRspBody;

    if (!DecodeClient(stClient, pProtoName, stReqBody))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                       << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    std::string strBonusItem;
    stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;
    auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
    if (stUserInfo)
    {
        stUserInfo->m_stCacheBonusData = {};
        CLogicCacheCommon::AddGameItem(stUserInfo, stReqBody.m_stItems, strBonusItem, iCmd);

        stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

        // 通知客户端
        CGMNotifyModifyItem notify;
        notify.m_stItems = stUserInfo->m_stCacheBonusData;
        LOGIC_CONNECTOR_SENDER->NotifyClientUser(nGroupID, iUin, MSG_LOGIC_NOTIFY_MODIFY_ITEM_EXTERNAL, notify);

        LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort << "|"
                       << pProtoName << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << "|CARD:" << stReqBody.m_strCardCode
                       << "|BONUS:" << strBonusItem << std::endl;
    }
    else
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort << "|"
                       << pProtoName << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << "|USER_ROLE_NOT_EXIST" << std::endl;
    }

    ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

    return true;
}

bool CLogicAdminProcessor::ProcessSendActiveMail(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestSendActiveMail stReqBody;
    if (!DecodeClient(stClient, pProtoName, stReqBody))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                        << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    CGMResponseSendActiveMail stRspBody;
    stRspBody.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;
    for (const auto& stGameItem : stReqBody.m_stBonusVec)
    {
        if (!LOGIC_GAME_ITEM_FACTORY_INS->CheckGameItem(
                    (CLogicConfigDefine::GAME_ITEM_COLLECTION)
                    stGameItem.m_iItemType,
                    stGameItem.m_iCardID))
        {
            LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                << "|" << pProtoName << "|INVALID GAME ITEM" << std::endl;

            ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
            return false;
        }
    }

    stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;
    auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
    if(stUserInfo)
    {
        CLogicGlobal::AddAsyncUserMail(stUserInfo->m_iUin, stUserInfo->m_iGroupID, stReqBody.m_strSender,
                                    stReqBody.m_strTitle, stReqBody.m_strContent, stReqBody.m_stBonusVec, pProtoName);

        stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

        LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort << "|"
                        << pProtoName << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;
    }
    else
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort << "|"
                        << pProtoName << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << "|USER_ROLE_NOT_EXIST" << std::endl;
    }

    ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

    return true;
}

bool CLogicAdminProcessor::ProcessModifyChapter(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
	CGMRequestModifyChapter stReqBody;
	CGMResponseModifyChapter stRspBody;

	if (!DecodeClient(stClient, pProtoName, stReqBody))
	{
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
			<< "|" << pProtoName << "|DECODE ERROR" << std::endl;

		stClient.Clear();
		return false;
	}

    stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;
	auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
	if (!stUserInfo)
	{
		using namespace SERVER_ERRCODE;
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
			<< "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

		stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

		ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

		return false;
	}

	if (stReqBody.m_iChapterType == CLogicConfigDefine::NORMAL_CHAPTER)
	{
		const auto& rstConfig = CLogicConfig::GetLevelConfig().m_stLevelConfig;
		for (const auto& iter : rstConfig)
		{
			if (iter.first >= 0 && iter.first < CLogicConfigDefine::NORMAL_CHAPTER_LIMIT && iter.first <= stReqBody.m_iLevelID)
			{
				if (ThreeStarPassLevel(stUserInfo, iCmd, iter.first))
				{
					LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
						<< "|" << pProtoName << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << "|LEVEL ID:" << iter.first << std::endl;
				}
			}
		}
	}
	else if (stReqBody.m_iChapterType == CLogicConfigDefine::ELITE_CHAPTER)
	{
	    const auto& rstConfig = CLogicConfig::GetLevelConfig().m_stLevelConfig;
		for (const auto& iter : rstConfig)
		{
			if (iter.first >= CLogicConfigDefine::NORMAL_CHAPTER_LIMIT && iter.first < CLogicConfigDefine::ELITE_CHAPTER_LIMIT && iter.first <= stReqBody.m_iLevelID)
			{
				if (ThreeStarPassLevel(stUserInfo, iCmd, iter.first))
				{
					LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
						<< "|" << pProtoName << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << "|LEVEL ID:" << iter.first << std::endl;
				}
			}
		}
	}
	else if (stReqBody.m_iChapterType == CLogicConfigDefine::DEVIL_CHAPTER)
	{
		const auto& rstConfig = CLogicConfig::GetLevelConfig().m_stLevelConfig;
		for (const auto& iter : rstConfig)
		{
			if (iter.first >= CLogicConfigDefine::ELITE_CHAPTER_LIMIT && iter.first < CLogicConfigDefine::DEVIL_CHAPTER_LIMIT && iter.first <= stReqBody.m_iLevelID)
			{
				if (ThreeStarPassLevel(stUserInfo, iCmd, iter.first))
				{
					LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
						<< "|" << pProtoName << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << "|LEVEL ID:" << iter.first << std::endl;
				}
			}
		}
	}
	else if (stReqBody.m_iChapterType == CLogicConfigDefine::TOWER_CHAPTER)
	{
		auto pstConfig = CLogicConfig::GetClimbTowerConfig().GetConfigElemByLevel(stReqBody.m_iLevelID);
		if (nullptr != pstConfig)
		{
			stUserInfo->m_stUserInfoTable.SetClimbTowerCurrentOrder(pstConfig->m_iOrder);
			stUserInfo->m_stUserInfoTable.SetClimbTowerMaxOrder(
				std::max(pstConfig->m_iOrder, stUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder())
			);
		}
	}

	stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

	LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":"
		<< stClient.m_usClientPort << "|" << pProtoName
		<< "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

	ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

	return true;
}

bool CLogicAdminProcessor::ProcessModifyHeroCard(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
	CGMRequestModifyHeroCard stReqBody;
	CGMResponseModifyHeroCard stRspBody;

	if (!DecodeClient(stClient, pProtoName, stReqBody))
	{
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
			<< "|" << pProtoName << "|DECODE ERROR" << std::endl;

		stClient.Clear();
		return false;
	}

    stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;
	auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
	if (!stUserInfo)
	{
		using namespace SERVER_ERRCODE;
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
			<< "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

		stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

		ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

		return false;
	}

    const auto* pstCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stReqBody.m_iHeroID);
    if (nullptr == pstCardConfig)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << HERO_CARD_IS_NOT_CONFIGED << "|MSG:" << GET_ERRMSG_BY_ERRCODE(HERO_CARD_IS_NOT_CONFIGED)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED;

        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

	auto iter =  stUserInfo->m_stHeroCardTableMap.Find(stReqBody.m_iHeroID);
	if (iter == stUserInfo->m_stHeroCardTableMap.End())
	{
		CLogicCacheCommon::AddGameItem(stUserInfo, CLogicConfigDefine::HERO_CARD, stReqBody.m_iHeroID, 1, iCmd);
		iter = stUserInfo->m_stHeroCardTableMap.Find(stReqBody.m_iHeroID);
	}

	if (iter == stUserInfo->m_stHeroCardTableMap.End())
	{
		using namespace SERVER_ERRCODE;
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << GAME_ITEM_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(GAME_ITEM_NOT_EXIST)
			<< "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

		stRspBody.m_iCode = SERVER_ERRCODE::GAME_ITEM_NOT_EXIST;

		ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

		return false;
	}

	if (stReqBody.m_iHeroLevel > 0)
	{
		iter->second->SetCardLevel(stReqBody.m_iHeroLevel);
	}

	if (stReqBody.m_iHeroColor > 0)
	{
		iter->second->SetCardColor(stReqBody.m_iHeroColor);
	}

	if (stReqBody.m_iHeroStar > 0)
	{
		iter->second->SetCardStar(stReqBody.m_iHeroStar);
	}

    if(stReqBody.m_iSkillLevel > 0)
    {
        auto targetSkill = 0;
        auto stData = iter->second->GetTalentMap();
        for(auto talentIter: pstCardConfig->m_stTalentMap)
        {
            targetSkill = stReqBody.m_iSkillLevel < talentIter.second.m_iMaxLevel ? stReqBody.m_iSkillLevel:talentIter.second.m_iMaxLevel;
            stData[talentIter.first] = targetSkill;
            iter->second->AddTalentTotalConsume(talentIter.second.m_iConsumePoint * targetSkill);
        }
        iter->second->SetTalentMap(stData);
    }

	stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

	LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
	    << "|" << pProtoName << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

	ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

	return true;
}

bool CLogicAdminProcessor::ProcessModifyCreateTime(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
	CGMRequestModifyCreateTime stReqBody;
	CGMResponseModifyCreateTime stRspBody;

	if (!DecodeClient(stClient, pProtoName, stReqBody))
	{
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
			<< "|" << pProtoName << "|DECODE ERROR" << std::endl;

		stClient.Clear();
		return false;
	}

	auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
	if (!stUserInfo)
	{
		using namespace SERVER_ERRCODE;
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
			<< "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

		stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

		ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

		return false;
	}

	stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

	stUserInfo->m_stUserInfoTable.SetCreateTime(stReqBody.m_iCreateTime);

	stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

	LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
	    << "|" << pProtoName << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

	ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

	return true;
}

bool CLogicAdminProcessor::ProcessModifyGuide(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
	CGMRequestModifyGuide stReqBody;
	CGMResponseModifyGuide stRspBody;

	if (!DecodeClient(stClient, pProtoName, stReqBody))
	{
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
			<< "|" << pProtoName << "|DECODE ERROR" << std::endl;

		stClient.Clear();
		return false;
	}

	auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
	if (!stUserInfo)
	{

		using namespace SERVER_ERRCODE;
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
			<< "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

		stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

		ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

		return false;
	}

	stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

	stUserInfo->m_stUserInfoTable.SetGuideMajorStep(stReqBody.m_iMajorStep);
	stUserInfo->m_stUserInfoTable.SetGuideMinorStep(stReqBody.m_iMinorStep);

	stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

	LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
	    << "|" << pProtoName << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

	ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

	return true;
}

bool CLogicAdminProcessor::ProcessModifyGuild(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CCrossRequestModifyGuild stReqBody;
    CCrossResponseModifyGuild stRspBody;

	if (!DecodeClient(stClient, pProtoName, stReqBody))
	{
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
			<< "|" << pProtoName << "|DECODE ERROR" << std::endl;

		stClient.Clear();
		return false;
	}

    if(stReqBody.m_iGuildID <= 0 || stReqBody.m_iGuildExp <= 0)
    {
        stRspBody.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;
        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    if (LOGIC_SERVICE_API_INS->GetCrossConnection()->IsConnected())
    {
        CMsgHeadCross stHead;
        stHead.m_byCmdType = enmCmdType_request;
        stHead.m_usCmd = MSG_CROSS_ADMIN_MODIFY_GUILD;
        stHead.m_nGroupID = static_cast<int16_t>(CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID);

        stReqBody.m_iFd = stClient.m_iFd;
        LOGIC_SERVICE_API_INS->GetCrossConnection()->SendToServer(stHead, stReqBody);
    }
    else
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << MATCH_SERVER_NOT_CONN << "|MSG:" << GET_ERRMSG_BY_ERRCODE(MATCH_SERVER_NOT_CONN)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = MATCH_SERVER_NOT_CONN;
        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

	return true;
}

bool CLogicAdminProcessor::ProcessDeleteHero(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
	CGMRequestDeleteHero stReqBody;
	CGMResponseDeleteHero stRspBody;

	if (!DecodeClient(stClient, pProtoName, stReqBody))
	{
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
			<< "|" << pProtoName << "|DECODE ERROR" << std::endl;

		stClient.Clear();
		return false;
	}

	auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
	if (!stUserInfo)
	{
		using namespace SERVER_ERRCODE;
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
			<< "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

		stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

		ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

		return false;
	}

	stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

	stUserInfo->m_stHeroCardTableMap.Erase(stReqBody.m_iHeroID);

	stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

	LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
	    << "|" << pProtoName << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

	ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

	return true;
}

bool CLogicAdminProcessor::ProcessDeleteRank(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
	CGMRequestDeleteRank stReqBody;
	CGMResponseDeleteRank stRspBody;

	if (!DecodeClient(stClient, pProtoName, stReqBody))
	{
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
			<< "|" << pProtoName << "|DECODE ERROR" << std::endl;

		stClient.Clear();
		return false;
	}

	if (stReqBody.m_strRank == "pata")
	{
		while (!LOGIC_CLIMB_TOWER_RANK_INS.empty())
		{
			auto ulRoleID = LOGIC_CLIMB_TOWER_RANK_INS.begin()->first;
            LOGIC_CLIMB_TOWER_RANK_INS.erase(ulRoleID);
		}
	}

	stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

	LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":"
		<< stClient.m_usClientPort << "|" << pProtoName << "|RANK:" << stReqBody.m_strRank << std::endl;

	ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

	return true;
}

bool CLogicAdminProcessor::ProcessClearCache(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
	CGMResponseClearCache stRspBody;

	LOGIC_USER_DATA_HASHMAP_INS->erase(user_info_key_t(iUin, nGroupID).m_ulRoleID);

	stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

	LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":"
		<< stClient.m_usClientPort << "|" << pProtoName << "|UIN:" << iUin << std::endl;

	ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

	return true;
}

bool CLogicAdminProcessor::ProcessSetGuildBulletin(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    /*
	CGMRequestSetGuildBulletin stReqBody;
	CGMResponseSetGuildBulletin stRspBody;

	if (!DecodeClient(stClient, pProtoName, stReqBody))
	{
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
			<< "|" << pProtoName << "|DECODE ERROR" << std::endl;

		stClient.Clear();
		return false;
	}

	auto* pstGuild = CLogicGuild::GetGuild(stReqBody.m_iGuildID);
    if (!pstGuild)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << GUILD_NOT_FOUND << "|MSG:" << GET_ERRMSG_BY_ERRCODE(GUILD_NOT_FOUND)
                       << "|GROUP_ID:" << nGroupID << "|GUILD_ID:" << stReqBody.m_iGuildID << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::GUILD_NOT_FOUND;

        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    pstGuild->SetGuildBulletin("");
    stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;

	LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":"
		<< stClient.m_usClientPort << "|" << pProtoName << "|GUILD:" << stReqBody.m_iGuildID << std::endl;

	ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
*/
	return true;
}

bool CLogicAdminProcessor::ProcessJubaoAward(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestJubaoAward stReqBody;
    CGMResponseJubaoAward stRspBody;

	if (!DecodeClient(stClient, pProtoName, stReqBody))
	{
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
			<< "|" << pProtoName << "|DECODE ERROR" << std::endl;

		stClient.Clear();
		return false;
	}

	auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
	if (!stUserInfo)
	{
		using namespace SERVER_ERRCODE;
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
			<< "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

		stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

		ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

		return false;
	}

	if(!CLogicGlobal::AddAsyncEventMail(stUserInfo->m_iUin, stUserInfo->m_iGroupID, stReqBody.m_strMailID, {{"nick", stReqBody.m_strDstNick}}))
	{
		using namespace SERVER_ERRCODE;
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
			<< "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

		stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

		ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

		return false;
	}

	stRspBody.m_iCode = SERVER_ERRCODE::SUCCESS;
	if (LOGIC_CHECK_DAY_REFRESH(stUserInfo->m_stUserInfoTable.GetLastJubaoAwardTime()))
	{
		stUserInfo->m_stUserInfoTable.SetTodayJubaoAwardCount(1);
	}
	else
	{
		stUserInfo->m_stUserInfoTable.AddTodayJubaoAwardCount(1);
	}

	stUserInfo->m_stUserInfoTable.AddJubaoAwardCount(1);
	stUserInfo->m_stUserInfoTable.SetLastJubaoAwardTime(CLogicCommon::GetSec());

	LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort << "|" << pProtoName  << std::endl;

	ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

	return true;
}

bool CLogicAdminProcessor::ProcessUserRank(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
	CGMResponseUserRank stRspBody;
	stRspBody.m_stPVPRankVec = CLogicRankList::GetUserRank(LOGIC_PVP_RANK_INS);
	stRspBody.m_stPowerRankVec = CLogicRankList::GetUserRank(LOGIC_FIGHT_POWER_RANK_INS);
	stRspBody.m_stTowerRankVec = CLogicRankList::GetUserRank(LOGIC_CLIMB_TOWER_RANK_INS);
	stRspBody.m_stCountdownRankVec = CLogicRankList::GetUserRank(LOGIC_COUNTDOWN_HERO_RANK_INS);
	stRspBody.m_stMilitarkRankVec = CLogicRankList::GetUserRank(LOGIC_MILITARY_LEVEL_RANK_INS);
	stRspBody.m_stHomeExpRankVec = CLogicRankList::GetUserRank(LOGIC_HOME_RANK_INS);
	stRspBody.m_stEntireWarRankVec = CLogicRankList::GetUserRank(LOGIC_ENTIRE_WAR_RANK_INS);
	stRspBody.m_stCharmRankVec = CLogicRankList::GetUserRank(LOGIC_CHARM_RANK_INS);
	stRspBody.m_stMatchPvpRankVec = CLogicRankList::GetUserRank(LOGIC_MATCH_PVP_RANK_INS);

	ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

	return true;
}

bool CLogicAdminProcessor::ProcessUserCrossDay(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
	CGMResponseCrossDay stRspBody;

	auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
	if (!stUserInfo)
	{
		using namespace SERVER_ERRCODE;
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
			<< "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

		stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

		ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

		return false;
	}

	stUserInfo->m_stUserDailyTable.SetLastRefreshTime(1);

	LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":"
		<< stClient.m_usClientPort << "|" << pProtoName << std::endl;

	ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

	return true;
}

bool CLogicAdminProcessor::ProcessAddFish(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestAddFish stReqBody;
    CGMResponseBody stRspBody;
    if (!DecodeClient(stClient, pProtoName, stReqBody))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                              << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    auto pUserData = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
    if (!pUserData)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    auto pConfig = CLogicConfig::GetHomeFuncConfig().GetFishConfig(stReqBody.m_iFishID);
    if(!pConfig)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << COMMON_CONFIG_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(COMMON_CONFIG_NOT_EXIST)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST;
        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    if(!CLogicConfigDefine::IsValidEnumCardGradeType(stReqBody.m_iGrade))
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << PARAMETER_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;
        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    CGMNotifyModifyItem stNotify;
    CPackHomeFishElem stFish;
    for(int i = 0; i < stReqBody.m_iNum; ++i)
    {
        TUserFishItemTableValueType stValue;
        stValue.m_iUid = iUin;
        stValue.m_iGroupID = nGroupID;
        stValue.m_iFishIndex = stFish.m_iFishIndex = CLogicCacheCommon::GetUserPrimaryKeyIndex(pUserData);
        stValue.m_iFishID = stFish.m_iFishID = stReqBody.m_iFishID;
        stValue.m_cGrade = stFish.m_cGrade = stReqBody.m_iGrade;
        pUserData->m_stFishItemMap.Insert(std::make_pair(stValue.m_iFishIndex, stValue));

        stNotify.m_stFishVec.push_back(stFish);
    }

    LOGIC_CONNECTOR_SENDER->NotifyClientUser(nGroupID, iUin, MSG_LOGIC_NOTIFY_MODIFY_ITEM_EXTERNAL, stNotify);

    LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort << "|" << pProtoName << std::endl;

    ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
    return true;
}

bool CLogicAdminProcessor::ProcessAddEquip(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestAddEquip stReqBody;
    CGMResponseBody stRspBody;

    if (!DecodeClient(stClient, pProtoName, stReqBody))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                              << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
    if (!stUserInfo)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    if(!CLogicConfigDefine::IsValidEnumCardGradeType(stReqBody.m_iGrade) ||
       !CLogicConfigDefine::IsValidEnumHeroEquipType(stReqBody.m_iEquipTag) ||
       !CLogicConfig::GetEquipAttrConfig().GetAttrConfig(stReqBody.m_iIndex))
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << PARAMETER_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;
        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
        return false;
    }

    TUserEquipTableValueType stValue;
    stValue.m_iUid = iUin;
    stValue.m_iGroupID = nGroupID;
    stValue.m_iUniqueID = CLogicCacheCommon::GetUserPrimaryKeyIndex(stUserInfo);
    stValue.m_iGrade = stReqBody.m_iGrade;
    stValue.m_iEquipmentTag = stReqBody.m_iEquipTag;
    stValue.m_iAttrIndex = stReqBody.m_iIndex;
    auto iterResult = stUserInfo->m_stEquipMap.Insert(std::make_pair(stValue.m_iUniqueID, stValue));
    if(!iterResult.second)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << INTERN_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::INTERN_ERROR;
        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
        return false;
    }

    auto& iterData = iterResult.first;

    // bill 记录
    CLogicBill::SaveEquipBill(iUin, nGroupID, stUserInfo->m_iChannelID, iCmd, 1, iterData->second->m_stData);

    LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort << "|" << pProtoName << std::endl;

    ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
    return true;
}

bool CLogicAdminProcessor::ProcessDismissGuild(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CCrossRequestDismissGuild stReqBody;
    CCrossResponseModifyGuild stRspBody;

	if (!DecodeClient(stClient, pProtoName, stReqBody))
	{
		LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
			<< "|" << pProtoName << "|DECODE ERROR" << std::endl;

		stClient.Clear();
		return false;
	}

    if(stReqBody.m_iGuildID <= 0 || stReqBody.m_iChiefUid <= 0)
    {
        stRspBody.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;
        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    if (LOGIC_SERVICE_API_INS->GetCrossConnection()->IsConnected())
    {
        CMsgHeadCross stHead;
        stHead.m_byCmdType = enmCmdType_request;
        stHead.m_usCmd = MSG_CROSS_ADMIN_DISMISS_GUILD;
        stHead.m_nGroupID = static_cast<int16_t>(CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID);

        stReqBody.m_iFd = stClient.m_iFd;
        LOGIC_SERVICE_API_INS->GetCrossConnection()->SendToServer(stHead, stReqBody);
    }
    else
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << MATCH_SERVER_NOT_CONN << "|MSG:" << GET_ERRMSG_BY_ERRCODE(MATCH_SERVER_NOT_CONN)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = MATCH_SERVER_NOT_CONN;
        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

	return true;
}

bool CLogicAdminProcessor::ProcessSetGuildChief(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CCrossRequestSetGuildChief stReqBody;
    CCrossResponseModifyGuild stRspBody;

    if (!DecodeClient(stClient, pProtoName, stReqBody))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                              << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    if(stReqBody.m_iGuildID <= 0)
    {
        stRspBody.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;
        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
    if (!stUserInfo)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    if (LOGIC_SERVICE_API_INS->GetCrossConnection()->IsConnected())
    {
        CMsgHeadCross stHead;
        stHead.m_byCmdType = enmCmdType_request;
        stHead.m_usCmd = MSG_CROSS_ADMIN_SET_GUILD_CHIEF;
        stHead.m_iUin = iUin;
        stHead.m_nGroupID = nGroupID;

        stReqBody.m_iFd = stClient.m_iFd;
        LOGIC_SERVICE_API_INS->GetCrossConnection()->SendToServer(stHead, stReqBody);
    }
    else
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << MATCH_SERVER_NOT_CONN << "|MSG:" << GET_ERRMSG_BY_ERRCODE(MATCH_SERVER_NOT_CONN)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = MATCH_SERVER_NOT_CONN;
        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    return true;
}

bool CLogicAdminProcessor::ProcessGetCanDeposit(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMResponseGetCanDeposit stRspBody;

    auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
    if (!stUserInfo)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    for (const auto& deposit : CLogicConfig::GetDepositConfig().m_stDepositConfig)
    {
        if (CLogicUserStat::GetLeftRechargeBuyCount(stUserInfo, deposit.first) > 0)
        {
            stRspBody.m_stDepositIDVec.push_back(deposit.first);
        }
    }

    ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

    return true;
}

bool CLogicAdminProcessor::ProcessSetEntireWarPass(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestSetEntireWarPass stReq;
    CGMResponseSetEntireWarPass stRsp;

    if (!DecodeClient(stClient, pProtoName, stReq))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                              << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    if(stReq.m_iThemeID != CLogicGlobal::GetEntireWarThemeID())
    {
        stRsp.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;
        ResponseMessage(stClient.m_iFd, iCmd, stRsp);

        return false;
    }

    auto pstConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(CLogicGlobal::GetEntireWarThemeID(), stReq.m_iWarID);
    if (nullptr == pstConfig)
    {
        stRsp.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;
        ResponseMessage(stClient.m_iFd, iCmd, stRsp);

        return false;
    }

    if(stReq.m_iCurCount >= 0)
    {
        CLogicGlobal::RefreshEntireWarNodeProgress(stReq.m_iWarID, stReq.m_iCurCount, true);
    }

    while (pstConfig->m_iPreWarID > 0 && !CLogicGlobal::IsEntireWarPass(pstConfig->m_iPreWarID))
    {
        const auto* pstWarValue = CLogicGlobal::GetEntireWarMapInfo(pstConfig->m_iPreWarID);
        if (nullptr == pstWarValue)
        {
            LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR" << "|MSG: GetEntireWarMapInfo(m_iPreWarID) == null" << "|PreWarID:"
                                  << pstConfig->m_iPreWarID << std::endl;

            stRsp.m_iCode = SERVER_ERRCODE::INTERN_ERROR;
            ResponseMessage(stClient.m_iFd, iCmd, stRsp);

            return false;
        }

        CLogicGlobal::RefreshEntireWarNodeProgress(pstConfig->m_iPreWarID, pstWarValue->m_ulTargetProgress, true);

        pstConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(CLogicGlobal::GetEntireWarThemeID(), pstConfig->m_iPreWarID);
    }

    stRsp.m_iCode = SERVER_ERRCODE::SUCCESS;
    ResponseMessage(stClient.m_iFd, iCmd, stRsp);

    return true;
}

bool CLogicAdminProcessor::ProcessChangeGuildName(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CCrossRequestChangeGuildName stReq;
    CCrossResponseModifyGuild stRsp;

    if (!DecodeClient(stClient, pProtoName, stReq))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                              << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    if(stReq.m_iGuildID <= 0)
    {
        stRsp.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;
        ResponseMessage(stClient.m_iFd, iCmd, stRsp);

        return false;
    }

    const auto iErrorCode = CLogicCacheCommon::CheckGuildName(stReq.m_strGuildName);
    if(iErrorCode != SERVER_ERRCODE::SUCCESS)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << iErrorCode << "|GROUP_ID:" << nGroupID << "|GUILD_ID:"
                              << stReq.m_iGuildID << "|CHIEF_ID:" << iUin << std::endl;

        stRsp.m_iCode = iErrorCode;
        ResponseMessage(stClient.m_iFd, iCmd, stRsp);

        return false;
    }

    if (LOGIC_SERVICE_API_INS->GetCrossConnection()->IsConnected())
    {
        CMsgHeadCross stHead;
        stHead.m_byCmdType = enmCmdType_request;
        stHead.m_usCmd = MSG_CROSS_ADMIN_CHANGE_GUILD_NAME;
        stHead.m_iUin = iUin;
        stHead.m_nGroupID = nGroupID;

        stReq.m_iFd = stClient.m_iFd;
        LOGIC_SERVICE_API_INS->GetCrossConnection()->SendToServer(stHead, stReq);
    }
    else
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << MATCH_SERVER_NOT_CONN << "|MSG:" << GET_ERRMSG_BY_ERRCODE(MATCH_SERVER_NOT_CONN)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRsp.m_iCode = MATCH_SERVER_NOT_CONN;
        ResponseMessage(stClient.m_iFd, iCmd, stRsp);

        return false;
    }

    return true;
}

bool CLogicAdminProcessor::ProcessCopyUser(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestCopyUser stReqBody;
    CGMResponseBody stRspBody;

    if (!DecodeClient(stClient, pProtoName, stReqBody))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                              << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    auto pUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
    if (!pUserInfo)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    // 只有新建角色才能复制
    if(CLogicCommon::GetSec() - pUserInfo->m_stUserInfoTable.GetCreateTime() > 3600 || pUserInfo->m_stUserInfoTable.GetLevel() > 1)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << NOT_MEET_CONDITION << "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_MEET_CONDITION)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::NOT_MEET_CONDITION;

        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    auto stSourceUserInfo = CLogicCacheCommon::LoadUserData(stReqBody.m_iSourceGroupID, stReqBody.m_iSourceUin);
    if (!stSourceUserInfo)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << PARAMETER_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;

        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    /// 复制数据 ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    pUserInfo->m_stUserInfoTable.SetEntireWarMilitarkLevel(stSourceUserInfo->m_stUserInfoTable.GetEntireWarMilitarkLevel());
    pUserInfo->m_stUserInfoTable.SetExp(stSourceUserInfo->m_stUserInfoTable.GetExp());
    pUserInfo->m_stUserInfoTable.SetGold(stSourceUserInfo->m_stUserInfoTable.GetGold());
    pUserInfo->m_stUserInfoTable.SetGuideMajorStep(stSourceUserInfo->m_stUserInfoTable.GetGuideMajorStep());
    pUserInfo->m_stUserInfoTable.SetGuideMinorStep(stSourceUserInfo->m_stUserInfoTable.GetGuideMinorStep());
    pUserInfo->m_stUserInfoTable.SetLevel(stSourceUserInfo->m_stUserInfoTable.GetLevel());
    pUserInfo->m_stUserInfoTable.SetPrimaryKeyIndex(stSourceUserInfo->m_stUserInfoTable.GetPrimaryKeyIndex());
    pUserInfo->m_stUserInfoTable.SetRmb(stSourceUserInfo->m_stUserInfoTable.GetRmb());
    pUserInfo->m_stUserInfoTable.SetVipLevel(stSourceUserInfo->m_stUserInfoTable.GetVipLevel());

    pUserInfo->m_stUserTeamInfoTable.SetTeam1(stSourceUserInfo->m_stUserTeamInfoTable.GetTeam1());

    // 伙伴
    while (!pUserInfo->m_stHeroCardTableMap.Empty())
    {
        pUserInfo->m_stHeroCardTableMap.Erase(pUserInfo->m_stHeroCardTableMap.Begin());
    }

    for(auto iter = stSourceUserInfo->m_stHeroCardTableMap.Begin(); iter != stSourceUserInfo->m_stHeroCardTableMap.End(); ++iter)
    {
        if(iter->second->GetCardLevel() < 1) continue;

        THeroCardInfoTableValueType stValue = iter->second->m_stData;
        stValue.m_iUid = iUin;
        stValue.m_iGroupID = nGroupID;
        stValue.m_iEquipConstellation1 = 0;
        stValue.m_iEquipConstellation2 = 0;
        stValue.m_iEquipConstellation3 = 0;
        stValue.m_iEquipConstellation4 = 0;
        stValue.m_iEquip1 = 0;
        stValue.m_iEquip2 = 0;
        stValue.m_iEquip3 = 0;
        stValue.m_iEquip4 = 0;
        pUserInfo->m_stHeroCardTableMap.Insert(std::make_pair(stValue.m_iCardID, stValue));
    }

    // 副本进度
    while (!pUserInfo->m_stLevelInfoTableMap.Empty())
    {
        pUserInfo->m_stLevelInfoTableMap.Erase(pUserInfo->m_stLevelInfoTableMap.Begin());
    }

    for(auto iter = stSourceUserInfo->m_stLevelInfoTableMap.Begin(); iter != stSourceUserInfo->m_stLevelInfoTableMap.End(); ++iter)
    {
        TLevelInfoTableValueType stValue = iter->second->m_stData;
        stValue.m_iUid = iUin;
        stValue.m_iGroupID = nGroupID;
        pUserInfo->m_stLevelInfoTableMap.Insert(std::make_pair(stValue.m_iLevelID, stValue));
    }

    // 时装
    while (!pUserInfo->m_stFashionInfoMap.Empty())
    {
        pUserInfo->m_stFashionInfoMap.Erase(pUserInfo->m_stFashionInfoMap.Begin());
    }

    for(auto iter = stSourceUserInfo->m_stFashionInfoMap.Begin(); iter != stSourceUserInfo->m_stFashionInfoMap.End(); ++iter)
    {
        TFashionInfoTableValueType stValue = iter->second->m_stData;
        stValue.m_iUid = iUin;
        stValue.m_iGroupID = nGroupID;
        pUserInfo->m_stFashionInfoMap.Insert(std::make_pair(stValue.m_iFashionID, stValue));
    }

    // 称号
    while (!pUserInfo->m_stUserHonorMap.Empty())
    {
        pUserInfo->m_stUserHonorMap.Erase(pUserInfo->m_stUserHonorMap.Begin());
    }

    for(auto iter = stSourceUserInfo->m_stUserHonorMap.Begin(); iter != stSourceUserInfo->m_stUserHonorMap.End(); ++iter)
    {
        TUserHonorTableValueType stValue = iter->second->m_stData;
        stValue.m_iUid = iUin;
        stValue.m_iGroupID = nGroupID;
        pUserInfo->m_stUserHonorMap.Insert(std::make_pair(stValue.m_iHonorID, stValue));
    }

    // 礼装
    while (!pUserInfo->m_stConstelMap.Empty())
    {
        pUserInfo->m_stConstelMap.Erase(pUserInfo->m_stConstelMap.Begin());
    }

    for(auto iter = stSourceUserInfo->m_stConstelMap.Begin(); iter != stSourceUserInfo->m_stConstelMap.End(); ++iter)
    {
        TUserConstellationTableValueType stValue = iter->second->m_stData;
        stValue.m_iUid = iUin;
        stValue.m_iGroupID = nGroupID;
        stValue.m_iUniqueID = CLogicCacheCommon::GetUserPrimaryKeyIndex(pUserInfo);
        stValue.m_iEquipCardID = 0;
        pUserInfo->m_stConstelMap.Insert(std::make_pair(stValue.m_iUniqueID, stValue));
    }

    // 装备
    while (!pUserInfo->m_stEquipMap.Empty())
    {
        pUserInfo->m_stEquipMap.Erase(pUserInfo->m_stEquipMap.Begin());
    }

    for(auto iter = stSourceUserInfo->m_stEquipMap.Begin(); iter != stSourceUserInfo->m_stEquipMap.End(); ++iter)
    {
        TUserEquipTableValueType stValue = iter->second->m_stData;
        stValue.m_iUid = iUin;
        stValue.m_iGroupID = nGroupID;
        stValue.m_iUniqueID = CLogicCacheCommon::GetUserPrimaryKeyIndex(pUserInfo);
        pUserInfo->m_stEquipMap.Insert(std::make_pair(stValue.m_iUniqueID, stValue));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":"
                          << stClient.m_usClientPort << "|" << pProtoName << std::endl;

    ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

    return true;
}

bool CLogicAdminProcessor::ProcessAddConstel(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestAddConstel stReqBody;
    CGMResponseBody stRspBody;

    if (!DecodeClient(stClient, pProtoName, stReqBody))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                              << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
    if (!stUserInfo)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    if(!CLogicConfigDefine::IsValidEnumCardGradeType(stReqBody.m_iGrade) ||
        !CLogicConfigDefine::IsValidEnumHeroEquipType(stReqBody.m_iEquipTag) ||
        !CLogicConfigDefine::IsValidEnumCardAttrType(stReqBody.m_iMainAttrType) ||
        CLogicConfig::GetConstelAttrConfig().m_stSuitMap.find(stReqBody.m_iSuit) == CLogicConfig::GetConstelAttrConfig().m_stSuitMap.end())
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << PARAMETER_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;
        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
        return false;
    }

    auto pConfig = CLogicConfig::GetConstelAttrConfig().GetGradeConfig(stReqBody.m_iGrade);
    if(nullptr == pConfig || stReqBody.m_iLevel < 0 || stReqBody.m_iLevel >= int(pConfig->m_stLevelExp.size()) ||
        pConfig->m_stMainAttr.find(stReqBody.m_iMainAttrType) == pConfig->m_stMainAttr.end())
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << PARAMETER_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;
        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
        return false;
    }

    std::map<int, std::pair<int, int>> stAttrVec;
    if(stReqBody.m_iAttrType1 > 0) stAttrVec[stReqBody.m_iAttrType1] = std::make_pair(stReqBody.m_iAttrValue1, 1);
    if(stReqBody.m_iAttrType2 > 0) stAttrVec[stReqBody.m_iAttrType2] = std::make_pair(stReqBody.m_iAttrValue2, 1);
    if(stReqBody.m_iAttrType3 > 0) stAttrVec[stReqBody.m_iAttrType3] = std::make_pair(stReqBody.m_iAttrValue3, 1);
    if(stReqBody.m_iAttrType4 > 0) stAttrVec[stReqBody.m_iAttrType4] = std::make_pair(stReqBody.m_iAttrValue4, 1);

    for(auto& attr : stAttrVec)
    {
        if(!CLogicConfigDefine::IsValidEnumCardAttrType(attr.first) || attr.second.first <= 0)
        {
            using namespace SERVER_ERRCODE;
            LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << PARAMETER_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                                  << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

            stRspBody.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;
            ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
            return false;
        }
    }

    TUserConstellationTableValueType stValue;
    stValue.m_iUid = iUin;
    stValue.m_iGroupID = nGroupID;
    stValue.m_iUniqueID = CLogicCacheCommon::GetUserPrimaryKeyIndex(stUserInfo);
    stValue.m_iGrade = stReqBody.m_iGrade;
    stValue.m_iEquipmentTag = stReqBody.m_iEquipTag;
    stValue.m_iSuit = stReqBody.m_iSuit;
    stValue.m_iLevel = stReqBody.m_iLevel;
    stValue.m_iMainAttrType = stReqBody.m_iMainAttrType;
    stValue.m_stExAttr = stAttrVec;
    auto iterResult = stUserInfo->m_stConstelMap.Insert(std::make_pair(stValue.m_iUniqueID, stValue));
    if(!iterResult.second)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << INTERN_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::INTERN_ERROR;
        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
        return false;
    }

    auto& iterData = iterResult.first;

    // bill 记录
    CLogicBill::SaveConstelBill(iUin, nGroupID, stUserInfo->m_iChannelID, iCmd, 1, iterData->second->m_stData);

    LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort << "|" << pProtoName << std::endl;

    ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
    return true;
}

bool CLogicAdminProcessor::ProcessDelConstel(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestDeleteConstel stReqBody;
    CGMResponseBody stRspBody;

    if (!DecodeClient(stClient, pProtoName, stReqBody))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                              << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
    if (!stUserInfo)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    const auto iterConstel = stUserInfo->m_stConstelMap.Find(stReqBody.m_iUniqueID);
    if(iterConstel == stUserInfo->m_stConstelMap.End())
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << PARAMETER_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << "|UNIQUE:" << stReqBody.m_iUniqueID << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;

        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    if(iterConstel->second->GetEquipCardID() > 0)
    {
        auto iterCard = stUserInfo->m_stHeroCardTableMap.Find(iterConstel->second->GetEquipCardID());
        if(iterCard != stUserInfo->m_stHeroCardTableMap.End())
        {
            if(iterCard->second->GetEquipConstellation1() == stReqBody.m_iUniqueID) iterCard->second->SetEquipConstellation1(0);
            if(iterCard->second->GetEquipConstellation2() == stReqBody.m_iUniqueID) iterCard->second->SetEquipConstellation2(0);
            if(iterCard->second->GetEquipConstellation3() == stReqBody.m_iUniqueID) iterCard->second->SetEquipConstellation3(0);
            if(iterCard->second->GetEquipConstellation4() == stReqBody.m_iUniqueID) iterCard->second->SetEquipConstellation4(0);
        }
    }

    // bill 记录
    CLogicBill::SaveConstelBill(iUin, nGroupID, stUserInfo->m_iChannelID, iCmd, -1, iterConstel->second->m_stData);

    stUserInfo->m_stConstelMap.Erase(iterConstel);

    LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort << "|" << pProtoName << std::endl;

    ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
    return true;
}

bool CLogicAdminProcessor::ProcessChangeAgeLimit(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient)
{
    CGMRequestChangeAgeLimit stReqBody;
    CGMResponseBody stRspBody;

    if (!DecodeClient(stClient, pProtoName, stReqBody))
    {
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                              << "|" << pProtoName << "|DECODE ERROR" << std::endl;

        stClient.Clear();
        return false;
    }

    auto stUserInfo = CLogicCacheCommon::LoadUserData(nGroupID, iUin);
    if (!stUserInfo)
    {
        using namespace SERVER_ERRCODE;
        LOGIC_LOG_TRACE_ERROR << "PROCESS ERROR:" << USER_ROLE_NOT_EXIST << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
                              << "|GROUP_ID:" << nGroupID << "|UIN:" << iUin << std::endl;

        stRspBody.m_iCode = SERVER_ERRCODE::USER_ROLE_NOT_EXIST;

        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);

        return false;
    }

    auto ageLimitConfig = CLogicConfig::GetDepositConfig().m_stAgeLimitConfig;

    if (stReqBody.iAgeLimitIndex < 0 && stReqBody.iAgeLimitIndex > (int32_t)ageLimitConfig.size())
    {
        stRspBody.m_iCode = SERVER_ERRCODE::PARAMETER_ERROR;

        ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
        return false;
    }

    stUserInfo->m_stUserInfoTable.SetMonthAgeLimit(stReqBody.iAgeLimitIndex);
    stUserInfo->m_stUserInfoTable.SetLastMonthAgeChange(CLogicCommon::GetSec());
    LOGIC_LOG_TRACE_DEBUG << "PROCESS SUCCESS|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort << "|" << pProtoName << std::endl;

    ResponseMessage(stClient.m_iFd, iCmd, stRspBody);
    return true;
}



template<typename T>
bool CLogicAdminProcessor::DecodeClient(stLogicSoAdminClient &stClient, const char *pProtoName, T &stMsgPackObj)
{
    size_t uiOffset = 0;
    msgpack::unpacked stResult;
    try
    {
        msgpack::unpack(&stResult, stClient.m_pBuffer + stClient.m_uiOffset, stClient.m_uiLength, &uiOffset);
    }
    catch(const msgpack::unpack_error& stEx)
    {
        LOGIC_LOG_TRACE_ERROR << "ADMIN CLIENT|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                       << "|UNPACK PROTOCOL ERROR|PROTO NAME:" << pProtoName
                       << "|MSG:" << stEx.what() << std::endl;

        return (false);
    }

    msgpack::object stMsgObj = stResult.get();
    LOGIC_LOG_TRACE_DEBUG << "Admin|PROTO_DATA:" << stMsgObj << std::endl;
    try
    {
        stMsgObj.convert(&stMsgPackObj);
    }
    catch(const std::bad_cast& stEx)
    {
        LOGIC_LOG_TRACE_ERROR << "ADMIN CLIENT|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort
                       << "|CONVERT OBJECT ERROR|PROTO NAME:" << pProtoName
                       << "|MSG:" << stEx.what() << "|PROTO_DATA:" << stMsgObj << std::endl;

        return (false);
    }

    stClient.m_uiOffset += uiOffset;
    stClient.m_uiLength -= uiOffset;

    return (true);
}

template<typename T>
void CLogicAdminProcessor::ResponseMessage(int32_t iFd, uint16_t usCmd, const T &stRsp)
{
    CMsgHeadAdmin stHeader;
    stHeader.m_usCmd = usCmd;

    static char szBuffer[max_admin_client_pkg_size];
    int32_t iBufferLength = sizeof(szBuffer);

    if (encode_message_to_buffer(szBuffer, iBufferLength, stHeader, stRsp) != success)
    {
        LOGIC_LOG_TRACE_ERROR << "ADMIN CLIENT|RESPONSE MESSAGE TO ENCODE FAIL"
                       << "|CMD:" << usCmd << "|FD:" << iFd << std::endl;
        return ;
    }

    ::write(iFd, szBuffer, iBufferLength);
}

bool CLogicAdminProcessor::ThreeStarPassLevel(user_data_table_ptr_type pUserData, int32_t iCmd, int iLevelID)
{
    CLogicCacheLevelOperation stLevelOP(pUserData);

    auto stUserLevelInfoPair = CLogicCacheLevelOperation::GetUserLevelInfo(pUserData, iLevelID);
    if (false == stUserLevelInfoPair.second)
        return false;

    auto pLevelInfoConfigElem = CLogicConfig::GetLevelConfig().GetLevelConfigElem(iLevelID);
    if (nullptr == pLevelInfoConfigElem)
        return false;

    std::vector<CPackGameItem> vecBonusItem;
    CLogicConfig::GetLevelDropConfig().GetRandBonusByName(pLevelInfoConfigElem->m_strBonusCardBag, vecBonusItem);
    CLogicConfig::GetLevelDropConfig().GetRandBonusByName(pLevelInfoConfigElem->m_strPerfectCardBag, vecBonusItem);

    std::string strBonusItem;
    CLogicCacheCommon::AddGameItem(pUserData, vecBonusItem, strBonusItem, iCmd);

    //剧情关卡通关，星级为0
    if(pLevelInfoConfigElem->m_iLevelType == CLogicConfigDefine::ELT_ShowLevel)
    {
        stUserLevelInfoPair.first->second->SetLevelStar(0);
    } else {
        stUserLevelInfoPair.first->second->SetLevelStar(CLogicConfigDefine::ESD_LevelFullStarFlag);
    }
    stUserLevelInfoPair.first->second->AddPlayTimes(1);
    stUserLevelInfoPair.first->second->AddTotalVictoryTimes(1);

    CLogicGameLevelChannel::data_type stData;

    stData.m_iLevelID = iLevelID;
    stData.m_iTimes = 1;

    LOGIC_PUBLISHER_INS->PublishGameLevelInfo(pUserData, stData);

    return true;
}
