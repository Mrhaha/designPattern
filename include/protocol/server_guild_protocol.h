#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"
#include "server_cross_protocol.h"
#include "server_home_protocol.h"

class CPackGuildInfo
{
public:
    CPackGuildInfo() : m_iGuildID(0), m_strGuildName(), m_iGuildExp(0), m_strBulletin(), m_stChiefInfo(),
          m_iMemberCount(0), m_iTotalDonate(0), m_iBossHurt(0) ,m_iSignLevel(0) {}

    MSGPACK_DEFINE(m_iGuildID, m_strGuildName, m_iGuildExp, m_strBulletin, m_stChiefInfo, m_iMemberCount,
                   m_iTotalDonate,m_iBossHurt,m_iSignLevel);

    int32_t	        m_iGuildID;
    std::string     m_strGuildName;
    int32_t         m_iGuildExp;
    std::string     m_strBulletin;
    CPackUserBrief  m_stChiefInfo;
    int32_t         m_iMemberCount;
    int32_t         m_iTotalDonate;         // 社团总贡献
    int64_t         m_iBossHurt;            // 公会boss伤害值
    int32_t         m_iSignLevel;           // 公会签到福利等级
};

class CPackGuildMember
{
public:
	CPackGuildMember() : m_stBriefInfo(), m_cPost(0), m_iContributionTotal(0), m_iJoinTime(0),m_iGuildWarHurt(0) {}

	MSGPACK_DEFINE(m_stBriefInfo, m_cPost, m_iContributionTotal, m_iJoinTime,m_iGuildWarHurt);

	CPackUserBrief  m_stBriefInfo;
	char            m_cPost;
	int32_t         m_iContributionTotal;
	int32_t         m_iJoinTime;
    uint64_t        m_iGuildWarHurt;
};

class CPackGuildApply
{
public:
	CPackGuildApply() : m_stUserBrief(), m_iApplyTime(0) {}

	MSGPACK_DEFINE(m_stUserBrief, m_iApplyTime);

	CPackUserBrief  m_stUserBrief;
    int32_t         m_iApplyTime;
};

class CPackGuildListInfo
{
public:
    CPackGuildListInfo() : m_stGuildInfo(), m_bApply(false) {}

    MSGPACK_DEFINE(m_stGuildInfo, m_bApply);

    CPackGuildInfo  m_stGuildInfo;
    bool            m_bApply;
};

class CResponseGetGuildList : public CPackUserSyncData
{
public:
	CResponseGetGuildList() : m_stGuildVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_stGuildVec);

	std::vector<CPackGuildListInfo> m_stGuildVec;
};

class CResponseGetSelfGuildInfo : public CPackUserSyncData
{
public:
    CResponseGetSelfGuildInfo() : m_stGuildInfo(), m_cPost(0), m_iWishCardID(0), m_iWishLastTime(0), m_stWishSwapCard(), m_iWishSwapTimes(0),
          m_bSign(false),m_bDonate(false),m_iRandomCount(0),m_iGuildBar(0),m_iChangeNameCD(0),m_cChangeNameFree(0),m_iUnlockItem(),
          m_iUnlockIndex(), m_iFootCloth(),m_iGuildDecorate(),m_stGuildBarInfo(),m_stGuildWarInfo(){}

	MSGPACK_DEFINE(m_stSyncData, m_stGuildInfo, m_cPost, m_iWishCardID, m_iWishLastTime, m_stWishSwapCard, m_iWishSwapTimes,
                   m_bSign,m_bDonate,m_iRandomCount,m_iGuildBar,m_iChangeNameCD,m_cChangeNameFree,m_iUnlockItem,m_iUnlockIndex,
                   m_iFootCloth,m_iGuildDecorate,m_stGuildBarInfo,m_stGuildWarInfo);

	CPackGuildInfo  m_stGuildInfo;
    char            m_cPost;
    int             m_iWishCardID;
    int             m_iWishLastTime;
    std::vector<int> m_stWishSwapCard;
    int             m_iWishSwapTimes;
    char            m_bSign;
    char            m_bDonate;
    int             m_iRandomCount;
    char            m_iGuildBar;

    int32_t         m_iChangeNameCD;        // 公会改名CD
    char            m_cChangeNameFree;      // 公会改名是否免费
    std::vector<int32_t> m_iUnlockItem;     // 公会解锁物品
    std::vector<int32_t> m_iUnlockIndex;    // 公会解锁主题

    std::map<int,std::map<int,std::map<int,char>>> m_iFootCloth; //地毯装饰
    std::map<int,CPackGuildHomeInfo> m_iGuildDecorate;           //公会装饰
    std::vector<CPackGuildUserBarInfo> m_stGuildBarInfo;         //公会吧台信息

    CPackGuildWarInfo   m_stGuildWarInfo;   //公会战信息

};

class CResponseGetGuildMembers : public CPackUserSyncData
{
public:
    CResponseGetGuildMembers() : m_stMemberVec() {}

    MSGPACK_DEFINE(m_stSyncData, m_stMemberVec);

    std::vector<CPackGuildMember> m_stMemberVec;
};

//CREATE_GUILD = 10187,				//创建公会
class CRequestCreateGuild
{
public:
	CRequestCreateGuild() : m_strGuildName() {}

	MSGPACK_DEFINE(m_strGuildName);

	std::string m_strGuildName;
};

class CResponseCreateGuild : public CPackUserSyncData
{
public:
	CResponseCreateGuild() : m_iGuildID(0), m_stItemData(), m_strGuildName() {}

	MSGPACK_DEFINE(m_stSyncData, m_iGuildID, m_stItemData, m_strGuildName);

	int32_t m_iGuildID;
    CPackTotalItemData m_stItemData;
    std::string m_strGuildName;
};

//UPDATE_GUILD_BULLETIN = 10189,		//更新公会公告
class CRequestUpdateGuildBulletin
{
public:
	CRequestUpdateGuildBulletin() : m_strBulletin() {}

	MSGPACK_DEFINE(m_strBulletin);

	std::string m_strBulletin;
};

class CResponseUpdateGuildBulletin : public CPackUserSyncData
{
public:
	CResponseUpdateGuildBulletin() : m_strBulletin() {}

	MSGPACK_DEFINE(m_stSyncData, m_strBulletin);

	std::string m_strBulletin;
};

//UPDATE_GUILD_POST = 10190,			//改变公会成员的职位
class CRequestUpdateGuildPost
{
public:
	CRequestUpdateGuildPost() : m_iDstUid(0), m_iDstGroupID(0), m_iDstPost(0) {}

	MSGPACK_DEFINE(m_iDstUid, m_iDstGroupID, m_iDstPost);

	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
	int32_t m_iDstPost;
};

class CResponseUpdateGuildPost : public CPackUserSyncData
{
public:
	CResponseUpdateGuildPost() : m_iDstUid(0), m_iDstGroupID(0), m_iDstPost(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iDstUid, m_iDstGroupID, m_iDstPost);

	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
	int32_t m_iDstPost;
};

//TRANSFER_GUILD = 10191,			//移交公会
class CRequestTransferGuild
{
public:
	CRequestTransferGuild() : m_iDstUid(0), m_iDstGroupID(0) {}

	MSGPACK_DEFINE(m_iDstUid, m_iDstGroupID);

	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
};

class CResponseTransferGuild : public CPackUserSyncData
{
public:
	CResponseTransferGuild() : m_iDstUid(0), m_iDstGroupID(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iDstUid, m_iDstGroupID);

	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
};

//EXPEL_GUILD = 10192,				//踢出公会
class CRequestExpelGuild
{
public:
	CRequestExpelGuild() : m_iDstUid(0), m_iDstGroupID(0) {}

	MSGPACK_DEFINE(m_iDstUid, m_iDstGroupID);

	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
};

class CResponseExpelGuild : public CPackUserSyncData
{
public:
	CResponseExpelGuild() : m_iDstUid(0), m_iDstGroupID(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iDstUid, m_iDstGroupID);

	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
};

//APPLY_JOIN_GUILD = 10193,			//申请加入公会		
class CRequestApplyJoinGuild
{
public:
	CRequestApplyJoinGuild() : m_iGuildID(0) {}

	MSGPACK_DEFINE(m_iGuildID);

	int32_t m_iGuildID;
};

class CResponseApplyJoinGuild : public CPackUserSyncData
{
public:
	CResponseApplyJoinGuild() : m_iGuildID(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iGuildID);

	int32_t m_iGuildID;
};

//DEAL_GUILD_APPLY = 10194,		//处理公会申请
class CRequestDealGuildApply
{
public:
	CRequestDealGuildApply() : m_iDstUid(0), m_iDstGroupID(0), m_bIsAgree(false) {}

	MSGPACK_DEFINE(m_iDstUid, m_iDstGroupID, m_bIsAgree);

	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
	bool m_bIsAgree;
};

class CResponseDealGuildApply : public CPackUserSyncData
{
public:
	CResponseDealGuildApply() : m_iDstUid(0), m_iDstGroupID(0), m_bIsAgree(false),m_iGuildID(0),m_strGuildName(""),m_iLevel(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iDstUid, m_iDstGroupID, m_bIsAgree,m_iGuildID,m_strGuildName,m_iLevel);

	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
	bool m_bIsAgree;
    int32_t m_iGuildID;
    std::string m_strGuildName;
    int32_t m_iLevel;
};

class CResponseGetGuildApplyList : public CPackUserSyncData
{
public:
	CResponseGetGuildApplyList() : m_stApplyVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_stApplyVec);

	std::vector<CPackGuildApply> m_stApplyVec;
};

class CRequestGetOtherPlayerDetail
{
public:
	CRequestGetOtherPlayerDetail() : m_iUid(0), m_iGroupID(0), m_iTeamType(0) {}

	MSGPACK_DEFINE(m_iUid, m_iGroupID, m_iTeamType);

	int32_t         m_iUid;
	int32_t         m_iGroupID;
    int32_t         m_iTeamType;        // 默认主线
};

class CResponseGetOtherPlayerDetail : public CPackUserSyncData
{
public:
	CResponseGetOtherPlayerDetail() : m_stInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_stInfo);

    CPackUserDetailInfo m_stInfo;
};

//SEARCH_GUILD = 10247,						//搜索公会
class CRequestSearchGuild
{
public:
	CRequestSearchGuild() : m_strSearch(),m_iGuildID(0) {}

	MSGPACK_DEFINE(m_strSearch,m_iGuildID);

	std::string m_strSearch;
    int32_t m_iGuildID;
};

class CResponseSearchGuild : public CPackUserSyncData
{
public:
	CResponseSearchGuild() : m_stGuildInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_stGuildInfo);

    CPackGuildListInfo m_stGuildInfo;
};

class CRequestChangeGuildName
{
public:
    CRequestChangeGuildName() : m_strNewGuildName("") {}

    MSGPACK_DEFINE(m_strNewGuildName);

    std::string     m_strNewGuildName;
};

class CResponseChangeGuildName : public CPackUserSyncData
{
public:
    CResponseChangeGuildName() : m_strNewGuildName(""), m_iChangeNameCD(0), m_cChangeNameFree(0), m_bConsume(true), m_stItemData(),m_strOldGuildName(""),m_iGuildID(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_strNewGuildName, m_iChangeNameCD, m_cChangeNameFree, m_bConsume, m_stItemData,m_strOldGuildName,m_iGuildID);

    std::string     m_strNewGuildName;
    int32_t         m_iChangeNameCD;
    char            m_cChangeNameFree;
    bool            m_bConsume;
    CPackTotalItemData m_stItemData;
    std::string     m_strOldGuildName;
    int32_t         m_iGuildID;
};

class CResponseGuildBossGetHurt : public CPackUserSyncData
{
public:
    CResponseGuildBossGetHurt() : m_stHurtMap() {}

    MSGPACK_DEFINE(m_stSyncData, m_stHurtMap);

    std::map<int32_t, uint64_t> m_stHurtMap;
};

class CPackGuildBossHurtRankInfo
{
public:
    CPackGuildBossHurtRankInfo() : m_stInfo(), m_ulHurt(0) {}

    MSGPACK_DEFINE(m_stInfo, m_ulHurt);

    CPackUserBrief  m_stInfo;
    uint64_t        m_ulHurt;
};

class CResponseGuildBossHurtList : public CPackUserSyncData
{
public:
    CResponseGuildBossHurtList() : m_stHurtRank() {}

    MSGPACK_DEFINE(m_stSyncData, m_stHurtRank);

    std::vector<CPackGuildBossHurtRankInfo> m_stHurtRank;
};

class CResponseGuildGetLog : public CPackUserSyncData
{
public:
    CResponseGuildGetLog() : m_stLogVec() {}

    MSGPACK_DEFINE(m_stSyncData, m_stLogVec);

    std::vector<CPackGuildLogInfo> m_stLogVec;
};

class CResponseGuildWishSetCard: public CPackUserSyncData
{
public:
    CResponseGuildWishSetCard() : m_iCardID(0) {}

    MSGPACK_DEFINE(m_stSyncData,m_iCardID);

    int32_t     m_iCardID;
};

class CRequestGuildWishSetCard
{
public:
    CRequestGuildWishSetCard() : m_iCardID(0) {}

    MSGPACK_DEFINE(m_iCardID);

    int32_t     m_iCardID;
};

class CPackGuildWish
{
public:
    CPackGuildWish() : m_iCardID(0), m_iCurNum(0), m_bSwap(false),m_bIsFull(false) {}

    MSGPACK_DEFINE(m_iCardID, m_iCurNum, m_bSwap,m_bIsFull);

    int32_t             m_iCardID;
    int32_t             m_iCurNum;
    bool                m_bSwap;
    bool                m_bIsFull;
};

class CRequestGuildWishSetSwapCard
{
public:
    CRequestGuildWishSetSwapCard() : m_stSwapCardVec() {}

    MSGPACK_DEFINE(m_stSwapCardVec);

    std::vector<int>    m_stSwapCardVec;
};

class CPackGuildWishDetail
{
public:
    CPackGuildWishDetail() : m_stInfo(), m_stWish() {}

    MSGPACK_DEFINE(m_stInfo, m_stWish);

    CPackUserBrief      m_stInfo;
    CPackGuildWish      m_stWish;
};

class CResponseGuildWishGetList : public CPackUserSyncData
{
public:
    CResponseGuildWishGetList() : m_stWishVec() {}

    MSGPACK_DEFINE(m_stSyncData, m_stWishVec);

    std::vector<CPackGuildWishDetail> m_stWishVec;
};

class CRequestGuildWishGetSwapInfo
{
public:
    CRequestGuildWishGetSwapInfo() : m_iDstUin(0), m_iDstGroupID(0) {}

    MSGPACK_DEFINE(m_iDstUin, m_iDstGroupID);

    int32_t     m_iDstUin;
    int32_t     m_iDstGroupID;
};

class CResponseGuildWishGetOtherSwapInfo : public CPackUserSyncData
{
public:
    CResponseGuildWishGetOtherSwapInfo() : m_iCardID(0), m_iCurNum(0), m_stSwapCardMap(), m_iRetCode(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iCardID, m_iCurNum, m_stSwapCardMap, m_iRetCode);

    int32_t             m_iCardID;
    int32_t             m_iCurNum;
    std::map<int, int>  m_stSwapCardMap;
    int32_t             m_iRetCode;
};

class CRequestGuildWishSwap
{
public:
    CRequestGuildWishSwap() : m_iDstUin(0), m_iDstGroupID(0), m_iCardID(0), m_iSwapCardID(0) {}

    MSGPACK_DEFINE(m_iDstUin, m_iDstGroupID, m_iCardID, m_iSwapCardID);

    int32_t     m_iDstUin;
    int32_t     m_iDstGroupID;
    int32_t     m_iCardID;
    int32_t     m_iSwapCardID;
};

class CCrossGuildWishOp
{
public:
    CCrossGuildWishOp() : m_iUin(0), m_iGroupID(0), m_iDstUin(0), m_iDstGroupID(0), m_iCardID(0), m_iSwapCardID(0), m_usCmd(0), m_iRetCode(0), m_iStatTime(0), m_strNick(),m_stUserInfo() {}

    MSGPACK_DEFINE(m_iUin, m_iGroupID, m_iDstUin, m_iDstGroupID, m_iCardID, m_iSwapCardID, m_usCmd, m_iRetCode, m_iStatTime, m_strNick,m_stUserInfo);

    int32_t     m_iUin;
    int32_t     m_iGroupID;
    int32_t     m_iDstUin;
    int32_t     m_iDstGroupID;
    int32_t     m_iCardID;
    int32_t     m_iSwapCardID;
    uint16_t	m_usCmd;
    int32_t     m_iRetCode;
    int32_t     m_iStatTime;
    std::string m_strNick;          // 赠送发起人的角色名，做记录用
    CPackUserDetailInfo m_stUserInfo;
};

class CResponseGuildWishRewardNotify : public CPackUserSyncData
{
public:
    CResponseGuildWishRewardNotify() : m_iCardID(0), m_stReward(), m_stLog() {}

    MSGPACK_DEFINE(m_stSyncData, m_iCardID, m_stReward, m_stLog);

    int32_t             m_iCardID;
    CPackGameItem       m_stReward;
    std::vector<std::pair<std::string, int>> m_stLog;
};

class CRequestGuildDonate
{
public:
    CRequestGuildDonate() : m_iDonateType() {}

    MSGPACK_DEFINE(m_iDonateType);

    int32_t m_iDonateType;
};


class CResponseGuildDonate: public CPackUserSyncData
{
public:
    CResponseGuildDonate() : m_iDonateType() ,m_iReward(){}

    MSGPACK_DEFINE(m_stSyncData,m_iDonateType,m_iReward);

    int32_t m_iDonateType;
    CPackTotalItemData m_iReward;
};

class CCrossGuildSignInReward
{
public:
    CCrossGuildSignInReward():m_iLevel(0) {}

    MSGPACK_DEFINE(m_iLevel);

    int32_t m_iLevel;   //当前社团的签到奖励等级
};

class CResponseGuildSign: public CPackUserSyncData
{
public:
    CResponseGuildSign():m_stRewardItem(){}

    MSGPACK_DEFINE(m_stSyncData,m_stRewardItem);

    CPackTotalItemData m_stRewardItem;
};

class CCrossGuildUpGradeSign: public CPackUserSyncData
{
public:
    CCrossGuildUpGradeSign():m_iCurSignLevel(0){}

    MSGPACK_DEFINE(m_stSyncData,m_iCurSignLevel);

    int32_t m_iCurSignLevel;
};

class CResponseGuildRandomInfo: public CPackUserSyncData
{
public:
    CResponseGuildRandomInfo():m_iTodayCount(0),m_iCurrType(0),m_iIndex(0){}

    MSGPACK_DEFINE(m_stSyncData,m_iTodayCount,m_iCurrType,m_iIndex);

    int32_t m_iTodayCount;
    int32_t m_iCurrType;
    int32_t m_iIndex;
};


class CRequestGuildGetRandomReward
{
public:
    CRequestGuildGetRandomReward():m_bWin(false){}

    MSGPACK_DEFINE(m_bWin);

    bool m_bWin;
};

class CResponseGuildGetRandomReward: public CPackUserSyncData
{
public:
    CResponseGuildGetRandomReward():m_stReward(){}

    MSGPACK_DEFINE(m_stSyncData,m_stReward);

    CPackTotalItemData m_stReward;
};

class CRequestGuildSave
{
public:
    CRequestGuildSave(): m_stUnEquipGoods(),m_stEquipGood(),m_stMoveGoods(){}

    MSGPACK_DEFINE(m_stUnEquipGoods, m_stEquipGood,m_stMoveGoods)

    std::vector<CRequestHomeUnequipGoods> m_stUnEquipGoods;
    std::vector<CRequestHomeEquipGoods> m_stEquipGood;
    std::vector<CRequestHomeGoodsMove> m_stMoveGoods;
};


class CResponseGuildSave: public CPackUserSyncData
{

public:
    CResponseGuildSave(): m_stUnEquipGoods(),m_stEquipGood(){}

    MSGPACK_DEFINE(m_stSyncData,m_stUnEquipGoods, m_stEquipGood)

    std::vector<CResponseHomeUnequipGoods> m_stUnEquipGoods;
    std::vector<CResponseHomeEquipGoods> m_stEquipGood;
};

class CResponseGetGuildBossHurtRank : public CPackUserSyncData
{
public:
    CResponseGetGuildBossHurtRank() : m_stGuildVec() {}

    MSGPACK_DEFINE(m_stSyncData, m_stGuildVec);

    std::vector<CPackGuildListInfo> m_stGuildVec;
};

class CRequestGuildSitBar
{
public:
    CRequestGuildSitBar(): m_iUid(0),m_iGroupID(0),m_iCardID(0),m_iIndex(0),m_strName(""),m_iDurationTime(0){}

    MSGPACK_DEFINE(m_iUid,m_iGroupID,m_iCardID,m_iIndex,m_strName)

    int32_t m_iUid;
    int32_t m_iGroupID;
    int32_t m_iCardID;
    int32_t m_iIndex;
    std::string m_strName;
    int32_t m_iDurationTime; //持续时间logic带给cross
};

class CResponseGuildSitBar : public CPackUserSyncData
{
public:
    CResponseGuildSitBar(): m_iUid(0),m_iGroupID(0),m_iCardID(0),m_strName(""),m_iEndTime(0){}

    MSGPACK_DEFINE(m_stSyncData,m_iUid,m_iGroupID,m_iCardID,m_strName,m_iEndTime)

    int32_t m_iUid;
    int32_t m_iGroupID;
    int32_t m_iCardID;
    int32_t m_iIndex;
    std::string m_strName;
    int32_t m_iEndTime;
};

class CCrossGuildGetSitBarReward
{
public:
    CCrossGuildGetSitBarReward():m_iCardID(0){}

    MSGPACK_DEFINE(m_iCardID)

    int32_t m_iCardID;
};


class CResponseGetGuildSitBarReward : public CPackUserSyncData
{
public:
    CResponseGetGuildSitBarReward(): m_stReward(){}

    MSGPACK_DEFINE(m_stSyncData,m_stReward)

    CPackTotalItemData m_stReward;
};

class CRequestUnlockCollection
{
public:
    CRequestUnlockCollection():m_iType(0),m_iIndex(0){}

    MSGPACK_DEFINE(m_iType,m_iIndex)

    int32_t m_iType;
    int32_t m_iIndex;
};

class CResponseUnlockCollection: public CPackUserSyncData
{
public:
    CResponseUnlockCollection():m_iType(0),m_iIndex(0),m_iGuildID(0),m_strGuildName(""){}

    MSGPACK_DEFINE(m_stSyncData,m_iType,m_iIndex,m_iGuildID,m_strGuildName)

    int32_t m_iType;
    int32_t m_iIndex;
    int32_t m_iGuildID;
    std::string m_strGuildName;
};

class CRequestSetDecorate
{
public:
    CRequestSetDecorate():m_iHomeID(0),m_iIndex(0){}

    MSGPACK_DEFINE(m_iHomeID,m_iIndex)

    int32_t m_iHomeID;
    int32_t m_iIndex;
};

class CResponseSetDecorate: public CPackUserSyncData
{
public:
    CResponseSetDecorate():m_iHomeID(0),m_iIndex(0){}

    MSGPACK_DEFINE(m_stSyncData,m_iHomeID,m_iIndex)

    int32_t m_iHomeID;
    int32_t m_iIndex;
};

class CResponseGuildFragmentHistory: public CPackUserSyncData
{
public:
    CResponseGuildFragmentHistory():m_stFragHistory(){}

    MSGPACK_DEFINE(m_stSyncData,m_stFragHistory)

    std::vector<CPackGuildFragmentsHistory> m_stFragHistory;
};

class CResponseGuildWishRewardInfo: public CPackUserSyncData
{
public:
    CResponseGuildWishRewardInfo():m_iWishNum(0){}

    MSGPACK_DEFINE(m_stSyncData,m_iWishNum)

    int32_t m_iWishNum;
};

class CResponseLeaveGuild: public CPackUserSyncData
{
public:
    CResponseLeaveGuild():m_iGuildID(0),m_strGuildName(""),m_iGuildExp(0){}

    MSGPACK_DEFINE(m_stSyncData,m_iGuildID,m_strGuildName,m_iGuildExp)

    int32_t m_iGuildID;
    std::string m_strGuildName;
    int32_t m_iGuildExp;
};
//获取公会战信息
class CResponseGuildWarInfo: public CPackUserSyncData
{
public:
    CResponseGuildWarInfo():m_stGuildBossInfo(),m_iGuildWarRank(0),m_stGuildWarRecord(),m_stUsedCard(),m_iDailyCount(0),m_stSelfGuildWarInfo(),m_iGuildWarScore(0){}

    MSGPACK_DEFINE(m_stSyncData,m_stGuildBossInfo,m_iGuildWarRank,m_stGuildWarRecord,m_stUsedCard,m_iDailyCount,m_stSelfGuildWarInfo,m_iGuildWarScore)

    std::map<int32_t,CPackGuildWarBossInfo> m_stGuildBossInfo;    //公会各个boss信息
    uint32_t m_iGuildWarRank;                                     //公会战排名
    std::vector<CPackGuildWarRecord> m_stGuildWarRecord;          //公会战历史记录
    std::map<int32_t,char> m_stUsedCard;                          //公会战个人已经使用的card
    int32_t m_iDailyCount;                                        //公会战每日挑战次数
    std::map<int32_t,CPackGuildWarSelfInfo> m_stSelfGuildWarInfo; //公会战个人信息
    uint64_t m_iGuildWarScore;                                    //公会战积分
};

//参与公会战
class CRequestGuildWarBegin
{
public:
    CRequestGuildWarBegin():m_iLevelID(0),m_stUseCard(){}

    MSGPACK_DEFINE(m_iLevelID,m_stUseCard)

    int32_t m_iLevelID;
    std::vector<int32_t> m_stUseCard;
};

class CResponseGuildWarBegin: public CPackUserSyncData
{
public:
    CResponseGuildWarBegin():m_iLevelID(0),m_stUseCard(){}

    MSGPACK_DEFINE(m_stSyncData,m_iLevelID,m_stUseCard)

    int32_t m_iLevelID;
    std::vector<int32_t> m_stUseCard;
};

//结算公会战
class CRequestGuildWarEnd
{
public:
    CRequestGuildWarEnd():m_iLevelID(0),m_stUseCard(),m_iHurt(0){}

    MSGPACK_DEFINE(m_iLevelID,m_stUseCard,m_iHurt)

    int32_t m_iLevelID;
    std::vector<int32_t> m_stUseCard;
    uint64_t m_iHurt;
};

class CResponseGuildWarEnd: public CPackUserSyncData
{
public:
    CResponseGuildWarEnd():m_iLevelID(0),m_stUseCard(),m_iHurt(0),m_iCurGuildRank(0){}

    MSGPACK_DEFINE(m_stSyncData,m_iLevelID,m_stUseCard,m_iHurt,m_iCurGuildRank)

    int32_t m_iLevelID;
    std::vector<int32_t> m_stUseCard;
    uint64_t m_iHurt;
    int32_t m_iCurGuildRank;        //当前公会排名
};

struct CPackGuildWarTotalRankInfo
{
    CPackGuildWarTotalRankInfo():m_iGuildID(0),m_iHurt(0),m_iRank(0),m_stGuildInfo(){}

    MSGPACK_DEFINE(m_iGuildID,m_iHurt,m_iRank,m_stGuildInfo)

    int32_t  m_iGuildID;                //公会id
    uint64_t m_iHurt;                   //公会总伤害
    uint32_t m_iRank;                   //公会排名
    CPackGuildInfo m_stGuildInfo;       //公会信息
};

//获取公会战排行
class CRequestGetGuildWarRank
{
public:
    CRequestGetGuildWarRank():m_iIndex(0){}

    MSGPACK_DEFINE(m_iIndex)

    int32_t m_iIndex;       //页数
};


class CResponseGuildWarGetRankInfo: public CPackUserSyncData
{
public:
    CResponseGuildWarGetRankInfo():m_iIndex(0),m_stTotalRank(){}

    MSGPACK_DEFINE(m_stSyncData,m_iIndex,m_stTotalRank)

    int32_t m_iIndex;
    std::vector<CPackGuildWarTotalRankInfo> m_stTotalRank;
};



class CResponseGuildWarUpdateRankInfo: public CPackUserSyncData
{
public:
    CResponseGuildWarUpdateRankInfo():m_stRankSelf(){}

    MSGPACK_DEFINE(m_stSyncData,m_stRankSelf)

    CPackGuildWarSelfRankInfo m_stRankSelf;
};

class CRequestGuildWarSelfRankInfo
{
public:
    CRequestGuildWarSelfRankInfo() : selfRankInfo() {}

    MSGPACK_DEFINE(selfRankInfo);

    CPackGuildWarSelfRankInfo selfRankInfo;
};