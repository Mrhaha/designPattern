#include "TeamProcessor.h"
#include <fstream>
#include <random>
#include "tc_md5.h"
#include "ServerConfig.h"
#include "server_errcode.h"
#include "tc_common.h"
#include "server_task_protocol.h"
#include "common_datetime.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "common_utf8handle.h"
#include "GameControl.h"

#define DUMP_TEAM_DATA_FILE "./team_data"

const int32_t BOSS_NUMBER_LIMIT = 5;    //当前待击杀boss限制数量
const int32_t BOSS_LIMIT_TIME = 120;     //Boss战斗超时间隔
const int32_t BOSS_INVITE_LIMIT = 3;    //Boss邀请人数限制

CTeamProcessor::CTeamProcessor(void)
{
    m_iLastRefreshTime = 0;
    m_iLastDailyRefreshTime = 0;
    m_uiBanquetID = 0;
}

CTeamProcessor::~CTeamProcessor(void)
{
}

bool CTeamProcessor::Initialize(int32_t nLogMask, const char* pszLogFileName, int32_t nProcessID)
{
    if (!CProcessor::Initialize(nLogMask, pszLogFileName, nProcessID))
    {
        return false;
    }

    m_iLastDailyRefreshTime = GetNowSec();
    m_iLastRefreshTime = GetNowSec() + 600;

    if (success != RecoveryDataFromFile())
    {
        Log(log_mask_system_error, "[CTeamProcessor:%s]RecoveryDataFromFile error!\n", __FUNCTION__);
        return false;
    }

    return true;
}

bool CTeamProcessor::Stop()
{
    SaveDataToFile();

    return CProcessor::Stop();
}

void CTeamProcessor::Refresh()
{
    const int32_t iNowSec = GetNowSec();
    // 队伍刷新
    if(iNowSec > m_iLastRefreshTime)
    {
        m_iLastRefreshTime = iNowSec + 1;

        RefreshTeam();
        RefreshLimitBoss();
    }

    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);
    const auto &dataReportConfig = cross_config->GetDataReportConfig();

    tm nowStruct = { };
    const time_t tNowSec = iNowSec;
    localtime_r(&tNowSec, &nowStruct);
    if(!is_same_day(iNowSec, m_iLastDailyRefreshTime) && nowStruct.tm_hour >= dataReportConfig.m_hour
        && nowStruct.tm_min >= dataReportConfig.m_min)
    {
        m_iLastDailyRefreshTime = iNowSec;

        SaveDataToFile();
    }
}

int32_t CTeamProcessor::ProcessMessages(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    int32_t iRet = fail;
    switch(stHeadInfo.m_usCmd)
    {
    case MSG_CROSS_CREATE_BANQUET_TEAM:
        iRet = ProcessCreateBanquetTeam(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_JOIN_BANQUET_TEAM:
        iRet = ProcessJoinBanquetTeam(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_GET_BANQUET_INFO:
        iRet = ProcessGetBanquetInfo(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_BANQUET_UPDATE_LOGIN:
        iRet = ProcessBanquetUpdateLogin(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_LOGIC_BANQUET_INVITE:
        iRet = ProcessBanquetInvite(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_BANQUET_QUIT_TEAM:
        iRet = ProcessBanquetQuitTeam(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_BANQUET_GET_FRIEND_OPEN:
        iRet = ProcessBanquetGetOpenTeam(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_NOTIFY:
        iRet = ProcessNewLimitBoss(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_HISTORY:
        iRet = ProcessGetLimitBossHistory(stHeadInfo,pszMsgBody,iBodyLength);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_BEGIN:
        iRet = ProcessLimitBossBegin(stHeadInfo,pszMsgBody,iBodyLength);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_END:
        iRet = ProcessLimitBossEnd(stHeadInfo,pszMsgBody,iBodyLength);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_INVITE:
        iRet = ProcessLimitBossInvite(stHeadInfo,pszMsgBody,iBodyLength);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_GET_INFO:
        iRet = ProcessLimitBossGetInfo(stHeadInfo,pszMsgBody,iBodyLength);
        break;
    default:
        Log(log_mask_system_error, "[CTeamProcessor:%s]team recv cmd %u not process\n", __FUNCTION__, stHeadInfo.m_usCmd);
        break;
    }

    return iRet;
}

int32_t CTeamProcessor::ProcessCreateBanquetTeam(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CCrossRequestCreateBanquetTeam stCross;
    if(!DecodeMsgPack(stCross, pszMsgBody, iBodyLength)) return fail;

    CCrossResponseCreateBanquetTeam stRsp;
    stRsp.m_iItemID = stCross.m_iItemID;
    auto ret = CreateOrUpdateTeam(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stCross,stRsp.m_uiTeamID);
    if(ret != SERVER_ERRCODE::SUCCESS)
    {
        return SendErrCode(stHeadInfo, ret);
    }

    SendResponse(stHeadInfo, stRsp);
    return success;
}

int32_t CTeamProcessor::ProcessJoinBanquetTeam(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestJoinBanquetTeam stCross;
    if(!DecodeMsgPack(stCross, pszMsgBody, iBodyLength)) return fail;

    auto ret = JoinTeam(stCross.m_iTeamID, stCross.m_iShowCardID, stCross.m_stBrief, stCross.m_cPos,stCross.m_iSkinID);
    if(ret != SERVER_ERRCODE::SUCCESS)
    {
        return SendErrCode(stHeadInfo, ret);
    }

    CResponseJoinBanquetTeam stRsp;
    stRsp.m_iTeamID = stCross.m_iTeamID;
    SendResponse(stHeadInfo, stRsp);

    return success;
}

int32_t CTeamProcessor::ProcessGetBanquetInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestGetBanquetInfo stCross;
    if(!DecodeMsgPack(stCross, pszMsgBody, iBodyLength)) return fail;

    auto pTeam = GetTeamInfo(stCross.m_iTeamID);
    if(!pTeam)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::TEAM_NOT_EXSIT);
    }

    CNotifyBanquetInfo stRsp;
    stRsp.m_stBanquetInfo = *pTeam;
    SendResponse(stHeadInfo, stRsp);

    return success;
}

int32_t CTeamProcessor::ProcessBanquetUpdateLogin(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CNotifyBanquetUpdateLogin stNotify;
    auto stTeamIds = GetTeamIDByUin(stHeadInfo.m_iUin);
    if (nullptr == stTeamIds) {
        stNotify.m_iCreateId = 0;
        stNotify.m_iJoinId = 0;
    } else {
        stNotify.m_iCreateId = stTeamIds->m_iCreateId;
        stNotify.m_iJoinId = stTeamIds->m_iJoinId;
    }

    SendResponse(stHeadInfo, stNotify);
    return success;
}

int32_t CTeamProcessor::ProcessBanquetInvite(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CCrossBanquetInvite stCross;
    if(!DecodeMsgPack(stCross, pszMsgBody, iBodyLength)) return fail;

    auto stTeamIds = GetTeamIDByUin(stHeadInfo.m_iUin);
    if (!stTeamIds)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::TEAM_NOT_EXSIT);
    }

    auto pTeam = GetTeamInfo(stTeamIds->m_iCreateId);
    if(!pTeam)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::TEAM_NOT_EXSIT);
    }

    if(int(pTeam->m_stGuestMap.size()) >= pTeam->m_iGuestMaxNum)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::TEAM_IS_FULL);
    }

    if(pTeam->m_iBanquetEndTime < GetNowSec())
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::BANQUET_IS_END);
    }

    CCrossNotifyBanquetInvite stNotify;
    stNotify.m_iTeamID = pTeam->m_iTeamID;
    stNotify.m_stOwnerBrief = stCross.m_stBrief;
    stNotify.m_iHomeID = stCross.m_iHomeID;
    stNotify.m_iItemID = pTeam->m_iItemID;
    stNotify.m_iEndTime = pTeam->m_iBanquetEndTime;
    for(auto& iter : stCross.m_stUinMap)
    {
        if(pTeam->m_stInviteSet.size() >= 50)
            pTeam->m_stInviteSet.erase(pTeam->m_stInviteSet.begin());
        pTeam->m_stInviteSet.insert(iter.first);

        NotifyMessage(iter.second, iter.first, MSG_CROSS_BANQUET_INVITE_NOTIFY, stNotify);
    }

    CResponseBody stRsp;
    SendResponse(stHeadInfo, stRsp);

    return success;
}

int32_t CTeamProcessor::ProcessBanquetQuitTeam(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength) {

    CCrossRequestBanquetQuitTeam stCross;
    if(!DecodeMsgPack(stCross, pszMsgBody, iBodyLength)) return fail;

    auto pTeam = GetTeamInfo(stCross.m_iTeamId);
    if(!pTeam)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::TEAM_NOT_EXSIT);
    }

    for(auto& iterGuest: pTeam->m_stGuestMap)
    {
        if (stHeadInfo.m_iUin == iterGuest.second.m_stBrief.m_iUid)
        {
            if(iterGuest.second.m_iRewardEndTime < GetNowSec())
            {
                return SendErrCode(stHeadInfo, SERVER_ERRCODE::BANQUET_IS_END);
            }
        }
    }

    auto ptrTeamIds = GetTeamIDByUin(stHeadInfo.m_iUin);
    if(nullptr == ptrTeamIds || 0 == ptrTeamIds->m_iJoinId)
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::NOT_IN_TEAM);

    for(auto& iterGuest: pTeam->m_stGuestMap)
    {
        if (stHeadInfo.m_iUin == iterGuest.second.m_stBrief.m_iUid)
        {
            pTeam->m_stGuestMap.erase(iterGuest.first);
            break;
        }
    }

    ptrTeamIds->m_iJoinId = 0;

    //通知队伍变更
    NotifyTeamInfo(pTeam);

    CCrossResponseQuitBanquetTeam stRsp;
    stRsp.m_iQuitId = stCross.m_iTeamId;
    SendResponse(stHeadInfo, stRsp);
    return success;
}

int32_t CTeamProcessor::ProcessBanquetGetOpenTeam(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CCrossResponseOpenBanquetTeam stRsp;

    CCrossRequestFriendBanquet stCross;
    if(!DecodeMsgPack(stCross, pszMsgBody, iBodyLength)) return fail;

    for (auto iterFriendTeam: stCross.m_stUserVec)
    {
        auto stTeamIds = GetTeamIDByUin(iterFriendTeam.m_iUin);
        if (!stTeamIds)
        {
            continue;
        }
        auto pTeam = GetTeamInfo(stTeamIds->m_iCreateId);
        if(!pTeam)
        {
            continue;
        }
        stRsp.m_stOpenBanquetInfo.emplace_back(*pTeam);
    }
    SendResponse(stHeadInfo, stRsp);
    return success;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPackBanquetTeamIds* CTeamProcessor::GetTeamIDByUin(int32_t iUin)
{
    auto iter = m_stUinToTeamID.find(iUin);
    return iter == m_stUinToTeamID.end() ? nullptr : &(iter->second);
}

CPackBanquetTeamInfo* CTeamProcessor::GetTeamInfo(uint64_t iTeamID)
{
    auto iter = m_stTeamMap.find(iTeamID);
    return iter == m_stTeamMap.end() ? nullptr : &(iter->second);
}

int32_t CTeamProcessor::CreateOrUpdateTeam(int iUin, int iGroupID, const CCrossRequestCreateBanquetTeam& stParam,uint64_t& teamID)
{
    auto ptrTeamIds = GetTeamIDByUin(iUin);
    if(nullptr != ptrTeamIds && 0 != ptrTeamIds->m_iCreateId)
        return SERVER_ERRCODE::BANQUET_IN_PROGRESS;

    if(stParam.m_cPos < 1 || stParam.m_cPos > stParam.m_iGuestNum)
        return SERVER_ERRCODE::PARAMETER_ERROR;


    int32_t iNow = GetNowSec()/1000;
    std::string strBanquetID = std::to_string(iUin) + std::to_string(iNow);
    uint64_t banquetID = std::atoll(strBanquetID.c_str());

    CPackBanquetTeamInfo stInfo;
    auto ret = m_stTeamMap.insert(std::make_pair(banquetID, stInfo));
    if(!ret.second)
        return SERVER_ERRCODE::INTERN_ERROR;

    auto pTeam = &(ret.first->second);

    pTeam->m_iUin = iUin;
    pTeam->m_iGroupID = iGroupID;
    pTeam->m_iHomeID = stParam.m_iHomeID;
    pTeam->m_iItemID = stParam.m_iItemID;
    pTeam->m_iBanquetEndTime = GetNowSec() + stParam.m_iBanquetTime;
    pTeam->m_iRewardTime = stParam.m_iRewardTime;
    pTeam->m_iGuestMaxNum = stParam.m_iGuestNum;
    pTeam->m_bOpen = stParam.m_bOpen;
    pTeam->m_stInviteSet.clear();

    // 添加宴会主人
    CPackBanquetGuestInfo stGuest;
    stGuest.m_iItemID = stParam.m_iItemID;
    stGuest.m_iRewardEndTime = GetNowSec() + stParam.m_iRewardTime;
    stGuest.m_iShowCardID = stParam.m_iShowCardID;
    stGuest.m_stBrief = stParam.m_stBrief;
    stGuest.m_iSkinID = stParam.m_iSkinID;
    pTeam->m_stGuestMap[stParam.m_cPos] = std::move(stGuest);
    pTeam->m_iTeamID = banquetID;
    // 记录组ID
    if (nullptr == ptrTeamIds) {
        CPackBanquetTeamIds stTeamIds;
        stTeamIds.m_iCreateId = banquetID;
        m_stUinToTeamID[iUin] = stTeamIds;
    } else {
        m_stUinToTeamID[iUin].m_iCreateId = banquetID;
    }

    // 通知队伍变更
    NotifyTeamInfo(pTeam);
    teamID = banquetID;
    return SERVER_ERRCODE::SUCCESS;
}

int32_t CTeamProcessor::JoinTeam(uint64_t iTeamID, int iShowCard, const CPackUserBrief& stBrief, char cPos,int32_t iSkinID)
{

    auto ptrTeamIds = GetTeamIDByUin(stBrief.m_iUid);
    if(nullptr != ptrTeamIds && 0 != ptrTeamIds->m_iJoinId)
        return SERVER_ERRCODE::BANQUET_IN_PROGRESS;

    auto pTeam = GetTeamInfo(iTeamID);
    if(!pTeam) return SERVER_ERRCODE::TEAM_NOT_EXSIT;

    if(cPos < 1 || cPos > pTeam->m_iGuestMaxNum)
        return SERVER_ERRCODE::PARAMETER_ERROR;

    if(pTeam->m_iBanquetEndTime < GetNowSec())
        return SERVER_ERRCODE::BANQUET_IS_END;

    if(int(pTeam->m_stGuestMap.size()) >= pTeam->m_iGuestMaxNum)
        return SERVER_ERRCODE::TEAM_IS_FULL;

    if(pTeam->m_stGuestMap.find(cPos) != pTeam->m_stGuestMap.end())
        return SERVER_ERRCODE::POS_ERROR;

    // 是否被邀请
    if(!pTeam->m_bOpen && pTeam->m_stInviteSet.find(stBrief.m_iUid) == pTeam->m_stInviteSet.end())
        return SERVER_ERRCODE::NOT_IN_INVITE;

    // 添加
    CPackBanquetGuestInfo stGuest;
    stGuest.m_iItemID = pTeam->m_iItemID;
    stGuest.m_iRewardEndTime = GetNowSec() + pTeam->m_iRewardTime;
    stGuest.m_iShowCardID = iShowCard;
    stGuest.m_stBrief = stBrief;
    stGuest.m_iSkinID = iSkinID;
    pTeam->m_stGuestMap[cPos] = std::move(stGuest);

    // 记录组ID
    if (nullptr == ptrTeamIds) {
        CPackBanquetTeamIds stTeamIds;
        stTeamIds.m_iJoinId = iTeamID;
        m_stUinToTeamID[stBrief.m_iUid] = stTeamIds;
    } else {
        m_stUinToTeamID[stBrief.m_iUid].m_iJoinId = iTeamID;
    }

    // 通知队伍变更
    NotifyTeamInfo(pTeam);

    return SERVER_ERRCODE::SUCCESS;
}

void CTeamProcessor::NotifyTeamInfo(const CPackBanquetTeamInfo* pTeam)
{
    CNotifyBanquetInfo stNotify;
    stNotify.m_stBanquetInfo = *pTeam;
    for(auto& player : pTeam->m_stGuestMap)
    {
        NotifyMessage(player.second.m_stBrief.m_iGroupID, player.second.m_stBrief.m_iUid, MSG_CROSS_BANQUET_INFO_NOTIFY, stNotify);
    }
}

void CTeamProcessor::RefreshTeam()
{
    const int iNowSec = GetNowSec();


    auto iter = m_stTeamMap.begin();
    auto iterEnd = m_stTeamMap.end();
    for(; iter != iterEnd; )
    {
        auto& stGuestMap = iter->second.m_stGuestMap;
        auto iterGuest = stGuestMap.begin();
        auto iterGuestEnd = stGuestMap.end();
        for(; iterGuest != iterGuestEnd; )
        {
            if(iterGuest->second.m_iRewardEndTime <= iNowSec && IsGroupConnecting(iterGuest->second.m_stBrief.m_iGroupID))
            {
                // 发奖时间已到
                CCrossNotifyBanquetReward stNotify;
                auto& stGuest = iterGuest->second;
                stNotify.m_iItemID = stGuest.m_iItemID;
                Log(log_mask_system_error, "[CTeamProcessor:%s]Banquet Reward Notify.[%d:%d:%d]\n",
                    __FUNCTION__, stGuest.m_stBrief.m_iGroupID, stGuest.m_stBrief.m_iUid, stGuest.m_iItemID);

                // 当前玩家没有正在参加或者创建的宴会才能移除
                auto ptrTeamIDs = GetTeamIDByUin(stGuest.m_stBrief.m_iUid);
                if (nullptr != ptrTeamIDs)
                {
                    if (iter->first == ptrTeamIDs->m_iCreateId)
                    {
                        ptrTeamIDs->m_iCreateId = 0;
                    }
                    else if (iter->first == ptrTeamIDs->m_iJoinId)
                    {
                        ptrTeamIDs->m_iJoinId = 0;
                    }

                    stNotify.m_iJoinID = ptrTeamIDs->m_iJoinId;
                    stNotify.m_iCreateID = ptrTeamIDs->m_iCreateId;
                    stNotify.m_iEndTeamID = iter->first;

                    NotifyMessage(stGuest.m_stBrief.m_iGroupID, stGuest.m_stBrief.m_iUid, MSG_CROSS_BANQUET_FINISH_NOTIFY, stNotify);

                    if (0 == ptrTeamIDs->m_iCreateId && 0 == ptrTeamIDs->m_iJoinId)
                    {
                        m_stUinToTeamID.erase(stGuest.m_stBrief.m_iUid);
                    }
                    iterGuest = stGuestMap.erase(iterGuest);
                }
            }
            else
            {
                ++iterGuest;
            }
        }

        // 如果队伍空了，移除队伍,且宴会结束时间到了
        if(stGuestMap.empty() && iter->second.m_iBanquetEndTime <= iNowSec)
        {
            iter = m_stTeamMap.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

int32_t CTeamProcessor::RecoveryDataFromFile()
{
    constexpr int32_t md5_length = 32;
    std::ifstream ifs(DUMP_TEAM_DATA_FILE);
    std::string strData = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    ifs.close();

    if (strData.empty())
    {
        return success;
    }

    if (strData.length() < md5_length)
    {
        Log(log_mask_system_error, "[CTeamProcessor:%s] team data file recovery fail, lack data\n" , __FUNCTION__);
        return fail;
    }

    std::string_view data_view = strData;
    auto md5_view = data_view.substr(0, 32);
    auto msg_view = data_view.substr(32);

    const auto calc_md5 = taf::TC_MD5::md5str(msg_view);
    if (md5_view != calc_md5)
    {
        Log(log_mask_system_error, "[CTeamProcessor:%s] team data file recovery fail, calc md5 is %s, not equal\n" , __FUNCTION__, calc_md5.c_str());
        return fail;
    }

    CPackTeamRecovery stRecoveryData;
    if (!DecodeMsgPack(stRecoveryData, msg_view.data(), msg_view.size()))
    {
        Log(log_mask_system_error, "[CTeamProcessor:%s] team data file recovery fail, decode msg pack fail\n" , __FUNCTION__);
        return fail;
    }

    m_stUinToTeamID = stRecoveryData.m_stUinToTeamID;
    m_stTeamMap = stRecoveryData.m_stTeamMap;

    m_stUin2LimitBossInfo = stRecoveryData.m_stUin2LimitBossInfo;
    m_stUin2LimitBossTeamID = stRecoveryData.m_stUin2LimitBossTeamID;
    return success;
}

int32_t CTeamProcessor::SaveDataToFile()
{
    CPackTeamRecovery stRecoveryData;

    stRecoveryData.m_stUinToTeamID = m_stUinToTeamID;
    stRecoveryData.m_stTeamMap = m_stTeamMap;
    stRecoveryData.m_stUin2LimitBossInfo = m_stUin2LimitBossInfo;
    stRecoveryData.m_stUin2LimitBossTeamID = m_stUin2LimitBossTeamID;

    msgpack::sbuffer stBuffer;
    msgpack::pack(stBuffer, stRecoveryData);
    const std::string strTotalData(stBuffer.data(), stBuffer.size());
    const std::string md5 = taf::TC_MD5::md5str(strTotalData);

    std::ofstream ofs(DUMP_TEAM_DATA_FILE, std::ofstream::out | std::ofstream::binary);
    ofs << md5 << strTotalData;
    ofs.close();

    return success;
}

CPackLimitBossTeamInfo* CTeamProcessor::GetBossTeamInfoByUin(int32_t iUin)
{
    auto iter = m_stUin2LimitBossInfo.find(iUin);
    return iter == m_stUin2LimitBossInfo.end() ? nullptr : &(iter->second);
}



int32_t CTeamProcessor::ProcessNewLimitBoss(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CCrossNotifyNewLimitBoss stCross;
    if(!DecodeMsgPack(stCross, pszMsgBody, iBodyLength)) return fail;

    auto pBossInfo = GetBossTeamInfoByUin(stCross.m_stBossInfo.m_iOwnerUin);
    if(pBossInfo)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::LIMIT_BOSS_HAS_EXIST);
    }

    CPackLimitBossTeamInfo teamInfo;
    teamInfo.m_stBossInfo = stCross.m_stBossInfo;
    teamInfo.m_stBossInfo.m_iEndTime = GetNowSec() + teamInfo.m_stBossInfo.m_iLimitTime;

    CPackLimitBossTeamInfo::user_key userKey;
    userKey.m_iUin = stHeadInfo.m_iUin;
    userKey.m_iGroupID = stHeadInfo.m_iGroupID;
    userKey.m_strName = stCross.m_strFinderName;
    userKey.m_iIconHead = stCross.m_iIconHead;
    userKey.m_iIconImage = stCross.m_iIconImage;
    teamInfo.m_stInviteUser.insert(std::make_pair(stHeadInfo.m_iUin,userKey));

    auto ret = m_stUin2LimitBossInfo.insert(std::make_pair(stCross.m_stBossInfo.m_iOwnerUin,teamInfo));
    if (!ret.second)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::INTERN_ERROR);
    }

    auto iterTeamID = m_stUin2LimitBossTeamID.find(stHeadInfo.m_iUin);
    if (iterTeamID != m_stUin2LimitBossTeamID.end())
    {
        iterTeamID->second.insert(stHeadInfo.m_iUin);
    } else
    {
        std::set<int32_t> setTeamID;
        setTeamID.insert(stHeadInfo.m_iUin);
        m_stUin2LimitBossTeamID.insert(std::make_pair(stHeadInfo.m_iUin,setTeamID));
    }

    std::string stEmpty;
    return SendResponse(stHeadInfo, stEmpty);
}

int32_t  CTeamProcessor::ProcessGetLimitBossHistory(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CResponseGetLimitBossHistory stRsp;
    const auto& iterTeamIDs =  m_stUin2LimitBossTeamID.find(stHeadInfo.m_iUin);
    if (iterTeamIDs != m_stUin2LimitBossTeamID.end())
    {
        for (auto teamID: iterTeamIDs->second)
        {
            auto iterBossInfo = m_stUin2LimitBossInfo.find(teamID);
            if (iterBossInfo != m_stUin2LimitBossInfo.end())
            {
                stRsp.m_stHistoryInfo.emplace_back(iterBossInfo->second);
            }
        }
    }
    return SendResponse(stHeadInfo, stRsp);
}

int32_t  CTeamProcessor::ProcessLimitBossBegin(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestLimitBossBegin stReq;
    if(!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    auto pBossInfo = GetBossTeamInfoByUin(stReq.m_iOwnerUin);
    if(!pBossInfo)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::LIMIT_BOSS_NOT_EXIST);
    }

    if (pBossInfo->m_stBossInfo.m_iEndTime < GetNowSec())
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::LIMIT_BOSS_TIME_OVER);
    }

    if (pBossInfo->m_stBossInfo.m_ulCurHurt > pBossInfo->m_stBossInfo.m_ulMaxHp)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::LIMIT_BOSS_DEAD);
    }

    auto iterInviteUser = pBossInfo->m_stInviteUser.find(stHeadInfo.m_iUin);
    if (iterInviteUser != pBossInfo->m_stInviteUser.end() && iterInviteUser->second.m_iGroupID == stHeadInfo.m_iGroupID)
    {
        iterInviteUser->second.m_bIsFight = true;
        pBossInfo->m_stBossInfo.m_iIsFighting += 1;
        pBossInfo->m_stBossInfo.m_iLastFightTime = GetNowSec();
    } else
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::LIMIT_BOSS_INVITE_NOT_EXIST);
    }

    return SendResponse(stHeadInfo, stReq);
}


int32_t  CTeamProcessor::ProcessLimitBossEnd(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestLimitBossEnd stReq;

    if(!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    auto pBossInfo = GetBossTeamInfoByUin(stReq.m_iOwnerUin);
    if(!pBossInfo)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::LIMIT_BOSS_NOT_EXIST);
    }

    //如果已死亡,本次伤害不计入
    if (pBossInfo->m_stBossInfo.m_ulMaxHp <= pBossInfo->m_stBossInfo.m_ulCurHurt)
    {
        return success;
    }

    auto iterInviterUser = pBossInfo->m_stInviteUser.find(stHeadInfo.m_iUin);
    if (iterInviterUser != pBossInfo->m_stInviteUser.end() && iterInviterUser->second.m_iGroupID == stHeadInfo.m_iGroupID)
    {
        if (stReq.m_ulHurt + pBossInfo->m_stBossInfo.m_ulCurHurt >= pBossInfo->m_stBossInfo.m_ulMaxHp)
        {
            stReq.m_ulHurt = pBossInfo->m_stBossInfo.m_ulMaxHp - pBossInfo->m_stBossInfo.m_ulCurHurt;
        }

        iterInviterUser->second.m_ulHurt += stReq.m_ulHurt;
        pBossInfo->m_stBossInfo.m_ulCurHurt += stReq.m_ulHurt;
    } else
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::LIMIT_BOSS_INVITE_NOT_EXIST);
    }

    pBossInfo->m_stBossInfo.m_iIsFighting -= 1;

    if (pBossInfo->m_stBossInfo.m_ulMaxHp <= pBossInfo->m_stBossInfo.m_ulCurHurt)
    {
        CCrossLimitBossDeadNotify stNotify;
        stNotify.m_iOwnerUin = pBossInfo->m_stBossInfo.m_iOwnerUin;
        stNotify.m_iOwnerGroupID = pBossInfo->m_stBossInfo.m_iOwnerGroupID;
        stNotify.m_iLevelID = pBossInfo->m_stBossInfo.m_iLevelID;
        stNotify.m_stTeamInfo = *pBossInfo;

        for (auto userKey: pBossInfo->m_stInviteUser)
        {
            NotifyMessage(userKey.second.m_iGroupID, userKey.second.m_iUin, MSG_CROSS_NEW_LIMIT_BOSS_DEAD_NOTIFY, stNotify);

            auto iterTeamIDSet =  m_stUin2LimitBossTeamID.find(userKey.first);
            if (iterTeamIDSet != m_stUin2LimitBossTeamID.end())
            {
                iterTeamIDSet->second.erase(pBossInfo->m_stBossInfo.m_iOwnerUin);

                if (iterTeamIDSet->second.empty())
                {
                    m_stUin2LimitBossTeamID.erase(userKey.first);
                }
            }
        }
        m_stUin2LimitBossInfo.erase(pBossInfo->m_stBossInfo.m_iOwnerUin);
    }

    CCrossLimitBossLevelEnd stCross;
    stCross.m_iLevelID = stReq.m_iLevelID;
    stCross.m_iOwnerUin = stReq.m_iOwnerUin;
    stCross.m_iOwnerGroupID = stReq.m_iOwnerGroupID;

    return SendResponse(stHeadInfo, stCross);
}


int32_t  CTeamProcessor::ProcessLimitBossInvite(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestNewLimitBossInvite stReq;
    if(!DecodeMsgPack(stReq, pszMsgBody, iBodyLength)) return fail;

    auto pBossInfo = GetBossTeamInfoByUin(stHeadInfo.m_iUin);
    if(!pBossInfo)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::LIMIT_BOSS_NOT_EXIST);
    }

    //inviterUser包含自己
    if (pBossInfo->m_stInviteUser.size() >= BOSS_INVITE_LIMIT + 1)
    {
        return SendErrCode(stHeadInfo,SERVER_ERRCODE::LIMIT_BOSS_INVITE_LIMIT);
    }

    CCrossLimitBossInviteNotify stNotify;
    stNotify.m_stBossInfo = pBossInfo->m_stBossInfo;

    CPackLimitBossTeamInfo::user_key userKey;
    for (auto inviteUser : stReq.m_stInviteUid)
    {
        auto iterTeamID = m_stUin2LimitBossTeamID.find(inviteUser.first);
        auto iterInviteUser = pBossInfo->m_stInviteUser.find(inviteUser.first);

        //boss的邀请列表中没有
        if (iterInviteUser == pBossInfo->m_stInviteUser.end())
        {
            if (inviteUser.second.size() < 3)
            {
                return SendErrCode(stHeadInfo, SERVER_ERRCODE::PARAMETER_ERROR);
            }

            //被邀请人如果当前待击杀Boss超过配置人数跳过
            if (iterTeamID != m_stUin2LimitBossTeamID.end() && iterTeamID->second.size() > BOSS_NUMBER_LIMIT)
            {
                continue;
            }

            userKey.m_iUin = inviteUser.first;
            userKey.m_iGroupID = inviteUser.second[0];
            userKey.m_iIconHead = inviteUser.second[1];
            userKey.m_iIconImage = inviteUser.second[2];
            userKey.m_strName = stReq.m_stUid2Name[inviteUser.first];
            pBossInfo->m_stInviteUser.insert(std::make_pair(inviteUser.first,userKey));
        } else {//存在就跳过
            continue;
        }

        if (iterTeamID != m_stUin2LimitBossTeamID.end())
        {
            iterTeamID->second.insert(stHeadInfo.m_iUin);
        } else
        {
            std::set<int32_t> setTeamID;
            setTeamID.insert(stHeadInfo.m_iUin);
            m_stUin2LimitBossTeamID.insert(std::make_pair(inviteUser.first,setTeamID));
        }

        NotifyMessage(inviteUser.second[0], inviteUser.first, MSG_CROSS_NEW_LIMIT_BOSS_INVITE_NOTIFY, stNotify);
    }

    return success;
}

int32_t CTeamProcessor::ProcessLimitBossGetInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CNotifyLimitBossInfo stNotify;
    auto pBossInfo = GetBossTeamInfoByUin(stHeadInfo.m_iUin);
    stNotify.m_bIsExist = (pBossInfo != nullptr);
    SendResponse(stHeadInfo, stNotify);
    return success;
}



void CTeamProcessor::RefreshLimitBoss()
{
    const int iNowSec = GetNowSec();
    CNotifyLimitBossRunaway stNotify;
    auto iter = m_stUin2LimitBossInfo.begin();
    auto iterEnd = m_stUin2LimitBossInfo.end();

    for (;iter != iterEnd;)
    {
        //刷新Boss挑战状态,防止客户端end没发送导致boss无法逃逸
        if (0 != iter->second.m_stBossInfo.m_iIsFighting && iter->second.m_stBossInfo.m_iLastFightTime + BOSS_LIMIT_TIME <= iNowSec)
        {
            iter->second.m_stBossInfo.m_iIsFighting = 0;
        }
        //逃逸时间到，且未死亡,且不在挑战中
        if (!iter->second.m_stBossInfo.m_iIsFighting &&
            iter->second.m_stBossInfo.m_iEndTime < iNowSec &&
            iter->second.m_stBossInfo.m_ulCurHurt < iter->second.m_stBossInfo.m_ulMaxHp)
        {
            iter->second.m_stBossInfo.m_bIsRun = true;
            stNotify.m_stBossTeamInfo = iter->second;
            for (auto iterInviteUser: iter->second.m_stInviteUser)
            {
                NotifyMessage(iterInviteUser.second.m_iGroupID, iterInviteUser.second.m_iUin, MSG_CROSS_NEW_LIMIT_BOSS_RUN_NOTIFY, stNotify);

                auto iterTeamIDSet =  m_stUin2LimitBossTeamID.find(iterInviteUser.first);
                if (iterTeamIDSet != m_stUin2LimitBossTeamID.end())
                {
                    iterTeamIDSet->second.erase(iter->second.m_stBossInfo.m_iOwnerUin);

                    if (iterTeamIDSet->second.empty())
                    {
                        m_stUin2LimitBossTeamID.erase(iterTeamIDSet);
                    }

                } else
                {
                    Log(log_mask_system_error, "[CTeamProcessor:%s]Can't find the LimitBossTeam Uid:%d error!\n", __FUNCTION__,iterInviteUser.first);
                }
            }
            iter = m_stUin2LimitBossInfo.erase(iter);
        } else
        {
            iter++;
        }
    }
}

