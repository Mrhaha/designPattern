//
// Created by lxh on 2021/10/8.
//

#pragma once

#include "cross_mysql_associate_container.hpp"
#include "cross_record_guild_info_table.h"
#include "cross_record_guild_member_table.h"
#include "cross_record_rank_sql_container.hpp"
#include "cross_record_guild_war_rank_table.h"
#include "server_guild_protocol.h"
#include "server_cross_protocol.h"
#include "Processor.h"
#include "ServerConfigParser.h"

enum
{
    post_min = -1,
    post_member = 0,    //公会成员
    post_admin = 1,		//公会副会长
    post_chief = 2,		//公会长

    post_max,
};

enum
{
    GuildLog_Leave = 1,
    GuildLog_Join = 2,
    GuildLog_SetChief = 3,
    GuildLog_SetAdmin = 4,
    GuildLog_SetMember = 5,

    GuildLog_MaxNum = 50,
};

enum EnumItemSubType
{
    EIST_HomePage = 20,             // 墙纸
    EIST_HomeFloorBG = 21,          // 地板皮肤
    EIST_HomeFootcloth = 22,        // 地毯
    EIST_HomeOrnament = 23,         // 摆件

    EIST_Max = 100,
};

enum EnumGuildUnlockType
{
    GuildUnlockType_Decorate = 1,      //解锁主题
    GuildUnlockType_Collection = 2,    //解锁藏品
};

enum EnumGuildHomeType
{
    GuildHomeTypeMain  = 1,         //社团大厅
    GuildHomeTypeCollection = 2,    //社团藏品室
};

enum EnumGuildBuffType
{
    GuildBuffInvalid = 0,
    GuildBuffFishing = 1,           //钓鱼获取5星鱼成功率提升
    GuildBuffGuildBoss = 2,         //攻击社团Boss，伤害提高
    GuildBuffGoldAdd   = 3,         //所有关卡金币奖励增加
    GuildBuffGuildDonate = 4,       //社团捐献时贡献提高
    GuildBuffHotSpring  = 5,        //社团温泉增加15点
};


const int E_UniqueIDBase = 100;             // 通过坐标计算ID的组合基数
const float GUILD_BUFF_RATION = 0.1;
const int32_t GUILD_WAR_TIME = 150;         //公会战战斗超时间隔
const int32_t GUILD_WAR_RECORD_SIZE = 20;   //公会战记录上限

class CCrossGuild
{
public:
    CCrossGuild();

    CPackGuildInfo GetGuildInfoPack() const;
    CPackGuildBriefInfo GetGuildBriefInfoPack() const;

    bool IsMemberFull() const;
    int8_t GetPosByUin(int32_t iUin) const;
    int32_t GetGuildID() const { return m_stGuildInfo.m_stData.m_iGuildID; }
    std::string GetGuildName() const { return m_stGuildInfo.GetGuildName(); }
    int32_t GetGuildExp() const { return m_stGuildInfo.GetGuildExp(); }
    int32_t GetChiefUin() const { return m_stGuildInfo.GetChiefUid(); }
    int32_t GetChiefGroupID() const { return m_stGuildInfo.GetChiefGroup(); }
    bool IsChief(int32_t iUin) const { return m_stGuildInfo.GetChiefUid() == iUin; };
    int32_t GetMemberCount() const { return m_stMemberMap.Size(); }
    static bool IsValidPost(int8_t cPos) { return (post_min < cPos && cPos < post_max); }

private:
    using guild_member_container_type = CCrossSQLRecordAssociateContainer<int32_t, TGuildMemberTable*, TGuildMemberTableValueType>;

    friend class CCrossSQLDataManager;

public:
    TGuildInfoTable             m_stGuildInfo;
private:
    guild_member_container_type m_stMemberMap;
};

using map_uin_vec = std::map<int32_t, std::vector<int32_t>>;

class CCrossSQLDataManager
{
public:
    CCrossSQLDataManager(CProcessor* pProcessor);

    ////////////// guild cache //////////////
    const CPackCentreCacheGuild& GetGuildCache() const { return m_stGuildCache; }
    void SetGuildCache(const CPackCentreCacheGuild& stCache) { m_stGuildCache = stCache; }
    ///////////////////////////////////////////////

    // 跨天时清空一些需要及时更新的数据
    void GuildCacheCrossOneDay();
    // 检查会长转移
    void GuildChiefTran();
    // 加载公会战排名
    void LoadGuildRank(int32_t iNowSec);
    // 公会战是否开始
    bool IsGuildWarOpen(int32_t iNowSec);


    void GetUserBriefInfo(int iUin, int iGroupID, CPackUserBrief& stInfo);
    CCrossGuild* GetGuild(int32_t iGuildID);
    CCrossGuild* GetGuildByUin(int32_t iUin, int32_t iGroupID);
    int32_t GetGuildIDByUin(int32_t iUin, int32_t iGroupID);
    std::vector<int32_t> GetRandGuildIDVec();
    std::vector<int32_t> GetGuildBossHurtRank();
    void UpdateUserBrief(const CPackUserBrief& stInfo);
    const guild_apply_map* GetGuildApplyMap(int32_t iUin, int32_t iGroupID);
    bool IsApplyGuild(int32_t iGuildID, int32_t iUin);
    const guild_boss_hurt_map* GetGuildBossHurtMap(int32_t iUin, int32_t iGroupID);
    const guild_log_list* GetGuildLogList(int32_t iUin, int32_t iGroupID);
    const guild_wish_map* GetGuildWishMap(int32_t iUin, int32_t iGroupID);
    map_uin_vec GetUinGroupVec(const CCrossGuild* pGuild);
    int32_t GetCurGuildID() const {return m_iCurGuildID;}
    void SetCurGuildID(int32_t iCurGuildID) {m_iCurGuildID = iCurGuildID;}

    // 公会
    int32_t GetGuildMembers(int32_t iUin, int32_t iGroupID, CResponseGetGuildMembers& stRsp);
    int32_t CreateGuild(int32_t iUin, int32_t iGroupID, const std::string& strGuildName,std::string& execSql);
    int32_t LeaveGuild(int32_t iUin, int32_t iGroupID);
    int32_t UpdateBulletin(int32_t iUin, int32_t iGroupID, const std::string& strBulletin);
    int32_t UpdatePost(int32_t iUin, int32_t iGroupID, int32_t iDstUin, int32_t iDstGroupID, int8_t cNewPos);
    int32_t TransferGuild(int32_t iUin, int32_t iGroupID, int32_t iDstUin, int32_t iDstGroupID);
    int32_t ExpelGuild(int32_t iUin, int32_t iGroupID, int32_t iDstUin, int32_t iDstGroupID);
    int32_t ApplyJoinGuild(int32_t iUin, int32_t iGroupID, int32_t iDstGuildID);
    int32_t DealGuildApply(int32_t iUin, int32_t iGroupID, int32_t iDstUin, int32_t iDstGroupID, bool bAgree);
    int32_t ChangeGuildName(int32_t iUin, int32_t iGroupID, const std::string& strGuildName, bool bIsGM, CResponseChangeGuildName& stRsp,std::string& mysql);
    void UpdateGuildBossHurt(int32_t iUin, int32_t iGroupID, uint64_t ulHurt);
    void UpdateGuildWishInfo(int32_t iUin, int32_t iGroupID, int32_t iCardID, int32_t iCurNum, bool bSwap,bool bIsFull);

    int32_t UpdateGuildExp(int32_t iUin, int32_t iGroupID,int32_t addExp);
    int32_t UpdateGuildDonate(int32_t iUin, int32_t iGroupID,const TLogicGuildDonateElem* donateConfig);
    int32_t GetGuildSignRewardLevel(int32_t iUin, int32_t iGroupID,int32_t& SignLevel);
    int32_t UpGradeGuildSignRewardLevel(int32_t iUin, int32_t iGroupID);
    int32_t SaveGuildDecorate(int32_t iUin,int32_t iGroupID);

    int32_t GMModifyGuild(int32_t iGuildID, int32_t iGuildExp);


    //公会装扮
    int32_t UnLockGuildCollection(int32_t iUin,int32_t iGroupID,CRequestUnlockCollection stReq);
    int32_t UnEquipGoods(int32_t iUin,int32_t iGroupID,CRequestHomeUnequipGoods& stReq);
    int32_t EquipGoods(int32_t iUin,int32_t iGroupID,CRequestHomeEquipGoods& stReq);
    int32_t MoveGoods(int32_t iUin,int32_t iGroupID,CRequestHomeGoodsMove& stReq);
    static int32_t GetUniqueIDByPos(int32_t iX, int32_t iY) { return (iX * E_UniqueIDBase + iY); }

    //公会吧台
    int32_t AddBarInfo(int32_t uin, int32_t iGroupID,CRequestGuildSitBar req);
    int32_t LeaveBarInfo(int32_t uin, int32_t iGroupID,CCrossGuildGetSitBarReward& cross);
    int32_t GetBarInfo(int32_t uin, int32_t iGroupID,std::vector<CPackGuildUserBarInfo>* pRsp);

    //公会战
    int32_t GetSelfGuildWarInfo(int32_t uin, int32_t iGroupID,std::map<int,CPackGuildWarBossInfo>& guildWarInfo);
    int32_t UpdateSelfGuildWarInfo(int32_t uin, int32_t iGroupID,const std::map<int,CPackGuildWarBossInfo>& guildWarInfo);
    int32_t ClearSelfGuildWarInfo(int32_t uin, int32_t iGroupID);
    int32_t UpdateGuildWarHurt(int32_t uin, int32_t iGroupID,uint64_t iHurt);
    int32_t UpdateGuildWarSelfHurt(int32_t uin, int32_t iGroupID,uint64_t iHurt);


private:
    void Refresh(CCrossGuild* pGuild);
    void LoadGuildFromDB(int32_t iGuildID);

    bool HandleGuildJoin(CCrossGuild* pGuild, int32_t iUin, int32_t iGroupID);
    bool HandleGuildLeave(CCrossGuild* pGuild, int32_t iUin, int32_t iGroupID, bool isExpel);

    void GuildCrossOneDay(CCrossGuild* pGuild);
    void NotifyGuildChange(int32_t iUin, int32_t iGroupID, int32_t iGuildID, const string& strGuildName, int32_t iExp, int8_t cPost, bool isExpel = false);
    void AddGuildLog(int iGuildID, int iType, int iUin, int iGroupID, int iAdmUin = 0, int iAdmGroupID = 0);

public:
    void DismissGuild(int32_t iGuildID);
    void UpdateGuildLazyData();

public:
    using guild_war_rank_container_type = CCrossSQLRankAssociateContainer<int32_t, TGlobalGuildWarRankTable*, TGlobalGuildWarRankTableValueType>;
    guild_war_rank_container_type guild_war_rank;
    std::map<int32_t, guild_war_rank_container_type> m_stGuildWarRankMap;

private:
    CProcessor* m_pProcess;
    int32_t m_iCurGuildID;
    std::unordered_map<int32_t, std::shared_ptr<CPackUserBrief>> m_stUserBriefMap;      // 玩家简要信息
    std::unordered_map<int32_t, int32_t>                        m_stUinToGuildID;       // [uin, guildid]
    std::unordered_map<int32_t, std::unique_ptr<CCrossGuild>>   m_stGuildMap;           // [guildID,guildInfo]
    CPackCentreCacheGuild                                       m_stGuildCache;         // 保存在文件的缓存信息

};
