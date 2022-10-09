#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"

class CPackEntireWarExtra
{
public:
	CPackEntireWarExtra() : m_stStatus() {}
	MSGPACK_DEFINE(m_stStatus);
	std::map<int32_t, std::vector<bool>> m_stStatus;
};

class CPackEntireWarBuff
{
public:
	CPackEntireWarBuff() : m_iType(0), m_iPercentage(0), m_iEndTime(0) {}
	MSGPACK_DEFINE(m_iType, m_iPercentage, m_iEndTime);
	int32_t m_iType;
	int32_t m_iPercentage;
	int32_t m_iEndTime;
};

class CPackEntireWarBuffVector
{
public:
	CPackEntireWarBuffVector() : m_stBuffVec() {}
	MSGPACK_DEFINE(m_stBuffVec);
	std::vector<CPackEntireWarBuff> m_stBuffVec;
};

class CPackPrivateTalkOneUser
{
public:
	CPackPrivateTalkOneUser() : m_bHasRead(false), m_stTalkCacheList() {}

	MSGPACK_DEFINE(m_bHasRead, m_stTalkCacheList);

    bool					m_bHasRead;
    talk_cache_list	        m_stTalkCacheList;
};

class CPackMatchStoreBuyCount
{
public:
	MSGPACK_DEFINE(m_iBuyCountMap);
	std::map<int32_t, int32_t> m_iBuyCountMap;
};

//战报
class CPackFightReportItem
{
public:
	CPackFightReportItem() :
		m_iReportType(0),
		m_iReportID(0), m_strReportContent(""), m_cFightResult(0),
        m_stParams(){}

	MSGPACK_DEFINE(m_iReportType, m_iReportID, m_strReportContent, m_cFightResult, m_stParams);

	int                     m_iReportType;
	int                     m_iReportID;
	std::string             m_strReportContent;
	char                    m_cFightResult;
	std::vector<int>        m_stParams;

	bool operator < (const CPackFightReportItem& other) const
	{
		return this->m_iReportID < other.m_iReportID;
	}
};

class CRequestGetFightReport
{
public:
	CRequestGetFightReport()
	{
	}
	MSGPACK_DEFINE(m_iReportType);

	int                     m_iReportType;
};

class CResponseGetFightReport : public CPackUserSyncData
{
public:
	CResponseGetFightReport() :m_stFightReports()
	{
	}

	MSGPACK_DEFINE(m_stSyncData, m_stFightReports);

	std::set<CPackFightReportItem>		m_stFightReports;
};


class CPackCangbaotuSuipian
{
public:
	CPackCangbaotuSuipian()
	{
	}
	MSGPACK_DEFINE(m_mpCangbaotuSuipian);

	std::map<int, uint32_t>		m_mpCangbaotuSuipian; //藏宝图id: 碎片位图
};

class CNotifyCangbaotuSuipian : public CPackUserSyncData
{
public:
	CNotifyCangbaotuSuipian()
	{
	}
	MSGPACK_DEFINE(m_stSyncData, m_mpCangbaotuSuipian, m_iSuipianIndex);

	std::map<int, std::string>		m_mpCangbaotuSuipian; //藏宝图id: 碎片位图
	int								m_iSuipianIndex;	//当前获取到的碎片（1-9）
};

//主公很忙 任务信息
class CPackIbusyTaskElem
{
public:
	CPackIbusyTaskElem() : m_iTaskID(0), m_iStar(0), m_bFinished(false) {}

	MSGPACK_DEFINE(m_iTaskID, m_iStar, m_bFinished);

	int				m_iTaskID;
	int				m_iStar;
	bool			m_bFinished;
};

class CPackIbusyTaskData
{
public:
	CPackIbusyTaskData() : m_mpTasks(){}

	MSGPACK_DEFINE(m_mpTasks);

	std::unordered_map<int, CPackIbusyTaskElem>	m_mpTasks;
};

// MSG_LOGIC_COMMON_NOTIFY = 10327,        // 通用弹确认框的通知
class CCommonNotify : public CPackUserSyncData
{
public:
    enum
    {
        ECCN_Null,
        ECCN_AntiAddiction = 1,     //防沉迷提示
    };

public:
    CCommonNotify() : m_iType(0), m_strNotify("") {}

    MSGPACK_DEFINE(m_stSyncData, m_iType, m_strNotify);

    int             m_iType;
    std::string     m_strNotify;
};

class CPackWenWenNewsElem
{
public:
    CPackWenWenNewsElem() : m_iIndex(0),m_iType(0),m_iID(0),m_iPara(){}

    MSGPACK_DEFINE(m_iIndex,m_iType,m_iID,m_iPara);
    int32_t     m_iIndex;
    int32_t     m_iType;
    int32_t     m_iID;
    std::vector<std::string>    m_iPara;
};

class CPackWenWenNewsServerElem
{
public:
    CPackWenWenNewsServerElem() : m_iIndexID(0),m_iID(0),m_iType(0), m_iGroupID(0), m_iUin(0),m_iPara(){}

    MSGPACK_DEFINE(m_iIndexID,m_iID,m_iType,m_iGroupID, m_iUin,m_iPara);

    int32_t     m_iIndexID;
    int32_t     m_iID;
    int32_t     m_iType;
    int32_t     m_iGroupID;
    int32_t     m_iUin;
    std::vector<std::string> m_iPara;
};

// 文文日报，全服数据
class CPackWenWenNewsServerData
{
public:
    CPackWenWenNewsServerData() : m_stDataMap() {}

    MSGPACK_DEFINE(m_stDataMap);

    std::multimap<int32_t, CPackWenWenNewsServerElem> m_stDataMap;
};

class CPackWenWenNewsDynamicData
{
public:
    CPackWenWenNewsDynamicData() : m_iType(0), m_iID(0), m_iPara(0), m_iUid(0), m_iGroupID(0), m_strName(""), m_iLevel(0),
                                   m_iAvatarIcon(0), m_iAvatarBorder(0), m_iVipLevel(0) {}

    MSGPACK_DEFINE(m_iType, m_iID, m_iPara, m_iUid, m_iGroupID, m_strName, m_iLevel, m_iAvatarIcon, m_iAvatarBorder, m_iVipLevel);

    int32_t     m_iType;
    int32_t     m_iID;
    int32_t     m_iPara;
    int32_t     m_iUid;
    int32_t     m_iGroupID;
    std::string m_strName;
    int32_t     m_iLevel;
    int32_t     m_iAvatarIcon;
    int32_t     m_iAvatarBorder;
    int32_t     m_iVipLevel;
};

class CResponseWenWenNews : public CPackUserSyncData
{
public:
    CResponseWenWenNews() : m_cChoose(0), m_stThemeData() {}

    MSGPACK_DEFINE(m_stSyncData,m_cChoose,m_stThemeData);

    char m_cChoose;                                         //阵营数据
    std::vector<CPackWenWenNewsElem>    m_stThemeData;      //主题数据
};

// 世界任务已领奖玩家信息
class CPackWorldQuestAwardInfo
{
public:
    CPackWorldQuestAwardInfo() : m_iUid(0), m_iGroupID(0), m_stReward(), m_bIsBest(false) {}

    MSGPACK_DEFINE(m_iUid, m_iGroupID, m_stReward, m_bIsBest);

    int32_t     m_iUid;
    int32_t     m_iGroupID;
    CPackGameItem m_stReward;
    bool        m_bIsBest;
};

// 家园房间内伙伴信息
class CPackHomeHeroInfo
{
public:
    CPackHomeHeroInfo() : m_iGoodsIndex(0), m_iHandleTime(0), m_iPara(0) {}

    MSGPACK_DEFINE(m_iGoodsIndex, m_iHandleTime, m_iPara);

    int32_t     m_iGoodsIndex;          // 家具索引
    int32_t     m_iHandleTime;          // 操作的结束时间(做菜，泡茶，读书的结束时间)
    int32_t     m_iPara;                // 菜谱ID，茶谱ID，书ID
};

class CPackNewLimitBossInfo
{
public:
    CPackNewLimitBossInfo() : m_iOwnerUin(0), m_iOwnerGroupID(0),m_iAppearLeveID(0),m_iLevelID(0), m_iLimitTime(0),m_iEndTime(0),
          m_ulMaxHp(0), m_ulCurHurt(0),m_bIsRun(false),m_iIsFighting(0),m_iLastFightTime(0) {}

    MSGPACK_DEFINE(m_iOwnerUin, m_iOwnerGroupID, m_iAppearLeveID ,m_iLevelID, m_iLimitTime,
                   m_iEndTime, m_ulMaxHp, m_ulCurHurt,m_bIsRun,m_iIsFighting);

    int32_t     m_iOwnerUin;            // 发现者
    int32_t     m_iOwnerGroupID;
    int32_t     m_iAppearLeveID;        // 出现的精英关卡ID
    int32_t     m_iLevelID;             // 对应的关卡ID
    int32_t     m_iLimitTime;           // BOSS限制时间
    int32_t     m_iEndTime;             // Boss消失时间
    uint64_t    m_ulMaxHp;              // BOSS最大血量
    uint64_t    m_ulCurHurt;            // BOSS已损失血量
    bool        m_bIsRun;               // Boss是否逃逸
    int32_t     m_iIsFighting;          // Boss是否被挑战中
    int32_t     m_iLastFightTime;       // Boss最近一次被挑战时间

};


class CPackLimitBossTeamInfo
{
public:
    struct user_key
    {
        user_key():m_iUin(0),m_iGroupID(0),m_ulHurt(0),m_strName(""),m_iIconHead(0),m_iIconImage(0),m_bIsFight(false),m_bGetReward(false){}

        MSGPACK_DEFINE(m_iUin, m_iGroupID,m_ulHurt,m_strName,m_iIconHead,m_iIconImage,m_bIsFight,m_bGetReward);

        int32_t m_iUin;
        int32_t m_iGroupID;
        int64_t m_ulHurt;
        std::string m_strName;
        int32_t m_iIconHead;
        int32_t m_iIconImage;
        bool  m_bIsFight;       //是否参与，不能用伤害为0判断是因为参与和造成伤害异步的，必须提前标记
        bool  m_bGetReward;
    };

    CPackLimitBossTeamInfo():m_stBossInfo(),m_stInviteUser(){}

    MSGPACK_DEFINE(m_stBossInfo, m_stInviteUser);

    CPackNewLimitBossInfo m_stBossInfo;
    std::unordered_map<int32_t,user_key> m_stInviteUser;
};

class CPackGuildRandomInfo
{
public:
    CPackGuildRandomInfo() : m_iRandomType(0),m_iRandomIndex(0){}

    MSGPACK_DEFINE(m_iRandomType,m_iRandomIndex);

    int32_t m_iRandomType;
    int32_t m_iRandomIndex;
};

class CPackGuildHomeItem
{
public:
    CPackGuildHomeItem(){}

    MSGPACK_DEFINE(m_iGoodID,m_iPosX,m_iPosY,m_iStat,m_iQuadrant);

    int32_t m_iGoodID;
    int32_t m_iPosX;
    int32_t m_iPosY;
    int32_t m_iStat;
    int32_t m_iQuadrant;
};

class CPackGuildHomeInfo
{
public:
    CPackGuildHomeInfo():m_iDecorateIndex(0),m_iFloorBG(0),m_iHomePage(0),m_stGuildHomeItems(){}

    MSGPACK_DEFINE(m_iDecorateIndex,m_iFloorBG,m_iHomePage,m_stGuildHomeItems);

    int32_t m_iDecorateIndex;                   //房间主题
    int32_t m_iFloorBG;
    int32_t m_iHomePage;
    std::map<int32_t,CPackGuildHomeItem> m_stGuildHomeItems;
};

class CPackRuneInfo
{
public:
    CPackRuneInfo():m_iRuneID(0),m_iLevel(0),m_stState(){};

    MSGPACK_DEFINE(m_iRuneID,m_iLevel,m_stState);

    int32_t m_iRuneID;
    int32_t m_iLevel;
    std::map<int32_t,bool>    m_stState;
};

class CPackZhuHaiBlessInfo
{
public:
    CPackZhuHaiBlessInfo():m_stBlessGroup(){}

    MSGPACK_DEFINE(m_stBlessGroup);

    std::vector<int> m_stBlessGroup;
};

class CRequestChooseWenWenReward
{
public:
    CRequestChooseWenWenReward():m_iChoose(0) {}

    MSGPACK_DEFINE(m_iChoose);

    int32_t m_iChoose;
};

class CResponseChooseWenWenReward: public CPackUserSyncData
{
public:
    CResponseChooseWenWenReward():m_iChoose(0) {}

    MSGPACK_DEFINE(m_stSyncData,m_iChoose);

    int32_t m_iChoose;
};

class CPackGuildFragmentsHistory
{
public:
    CPackGuildFragmentsHistory():m_stScrDetail(),m_stDestDetail(),m_iSwapCardID(0),m_iCardID(0){}

    MSGPACK_DEFINE(m_stScrDetail,m_stDestDetail,m_iSwapCardID,m_iCardID);
    CPackUserDetailInfo m_stScrDetail;
    CPackUserDetailInfo m_stDestDetail;
    int32_t m_iSwapCardID;
    int32_t m_iCardID;
};

//付费钻石平账记录
class CPackPayDiamond
{
public:
    CPackPayDiamond():m_iNum(0),m_iRatio(0){}

    MSGPACK_DEFINE(m_iNum,m_iRatio);

    int32_t m_iNum;
    double  m_iRatio;
};

class CPackCrushBossInfo
{
public:
    CPackCrushBossInfo() : m_iLevelID(0), m_ulMaxHp(0), m_ulCurHurt(0),m_iGrade(0){}

    MSGPACK_DEFINE(m_iLevelID, m_ulMaxHp, m_ulCurHurt,m_iGrade);

    int32_t     m_iLevelID;             // 对应的关卡ID
    uint64_t    m_ulMaxHp;              // BOSS最大血量
    uint64_t    m_ulCurHurt;            // BOSS已损失血量
    int32_t     m_iGrade;               // Boss品阶
};

struct CPackHomeGoodsBuffEffect
{
    CPackHomeGoodsBuffEffect():m_iGoodID(0),m_iBuffID(0),m_fParam1(0){}

    MSGPACK_DEFINE(m_iGoodID, m_iBuffID, m_fParam1);

    int32_t m_iGoodID;
    int32_t m_iBuffID;
    double m_fParam1;
};

struct CPackGuildWarInfo
{
    CPackGuildWarInfo():m_iGuildRank(0),m_iGuildRankScore(0){}

    MSGPACK_DEFINE(m_iGuildRank, m_iGuildRankScore);

    uint32_t m_iGuildRank;               //公会战排名
    uint64_t m_iGuildRankScore;          //公会战积分
};

struct CPackGuildWarSingleBossHurt
{
    CPackGuildWarSingleBossHurt():m_stUserBrief(),m_iGuildRankHurt(0){}

    MSGPACK_DEFINE(m_stUserBrief,m_iGuildRankHurt);

    CPackUserBrief  m_stUserBrief;           //玩家信息
    uint64_t m_iGuildRankHurt;              //单个boss造成伤害
};

struct CPackGuildWarBossInfo
{
    CPackGuildWarBossInfo():m_iLevelID(0),m_iCurHurt(0),m_isBeFight(false),m_iBossHp(0),m_iDeep(0),m_iLastStartTime(0),m_stBossRankInfo(),m_iUid(0){}

    MSGPACK_DEFINE(m_iLevelID,m_iCurHurt,m_isBeFight,m_iBossHp,m_iDeep,m_iLastStartTime,m_stBossRankInfo,m_iUid);

    int32_t m_iLevelID;
    uint64_t m_iCurHurt;            //当前伤害
    bool m_isBeFight;               //当前是否被挑战
    uint64_t m_iBossHp;             //boss总血量
    int32_t m_iDeep;                //层数
    int32_t m_iLastStartTime;       //上次挑战时间
    std::map<int32_t,CPackGuildWarSingleBossHurt> m_stBossRankInfo; //单个boss排名
    int32_t m_iUid;                 //当前挑战者
};

struct CPackGuildWarRecord
{
    CPackGuildWarRecord():m_iUid(0),m_strNickName(""),m_iHurt(0),m_iLevel(0),m_iTimeStamp(0){}

    MSGPACK_DEFINE(m_iUid,m_strNickName,m_iHurt,m_iLevel,m_iTimeStamp)

    int32_t m_iUid;
    std::string m_strNickName;
    uint64_t m_iHurt;
    int32_t m_iLevel;
    int32_t m_iTimeStamp;
};

struct CPackGuildWarSelfInfo
{
    CPackGuildWarSelfInfo():m_iHurt(0),m_iRankScore(0){}

    MSGPACK_DEFINE(m_iHurt,m_iRankScore)

    uint64_t m_iHurt;                   //个人伤害
    int32_t  m_iRankScore;              //个人排名
};

struct CPackGuildWarSelfTotalInfo
{
    CPackGuildWarSelfTotalInfo():m_iHurt(0),m_iRankScore(0){}

    MSGPACK_DEFINE(m_iHurt,m_iRankScore)

    uint64_t m_iHurt;                   //个人总伤害
    int32_t  m_iRankScore;              //个人排名
};

struct CPackGuildBriefInfo
{
    CPackGuildBriefInfo():m_iGuildID(0),m_strGuildName("") {}

    MSGPACK_DEFINE(m_iGuildID,m_strGuildName)

    int32_t	        m_iGuildID;             //公会id
    std::string     m_strGuildName;         //公会名
};

struct CPackGuildWarSelfRankInfo
{
    CPackGuildWarSelfRankInfo():m_iGuildID(0),m_iRank(0),m_iRankID(0),m_iGuildWarEndTime(0){}

    MSGPACK_DEFINE(m_iGuildID,m_iRank,m_iRankID,m_iGuildWarEndTime)

    int32_t  m_iGuildID;                //公会id
    uint32_t m_iRank;                   //公会排名
    int32_t  m_iRankID;                 //本次公会战使用的排行榜id
    int32_t  m_iGuildWarEndTime;        //公会战结束时间
};

class CResponseGetControlInfo: public CPackUserSyncData
{
public:
    CResponseGetControlInfo():m_iSwitchControl() {}

    MSGPACK_DEFINE(m_stSyncData,m_iSwitchControl);

    std::vector<int32_t> m_iSwitchControl;
};



