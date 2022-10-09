#include "tc_md5.h"
#include <fstream>
#include "CentreDataProcessor.h"
#include "server_cmd_cross_define.h"
#include "server_cmd_logic_define.h"
#include "server_cross_protocol.h"
#include "server_guild_protocol.h"
#include "server_task_protocol.h"
#include "ServerConfig.h"

#define DUMP_CENTRE_DATA_FILE "./centre_data"

CCentreDataProcessor::CCentreDataProcessor(void) : m_stSQLDataManager(this)
{
    m_iLastRefreshTime = 0;
    m_iLastLazyRefreshTime = 0;
}

CCentreDataProcessor::~CCentreDataProcessor(void)
{

}

bool CCentreDataProcessor::Stop()
{
    SaveDataToFile();

    return CProcessor::Stop();
}

bool CCentreDataProcessor::Initialize(int32_t nLogMask, const char* pszLogFileName, int32_t nProcessID)
{
    if (!CProcessor::Initialize(nLogMask, pszLogFileName, nProcessID))
    {
        return false;
    }

    m_iLastRefreshTime = GetNowSec();
    m_iLastLazyRefreshTime = GetNowSec();

    if (success != RecoveryDataFromFile())
    {
        Log(log_mask_system_error, "[CCentreDataProcessor:%s]RecoveryDataFromFile error!\n", __FUNCTION__);
        return false;
    }


    return true;
}

void CCentreDataProcessor::Refresh()
{
    const int32_t iNowSec = GetNowSec();

    if(iNowSec > m_iLastRefreshTime + 10)
    {
        m_stSQLDataManager.LoadGuildRank(iNowSec);
        m_iLastRefreshTime = iNowSec;
    }

    //每天5点刷新
    if (IsRefreshByDay(iNowSec,m_iLastRefreshTime, 5))
    {
        m_iLastRefreshTime = iNowSec;

        // 公会缓存跨天
        m_stSQLDataManager.GuildCacheCrossOneDay();

        // 每天保存一次
        SaveDataToFile();
    }

    // 有些实时要求不高的数据， 凌晨五点时开始处理
    if (iNowSec > m_iLastLazyRefreshTime)
    {
        m_iLastLazyRefreshTime = get_hour_timestamp(iNowSec, 5) + 1_day;

        // 刷新公会非实时数据
        m_stSQLDataManager.UpdateGuildLazyData();
    }
}

int32_t CCentreDataProcessor::ProcessMessages(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    int32_t iReturn = fail;
    switch (stHeadInfo.m_usCmd)
    {
    case MSG_CROSS_USER_BRIEF_UPDATE:
        iReturn = ProcessUserBriefUpdate(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GET_FRIEND_LIST:
        iReturn = ProcessGetFriendList(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GET_FRIEND_APPLY_LIST:
        iReturn = ProcessGetFriendApplyList(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_SEARCH_FRIEND:
        iReturn = ProcessSearchFriend(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GET_GUILD_LIST:
        iReturn = ProcessGetGuildList(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GET_SELF_GUILD_INFO:
        iReturn = ProcessGetSelfGuildInfo(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GET_GUILD_MEMBERS:
        iReturn = ProcessGetGuildMembers(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_CREATE_GUILD:
        iReturn = ProcessCreateGuild(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_LEAVE_GUILD:
        iReturn = ProcessLeaveGuild(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_UPDATE_GUILD_BULLETIN:
        iReturn = ProcessUpdateGuildBulletin(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_UPDATE_GUILD_POST:
        iReturn = ProcessUpdateGuildPost(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_TRANSFER_GUILD:
        iReturn = ProcessTransferGuild(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_EXPEL_GUILD:
        iReturn = ProcessExpelGuild(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_APPLY_JOIN_GUILD:
        iReturn = ProcessApplyJoinGuild(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_DEAL_GUILD_APPLY:
        iReturn = ProcessDealGuildApply(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GET_GUILD_APPLY_LIST:
        iReturn = ProcessGetGuildApplyList(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_SEARCH_GUILD:
        iReturn = ProcessSearchGuild(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_TOUHOU_CHANGE_GUILD_NAME:
        iReturn = ProcessChangeGuildName(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_ADMIN_MODIFY_GUILD:
        iReturn = ProcessAdminModifyGuild(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_ADMIN_DISMISS_GUILD:
        iReturn = ProcessAdminDismissGuild(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_ADMIN_SET_GUILD_CHIEF:
        iReturn = ProcessAdminSetGuildChief(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_ADMIN_CHANGE_GUILD_NAME:
        iReturn = ProcessAdminChangeGuildName(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_GUILD_BOSS_HURT_NOTIFY:
        iReturn = ProcessGuildBossHurtNotify(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GUILD_BOSS_HURT_LIST:
        iReturn = ProcessGuildBossHurtList(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_USER_TALK:
        iReturn = ProcessUserTalk(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GUILD_GET_LOG:
        iReturn = ProcessGuildGetLog(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_GUILD_WISH_NOTIFY:
        iReturn = ProcessGuildWishNotify(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_GUILD_WISH_GET_LIST:
        iReturn = ProcessGuildWishGetList(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GUILD_DONATE:
        iReturn = ProcessGuildDonate(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GUILD_SIGN_IN:
        iReturn = ProcessGuildSignIn(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GUILD_SIGN_UPGRADE:
        iReturn = ProcessGuildSignUpGrade(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GUILD_SAVE_DECORATE:
        iReturn = ProcessGuildDecorateSave(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GUILD_GET_BOSS_HURT_RANK:
        iReturn = ProcessGuildGetBossHurtRank(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GUILD_SIT_BAR:
        iReturn = ProcessGuildSitBar(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GUILD_GET_BAR_REWARD:
        iReturn = ProcessGuildGetBarReward(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GUILD_UNLOCK_COLLECTION:
        iReturn = ProcessGuildUnlockCollection(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_GUILD_SET_DECORATE:
        iReturn = ProcessGuildSetDecorate(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_GUILD_WAR_GET_INFO:
        iReturn = ProcessGuildWarInfo(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_GUILD_WAR_BEGIN:
        iReturn = ProcessGuildWarBegin(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_GUILD_WAR_END:
        iReturn = ProcessGuildWarEnd(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_GUILD_WAR_RANK:
        iReturn = ProcessGuildWarGetRankInfo(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_GUILD_WAR_GET_SELF_RANK:
        iReturn = ProcessGuildWarUpdateRankInfo(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    default:
        Log(log_mask_system_error, "[CHttpProcessor::%s]recv unknown msg, cmd=%d\n", __FUNCTION__, stHeadInfo.m_usCmd);
        break;
    }

    ///////////////// Mysql Delegate
    LOGIC_RECORD_CACHE_INS->ExecSQL(stHeadInfo.m_iGroupID, stHeadInfo.m_iUin, std::to_string(stHeadInfo.m_usCmd));

    return iReturn;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int32_t CCentreDataProcessor::ProcessUserBriefUpdate(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CPackUserBrief stBriefInfo;
    if (!DecodeMsgPack(stBriefInfo, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    m_stSQLDataManager.UpdateUserBrief(stBriefInfo);

    return success;
}

int32_t CCentreDataProcessor::ProcessGetFriendList(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestGetFriendList stReq;
    CResponseGetFriendList stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    for(auto& key : stReq.m_stUserVec)
    {
        CPackFriendInfo stInfo;
        m_stSQLDataManager.GetUserBriefInfo(key.m_iUin, key.m_iGroupID, stInfo.m_stBriefInfo);
        stRsp.m_stFriendVec.emplace_back(stInfo);
    }

    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGetFriendApplyList(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestGetFriendApplyList stReq;
    CResponseGetFriendApplyList stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    for(auto& key : stReq.m_stUserVec)
    {
        CPackFriendApply stApplyInfo;
        m_stSQLDataManager.GetUserBriefInfo(key.m_iUin, key.m_iGroupID, stApplyInfo.m_stBriefInfo);
        stApplyInfo.m_iApplyTime = key.m_iApplyTime;
        stRsp.m_stApplyVec.emplace_back(stApplyInfo);
    }

    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessSearchFriend(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestAddFriend stReq;
    CResponseSearchFriend stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    // 判断是否同跨服
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);
    if(cross_config->GetZooCrossConfig().GetServerIDByGroupID(stReq.m_iDstGroupID) <= 0)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::USER_ROLE_NOT_EXIST);
    }

    m_stSQLDataManager.GetUserBriefInfo(stReq.m_iDstUin, stReq.m_iDstGroupID, stRsp.m_stUserInfo);
    if (stRsp.m_stUserInfo.m_iUid <= 0)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::USER_ROLE_NOT_EXIST);
    }

    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGetGuildList(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CResponseGetGuildList stRsp;

    const int iMaxCount = 20;
    auto stGuildIDVec = m_stSQLDataManager.GetRandGuildIDVec();
    std::random_shuffle(stGuildIDVec.begin(), stGuildIDVec.end());
    if(stGuildIDVec.size() > iMaxCount)
    {
        stGuildIDVec.resize(iMaxCount);
    }

    for(auto& guild_id : stGuildIDVec)
    {
        CPackGuildListInfo stInfo;
        auto pGuild = m_stSQLDataManager.GetGuild(guild_id);
        if(nullptr == pGuild) continue;

        stInfo.m_stGuildInfo = pGuild->GetGuildInfoPack();
        m_stSQLDataManager.GetUserBriefInfo(pGuild->GetChiefUin(), pGuild->GetChiefGroupID(), stInfo.m_stGuildInfo.m_stChiefInfo);
        stInfo.m_bApply = m_stSQLDataManager.IsApplyGuild(guild_id, stHeadInfo.m_iUin);
        stRsp.m_stGuildVec.emplace_back(stInfo);
    }

    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGetSelfGuildInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CResponseGetSelfGuildInfo stRsp;

    auto pGuild = m_stSQLDataManager.GetGuildByUin(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
    if(nullptr == pGuild)
    {
        // 没有公会就发空数据回去
        return SendResponse(stHeadInfo, stRsp);
    }

    stRsp.m_stGuildInfo = pGuild->GetGuildInfoPack();
    stRsp.m_cPost = pGuild->GetPosByUin(stHeadInfo.m_iUin);
    stRsp.m_iChangeNameCD = pGuild->m_stGuildInfo.GetChangeNameCd();
    stRsp.m_cChangeNameFree = pGuild->m_stGuildInfo.GetChangeNameFree();
    stRsp.m_iUnlockItem = pGuild->m_stGuildInfo.GetGuildUnlockItem();
    stRsp.m_iUnlockIndex = pGuild->m_stGuildInfo.GetGuildUnlockDecorate();
    stRsp.m_iFootCloth = pGuild->m_stGuildInfo.GetGuildFootCloth();
    stRsp.m_iGuildDecorate = pGuild->m_stGuildInfo.GetGuildDecorate();

    //添加公会战信息
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

    int32_t iNowSec = GetNowSec();
    int32_t iRankID = cross_config->GetGuildConfig().m_iRankID;
    if (iNowSec < cross_config->GetGuildConfig().m_iGuildWarStartTime)
    {
        iRankID -= 1;
    }
    auto curGuildWarRank = m_stSQLDataManager.m_stGuildWarRankMap.find(iRankID);
    if (curGuildWarRank != m_stSQLDataManager.m_stGuildWarRankMap.end())
    {
        stRsp.m_stGuildWarInfo.m_iGuildRank = curGuildWarRank->second.GetRank(pGuild->GetGuildID());
    }
    stRsp.m_stGuildWarInfo.m_iGuildRankScore = pGuild->m_stGuildInfo.GetGuildWarScore();
    m_stSQLDataManager.GetUserBriefInfo(pGuild->GetChiefUin(), pGuild->GetChiefGroupID(), stRsp.m_stGuildInfo.m_stChiefInfo);
    m_stSQLDataManager.GetBarInfo(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,&stRsp.m_stGuildBarInfo);
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGetGuildMembers(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CResponseGetGuildMembers stRsp;
    m_stSQLDataManager.GetGuildMembers(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stRsp);

    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessCreateGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestCreateGuild stReq;
    CResponseCreateGuild stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    if(m_stSQLDataManager.GetGuildIDByUin(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID) > 0)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::USER_ALREADY_IN_GUILD);
    }
    std::string execSql("");
    int32_t iRetCode = m_stSQLDataManager.CreateGuild(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stReq.m_strGuildName,execSql);
    if (SERVER_ERRCODE::SUCCESS != iRetCode)
    {
        return SendErrCode(stHeadInfo, iRetCode);
    }

    stRsp.m_iGuildID = m_stSQLDataManager.GetCurGuildID();
    stRsp.m_strGuildName = stReq.m_strGuildName;

    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessLeaveGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    auto guildID = m_stSQLDataManager.GetGuildIDByUin(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
    if(guildID <= 0)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND);
    }

    auto pGuild = m_stSQLDataManager.GetGuild(guildID);
    if(nullptr == pGuild)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_NOT_FOUND);
    }

    CResponseLeaveGuild stRsp;
    stRsp.m_iGuildID = pGuild->GetGuildID();
    stRsp.m_strGuildName = pGuild->GetGuildName();
    stRsp.m_iGuildExp = pGuild->GetGuildExp();

    int32_t iRetCode = m_stSQLDataManager.LeaveGuild(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
    if(SERVER_ERRCODE::SUCCESS != iRetCode)
    {
        return SendErrCode(stHeadInfo, iRetCode);
    }
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessUpdateGuildBulletin(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestUpdateGuildBulletin stReq;
    CResponseUpdateGuildBulletin stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    int32_t iRetCode = m_stSQLDataManager.UpdateBulletin(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stReq.m_strBulletin);
    if(SERVER_ERRCODE::SUCCESS != iRetCode)
    {
        return SendErrCode(stHeadInfo, iRetCode);
    }

    stRsp.m_strBulletin = stReq.m_strBulletin;
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessUpdateGuildPost(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestUpdateGuildPost stReq;
    CResponseUpdateGuildPost stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    int32_t iRetCode = m_stSQLDataManager.UpdatePost(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stReq.m_iDstUid, stReq.m_iDstGroupID, stReq.m_iDstPost);
    if(SERVER_ERRCODE::SUCCESS != iRetCode)
    {
        return SendErrCode(stHeadInfo, iRetCode);
    }

    stRsp.m_iDstUid = stReq.m_iDstUid;
    stRsp.m_iDstGroupID = stReq.m_iDstGroupID;
    stRsp.m_iDstPost = stReq.m_iDstPost;
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessTransferGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestTransferGuild stReq;
    CResponseTransferGuild stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    int32_t iRetCode = m_stSQLDataManager.TransferGuild(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stReq.m_iDstUid, stReq.m_iDstGroupID);
    if(SERVER_ERRCODE::SUCCESS != iRetCode)
    {
        return SendErrCode(stHeadInfo, iRetCode);
    }

    stRsp.m_iDstUid = stReq.m_iDstUid;
    stRsp.m_iDstGroupID = stReq.m_iDstGroupID;
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessExpelGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestExpelGuild stReq;
    CResponseExpelGuild stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    int32_t iRetCode = m_stSQLDataManager.ExpelGuild(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stReq.m_iDstUid, stReq.m_iDstGroupID);
    if(SERVER_ERRCODE::SUCCESS != iRetCode)
    {
        return SendErrCode(stHeadInfo, iRetCode);
    }

    stRsp.m_iDstUid = stReq.m_iDstUid;
    stRsp.m_iDstGroupID = stReq.m_iDstGroupID;
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessApplyJoinGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestApplyJoinGuild stReq;
    CResponseApplyJoinGuild stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    int32_t iRetCode = m_stSQLDataManager.ApplyJoinGuild(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stReq.m_iGuildID);
    if(SERVER_ERRCODE::SUCCESS != iRetCode)
    {
        return SendErrCode(stHeadInfo, iRetCode);
    }

    stRsp.m_iGuildID = stReq.m_iGuildID;
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessDealGuildApply(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestDealGuildApply stReq;
    CResponseDealGuildApply stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    auto guildID = m_stSQLDataManager.GetGuildIDByUin(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
    if(guildID <= 0)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND);
    }

    auto pGuild = m_stSQLDataManager.GetGuild(guildID);
    if(nullptr == pGuild)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_NOT_FOUND);
    }

    int32_t iRetCode = m_stSQLDataManager.DealGuildApply(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stReq.m_iDstUid, stReq.m_iDstGroupID, stReq.m_bIsAgree);
    if(SERVER_ERRCODE::SUCCESS != iRetCode)
    {
        return SendErrCode(stHeadInfo, iRetCode);
    }

    stRsp.m_iDstUid = stReq.m_iDstUid;
    stRsp.m_iDstGroupID = stReq.m_iDstGroupID;
    stRsp.m_bIsAgree = stReq.m_bIsAgree;
    stRsp.m_iGuildID = pGuild->GetGuildID();
    stRsp.m_strGuildName = pGuild->GetGuildName();
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGetGuildApplyList(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CResponseGetGuildApplyList stRsp;

    auto pMap = m_stSQLDataManager.GetGuildApplyMap(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
    if(pMap)
    {
        CPackGuildApply stMem;
        for(auto iter = pMap->begin(); iter != pMap->end(); ++iter)
        {
            m_stSQLDataManager.GetUserBriefInfo(iter->first, iter->second.m_iGroupID, stMem.m_stUserBrief);
            stMem.m_iApplyTime = iter->second.m_iApplyTime;
            stRsp.m_stApplyVec.push_back(stMem);
        }
    }

    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessSearchGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestApplyJoinGuild stReq;
    CResponseSearchGuild stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    auto pGuild = m_stSQLDataManager.GetGuild(stReq.m_iGuildID);
    if(nullptr == pGuild)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_NOT_FOUND);
    }

    stRsp.m_stGuildInfo.m_stGuildInfo = pGuild->GetGuildInfoPack();
    m_stSQLDataManager.GetUserBriefInfo(pGuild->GetChiefUin(), pGuild->GetChiefGroupID(), stRsp.m_stGuildInfo.m_stGuildInfo.m_stChiefInfo);
    stRsp.m_stGuildInfo.m_bApply = m_stSQLDataManager.IsApplyGuild(stReq.m_iGuildID, stHeadInfo.m_iUin);

    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessChangeGuildName(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestChangeGuildName stReq;
    CResponseChangeGuildName stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    auto guildID = m_stSQLDataManager.GetGuildIDByUin(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
    if(guildID <= 0)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND);
    }

    std::string oldName = "";
    int32_t iRetCode = m_stSQLDataManager.ChangeGuildName(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stReq.m_strNewGuildName, false, stRsp,oldName);

    if(SERVER_ERRCODE::SUCCESS != iRetCode)
    {
        return SendErrCode(stHeadInfo, iRetCode);
    }

    stRsp.m_strNewGuildName = stReq.m_strNewGuildName;
    stRsp.m_strOldGuildName = oldName;
    stRsp.m_iGuildID = guildID;

    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessAdminModifyGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CCrossRequestModifyGuild stReq;
    CCrossResponseModifyGuild stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    stRsp.m_iCode = m_stSQLDataManager.GMModifyGuild(stReq.m_iGuildID, stReq.m_iGuildExp);
    stRsp.m_iFd = stReq.m_iFd;
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessAdminDismissGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CCrossRequestDismissGuild stReq;
    CCrossResponseModifyGuild stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    stRsp.m_iFd = stReq.m_iFd;

    auto pGuild = m_stSQLDataManager.GetGuild(stReq.m_iGuildID);
    if(nullptr == pGuild)
    {
        stRsp.m_iCode = SERVER_ERRCODE::GUILD_NOT_FOUND;
        return SendResponse(stHeadInfo, stRsp);
    }

    if(!pGuild->IsChief(stReq.m_iChiefUid))
    {
        stRsp.m_iCode = SERVER_ERRCODE::GUILD_CHIEF_NOT_FOUND;
        return SendResponse(stHeadInfo, stRsp);
    }

    m_stSQLDataManager.DismissGuild(stReq.m_iGuildID);

    stRsp.m_iCode = SERVER_ERRCODE::SUCCESS;
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessAdminSetGuildChief(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CCrossRequestSetGuildChief stReq;
    CCrossResponseModifyGuild stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    auto pGuild = m_stSQLDataManager.GetGuild(stReq.m_iGuildID);
    if(nullptr == pGuild)
    {
        stRsp.m_iCode = SERVER_ERRCODE::GUILD_NOT_FOUND;
        return SendResponse(stHeadInfo, stRsp);
    }

    stRsp.m_iCode = m_stSQLDataManager.TransferGuild(pGuild->GetChiefUin(), pGuild->GetChiefGroupID(), stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessAdminChangeGuildName(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CCrossRequestChangeGuildName stReq;
    CCrossResponseModifyGuild stRsp;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    CResponseChangeGuildName stEmpty;
    std::string mysql;
    stRsp.m_iCode = m_stSQLDataManager.ChangeGuildName(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stReq.m_strGuildName, true, stEmpty,mysql);
    if(stRsp.m_iCode == SERVER_ERRCODE::SUCCESS)
    {
        // pstGuild->SendMailToChief(stReq.m_strSender, stReq.m_strTitle, stReq.m_strText, {}, pProtoName);
    }

    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGuildBossHurtNotify(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CCrossGuildBossHurtNotify stNotify;
    if (!DecodeMsgPack(stNotify, pszMsgBody, iBodyLength)) return fail;

    m_stSQLDataManager.UpdateGuildBossHurt(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stNotify.m_ulHurt);

    return success;
}

int32_t CCentreDataProcessor::ProcessGuildBossHurtList(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CResponseGuildBossHurtList stRsp;

    auto pMap = m_stSQLDataManager.GetGuildBossHurtMap(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
    if(pMap)
    {
        CPackGuildBossHurtRankInfo stMem;
        for(auto iter = pMap->begin(); iter != pMap->end(); ++iter)
        {
            m_stSQLDataManager.GetUserBriefInfo(iter->first, iter->second.m_iGroupID, stMem.m_stInfo);
            stMem.m_ulHurt = iter->second.m_ulHurt;
            stRsp.m_stHurtRank.push_back(stMem);
        }
    }

    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGuildGetLog(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CResponseGuildGetLog stRsp;

    auto pList = m_stSQLDataManager.GetGuildLogList(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
    if(pList)
    {
        stRsp.m_stLogVec.assign(pList->begin(), pList->end());
    }

    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessUserTalk(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestTalk stReq;
    if (!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    CSNotifyUserTalk stNotify;
    m_stSQLDataManager.GetUserBriefInfo(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stNotify.m_stTalkInfo.m_stUserBrief);
    if(stNotify.m_stTalkInfo.m_stUserBrief.m_iUid <= 0)
    {
        Log(log_mask_system_error, "[CCentreDataProcessor:%s]player(%d,%d) brief info error\n", __FUNCTION__, stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
        return fail;
    }

    stNotify.m_stTalkInfo.m_strTalkData = stReq.m_strTalkData;
    stNotify.m_stTalkInfo.m_iTalkTime = GetNowSec();
    stNotify.m_stTalkInfo.m_iTalkChannel = stReq.m_iTalkChannel;
    stNotify.m_stTalkInfo.m_iDstUin = stReq.m_iDstUin;

    switch (stReq.m_iTalkChannel)
    {
    case CRequestTalk::ETCT_World:
    {
        std::shared_ptr<CCrossServerConfig> config;
        CCrossServerConfig::GetConfig(config);
        for (const auto& logic : config->GetZooCrossConfig().m_stLogicConfigMap)
        {
            stNotify.m_iDstGroupID = logic.first;
            NotifyMessage(stNotify.m_iDstGroupID, 0, MSG_CROSS_USER_TALK_NOTIFY, stNotify);
        }

        break;
    }
    case CRequestTalk::ETCT_Guild:
    {
        auto pGuild = m_stSQLDataManager.GetGuildByUin(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
        if(!pGuild) return fail;

        stNotify.m_iDstUin = pGuild->GetGuildID();
        auto stUinMap = m_stSQLDataManager.GetUinGroupVec(pGuild);
        for(auto& iterVec : stUinMap)
        {
            stNotify.m_iDstGroupID = iterVec.first;
            stNotify.m_stUinVec = iterVec.second;
            NotifyMessage(stNotify.m_iDstGroupID, 0, MSG_CROSS_USER_TALK_NOTIFY, stNotify);
        }

        break;
    }
    case CRequestTalk::ETCT_Private:
    {
        std::shared_ptr<CCrossServerConfig> cross_config;
        CCrossServerConfig::GetConfig(cross_config);
        if(cross_config->GetZooCrossConfig().GetServerIDByGroupID(stReq.m_iDstGroupID) <= 0)
            return fail;

        // 发送给被私聊的人
        stNotify.m_iDstUin = stReq.m_iDstUin;
        stNotify.m_iDstGroupID = stReq.m_iDstGroupID;
        NotifyMessage(stNotify.m_iDstGroupID, 0, MSG_CROSS_USER_TALK_NOTIFY, stNotify);

        // 发送给发起私聊的人
        stNotify.m_iDstUin = stHeadInfo.m_iUin;
        stNotify.m_iDstGroupID = stHeadInfo.m_iGroupID;
        NotifyMessage(stNotify.m_iDstGroupID, 0, MSG_CROSS_USER_TALK_NOTIFY, stNotify);
        break;
    }
    default:
    {
        return fail;
    }
    }

    return success;
}

int32_t CCentreDataProcessor::ProcessGuildWishNotify(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CPackGuildWish stNotify;
    if(!DecodeMsgPack(stNotify, pszMsgBody, iBodyLength)) return fail;

    m_stSQLDataManager.UpdateGuildWishInfo(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stNotify.m_iCardID, stNotify.m_iCurNum, stNotify.m_bSwap,stNotify.m_bIsFull);
    return success;
}

int32_t CCentreDataProcessor::ProcessGuildWishGetList(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CResponseGuildWishGetList stRsp;

    auto pMap = m_stSQLDataManager.GetGuildWishMap(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
    if(pMap)
    {
        CPackGuildWishDetail stInfo;
        for(auto iter = pMap->begin(); iter != pMap->end(); ++iter)
        {
            m_stSQLDataManager.GetUserBriefInfo(iter->first, iter->second.m_iGroupID, stInfo.m_stInfo);
            stInfo.m_stWish.m_iCardID = iter->second.m_iCardID;
            stInfo.m_stWish.m_iCurNum = iter->second.m_iCurNum;
            stInfo.m_stWish.m_bSwap = iter->second.m_bCanSwap;
            stInfo.m_stWish.m_bIsFull = iter->second.m_bIsFull;
            stRsp.m_stWishVec.push_back(stInfo);
        }
    }

    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGuildDonate(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestGuildDonate stRsp;
    if(!DecodeMsgPack(stRsp, pszMsgBody, iBodyLength)) return fail;

    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

    auto pDonateConfig = cross_config->GetGuildConfig().GetDonateConfig(stRsp.m_iDonateType);
    if (pDonateConfig == nullptr)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    int32_t ret = m_stSQLDataManager.UpdateGuildExp(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID,pDonateConfig->m_iGuildExp);
    if (ret != SERVER_ERRCODE::SUCCESS)
    {
        return SendErrCode(stHeadInfo, ret);
    }

    ret = m_stSQLDataManager.UpdateGuildDonate(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,pDonateConfig);
    if (ret != SERVER_ERRCODE::SUCCESS)
    {
        return SendErrCode(stHeadInfo, ret);
    }
    return SendResponse(stHeadInfo, stRsp);
}


int32_t CCentreDataProcessor::ProcessGuildSignIn(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CCrossGuildSignInReward stRsp;

    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

    int32_t iLevel = 0;
    int32_t ret = m_stSQLDataManager.GetGuildSignRewardLevel(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,iLevel);

    if (ret != SERVER_ERRCODE::SUCCESS)
    {
        return SendErrCode(stHeadInfo, ret);
    }
    stRsp.m_iLevel = iLevel;
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGuildSignUpGrade(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CCrossGuildUpGradeSign cross;

    int32_t ret = m_stSQLDataManager.UpGradeGuildSignRewardLevel(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID);

    if (ret != SERVER_ERRCODE::SUCCESS)
    {
        return SendErrCode(stHeadInfo, ret);
    }
    m_stSQLDataManager.GetGuildSignRewardLevel(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,cross.m_iCurSignLevel);

    return SendResponse(stHeadInfo,cross);
}

int32_t CCentreDataProcessor::ProcessGuildDecorateSave(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestGuildSave stReq;
    if(!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    CResponseGuildSave stAllRsp;
    for(auto unEquip: stReq.m_stUnEquipGoods)
    {
        int32_t bRet = m_stSQLDataManager.UnEquipGoods(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,unEquip);
        if (bRet != SERVER_ERRCODE::SUCCESS)
        {
            return SendErrCode(stHeadInfo,bRet);
        }
        CResponseHomeUnequipGoods stRsp;
        stRsp.m_iHomeID = unEquip.m_iHomeID;
        stRsp.m_iItemID = unEquip.m_iItemID;
        stRsp.m_iPara = unEquip.m_iPara;
        stAllRsp.m_stUnEquipGoods.emplace_back(stRsp);
    }

    for(auto moveGoods: stReq.m_stMoveGoods)
    {
        int32_t bRet = m_stSQLDataManager.MoveGoods(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,moveGoods);
        if (bRet != SERVER_ERRCODE::SUCCESS)
        {
            return SendErrCode(stHeadInfo,bRet);
        }
    }

    for(auto equip: stReq.m_stEquipGood)
    {
        int32_t bRet = m_stSQLDataManager.EquipGoods(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,equip);
        if (bRet != SERVER_ERRCODE::SUCCESS)
        {
            return SendErrCode(stHeadInfo,bRet);
        }
        CResponseHomeEquipGoods equipRsp;
        equipRsp.m_iHomeID = equip.m_iHomeID;
        equipRsp.m_stGoods = equip.m_stGoodsPara;
        stAllRsp.m_stEquipGood.push_back(equipRsp);
    }



    return SendResponse(stHeadInfo,stAllRsp);
}

int32_t CCentreDataProcessor::ProcessGuildGetBossHurtRank(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CResponseGetGuildBossHurtRank stRsp;
    auto stGuildIDVec = m_stSQLDataManager.GetGuildBossHurtRank();

    for(auto& guild_id : stGuildIDVec)
    {
        CPackGuildListInfo stInfo;
        auto pGuild = m_stSQLDataManager.GetGuild(guild_id);
        if(nullptr == pGuild) continue;

        stInfo.m_stGuildInfo = pGuild->GetGuildInfoPack();
        m_stSQLDataManager.GetUserBriefInfo(pGuild->GetChiefUin(), pGuild->GetChiefGroupID(), stInfo.m_stGuildInfo.m_stChiefInfo);
        stInfo.m_bApply = m_stSQLDataManager.IsApplyGuild(guild_id, stHeadInfo.m_iUin);
        stRsp.m_stGuildVec.emplace_back(stInfo);
    }

    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGuildSitBar(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestGuildSitBar stReq;
    if(!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    int32_t bRet = m_stSQLDataManager.AddBarInfo(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,stReq);
    if (bRet != SERVER_ERRCODE::SUCCESS)
    {
        return SendErrCode(stHeadInfo,bRet);
    }

    CResponseGuildSitBar stRsp;
    stRsp.m_iCardID = stReq.m_iCardID;
    stRsp.m_iUid = stReq.m_iUid;
    stRsp.m_iIndex = stReq.m_iIndex;
    stRsp.m_iGroupID = stReq.m_iGroupID;
    stRsp.m_strName = stReq.m_strName;
    stRsp.m_iEndTime = GetNowSec() + stReq.m_iDurationTime;
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGuildGetBarReward(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CCrossGuildGetSitBarReward cross;
    int32_t bRet = m_stSQLDataManager.LeaveBarInfo(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,cross);
    if (bRet != SERVER_ERRCODE::SUCCESS)
    {
        return SendErrCode(stHeadInfo,bRet);
    }
    return SendResponse(stHeadInfo, cross);
}

int32_t CCentreDataProcessor::ProcessGuildUnlockCollection(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestUnlockCollection stReq;
    if(!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    int32_t bRet = m_stSQLDataManager.UnLockGuildCollection(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,stReq);
    if (bRet != SERVER_ERRCODE::SUCCESS)
    {
        return SendErrCode(stHeadInfo,bRet);
    }

    auto guildID = m_stSQLDataManager.GetGuildIDByUin(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
    if(guildID <= 0)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND);
    }

    auto pGuild = m_stSQLDataManager.GetGuild(guildID);
    if(nullptr == pGuild)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_NOT_FOUND);
    }

    CResponseUnlockCollection stRsp;
    stRsp.m_iType = stReq.m_iType;
    stRsp.m_iIndex = stReq.m_iIndex;
    stRsp.m_iGuildID = pGuild->GetGuildID();
    stRsp.m_strGuildName = pGuild->GetGuildName();

    return SendResponse(stHeadInfo, stRsp);
}


int32_t CCentreDataProcessor::ProcessGuildSetDecorate(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestSetDecorate stReq;
    if(!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    auto pGuild = m_stSQLDataManager.GetGuildByUin(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
    if(!pGuild) return fail;

    auto guildDecorate = pGuild->m_stGuildInfo.GetGuildDecorate();

    CPackGuildHomeInfo homeInfo;
    homeInfo.m_iDecorateIndex = stReq.m_iIndex;
    guildDecorate[stReq.m_iHomeID] = homeInfo;

    CResponseSetDecorate stRsp;
    stRsp.m_iHomeID = stReq.m_iHomeID;
    stRsp.m_iIndex = stReq.m_iIndex;
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGuildWarInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CResponseGuildWarInfo stRsp;
    auto pGuild = m_stSQLDataManager.GetGuildByUin(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
    if(!pGuild) return fail;

    std::shared_ptr<CCrossServerConfig> config;
    CCrossServerConfig::GetConfig(config);

    //开始新的公会战周期,清空之前信息
    if (config->GetGuildConfig().m_iGuildWarStartTime >= pGuild->m_stGuildInfo.GetGuildWarEnd())
    {
        pGuild->m_stGuildInfo.SetGuildWarBossInfo({});
        pGuild->m_stGuildInfo.SetGuildWarEnd(config->GetGuildConfig().m_iGuildWarEndTime);
        pGuild->m_stGuildInfo.SetGuildWarScore(0);
        pGuild->m_stGuildInfo.SetGuildWarRecord({});
    }

    if(pGuild->m_stGuildInfo.GetGuildWarBossInfo().empty())
    {
        std::map<int,CPackGuildWarBossInfo> guildWar;
        for(auto& guildWarInfo: config->GetGuildConfig().m_stGuildWarLevel)
        {
            CPackGuildWarBossInfo info;
            info.m_iLevelID = guildWarInfo.second.m_stGuildWarInfo.begin()->first;
            info.m_iBossHp = guildWarInfo.second.m_stGuildWarInfo.begin()->second.m_iBossHp;
            guildWar.insert(std::make_pair(info.m_iLevelID,info));
        }
        pGuild->m_stGuildInfo.SetGuildWarBossInfo(guildWar);
    }
    auto guildWarBossInfo = pGuild->m_stGuildInfo.GetGuildWarBossInfo();
    //检查一遍挑战状态
    time_t iNow = GetNowSec();
    for(auto& bossInfo : guildWarBossInfo)
    {
        if (iNow - bossInfo.second.m_iLastStartTime >= GUILD_WAR_TIME)
        {
            bossInfo.second.m_isBeFight = false;
        }
    }

    stRsp.m_stGuildBossInfo = guildWarBossInfo;
    stRsp.m_stGuildWarRecord = pGuild->m_stGuildInfo.GetGuildWarRecord();
    stRsp.m_iGuildWarScore = pGuild->m_stGuildInfo.GetGuildWarScore();
    stRsp.m_iGuildWarRank = m_stSQLDataManager.m_stGuildWarRankMap[config->GetGuildConfig().m_iRankID].GetRank(pGuild->GetGuildID());
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGuildWarBegin(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestGuildWarBegin stReq;
    if(!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    std::map<int,CPackGuildWarBossInfo> guildWarInfo;
    m_stSQLDataManager.GetSelfGuildWarInfo(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,guildWarInfo);

    auto iterGuildWarInfo = guildWarInfo.find(stReq.m_iLevelID);
    if (iterGuildWarInfo == guildWarInfo.end())
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_WAR_LEVEL_NOT_EXIST);
    }

    time_t iNow = GetNowSec();
    if (iterGuildWarInfo->second.m_isBeFight && iNow - iterGuildWarInfo->second.m_iLastStartTime < GUILD_WAR_TIME)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_WAR_LEVEL_IS_CONTINUE);
    }

    if (iterGuildWarInfo->second.m_iCurHurt >= iterGuildWarInfo->second.m_iBossHp)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_WAR_LEVEL_BOSS_DEAD);
    }

    iterGuildWarInfo->second.m_isBeFight = true;
    iterGuildWarInfo->second.m_iLastStartTime = GetNowSec();
    iterGuildWarInfo->second.m_iUid = stHeadInfo.m_iUin;

    m_stSQLDataManager.UpdateSelfGuildWarInfo(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,guildWarInfo);

    CResponseGuildWarBegin stRsp;
    stRsp.m_iLevelID = stReq.m_iLevelID;
    stRsp.m_stUseCard = stReq.m_stUseCard;
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGuildWarEnd(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestGuildWarEnd stReq;
    if(!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    std::map<int,CPackGuildWarBossInfo> guildWarInfo;
    m_stSQLDataManager.GetSelfGuildWarInfo(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,guildWarInfo);

    auto iterGuildWarInfo = guildWarInfo.find(stReq.m_iLevelID);
    if (iterGuildWarInfo == guildWarInfo.end())
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_WAR_LEVEL_NOT_EXIST);
    }

    if (iterGuildWarInfo->second.m_iUid != stHeadInfo.m_iUin)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_WAR_PASS_TIME);
    }

    if (GetNowSec() - iterGuildWarInfo->second.m_iLastStartTime >= GUILD_WAR_TIME)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_WAR_PASS_TIME);
    }

    //溢出伤害修正
    if (stReq.m_iHurt + iterGuildWarInfo->second.m_iCurHurt > iterGuildWarInfo->second.m_iBossHp)
    {
        stReq.m_iHurt = iterGuildWarInfo->second.m_iBossHp - iterGuildWarInfo->second.m_iCurHurt;
    }

    std::shared_ptr<CCrossServerConfig> config;
    CCrossServerConfig::GetConfig(config);

    //记录伤害
    iterGuildWarInfo->second.m_isBeFight = false;
    iterGuildWarInfo->second.m_iCurHurt += stReq.m_iHurt;

    //boss被击杀进入下一层
    CPackGuildWarBossInfo bossInfo;
    bool bAllKilled = true;
    for(auto& guildWarBoss: guildWarInfo)
    {
        if (guildWarBoss.second.m_iCurHurt < guildWarBoss.second.m_iBossHp)
        {
            bAllKilled = false;
            break;
        }
    }

    //战斗记录
    auto pGuild = m_stSQLDataManager.GetGuildByUin(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
    if(!pGuild) return fail;

    CPackFriendInfo stInfo;
    m_stSQLDataManager.GetUserBriefInfo(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stInfo.m_stBriefInfo);

    auto guildWarRecord = pGuild->m_stGuildInfo.GetGuildWarRecord();
    CPackGuildWarRecord record;
    record.m_iUid = stHeadInfo.m_iUin;
    record.m_iHurt = stReq.m_iHurt;
    record.m_iLevel = stReq.m_iLevelID;
    record.m_strNickName = stInfo.m_stBriefInfo.m_strNick;
    record.m_iTimeStamp = GetNowSec();
    if (guildWarRecord.size() > GUILD_WAR_RECORD_SIZE)
    {
        guildWarRecord.erase(guildWarRecord.begin());
    }
    guildWarRecord.push_back(record);
    pGuild->m_stGuildInfo.SetGuildWarRecord(guildWarRecord);

    if(bAllKilled)
    {
        auto nextDeep = ++iterGuildWarInfo->second.m_iDeep;
        m_stSQLDataManager.ClearSelfGuildWarInfo(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID);

        std::map<int,CPackGuildWarBossInfo> guildWar;
        for(auto& guildWarInfoConfig: config->GetGuildConfig().m_stGuildWarLevel)
        {
            auto iterLevel = guildWarInfoConfig.second.m_stGuildWarInfo.begin();
            for(auto i = nextDeep; i > 0 && iterLevel != guildWarInfoConfig.second.m_stGuildWarInfo.end(); i--)
            {
                iterLevel++;
            }
            if (iterLevel != guildWarInfoConfig.second.m_stGuildWarInfo.end())
            {
                CPackGuildWarBossInfo info;
                info.m_iLevelID = iterLevel->first;
                info.m_iBossHp = iterLevel->second.m_iBossHp;
                info.m_iDeep = nextDeep;
                guildWar.insert(std::make_pair(iterLevel->first,info));
            }
        }
        if(!guildWar.empty())
        {
            m_stSQLDataManager.UpdateSelfGuildWarInfo(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,guildWar);
        }
    }
    else
    {
        auto iterSelfRank = iterGuildWarInfo->second.m_stBossRankInfo.find(stHeadInfo.m_iUin);
        if (iterSelfRank == iterGuildWarInfo->second.m_stBossRankInfo.end())
        {
            CPackGuildWarSingleBossHurt singleBossHurt;
            m_stSQLDataManager.GetUserBriefInfo(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, singleBossHurt.m_stUserBrief);
            singleBossHurt.m_iGuildRankHurt = stReq.m_iHurt;
            iterGuildWarInfo->second.m_stBossRankInfo.insert(std::make_pair(stHeadInfo.m_iUin,singleBossHurt));
        } else
        {
            iterSelfRank->second.m_iGuildRankHurt += stReq.m_iHurt;
        }
        m_stSQLDataManager.UpdateSelfGuildWarInfo(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,guildWarInfo);
    }

    //同时添加公会总伤害
    m_stSQLDataManager.UpdateGuildWarHurt(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,stReq.m_iHurt);

    //同时添加个人伤害
    m_stSQLDataManager.UpdateGuildWarSelfHurt(stHeadInfo.m_iUin,stHeadInfo.m_iGroupID,stReq.m_iHurt);

    //更新公会排行榜
    TGlobalGuildWarRankTableValueType value;
    value.m_iGuildID = pGuild->GetGuildID();
    value.m_ulRankScore = pGuild->m_stGuildInfo.GetGuildWarScore();
    value.m_iGuildRankID = config->GetGuildConfig().m_iRankID;
    m_stSQLDataManager.m_stGuildWarRankMap[config->GetGuildConfig().m_iRankID].UpdateOrInsert(std::make_pair(value.m_iGuildID,value));

    CResponseGuildWarEnd stRsp;
    stRsp.m_stUseCard = stReq.m_stUseCard;
    stRsp.m_iLevelID = stReq.m_iLevelID;
    stRsp.m_iHurt = stReq.m_iHurt;
    stRsp.m_iCurGuildRank = m_stSQLDataManager.m_stGuildWarRankMap[config->GetGuildConfig().m_iRankID].GetRank(pGuild->GetGuildID());
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGuildWarGetRankInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestGetGuildWarRank stReq;
    if(!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    std::shared_ptr<CCrossServerConfig> config;
    CCrossServerConfig::GetConfig(config);

    CResponseGuildWarGetRankInfo stRsp;
    stRsp.m_iIndex = stReq.m_iIndex;

    int32_t iRankID = config->GetGuildConfig().m_iRankID;
    int32_t iNowSec = GetNowSec();

    if (iNowSec < config->GetGuildConfig().m_iGuildWarStartTime)
    {
        iRankID -= 1;
    }

    int32_t iStartIndex = stReq.m_iIndex * 50;
    const int32_t IndexNum = 50;
    auto iterRank = m_stSQLDataManager.m_stGuildWarRankMap[iRankID].begin();
    while(--iStartIndex >= 0 && iterRank != m_stSQLDataManager.m_stGuildWarRankMap[iRankID].end())
    {
        iterRank++;
    }
    for (; iterRank != m_stSQLDataManager.m_stGuildWarRankMap[iRankID].end() && (int32_t)stRsp.m_stTotalRank.size() < IndexNum; iterRank++)
    {
        auto pGuild = m_stSQLDataManager.GetGuild(iterRank->first);
        if(nullptr == pGuild) continue;

        CPackGuildInfo stInfo;
        stInfo = pGuild->GetGuildInfoPack();
        m_stSQLDataManager.GetUserBriefInfo(pGuild->GetChiefUin(), pGuild->GetChiefGroupID(), stInfo.m_stChiefInfo);

        CPackGuildWarTotalRankInfo guildWarRankInfo;
        guildWarRankInfo.m_iGuildID = pGuild->GetGuildID();
        guildWarRankInfo.m_stGuildInfo = stInfo;
        guildWarRankInfo.m_iRank = m_stSQLDataManager.m_stGuildWarRankMap[iRankID].GetRank(pGuild->GetGuildID());
        guildWarRankInfo.m_iHurt = iterRank->second;
        stRsp.m_stTotalRank.emplace_back(guildWarRankInfo);
    }
    return SendResponse(stHeadInfo, stRsp);
}

int32_t CCentreDataProcessor::ProcessGuildWarUpdateRankInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestGuildWarSelfRankInfo stReq;
    if(!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    CResponseGuildWarUpdateRankInfo stRsp;
    auto pGuild = m_stSQLDataManager.GetGuild(stReq.selfRankInfo.m_iGuildID);
    if(nullptr == pGuild) return SendResponse(stHeadInfo, stRsp);

    std::shared_ptr<CCrossServerConfig> config;
    CCrossServerConfig::GetConfig(config);

    auto iterGuildWarRank = m_stSQLDataManager.m_stGuildWarRankMap.find(stReq.selfRankInfo.m_iRankID);
    if (iterGuildWarRank != m_stSQLDataManager.m_stGuildWarRankMap.end())
    {
        auto iRank = iterGuildWarRank->second.GetRank(pGuild->GetGuildID());
        stRsp.m_stRankSelf.m_iRank = iRank;
    }
    return SendResponse(stHeadInfo, stRsp);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int32_t CCentreDataProcessor::RecoveryDataFromFile()
{
    constexpr int32_t md5_length = 32;
    std::ifstream ifs(DUMP_CENTRE_DATA_FILE);
    std::string strData = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    ifs.close();

    if (strData.empty())
    {
        return success;
    }

    if (strData.length() < md5_length)
    {
        Log(log_mask_system_error, "[CCentreDataProcessor:%s] centre data file recovery fail, lack data\n" , __FUNCTION__);
        return fail;
    }

    std::string_view data_view = strData;
    auto md5_view = data_view.substr(0, 32);
    auto msg_view = data_view.substr(32);

    const auto calc_md5 = taf::TC_MD5::md5str(msg_view);
    if (md5_view != calc_md5)
    {
        Log(log_mask_system_error, "[CCentreDataProcessor:%s] centre data file recovery fail, calc md5 is %s, not equal\n" , __FUNCTION__, calc_md5.c_str());
        return fail;
    }

    CPackCentreDataRecovery stRecoveryData;
    if (!DecodeMsgPack(stRecoveryData, msg_view.data(), msg_view.size()))
    {
        Log(log_mask_system_error, "[CCentreDataProcessor:%s] centre data file recovery fail, decode msg pack fail\n" , __FUNCTION__);
        return fail;
    }

    m_stSQLDataManager.SetGuildCache(stRecoveryData.m_stGuildCache);

    return success;
}

int32_t CCentreDataProcessor::SaveDataToFile()
{
    CPackCentreDataRecovery stRecoveryData;

    // 保存
    stRecoveryData.m_stGuildCache = m_stSQLDataManager.GetGuildCache();

    msgpack::sbuffer stBuffer;
    msgpack::pack(stBuffer, stRecoveryData);
    const std::string strTotalData(stBuffer.data(), stBuffer.size());
    const std::string md5 = taf::TC_MD5::md5str(strTotalData);

    std::ofstream ofs(DUMP_CENTRE_DATA_FILE, std::ofstream::out | std::ofstream::binary);
    ofs << md5 << strTotalData;
    ofs.close();

    return success;
}


