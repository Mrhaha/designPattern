#include "CentreDataDefine.h"
#include "ServerConfig.h"
#include "server_errcode.h"
#include "server_cmd_cross_define.h"
#include "cross_record_guild_chief_insert.h"
#include "cross_record_guild_id_table.h"
#include "cross_record_guild_war_rank_table.h"

CCrossGuild::CCrossGuild() : m_stGuildInfo(), m_stMemberMap() {}

CPackGuildInfo CCrossGuild::GetGuildInfoPack() const
{
    CPackGuildInfo stInfo;
    stInfo.m_iGuildID = m_stGuildInfo.m_stData.m_iGuildID;
    stInfo.m_strGuildName = m_stGuildInfo.GetGuildName();
    stInfo.m_iGuildExp = m_stGuildInfo.GetGuildExp();
    stInfo.m_strBulletin = m_stGuildInfo.GetBulletin();
    stInfo.m_iMemberCount = m_stMemberMap.Size();
    stInfo.m_iTotalDonate = m_stGuildInfo.GetGuildDonate();
    stInfo.m_iBossHurt = m_stGuildInfo.GetGuildBossTotalHurt();
    stInfo.m_iSignLevel = m_stGuildInfo.GetGuildSignRewardLevel();
    return stInfo;
}

bool CCrossGuild::IsMemberFull() const
{
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);
    return GetMemberCount() >= cross_config->GetGuildConfig().GetMaxMemberCount(m_stGuildInfo.GetGuildExp());
}

int8_t CCrossGuild::GetPosByUin(int32_t iUin) const
{
    auto iterMem = m_stMemberMap.Find(iUin);
    return iterMem == m_stMemberMap.End() ? 0 : iterMem->second->GetPost();
}

////////////////////////////////////////  CCrossSQLDataManager  ////////////////////////////////////////////////
CCrossSQLDataManager::CCrossSQLDataManager(CProcessor* pProcessor) : m_pProcess(pProcessor),
      m_iCurGuildID(0),m_stUserBriefMap(), m_stUinToGuildID(), m_stGuildMap(), m_stGuildCache() {}

CCrossGuild* CCrossSQLDataManager::GetGuild(int32_t iGuildID)
{
    if(iGuildID <= 0) return nullptr;

    auto iter = m_stGuildMap.find(iGuildID);
    if(iter == m_stGuildMap.end())
    {
        // 拉取数据库
        LoadGuildFromDB(iGuildID);

        iter = m_stGuildMap.find(iGuildID);
    }

    if(iter != m_stGuildMap.end())
    {
        Refresh(iter->second.get());
        return iter->second.get();
    }

    return nullptr;
}

CCrossGuild* CCrossSQLDataManager::GetGuildByUin(int32_t iUin, int32_t iGroupID)
{
    return GetGuild(GetGuildIDByUin(iUin, iGroupID));
}

int32_t CCrossSQLDataManager::GetGuildIDByUin(int32_t iUin, int32_t iGroupID)
{
    auto iter = m_stUinToGuildID.find(iUin);
    if(iter == m_stUinToGuildID.end())
    {
        auto stUserGuildVec = CRecordGuildMemberTable::SelectDataFromSQL(iUin, iGroupID);
        if(stUserGuildVec.empty())
        {
            m_stUinToGuildID[iUin] = 0;
            return 0;
        }
        else
        {
            m_stUinToGuildID[iUin] = stUserGuildVec.begin()->m_iGuildID;
            return stUserGuildVec.begin()->m_iGuildID;
        }
    }
    else
    {
        return iter->second;
    }
}

std::vector<int32_t> CCrossSQLDataManager::GetRandGuildIDVec()
{
    std::vector<int32_t> stVec;
    for(const auto& iter : m_stGuildMap)
    {
        if (iter.second->m_stMemberMap.Size() <= 10) continue;
        if(stVec.size() > 100) break;
        stVec.push_back(iter.first);
    }

    return stVec;
}

std::vector<int32_t> CCrossSQLDataManager::GetGuildBossHurtRank()
{
    std::map<uint64_t ,int32_t> stBossHurtGuidID;

    std::vector<int32_t> stRankVec;
    for(const auto& iter : m_stGuildMap)
    {
        if (iter.second->m_stGuildInfo.GetGuildBossTotalHurt() == 0) continue;
        stBossHurtGuidID.insert(std::make_pair(iter.second->m_stGuildInfo.GetGuildBossTotalHurt(),iter.first));
    }

    for (auto iter = stBossHurtGuidID.rbegin(); stRankVec.size() != 10 && iter != stBossHurtGuidID.rend(); iter++)
    {
        stRankVec.push_back(iter->second);
    }
    return stRankVec;
}

void CCrossSQLDataManager::UpdateUserBrief(const CPackUserBrief& stInfo)
{
    auto iter = m_stUserBriefMap.find(stInfo.m_iUid);
    if(iter == m_stUserBriefMap.end())
    {
        auto pInfo = std::make_shared<CPackUserBrief>();
        *pInfo = stInfo;
        m_stUserBriefMap.emplace(stInfo.m_iUid, pInfo);
    }
    else
    {
        *(iter->second) = stInfo;
    }
}

const guild_apply_map* CCrossSQLDataManager::GetGuildApplyMap(int32_t iUin, int32_t iGroupID)
{
    int iGuildID = GetGuildIDByUin(iUin, iGroupID);
    if(iGuildID <= 0) return nullptr;

    auto iterApplyMap = m_stGuildCache.m_stGuildApplyMap.find(iGuildID);
    return (iterApplyMap == m_stGuildCache.m_stGuildApplyMap.end() ? nullptr : &(iterApplyMap->second));
}

bool CCrossSQLDataManager::IsApplyGuild(int32_t iGuildID, int32_t iUin)
{
    auto iterGuild = m_stGuildCache.m_stGuildApplyMap.find(iGuildID);
    if(iterGuild == m_stGuildCache.m_stGuildApplyMap.end())
        return false;

    return iterGuild->second.find(iUin) != iterGuild->second.end();
}

const guild_boss_hurt_map* CCrossSQLDataManager::GetGuildBossHurtMap(int32_t iUin, int32_t iGroupID)
{
    int iGuildID = GetGuildIDByUin(iUin, iGroupID);
    if(iGuildID <= 0) return nullptr;

    auto iterMap = m_stGuildCache.m_stGuildBossHurtMap.find(iGuildID);
    return (iterMap == m_stGuildCache.m_stGuildBossHurtMap.end() ? nullptr : &(iterMap->second));
}

const guild_log_list* CCrossSQLDataManager::GetGuildLogList(int32_t iUin, int32_t iGroupID)
{
    int iGuildID = GetGuildIDByUin(iUin, iGroupID);
    if(iGuildID <= 0) return nullptr;

    auto iter = m_stGuildCache.m_stGuildLogMap.find(iGuildID);
    return iter == m_stGuildCache.m_stGuildLogMap.end() ? nullptr : &(iter->second);
}

const guild_wish_map* CCrossSQLDataManager::GetGuildWishMap(int32_t iUin, int32_t iGroupID)
{
    int iGuildID = GetGuildIDByUin(iUin, iGroupID);
    if(iGuildID <= 0) return nullptr;

    auto iter = m_stGuildCache.m_stGuildWishMap.find(iGuildID);
    return iter == m_stGuildCache.m_stGuildWishMap.end() ? nullptr : &(iter->second);
}

map_uin_vec CCrossSQLDataManager::GetUinGroupVec(const CCrossGuild* pGuild)
{
    map_uin_vec stUinMap;
    if(pGuild)
    {
        for(auto iter = pGuild->m_stMemberMap.Begin(); iter != pGuild->m_stMemberMap.End(); ++iter)
        {
            stUinMap[iter->second->m_stData.m_iGroupID].push_back(iter->second->m_stData.m_iUid);
        }
    }

    return stUinMap;
}

int32_t CCrossSQLDataManager::GetGuildMembers(int32_t iUin, int32_t iGroupID, CResponseGetGuildMembers& stRsp)
{
    auto pGuild = GetGuildByUin(iUin, iGroupID);
    if(nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    bool bClearGuildWarHurt = false;
    if(pGuild->m_stGuildInfo.GetGuildWarEnd() != 0 && !IsGuildWarOpen(pGuild->m_stGuildInfo.GetGuildWarEnd()))
    {
        bClearGuildWarHurt = true;
    }

    CPackGuildMember stMem;
    for(auto iter = pGuild->m_stMemberMap.Begin(); iter != pGuild->m_stMemberMap.End(); ++iter)
    {
        GetUserBriefInfo(iter->second->m_stData.m_iUid, iter->second->m_stData.m_iGroupID, stMem.m_stBriefInfo);
        stMem.m_cPost = iter->second->GetPost();
        stMem.m_iContributionTotal = iter->second->GetContributionTotal();
        stMem.m_iJoinTime = iter->second->GetJoinTime();

        if (bClearGuildWarHurt) iter->second->SetGuildWarHurt(0);
        stMem.m_iGuildWarHurt = iter->second->GetGuildWarHurt();
        stRsp.m_stMemberVec.push_back(stMem);
    }

    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager:: CreateGuild(int32_t iUin, int32_t iGroupID, const std::string& strGuildName,std::string& exeSql)
{
    // 再次验证是否已有公会
    auto stUserGuildVec = CRecordGuildMemberTable::SelectDataFromSQL(iUin, iGroupID);
    if(!stUserGuildVec.empty())
    {
        return SERVER_ERRCODE::USER_ALREADY_IN_GUILD;
    }
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);


    std::map<int,CPackGuildHomeInfo> stGuildDecorate;
    CPackGuildHomeInfo homeInfo;
    homeInfo.m_iDecorateIndex = cross_config->GetGuildConfig().GetGuildDefaultThemeConfig(EnumGuildHomeType::GuildHomeTypeMain)->m_iDefaultInit;
    stGuildDecorate[EnumGuildHomeType::GuildHomeTypeMain] = homeInfo;

    int32_t iCurGuildID = GetCurGuildID();
    CRecordGuildIDTable guildIdTable;
    guildIdTable.SetPrimaryKey(1);
    if (iCurGuildID == 0)
    {
        auto vecRecordGuildID = CRecordGuildIDTable::SelectDataFromSQL(1);
        if (vecRecordGuildID.empty())
        {
            guildIdTable.SetGuildID(1);
            SetCurGuildID(1);
            CLogicSQLTableRecordBase::GetMysqlConn()->execute(guildIdTable.MakeInsertSQL());
        } else
        {
            SetCurGuildID(vecRecordGuildID[0].m_iGuildID+1);
            guildIdTable.SetGuildID(1);
            CLogicSQLTableRecordBase::GetMysqlConn()->execute(guildIdTable.MakeUpdateSQL());
        }
    } else
    {
        SetCurGuildID(GetCurGuildID()+1);
        guildIdTable.SetGuildID(1);
        CLogicSQLTableRecordBase::GetMysqlConn()->execute(guildIdTable.MakeUpdateSQL());
    }

    CRecordGuildInfoTable stInsertGuildInfoRecord;
    stInsertGuildInfoRecord.SetOperation(CLogicSQLTableRecordBase::INSERT);
    stInsertGuildInfoRecord.SetGuildName(strGuildName);
    stInsertGuildInfoRecord.SetChiefUid(iUin);
    stInsertGuildInfoRecord.SetChiefGroup(iGroupID);
    stInsertGuildInfoRecord.SetGuildDecorate(stGuildDecorate);
    stInsertGuildInfoRecord.SetPrimaryKey(GetCurGuildID());


    TGuildInfoTableValueType guildInfoType;
    guildInfoType.m_strGuildName = strGuildName;
    guildInfoType.m_iChiefUid = iUin;
    guildInfoType.m_iChiefGroup = iGroupID;
    guildInfoType.m_stGuildDecorate = stGuildDecorate;
    guildInfoType.m_iGuildID = GetCurGuildID();


    CRecordGuildMemberTable stInsertGuildChiefRecord;
    stInsertGuildChiefRecord.SetOperation(CLogicSQLTableRecordBase::INSERT);
    stInsertGuildChiefRecord.SetPrimaryKey(iUin, iGroupID);
    stInsertGuildChiefRecord.SetGuildID(GetCurGuildID());
    stInsertGuildChiefRecord.SetPost(post_chief);
    stInsertGuildChiefRecord.SetContributionTotal(0);
    stInsertGuildChiefRecord.SetJoinTime(CProcessor::GetNowSec());

    TGuildMemberTableValueType guildMemberType;
    guildMemberType.m_iUid = iUin;
    guildMemberType.m_iGroupID = iGroupID;
    guildMemberType.m_iGuildID = GetCurGuildID();
    guildMemberType.m_cPost = post_chief;
    guildMemberType.m_iJoinTime = CProcessor::GetNowSec();

    std::vector<std::string> stTransactionSQL;
    stTransactionSQL.emplace_back(CLogicSQLTableRecordBase::SQL_TRANSACTION_START);
    stTransactionSQL.emplace_back(stInsertGuildInfoRecord.MakeInsertSQL());
    stTransactionSQL.emplace_back(stInsertGuildChiefRecord.MakeInsertSQL());
    stTransactionSQL.emplace_back(CLogicSQLTableRecordBase::SQL_TRANSACTION_END);

    //create guild
    try
    {
        for (auto & sql : stTransactionSQL)
        {
            exeSql += sql;
            CLogicSQLTableRecordBase::GetMysqlConn()->execute(sql);
        }
    }
    catch (const taf::TC_Mysql_Exception& stEx)
    {
        CLogicSQLTableRecordBase::GetMysqlConn()->execute("ROLLBACK;");

        return SERVER_ERRCODE::GUILD_NAME_REPEAT;
    }

    //插入成功直接写入缓存
    auto pGuild = std::make_unique<CCrossGuild>();
    pGuild->m_stGuildInfo.InitializeWithoutSQL(guildInfoType);
    pGuild->m_stMemberMap.InsertWithoutSQL(std::make_pair(iUin, guildMemberType));
    m_stUinToGuildID[iUin] = GetCurGuildID();
    m_stGuildMap.emplace(GetCurGuildID(),std::move(pGuild));
    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::LeaveGuild(int32_t iUin, int32_t iGroupID)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    auto iterMember = pGuild->m_stMemberMap.Find(iUin);
    if(iterMember == pGuild->m_stMemberMap.End())
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND;
    }

    if(pGuild->IsChief(iUin) && pGuild->GetMemberCount() > 1)
    {
        return SERVER_ERRCODE::GUILD_CHIEF_CANT_LEAVE;
    }

    HandleGuildLeave(pGuild, iUin, iGroupID, false);

    // 如果没人了，就解散公会
    if(pGuild->m_stMemberMap.Empty())
    {
        DismissGuild(iGuildID);
    }

    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::UpdateBulletin(int32_t iUin, int32_t iGroupID, const std::string& strBulletin)
{
    auto pGuild = GetGuildByUin(iUin, iGroupID);
    if (nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    auto iterMember = pGuild->m_stMemberMap.Find(iUin);
    if(iterMember == pGuild->m_stMemberMap.End())
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND;
    }

    if(iterMember->second->GetPost() != post_chief && iterMember->second->GetPost() != post_admin)
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_ADMIN;
    }

    pGuild->m_stGuildInfo.SetBulletin(strBulletin);

    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::UpdatePost(int32_t iUin, int32_t iGroupID, int32_t iDstUin, int32_t iDstGroupID, int8_t cNewPos)
{
    if(cNewPos == post_chief || iUin == iDstUin)
    {
        return SERVER_ERRCODE::PARAMETER_ERROR;
    }

    if(!CCrossGuild::IsValidPost(cNewPos))
    {
        return SERVER_ERRCODE::PARAMETER_ERROR;
    }

    auto pGuild = GetGuildByUin(iUin, iGroupID);
    if (nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    if(!pGuild->IsChief(iUin))
    {
        return SERVER_ERRCODE::NOT_GUILD_CHIEF;
    }

    auto iterDst = pGuild->m_stMemberMap.Find(iDstUin);
    if(iterDst == pGuild->m_stMemberMap.End() || iterDst->second->m_stData.m_iGroupID != iDstGroupID)
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND;
    }

    if(iterDst->second->GetPost() == cNewPos)
    {
        return SERVER_ERRCODE::SUCCESS;
    }

    if(cNewPos == post_admin)
    {
        std::shared_ptr<CCrossServerConfig> cross_config;
        CCrossServerConfig::GetConfig(cross_config);

        int iAdminNum = 0;
        for(auto iter = pGuild->m_stMemberMap.Begin(); iter != pGuild->m_stMemberMap.End(); ++iter)
        {
            if(iter->second->GetPost() == post_admin) ++iAdminNum;
        }

        if(iAdminNum >= cross_config->GetGuildConfig().GetMaxAdminCount(pGuild->m_stGuildInfo.GetGuildExp()))
        {
            return SERVER_ERRCODE::GUILD_ADMIN_TOO_MUCH;
        }
    }

    iterDst->second->SetPost(cNewPos);

    // 通知
    NotifyGuildChange(iDstUin, iDstGroupID, pGuild->GetGuildID(), pGuild->GetGuildName(), pGuild->GetGuildExp(), cNewPos);

    // 日志
    AddGuildLog(pGuild->GetGuildID(), (cNewPos == post_admin ? GuildLog_SetAdmin : GuildLog_SetMember), iDstUin, iDstGroupID, iUin, iGroupID);

    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::TransferGuild(int32_t iUin, int32_t iGroupID, int32_t iDstUin, int32_t iDstGroupID)
{
    if(iUin == iDstUin)
    {
        return SERVER_ERRCODE::PARAMETER_ERROR;
    }

    auto pGuild = GetGuildByUin(iUin, iGroupID);
    if (nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    auto iterChief = pGuild->m_stMemberMap.Find(iUin);
    if(iterChief == pGuild->m_stMemberMap.End())
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND;
    }

    if(iterChief->second->GetPost() != post_chief)
    {
        return SERVER_ERRCODE::NOT_GUILD_CHIEF;
    }

    auto iterDst = pGuild->m_stMemberMap.Find(iDstUin);
    if(iterDst == pGuild->m_stMemberMap.End() || iterDst->second->m_stData.m_iGroupID != iDstGroupID)
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND;
    }

    iterChief->second->SetPost(post_member);
    iterDst->second->SetPost(post_chief);
    pGuild->m_stGuildInfo.SetChiefUid(iDstUin);
    pGuild->m_stGuildInfo.SetChiefGroup(iDstGroupID);

    // 通知
    NotifyGuildChange(iDstUin, iDstGroupID, pGuild->GetGuildID(), pGuild->GetGuildName(), pGuild->GetGuildExp(), iterDst->second->GetPost());

    // 日志
    AddGuildLog(pGuild->GetGuildID(), GuildLog_SetChief, iDstUin, iDstGroupID, iUin, iGroupID);

    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::ExpelGuild(int32_t iUin, int32_t iGroupID, int32_t iDstUin, int32_t iDstGroupID)
{
    if(iUin == iDstUin)
    {
        return SERVER_ERRCODE::PARAMETER_ERROR;
    }

    auto pGuild = GetGuildByUin(iUin, iGroupID);
    if (nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    auto iterMy = pGuild->m_stMemberMap.Find(iUin);
    if(iterMy == pGuild->m_stMemberMap.End())
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND;
    }

    if(iterMy->second->GetPost() != post_chief && iterMy->second->GetPost() != post_admin)
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_ADMIN;
    }

    auto iterDst = pGuild->m_stMemberMap.Find(iDstUin);
    if(iterDst == pGuild->m_stMemberMap.End())
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND;
    }

    if(iterDst->second->GetPost() != post_member)
    {
        return SERVER_ERRCODE::GUILD_CANT_EXPEL_ADMIN;
    }

    HandleGuildLeave(pGuild, iDstUin, iDstGroupID, true);

    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::ApplyJoinGuild(int32_t iUin, int32_t iGroupID, int32_t iDstGuildID)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    if(iGuildID == iDstGuildID || iDstGuildID <= 0)
    {
        return SERVER_ERRCODE::PARAMETER_ERROR;
    }

    if(iGuildID > 0)
    {
        return SERVER_ERRCODE::USER_ALREADY_IN_GUILD;
    }

    auto pGuild = GetGuild(iDstGuildID);
    if (nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    auto& stApplyMap = m_stGuildCache.m_stGuildApplyMap[iDstGuildID];
    auto& stApplyInfo = stApplyMap[iUin];
    stApplyInfo.m_iGroupID = iGroupID;
    stApplyInfo.m_iApplyTime = CProcessor::GetNowSec();

    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::DealGuildApply(int32_t iUin, int32_t iGroupID, int32_t iDstUin, int32_t iDstGroupID, bool bAgree)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    if(iGuildID <= 0)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    if(bAgree && pGuild->IsMemberFull())
    {
        return SERVER_ERRCODE::GUILD_MEMBER_FULL;
    }

    auto iterMy = pGuild->m_stMemberMap.Find(iUin);
    if(iterMy == pGuild->m_stMemberMap.End())
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND;
    }

    if(iterMy->second->GetPost() != post_admin && iterMy->second->GetPost() != post_chief)
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_ADMIN;
    }

    auto iterApplyMap = m_stGuildCache.m_stGuildApplyMap.find(iGuildID);
    if(iterApplyMap == m_stGuildCache.m_stGuildApplyMap.end())
    {
        return SERVER_ERRCODE::GUILD_APPLY_EXPIRED;
    }

    auto iterApply = iterApplyMap->second.find(iDstUin);
    if(iterApply == iterApplyMap->second.end())
    {
        return SERVER_ERRCODE::GUILD_APPLY_EXPIRED;
    }

    if(iterApply->second.m_iGroupID != iDstGroupID)
    {
        return SERVER_ERRCODE::PARAMETER_ERROR;
    }

    iterApplyMap->second.erase(iterApply);

    if(bAgree)
    {
        // 检查对方是否已有公会
        if(GetGuildIDByUin(iDstUin, iDstGroupID) > 0)
        {
            return SERVER_ERRCODE::USER_ALREADY_IN_GUILD;
        }

        if(!HandleGuildJoin(pGuild, iDstUin, iDstGroupID))
        {
            return SERVER_ERRCODE::INTERN_ERROR;
        }
    }

    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::ChangeGuildName(int32_t iUin, int32_t iGroupID, const std::string& strGuildName, bool bIsGM, CResponseChangeGuildName& stRsp,std::string& oldName)
{
    auto pGuild = GetGuildByUin(iUin, iGroupID);
    if (nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    if(!pGuild->IsChief(iUin))
    {
        return SERVER_ERRCODE::NOT_GUILD_CHIEF;
    }

    const int32_t iNowTime = CProcessor::GetNowSec();
    oldName = pGuild->GetGuildName();
    stRsp.m_bConsume = false;
    if(!bIsGM)
    {
        if(pGuild->m_stGuildInfo.GetChangeNameCd() > iNowTime)
        {
            return SERVER_ERRCODE::CHANGE_NAME_CDING;
        }

        std::shared_ptr<CCrossServerConfig> cross_config;
        CCrossServerConfig::GetConfig(cross_config);
        if(pGuild->m_stGuildInfo.GetChangeNameFree() == 0)
        {
            stRsp.m_bConsume = true;
            if(cross_config->GetGuildConfig().m_iChangeNameCost <= 0)
            {
                return SERVER_ERRCODE::PROTO_SWITCH_LIMIT;
            }
        }

        stRsp.m_cChangeNameFree = 0;
        stRsp.m_iChangeNameCD = iNowTime + cross_config->GetGuildConfig().m_iChangeNameCD;
    }
    else
    {
        stRsp.m_cChangeNameFree = 1;
        stRsp.m_iChangeNameCD = iNowTime;
    }

    // 操作数据库
    try
    {
        const std::string strEscapeName = CLogicSQLTableRecordBase::GetMysqlConn()->escapeString(strGuildName);
        CRecordGuildInfoTable stGuildInfoRecord;
        stGuildInfoRecord.SetPrimaryKey(pGuild->GetGuildID());
        stGuildInfoRecord.SetOperation(CLogicSQLTableRecordBase::UPDATE);
        stGuildInfoRecord.SetGuildName(strEscapeName);
        stGuildInfoRecord.SetChangeNameCd(stRsp.m_iChangeNameCD - pGuild->m_stGuildInfo.GetChangeNameCd());
        stGuildInfoRecord.SetChangeNameFree(stRsp.m_cChangeNameFree - pGuild->m_stGuildInfo.GetChangeNameFree());
        CLogicSQLTableRecordBase::GetMysqlConn()->execute(stGuildInfoRecord.MakeSQL());
    }
    catch (const taf::TC_Mysql_Exception& stEx)
    {
        return SERVER_ERRCODE::GUILD_NAME_REPEAT;
    }

    pGuild->m_stGuildInfo.m_stData.m_strGuildName = strGuildName;
    pGuild->m_stGuildInfo.m_stData.m_iChangeNameCd = stRsp.m_iChangeNameCD;
    pGuild->m_stGuildInfo.m_stData.m_cChangeNameFree = stRsp.m_cChangeNameFree;

    return SERVER_ERRCODE::SUCCESS;
}

void CCrossSQLDataManager::UpdateGuildBossHurt(int32_t iUin, int32_t iGroupID, uint64_t ulHurt)
{
    if(ulHurt <= 0)
        return;

    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    if(iGuildID <= 0)
        return;

    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
        return;

    if(std::numeric_limits<uint64_t>::max() - pGuild->m_stGuildInfo.GetGuildBossTotalHurt() > ulHurt)
    {
        pGuild->m_stGuildInfo.AddGuildBossTotalHurt(ulHurt);
    }

    // 更新伤害排行
    auto& stHurtMap = m_stGuildCache.m_stGuildBossHurtMap[iGuildID];
    if(stHurtMap[iUin].m_ulHurt < ulHurt)
    {
        stHurtMap[iUin].m_iGroupID = iGroupID;
        stHurtMap[iUin].m_ulHurt = ulHurt;
    }
}

void CCrossSQLDataManager::UpdateGuildWishInfo(int32_t iUin, int32_t iGroupID, int32_t iCardID, int32_t iCurNum, bool bSwap,bool isFull)
{
    if(iCardID <= 0)
        return;

    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    if(iGuildID <= 0)
        return;

    // 更新祈愿
    auto& stWishMap = m_stGuildCache.m_stGuildWishMap[iGuildID];
    auto& stInfo = stWishMap[iUin];
    stInfo.m_iGroupID = iGroupID;
    stInfo.m_iCardID = iCardID;
    stInfo.m_iCurNum = iCurNum;
    stInfo.m_bCanSwap = bSwap;
    stInfo.m_bIsFull = isFull;
}

int32_t CCrossSQLDataManager::UpdateGuildExp(int32_t iUin, int32_t iGroupID, int32_t addExp)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    if(iGuildID <= 0)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
        return  SERVER_ERRCODE::GUILD_NOT_FOUND;

    pGuild->m_stGuildInfo.AddGuildExp(addExp);
    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::UpdateGuildDonate(int32_t iUin, int32_t iGroupID,const TLogicGuildDonateElem* donateConfig)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    if(iGuildID <= 0)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
        return  SERVER_ERRCODE::GUILD_NOT_FOUND;

    auto iterMember = pGuild->m_stMemberMap.Find(iUin);
    if (iterMember == pGuild->m_stMemberMap.End())
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND;
    }

    auto curContribution = iterMember->second->GetContributionTotal();
    auto curGuildDonate = pGuild->m_stGuildInfo.GetGuildDonate();

    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

    bool hasGuildBuff = false;
    //检查是否存在公会buff
    for(auto unLockItemID : pGuild->m_stGuildInfo.GetGuildUnlockItem())
    {
        auto unLockConfig = cross_config->GetGuildConfig().GetGuildCollectionConfig(unLockItemID);
        if (unLockConfig->m_iGuildBuffID == EnumGuildBuffType::GuildBuffGuildDonate)
        {
            hasGuildBuff = true;
            break;
        }
    }

    int32_t iGuildDonate = donateConfig->m_iGuildDonate;
    int32_t iPrivateDonate = donateConfig->m_iPrivateDonate;

    if (hasGuildBuff)
    {
        iGuildDonate = int32_t((float)iGuildDonate * (1 + GUILD_BUFF_RATION));
        iPrivateDonate = int32_t((float)iPrivateDonate * (1 + GUILD_BUFF_RATION));
    }

    if (curContribution + iPrivateDonate < std::numeric_limits<int>::max())
    {
        iterMember->second->AddContributionTotal(iPrivateDonate);
    } else
    {
        iterMember->second->SetContributionTotal(std::numeric_limits<int>::max());
    }

    if (curGuildDonate + iGuildDonate < std::numeric_limits<int>::max())
    {
        pGuild->m_stGuildInfo.AddGuildDonate(iGuildDonate);
    } else
    {
        pGuild->m_stGuildInfo.SetGuildDonate(std::numeric_limits<int>::max());
    }
    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::GetGuildSignRewardLevel(int32_t iUin, int32_t iGroupID,int32_t& SignLevel)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    if(iGuildID <= 0)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
        return  SERVER_ERRCODE::GUILD_NOT_FOUND;

    SignLevel = pGuild->m_stGuildInfo.GetGuildSignRewardLevel();

    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::UpGradeGuildSignRewardLevel(int32_t iUin, int32_t iGroupID)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    auto iterMember = pGuild->m_stMemberMap.Find(iUin);
    if(iterMember == pGuild->m_stMemberMap.End())
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND;
    }

    if(!pGuild->IsChief(iUin))
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_ADMIN;
    }
    //cross_config
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

    auto curGuildLevel =  cross_config->GetGuildConfig().GetGuildLevel(pGuild->GetGuildExp());
    auto curRewardLevel = pGuild->m_stGuildInfo.GetGuildSignRewardLevel();
    auto curGuildDonate = pGuild->m_stGuildInfo.GetGuildDonate();

    auto guildConfig = cross_config->GetGuildConfig().GetGuildSignConfig(curRewardLevel);

    if (guildConfig == nullptr || guildConfig->m_iCostGuildDonate > curGuildDonate || curGuildLevel < guildConfig->m_iNeedLevel)
    {
        return SERVER_ERRCODE::GUILD_UPGRADE_SIGN_REWARD_ERROR;
    }

    //扣除对应的公会贡献点数
    pGuild->m_stGuildInfo.SetGuildDonate(curGuildDonate - guildConfig->m_iCostGuildDonate);

    //提升福利等级
    pGuild->m_stGuildInfo.SetGuildSignRewardLevel(curRewardLevel+1);
    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::SaveGuildDecorate(int32_t iUin, int32_t iGroupID)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    auto iterMy = pGuild->m_stMemberMap.Find(iUin);
    if(iterMy == pGuild->m_stMemberMap.End())
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND;
    }

    if(iterMy->second->GetPost() != post_admin && iterMy->second->GetPost() != post_chief)
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_ADMIN;
    }

    return SERVER_ERRCODE::GUILD_MEMBER_NOT_ADMIN;
}

int32_t CCrossSQLDataManager::UnEquipGoods(int32_t iUin, int32_t iGroupID,CRequestHomeUnequipGoods& stReq)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    auto stDecorate = pGuild->m_stGuildInfo.GetGuildDecorate();
    auto stFootCloth = pGuild->m_stGuildInfo.GetGuildFootCloth();

    auto iterHome = stDecorate.find(stReq.m_iHomeID);

    if(iterHome == pGuild->m_stGuildInfo.GetGuildDecorate().end())
    {
        return (SERVER_ERRCODE::HOME_NONEXISTENT);
    }

    switch (stReq.m_iType)
    {
    case EnumItemSubType::EIST_HomePage:
    {
        if(iterHome->second.m_iHomePage != stReq.m_iItemID)
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        iterHome->second.m_iHomePage = 0;
        break;
    }
    case EnumItemSubType::EIST_HomeFloorBG:
    {
        if(iterHome->second.m_iFloorBG != stReq.m_iItemID)
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        iterHome->second.m_iFloorBG = 0;

        break;
    }
    case EnumItemSubType::EIST_HomeOrnament:
    {
        auto iterGoods = iterHome->second.m_stGuildHomeItems.find(stReq.m_iPara);
        if(iterGoods == iterHome->second.m_stGuildHomeItems.end() || iterGoods->second.m_iGoodID != stReq.m_iItemID)
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        iterHome->second.m_stGuildHomeItems.erase(iterGoods);
        break;
    }
    case EnumItemSubType::EIST_HomeFootcloth:
    {
        auto iterHomeFootCloth = stFootCloth.find(stReq.m_iHomeID);

        if(iterHomeFootCloth == pGuild->m_stGuildInfo.GetGuildFootCloth().end())
        {
            return (SERVER_ERRCODE::HOME_NONEXISTENT);
        }

        auto iterFoot = iterHomeFootCloth->second.find(stReq.m_iItemID);
        if(iterFoot == iterHomeFootCloth->second.end())
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        if(iterFoot->second.erase(stReq.m_iPara) <= 0)
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }
        break;
    }
    default:
    {
        return (SERVER_ERRCODE::PARAMETER_ERROR);
    }
    }

    pGuild->m_stGuildInfo.SetGuildDecorate(stDecorate);
    pGuild->m_stGuildInfo.SetGuildFootCloth(stFootCloth);

    return SERVER_ERRCODE::SUCCESS;
}


int32_t CCrossSQLDataManager::EquipGoods(int32_t iUin, int32_t iGroupID,CRequestHomeEquipGoods& stReq)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    auto stDecorate = pGuild->m_stGuildInfo.GetGuildDecorate();
    auto stFootCloth = pGuild->m_stGuildInfo.GetGuildFootCloth();

    auto iterHome = stDecorate.find(stReq.m_iHomeID);

    if(iterHome == stDecorate.end())
    {
        CPackGuildHomeInfo homeInfo;
        stDecorate[stReq.m_iHomeID] = homeInfo;
        iterHome = stDecorate.find(stReq.m_iHomeID);
    }

    switch (stReq.m_iType)
    {
    case EnumItemSubType::EIST_HomePage:
    {
        if(iterHome->second.m_iHomePage == stReq.m_stGoodsPara.m_iGoodsID)
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        iterHome->second.m_iHomePage = stReq.m_stGoodsPara.m_iGoodsID;
        break;
    }
    case EnumItemSubType::EIST_HomeFloorBG:
    {
        if(iterHome->second.m_iFloorBG == stReq.m_stGoodsPara.m_iGoodsID)
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        iterHome->second.m_iFloorBG = stReq.m_stGoodsPara.m_iGoodsID;
        break;
    }
    case EnumItemSubType::EIST_HomeOrnament:
    {
        CPackGuildHomeItem homeItem;
        homeItem.m_iGoodID = stReq.m_stGoodsPara.m_iGoodsID;
        homeItem.m_iQuadrant = stReq.m_stGoodsPara.m_cQuadrant;
        homeItem.m_iStat = stReq.m_stGoodsPara.m_iSeatIndex;
        homeItem.m_iPosY = stReq.m_stGoodsPara.m_iPosY;
        homeItem.m_iPosX = stReq.m_stGoodsPara.m_iPosX;

        const auto stRet = iterHome->second.m_stGuildHomeItems.insert(std::make_pair(stReq.m_stGoodsPara.m_iGoodsIndex, homeItem));
        if (!stRet.second)
        {
            return false;
        }
        break;
    }
    case EnumItemSubType::EIST_HomeFootcloth:
    {
        auto stFootsMap = stFootCloth.find(stReq.m_iHomeID); //<key=homeID,<GoodsID,<Pos,stat>>>
        if (stFootsMap == stFootCloth.end())
        {
            std::map<int,std::map<int,char>> footElem;
            stFootCloth[stReq.m_iHomeID] = footElem;
            stFootsMap = stFootCloth.find(stReq.m_iHomeID);
        }

        int32_t iPos = GetUniqueIDByPos(stReq.m_stGoodsPara.m_iPosX, stReq.m_stGoodsPara.m_iPosY);
        auto iterFoot = stFootsMap->second.find(stReq.m_stGoodsPara.m_iGoodsID);
        if(iterFoot == stFootsMap->second.end())
        {
            std::map<int32_t,char> footCloth;
            footCloth[iPos] = stReq.m_stGoodsPara.m_cStat;
            auto stRet = stFootsMap->second.insert(std::make_pair(stReq.m_stGoodsPara.m_iGoodsID,footCloth));
            if (!stRet.second)
            {
                return (SERVER_ERRCODE::INTERN_ERROR);
            }

            iterFoot = stRet.first;
        } else
        {
            iterFoot->second[iPos] = stReq.m_stGoodsPara.m_cStat;
        }
        break;
    }
    default:
    {
        return (SERVER_ERRCODE::PARAMETER_ERROR);
    }
    }

    pGuild->m_stGuildInfo.SetGuildDecorate(stDecorate);
    pGuild->m_stGuildInfo.SetGuildFootCloth(stFootCloth);

    return SERVER_ERRCODE::SUCCESS;
}


int32_t CCrossSQLDataManager::MoveGoods(int32_t iUin, int32_t iGroupID,CRequestHomeGoodsMove& stReq)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    auto stDecorate = pGuild->m_stGuildInfo.GetGuildDecorate();
    auto stFootCloth = pGuild->m_stGuildInfo.GetGuildFootCloth();

    auto iterHome = stDecorate.find(stReq.m_iHomeID);

    if(iterHome == pGuild->m_stGuildInfo.GetGuildDecorate().end())
    {
        return (SERVER_ERRCODE::HOME_NONEXISTENT);
    }

    // 地毯
    if(stReq.m_bFootcloth)
    {
        auto iterFoot = stFootCloth.find(stReq.m_iHomeID)->second.find(stReq.m_iPara);
        if(iterFoot == stFootCloth.find(stReq.m_iHomeID)->second.end())
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        int32_t iIndex = GetUniqueIDByPos(stReq.m_iCurX, stReq.m_iCurY);
        auto iterCurPos = iterFoot->second.find(iIndex);
        if(iterCurPos == iterFoot->second.end())
        {
            return (SERVER_ERRCODE::POS_ERROR);
        }

        int32_t iNewIndex = GetUniqueIDByPos(stReq.m_iNewX, stReq.m_iNewY);

        // 只是转方向
        if(iIndex == iNewIndex)
        {
            if(stReq.m_cStat == iterCurPos->second)
            {
                return true;
            }

            iterCurPos->second = stReq.m_cStat;
        }
        else
        {
            if(iterFoot->second.find(iNewIndex) != iterFoot->second.end())
            {
                return (SERVER_ERRCODE::POS_ERROR);
            }

            iterFoot->second.erase(iterCurPos);
            iterFoot->second[iNewIndex] = stReq.m_cStat;
        }
    }
    else
    {
        // 家具
        auto iterGoods = iterHome->second.m_stGuildHomeItems.find(stReq.m_iPara);
        if(iterGoods == iterHome->second.m_stGuildHomeItems.end())
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        iterGoods->second.m_iPosX = stReq.m_iNewX;
        iterGoods->second.m_iPosY = stReq.m_iNewY;
        iterGoods->second.m_iStat = stReq.m_cStat;
        iterGoods->second.m_iQuadrant = stReq.m_cQuadrant;
    }

    pGuild->m_stGuildInfo.SetGuildDecorate(stDecorate);
    pGuild->m_stGuildInfo.SetGuildFootCloth(stFootCloth);

    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::AddBarInfo(int32_t iUin, int32_t iGroupID,CRequestGuildSitBar stReq)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    if(iGuildID <= 0)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    //更新吧台信息
    CPackGuildUserBarInfo barInfo;
    barInfo.m_iUid = iUin;
    barInfo.m_iGroupID = iGroupID;
    barInfo.m_iCardID = stReq.m_iCardID;
    barInfo.m_iEndTime = CProcessor::GetNowSec() + stReq.m_iDurationTime;
    barInfo.m_iIndex = stReq.m_iIndex;
    barInfo.m_strName = stReq.m_strName;

    auto iterBarInfo =  m_stGuildCache.m_stGuildBarInfo.find(iGuildID);
    if (iterBarInfo ==  m_stGuildCache.m_stGuildBarInfo.end())
    {
        guild_bar_map stBarInfo;
        stBarInfo[iUin] = barInfo;
        m_stGuildCache.m_stGuildBarInfo.insert(std::make_pair(iGuildID,stBarInfo));
    } else
    {
        iterBarInfo->second.insert(std::make_pair(iUin,barInfo));
    }
    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::LeaveBarInfo(int32_t iUin, int32_t iGroupID,CCrossGuildGetSitBarReward& cross)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    if(iGuildID <= 0)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    const int32_t iNowTime = CProcessor::GetNowSec();

    //更新吧台信息
    auto iterBarInfo =  m_stGuildCache.m_stGuildBarInfo.find(iGuildID);
    if (iterBarInfo ==  m_stGuildCache.m_stGuildBarInfo.end())
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    } else
    {
        auto stUserBarInfo = iterBarInfo->second.find(iUin);
        if (stUserBarInfo != iterBarInfo->second.end() && iNowTime > stUserBarInfo->second.m_iEndTime)
        {
            cross.m_iCardID = stUserBarInfo->second.m_iCardID;
            iterBarInfo->second.erase(stUserBarInfo);
        } else
        {
            return SERVER_ERRCODE::GUILD_BAR_TIME_NOT_END;
        }
    }

    return SERVER_ERRCODE::SUCCESS;
}


int32_t CCrossSQLDataManager::GetBarInfo(int32_t iUin, int32_t iGroupID,std::vector<CPackGuildUserBarInfo>* pRsp)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    if(iGuildID <= 0)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    auto iterBarInfo =  m_stGuildCache.m_stGuildBarInfo.find(iGuildID);
    if (iterBarInfo != m_stGuildCache.m_stGuildBarInfo.end())
    {
        for(auto barInfo: iterBarInfo->second) {
            pRsp->emplace_back(barInfo.second);
        }
    }

    return SERVER_ERRCODE::SUCCESS;
}


int32_t CCrossSQLDataManager::UnLockGuildCollection(int32_t iUin,int32_t iGroupID,CRequestUnlockCollection stReq)
{
    int32_t iGuildID = GetGuildIDByUin(iUin, iGroupID);
    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    auto iterMy = pGuild->m_stMemberMap.Find(iUin);
    if(iterMy == pGuild->m_stMemberMap.End())
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND;
    }

    if(iterMy->second->GetPost() != post_admin && iterMy->second->GetPost() != post_chief)
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_ADMIN;
    }

    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

    auto iCurGuildDonate = pGuild->m_stGuildInfo.GetGuildDonate();
    auto iCostDonate = 0;
    //根据不同的解锁类型来读取配置和记录
    if (stReq.m_iType == (int32_t)EnumGuildUnlockType::GuildUnlockType_Decorate)
    {
        auto unLockConfig = cross_config->GetGuildConfig().GetGuildDecorateConfig(stReq.m_iIndex);
        if (unLockConfig == nullptr)
        {
            return SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST;
        }

        if (iCurGuildDonate < unLockConfig->m_iCostGuildDonate)
        {
            return SERVER_ERRCODE::GUILD_DONATE_IS_NOT_ENOUGH;
        }

        for (auto unLockItem : pGuild->m_stGuildInfo.GetGuildUnlockDecorate())
        {
            if (unLockItem == stReq.m_iIndex)
            {
                return SERVER_ERRCODE::GUILD_ITEM_HAS_UNLOCK;
            }
        }

        iCostDonate = unLockConfig->m_iCostGuildDonate;

        //扣除贡献
        pGuild->m_stGuildInfo.SetGuildDonate(iCurGuildDonate - iCostDonate);

        auto unlockDecorate = pGuild->m_stGuildInfo.GetGuildUnlockDecorate();
        unlockDecorate.push_back(stReq.m_iIndex);
        pGuild->m_stGuildInfo.SetGuildUnlockDecorate(unlockDecorate);

    } else if (stReq.m_iType == (int32_t)EnumGuildUnlockType::GuildUnlockType_Collection)
    {
        auto unLockConfig = cross_config->GetGuildConfig().GetGuildCollectionConfig(stReq.m_iIndex);
        if (unLockConfig == nullptr)
        {
            return SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST;
        }

        if (iCurGuildDonate < unLockConfig->m_iCostGuildDonate)
        {
            return SERVER_ERRCODE::GUILD_DONATE_IS_NOT_ENOUGH;
        }

        auto curGuildLevel =  cross_config->GetGuildConfig().GetGuildLevel(pGuild->GetGuildExp());
        if (curGuildLevel < unLockConfig->m_iUnlockLevel)
        {
            return SERVER_ERRCODE::NOT_ENOUGH_GUILD_LEVEL;
        }

        for (auto unLockDecorate : pGuild->m_stGuildInfo.GetGuildUnlockItem())
        {
            if (unLockDecorate == stReq.m_iIndex)
            {
                return SERVER_ERRCODE::GUILD_ITEM_HAS_UNLOCK;
            }
        }


        iCostDonate = unLockConfig->m_iCostGuildDonate;

        //扣除贡献
        pGuild->m_stGuildInfo.SetGuildDonate(iCurGuildDonate - iCostDonate);

        auto unlockDecorate = pGuild->m_stGuildInfo.GetGuildUnlockItem();
        unlockDecorate.push_back(stReq.m_iIndex);
        pGuild->m_stGuildInfo.SetGuildUnlockItem(unlockDecorate);
    }

    return SERVER_ERRCODE::SUCCESS;
}


int32_t CCrossSQLDataManager::GMModifyGuild(int32_t iGuildID, int32_t iGuildExp)
{
    if(iGuildExp <= 0)
    {
        return SERVER_ERRCODE::PARAMETER_ERROR;
    }

    auto pGuild = GetGuild(iGuildID);
    if(nullptr == pGuild)
    {
        return SERVER_ERRCODE::GUILD_NOT_FOUND;
    }

    pGuild->m_stGuildInfo.SetGuildExp(iGuildExp);
    return SERVER_ERRCODE::SUCCESS;
}

void CCrossSQLDataManager::Refresh(CCrossGuild* pGuild)
{
    const int32_t iNowSec = CProcessor::GetNowSec();
    if(IsRefreshByDay(iNowSec, pGuild->m_stGuildInfo.GetLastRefreshTime(),5))
    {
        GuildCrossOneDay(pGuild);

        pGuild->m_stGuildInfo.SetLastRefreshTime(iNowSec);
    }
}

void CCrossSQLDataManager::LoadGuildFromDB(int32_t iGuildID)
{
    auto pGuild = std::make_unique<CCrossGuild>();

    const auto stGuildInfoList = CRecordGuildInfoTable::SelectDataFromSQL(iGuildID);
    if (stGuildInfoList.empty()) return;

    // 判断是否同跨服
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);
    if(cross_config->GetZooCrossConfig().GetServerIDByGroupID(stGuildInfoList[0].m_iChiefGroup) <= 0)
        return;

    const auto stGuildMemberList = CRecordGuildMemberTable::SelectDataFromSQLWithGuildID(iGuildID);
    if (stGuildMemberList.empty()) return;

    pGuild->m_stGuildInfo.InitializeWithoutSQL(stGuildInfoList[0]);
    for(auto& member : stGuildMemberList)
    {
        pGuild->m_stMemberMap.InsertWithoutSQL(std::make_pair(member.m_iUid, member));
        m_stUinToGuildID[member.m_iUid] = iGuildID;
    }

    m_stGuildMap.emplace(iGuildID, std::move(pGuild));
}

bool CCrossSQLDataManager::IsGuildWarOpen(int32_t iNowSec)
{
    std::shared_ptr<CCrossServerConfig> config;
    CCrossServerConfig::GetConfig(config);

    if(iNowSec >= config->GetGuildConfig().m_iGuildWarStartTime &&
        iNowSec <= config->GetGuildConfig().m_iGuildWarEndTime)
    {
        return true;
    }
    return false;
}

//加载公会战排行榜
void CCrossSQLDataManager::LoadGuildRank(int32_t iNowSec)
{
    std::shared_ptr<CCrossServerConfig> config;
    CCrossServerConfig::GetConfig(config);

    auto iCurRankID = config->GetGuildConfig().m_iRankID;
    //本期排行榜
    auto iterRankContainer =  m_stGuildWarRankMap.find(iCurRankID);
    if (m_stGuildWarRankMap.end() == iterRankContainer || iterRankContainer->second.empty())
    {
        m_stGuildWarRankMap[iCurRankID].SetMaxRank(12000);
        const auto stGuildWarRankList = CRecordGlobalGuildWarRankTable::SelectDataFromSQL(iCurRankID);
        for (auto& guildRank : stGuildWarRankList)
        {
            m_stGuildWarRankMap[iCurRankID].InsertWithoutSQL(std::make_pair(guildRank.m_iGuildID, guildRank));
        }
    }
    //上期排行榜
    auto iterLastRankContainer =  m_stGuildWarRankMap.find(iCurRankID-1);
    if (m_stGuildWarRankMap.end() == iterLastRankContainer || iterLastRankContainer->second.empty())
    {
        m_stGuildWarRankMap[iCurRankID-1].SetMaxRank(12000);
        const auto stGuildWarRankList = CRecordGlobalGuildWarRankTable::SelectDataFromSQL(iCurRankID-1);
        for (auto& guildRank : stGuildWarRankList)
        {
            m_stGuildWarRankMap[iCurRankID-1].InsertWithoutSQL(std::make_pair(guildRank.m_iGuildID, guildRank));
        }
    }

    //进入新赛季先清除之前的榜单
    if (iNowSec + 20 >= config->GetGuildConfig().m_iGuildWarStartTime)
    {
        auto iterDelRankContainer =  m_stGuildWarRankMap.find(iCurRankID-2);
        if (m_stGuildWarRankMap.end() != iterDelRankContainer)
        {
            m_stGuildWarRankMap[iCurRankID-2].clear();
        }
    }

}

bool CCrossSQLDataManager::HandleGuildJoin(CCrossGuild* pGuild, int32_t iUin, int32_t iGroupID)
{
    TGuildMemberTableValueType stValue;
    stValue.m_iUid = iUin;
    stValue.m_iGroupID = iGroupID;
    stValue.m_iGuildID = pGuild->GetGuildID();
    stValue.m_iJoinTime = CProcessor::GetNowSec();
    auto stRet = pGuild->m_stMemberMap.Insert(std::make_pair(iUin, stValue));
    if (!stRet.second)
    {
        return false;
    }

    m_stUinToGuildID[iUin] = pGuild->GetGuildID();

    // 通知
    NotifyGuildChange(iUin, iGroupID, pGuild->GetGuildID(), pGuild->GetGuildName(), pGuild->GetGuildExp(), stValue.m_cPost);

    // 日志
    AddGuildLog(pGuild->GetGuildID(), GuildLog_Join, iUin, iGroupID);

    return true;
}

bool CCrossSQLDataManager::HandleGuildLeave(CCrossGuild* pGuild, int32_t iUin, int32_t iGroupID, bool isExpel)
{
    pGuild->m_stMemberMap.Erase(iUin);
    m_stUinToGuildID[iUin] = 0;

    // 通知
    NotifyGuildChange(iUin, iGroupID, 0, "", 0, 0, isExpel);

    // 日志
    AddGuildLog(pGuild->GetGuildID(), GuildLog_Leave, iUin, iGroupID);

    return true;
}

void CCrossSQLDataManager::GuildCrossOneDay(CCrossGuild* pGuild)
{
    // 公会BOSS伤害发奖
    if(pGuild->m_stGuildInfo.GetGuildBossTotalHurt() > 0)
    {
        auto stUinMap = GetUinGroupVec(pGuild);
        CCrossGuildBossRewardNotify stNotify;
        stNotify.m_ulTotalHurt = pGuild->m_stGuildInfo.GetGuildBossTotalHurt();
        stNotify.m_iWeek = weekday(CProcessor::GetNowSec() - 1_day);
        for(auto& iterVec : stUinMap)
        {
            stNotify.m_iGroupID = iterVec.first;
            stNotify.m_stUinVec = iterVec.second;
            m_pProcess->NotifyMessage(stNotify.m_iGroupID, 0, MSG_CROSS_GUILD_BOSS_REWARD_NOTIFY, stNotify);
        }

        pGuild->m_stGuildInfo.SetGuildBossTotalHurt(0);
    }
}

void CCrossSQLDataManager::NotifyGuildChange(int32_t iUin, int32_t iGroupID, int32_t iGuildID, const string& strGuildName, int32_t iExp, int8_t cPost, bool isExpel/* = false*/)
{
    CNotifyGuildChange stNotify;
    stNotify.m_iGuildID = iGuildID;
    stNotify.m_strGuildName = strGuildName;
    stNotify.m_iGuildExp = iExp;
    stNotify.m_iGuildPost = cPost;
    stNotify.m_bIsExpel = isExpel;
    m_pProcess->NotifyMessage(iGroupID, iUin, MSG_CROSS_GUILD_CHANGE_NOTIFY, stNotify);
}

void CCrossSQLDataManager::AddGuildLog(int iGuildID, int iType, int iUin, int iGroupID, int iAdmUin/* = 0*/, int iAdmGroupID/* = 0*/)
{
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);
    auto iter = cross_config->GetGuildConfig().m_stLogMap.find(iType);
    if(iter == cross_config->GetGuildConfig().m_stLogMap.end()) return;

    auto& stLogList = m_stGuildCache.m_stGuildLogMap[iGuildID];

    CPackUserBrief stUserInfo;
    GetUserBriefInfo(iUin, iGroupID, stUserInfo);
    CPackUserBrief stAdmUserInfo = stUserInfo;
    if(iAdmUin > 0)
    {
        GetUserBriefInfo(iAdmUin, iAdmGroupID, stAdmUserInfo);
    }

    CPackGuildLogInfo stInfo;
    stInfo.m_iTime = CProcessor::GetNowSec();
    stInfo.m_strLog = iter->second.Replace({{"nick", stUserInfo.m_strNick}, {"adm_nick", stAdmUserInfo.m_strNick}});
    stLogList.emplace_back(stInfo);

    if(stLogList.size() > GuildLog_MaxNum)
    {
        stLogList.pop_front();
    }
}

void CCrossSQLDataManager::DismissGuild(int32_t iGuildID)
{
    auto pGuild = GetGuild(iGuildID);
    if(nullptr == pGuild)
        return;

    // 发送通知

    // 删除数据库
    pGuild->m_stGuildInfo.ClearFromSQL();
    while(!pGuild->m_stMemberMap.Empty())
    {
        int iUin = pGuild->m_stMemberMap.Begin()->first;
        int iGroupID = pGuild->m_stMemberMap.Begin()->second->m_stData.m_iGroupID;

        HandleGuildLeave(pGuild, iUin, iGroupID, true);
    }

    m_stGuildMap.erase(iGuildID);
}

void CCrossSQLDataManager::UpdateGuildLazyData()
{
    int32_t iNowTime = CProcessor::GetNowSec();
    // 删除过期申请
    for(auto iterGuild = m_stGuildCache.m_stGuildApplyMap.begin(); iterGuild != m_stGuildCache.m_stGuildApplyMap.end(); )
    {
        for(auto iterApply = iterGuild->second.begin(); iterApply != iterGuild->second.end(); )
        {
            if(iNowTime - iterApply->second.m_iApplyTime > int32_t(7_day))
            {
                iterGuild->second.erase(iterApply++);
            }
            else
            {
                ++iterApply;
            }
        }

        if(iterGuild->second.empty())
            m_stGuildCache.m_stGuildApplyMap.erase(iterGuild++);
        else
            ++iterGuild;
    }

    // 删除过期日志
    for(auto iterGuild = m_stGuildCache.m_stGuildLogMap.begin(); iterGuild != m_stGuildCache.m_stGuildLogMap.end(); )
    {
        while(!iterGuild->second.empty() && (iNowTime - iterGuild->second.front().m_iTime > int32_t(30_day)))
        {
            iterGuild->second.pop_front();
        }

        if(iterGuild->second.empty())
            m_stGuildCache.m_stGuildLogMap.erase(iterGuild++);
        else
            ++iterGuild;
    }
}

void CCrossSQLDataManager::GuildCacheCrossOneDay()
{
    // BOSS伤害记录清空
    m_stGuildCache.m_stGuildBossHurtMap.clear();
    // 祈愿信息清空
    m_stGuildCache.m_stGuildWishMap.clear();
    // 检查会长转移
    GuildChiefTran();
}

void CCrossSQLDataManager::GetUserBriefInfo(int iUin, int iGroupID, CPackUserBrief& stInfo)
{
    const int32_t TABLE_DIV = 32;

    auto iter = m_stUserBriefMap.find(iUin);
    if(iter == m_stUserBriefMap.end())
    {
        auto pInfo = std::make_shared<CPackUserBrief>();
        int iTableMod = iUin % TABLE_DIV;
        char szBuffer[1024];
        snprintf(szBuffer, sizeof(szBuffer),
                 "SELECT nick, level, avatar_id, avatar_border, last_logout FROM user_info_t_%d WHERE uid=%d AND group_id=%d;",
                 iTableMod, iUin, iGroupID);
        try {
            taf::TC_Mysql::MysqlData stData = CLogicSQLTableRecordBase::GetMysqlConn()->queryRecord(szBuffer);
            if(stData.size() > 0)
            {
                auto& stQueryData = stData.data()[0];
                pInfo->m_iUid = iUin;
                pInfo->m_iGroupID = iGroupID;
                pInfo->m_strNick = stQueryData["nick"];
                pInfo->m_iLevel = ::atoi(stQueryData["level"].c_str());
                pInfo->m_iAvatarID = ::atoi(stQueryData["avatar_id"].c_str());
                pInfo->m_iAvatarBorder = ::atoi(stQueryData["avatar_border"].c_str());
                pInfo->m_iLastLogoutTime = ::atoi(stQueryData["last_logout"].c_str());
                m_stUserBriefMap.emplace(iUin, pInfo);
            }
        } catch(const taf::TC_Mysql_Exception& stEx) {}

        stInfo = *pInfo;
    }
    else
    {
        stInfo = *(iter->second);
    }
}

void CCrossSQLDataManager::GuildChiefTran()
{
    for(auto& guildInfo : m_stGuildMap)
    {
        CPackUserBrief stUserInfo;
        auto chiefUid = guildInfo.second->m_stGuildInfo.GetChiefUid();
        auto chiefGroupId = guildInfo.second->m_stGuildInfo.GetChiefGroup();
        GetUserBriefInfo(chiefUid,chiefGroupId,stUserInfo);

        //这里配置会长过期天数
        int32_t iNowSec = CProcessor::GetNowSec();

        std::shared_ptr<CCrossServerConfig> cross_config;
        CCrossServerConfig::GetConfig(cross_config);

        //会长不在线且长时间未登录
        if ((stUserInfo.m_iLastLogoutTime + (int32_t)3_day < iNowSec) && !stUserInfo.m_bIsOnline)
        {
            //开始找公会里的新会长(一天内登录且贡献值最高)
            int32_t iNewChiefUin = 0;
            int32_t iNewChiefGroup = 0;
            int32_t iContributeTotal = 0;
            for(auto iter = guildInfo.second->m_stMemberMap.Begin(); iter != guildInfo.second->m_stMemberMap.End(); ++iter)
            {
                if (iter->second->GetContributionTotal() < iContributeTotal)
                {
                    continue;
                }

                CPackUserBrief stMemberInfo;
                GetUserBriefInfo(iter->second->m_stData.m_iUid,iter->second->m_stData.m_iGroupID,stMemberInfo);
                if (!is_same_day(iNowSec,stMemberInfo.m_iLastLogoutTime) && !stMemberInfo.m_bIsOnline)
                {
                    continue;
                }

                iNewChiefUin = iter->second->m_stData.m_iUid;
                iNewChiefGroup = iter->second->m_stData.m_iGroupID;
            }

            if (0 != iNewChiefUin && 0 != iNewChiefGroup)
            {
                int32_t iRetCode = TransferGuild(chiefUid, chiefGroupId, iNewChiefUin, iNewChiefGroup);
                if(SERVER_ERRCODE::SUCCESS != iRetCode)
                {
                    continue;
                }
                // 日志
                AddGuildLog(guildInfo.first, GuildLog_SetChief, iNewChiefUin, iNewChiefGroup, chiefUid, chiefGroupId);
            }
        }
    }
}


int32_t CCrossSQLDataManager::GetSelfGuildWarInfo(int32_t uin, int32_t iGroupID,std::map<int,CPackGuildWarBossInfo>& guildWarInfo)
{
    int32_t iGuildID = GetGuildIDByUin(uin, iGroupID);
    if(iGuildID <= 0)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    guildWarInfo = pGuild->m_stGuildInfo.GetGuildWarBossInfo();
    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::UpdateSelfGuildWarInfo(int32_t uin, int32_t iGroupID,const std::map<int,CPackGuildWarBossInfo>& guildWarInfo)
{
    int32_t iGuildID = GetGuildIDByUin(uin, iGroupID);
    if(iGuildID <= 0)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    pGuild->m_stGuildInfo.SetGuildWarBossInfo(guildWarInfo);
    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::ClearSelfGuildWarInfo(int32_t uin, int32_t iGroupID)
{
    int32_t iGuildID = GetGuildIDByUin(uin, iGroupID);
    if(iGuildID <= 0)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    pGuild->m_stGuildInfo.SetGuildWarBossInfo({});
    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::UpdateGuildWarHurt(int32_t uin, int32_t iGroupID,uint64_t iHurt)
{
    int32_t iGuildID = GetGuildIDByUin(uin, iGroupID);
    if(iGuildID <= 0)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    pGuild->m_stGuildInfo.AddGuildWarScore(iHurt);
    return SERVER_ERRCODE::SUCCESS;
}

int32_t CCrossSQLDataManager::UpdateGuildWarSelfHurt(int32_t uin, int32_t iGroupID,uint64_t iHurt)
{
    int32_t iGuildID = GetGuildIDByUin(uin, iGroupID);
    if(iGuildID <= 0)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    auto pGuild = GetGuild(iGuildID);
    if (nullptr == pGuild)
        return SERVER_ERRCODE::GUILD_NOT_FOUND;

    auto iterMember = pGuild->m_stMemberMap.Find(uin);
    if (iterMember == pGuild->m_stMemberMap.End())
    {
        return SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND;
    }

    iterMember->second->AddGuildWarHurt(iHurt);
    return SERVER_ERRCODE::SUCCESS;
}
