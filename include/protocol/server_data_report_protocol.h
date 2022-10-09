#pragma once

#include "msgpack.hpp"
#include "server_base_protocol.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
class CDataReportCommon
{
public:
    CDataReportCommon():m_strEven(""),m_strAccount(""),m_strUdid(""),m_strNickName(""){}

    MSGPACK_DEFINE(m_strEven,m_strAccount,m_strUdid,m_strNickName);

    std::string m_strEven;
    std::string m_strAccount;
    std::string m_strUdid;
    std::string m_strNickName;
};


class CDataReportSetMusic
{
public:
    CDataReportSetMusic() : m_stCommon(),m_stMusicID(),m_iPos(0) {}

    MSGPACK_DEFINE(m_stCommon,m_stMusicID,m_iPos)

    CDataReportCommon m_stCommon;
    std::list<int32_t> m_stMusicID;
    int32_t m_iPos;
};

class CDataReportPackEquipInfo
{
public:
    CDataReportPackEquipInfo(): m_iEquipID(0),m_iEquipStar(0),m_iEquipType(0){}

    MSGPACK_DEFINE(m_iEquipID,m_iEquipStar,m_iEquipType)

    int32_t m_iEquipID;
    int32_t m_iEquipStar;
    int32_t m_iEquipType;
};

class CDataReportGetEquip
{
public:
    CDataReportGetEquip(): m_vecEquipInfo(){}

    MSGPACK_DEFINE(m_stCommon,m_iGetLevelID,m_vecEquipInfo)

    CDataReportCommon m_stCommon;
    int32_t m_iGetLevelID;
    std::vector<CDataReportPackEquipInfo> m_vecEquipInfo;
};

class CDataReportUpdateTalent
{
public:
    CDataReportUpdateTalent(): m_stCommon(),m_iCardID(),m_iCardLevel(0),m_iSkillID(0),m_iSkillAfterLevel(0){}

    MSGPACK_DEFINE(m_stCommon,m_iCardID,m_iCardLevel,m_iSkillID,m_iSkillAfterLevel)

    CDataReportCommon m_stCommon;
    int32_t m_iCardID;
    int32_t m_iCardLevel;
    int32_t m_iSkillID;
    int32_t m_iSkillAfterLevel;
};


class CDataReportUpdateConstel
{
public:
    CDataReportUpdateConstel(): m_stCommon(),m_iConstelID(0),m_iConstelStar(0),m_iConstelSuit(0),m_iConstelPos(0),m_iConstelAfterLevel(0){}

    MSGPACK_DEFINE(m_stCommon,m_iConstelID,m_iConstelStar,m_iConstelSuit,m_iConstelPos,m_iConstelAfterLevel)

    CDataReportCommon m_stCommon;
    int32_t m_iConstelID;
    int32_t m_iConstelStar;
    int32_t m_iConstelSuit;
    int32_t m_iConstelPos;
    int32_t m_iConstelAfterLevel;
};

class CDataReportPackConstelInfo
{
public:
    CDataReportPackConstelInfo(): m_iConstelID(0),m_iConstelStar(0),m_iConstelSuit(0),m_iConstelPos(0){}

    MSGPACK_DEFINE(m_iConstelID,m_iConstelStar,m_iConstelSuit,m_iConstelPos)

    int32_t m_iConstelID;
    int32_t m_iConstelStar;
    int32_t m_iConstelSuit;
    int32_t m_iConstelPos;
};

class CDataReportGetConstel
{
public:
    CDataReportGetConstel(): m_stCommon(),m_iGetLevelID(0),m_vecConstelInfo(){}

    MSGPACK_DEFINE(m_stCommon,m_iGetLevelID,m_vecConstelInfo)

    CDataReportCommon m_stCommon;
    int32_t m_iGetLevelID;
    std::vector<CDataReportPackConstelInfo> m_vecConstelInfo;
};

class CDataReportStartFightPVE
{
public:
    CDataReportStartFightPVE(): m_stCommon(),m_iLevelID(0),m_vecTeamCardID(){}

    MSGPACK_DEFINE(m_stCommon,m_iLevelID,m_vecTeamCardID)

    CDataReportCommon m_stCommon;
    int32_t m_iLevelID;
    std::vector<int32_t> m_vecTeamCardID;
};

class CDataReportGainEmail
{
public:
    CDataReportGainEmail(): m_stCommon(),m_iEmailID(0),m_stReward(){}

    MSGPACK_DEFINE(m_stCommon,m_iEmailID,m_stReward)

    CDataReportCommon m_stCommon;
    int32_t m_iEmailID;
    std::vector<CPackGameItem> m_stReward;
};

class CDataReportAchieveMission
{
public:
    CDataReportAchieveMission(): m_stCommon(),m_iQuestID(0),m_iQuestType(0){}

    MSGPACK_DEFINE(m_stCommon,m_iQuestID,m_iQuestType)

    CDataReportCommon m_stCommon;
    int32_t m_iQuestID;
    int32_t m_iQuestType;
};

class CDataReportGetRewardMission
{

public:
    CDataReportGetRewardMission(): m_iQuestID(0),m_iQuestType(0){}

    MSGPACK_DEFINE(m_iQuestID,m_iQuestType)

    int32_t m_iQuestID;
    int32_t m_iQuestType;

};

class CDataReportTotalRewardMission
{
public:
    CDataReportTotalRewardMission(): m_stCommon(),m_stMissionReward(){}

    MSGPACK_DEFINE(m_stCommon,m_stMissionReward)

    CDataReportCommon m_stCommon;
    std::vector<CDataReportGetRewardMission> m_stMissionReward;
    std::vector<CPackGameItem> m_stRewardItem;
};

class CDataReportGetPassReward
{
public:
    CDataReportGetPassReward():m_stCommon(),m_stNormalLevel(),m_stAdvancedLevel(),m_stTotalReward(){}

    MSGPACK_DEFINE(m_stCommon,m_stNormalLevel,m_stAdvancedLevel,m_stTotalReward)

    CDataReportCommon m_stCommon;
    std::vector<int32_t> m_stNormalLevel;
    std::vector<int32_t> m_stAdvancedLevel;
    std::vector<CPackGameItem> m_stTotalReward;
};

class CDataReportPVP
{
public:
    CDataReportPVP():m_stCommon(),m_iAttackUin(0),m_iDefendUin(0),m_iAttackRank(0),m_iDefendRank(0),m_iAttackCardID(),m_iDefendCardID(),m_bWin(false),m_iAttackPoint(0),m_iDefendPoint(0),m_bBot(false){}

    MSGPACK_DEFINE(m_stCommon,m_iAttackUin,m_iDefendUin,m_iAttackRank,m_iDefendRank,m_iAttackCardID,m_iDefendCardID,m_bWin,m_iAttackPoint,m_iDefendPoint,m_bBot)

    CDataReportCommon m_stCommon;
    int32_t m_iAttackUin;
    int32_t m_iDefendUin;
    int32_t m_iAttackRank;
    int32_t m_iDefendRank;

    std::vector<int32_t> m_iAttackCardID;
    std::vector<int32_t> m_iDefendCardID;

    bool m_bWin;
    int32_t m_iAttackPoint;
    int32_t m_iDefendPoint;
    bool m_bBot;
};

class CDataReportPVPInfo
{
public:
    CDataReportPVPInfo():m_iUin(0),m_iRank(0){}

    MSGPACK_DEFINE(m_iUin,m_iRank)

    int32_t m_iUin;
    int32_t m_iRank;
};

class CDataReportRefreshPVP
{
public:
    CDataReportRefreshPVP():m_stCommon(),m_stPVPInfo(){}

    MSGPACK_DEFINE(m_stCommon,m_stPVPInfo);

    CDataReportCommon m_stCommon;
    std::vector<CDataReportPVPInfo> m_stPVPInfo;
};

class CDataReportRewardPVP
{
public:
    CDataReportRewardPVP():m_stCommon(),m_iUin(0),m_iRankScore(0),m_stReward(){}

    MSGPACK_DEFINE(m_stCommon,m_iUin,m_iRankScore,m_stReward);

    CDataReportCommon m_stCommon;
    int32_t m_iUin;
    int32_t m_iRankScore;
    std::vector<CPackGameItem> m_stReward;
};

class CDataReportChangeTeam
{
public:
    CDataReportChangeTeam():m_stCommon(),m_iTeamType(),m_stBeforeFormation(),m_stAfterFormation(){}

    MSGPACK_DEFINE(m_stCommon,m_iTeamType,m_stBeforeFormation,m_stAfterFormation);

    CDataReportCommon m_stCommon;
    int32_t m_iTeamType;
    std::vector<int32_t> m_stBeforeFormation;
    std::vector<int32_t> m_stAfterFormation;
};

class CDataReportCreateGuild
{
public:
    CDataReportCreateGuild():m_stCommon(),m_iGuildID(0),m_strGuildName(""){}

    MSGPACK_DEFINE(m_stCommon,m_iGuildID,m_strGuildName);

    CDataReportCommon m_stCommon;
    int32_t m_iGuildID;
    std::string m_strGuildName;
};

class CDataReportJoinGuild
{

public:
    CDataReportJoinGuild():m_stCommon(),m_iGuildID(0),m_strGuildName(""),m_iGuildUid(0){}

    MSGPACK_DEFINE(m_stCommon,m_iGuildID,m_strGuildName,m_iGuildUid);

    CDataReportCommon m_stCommon;
    int32_t m_iGuildID;
    std::string m_strGuildName;
    int32_t m_iGuildUid;
};

class CDataReportLeave
{
public:
    CDataReportLeave():m_stCommon(),m_iGuildID(0),m_strGuildName(""),m_iGuildExp(0),m_iActiveness(0){}

    MSGPACK_DEFINE(m_stCommon,m_iGuildID,m_strGuildName,m_iGuildExp,m_iActiveness);

    CDataReportCommon m_stCommon;
    int32_t m_iGuildID;
    std::string m_strGuildName;
    int32_t m_iGuildExp;
    int32_t m_iActiveness;
};

class CDataReportChangeName
{
public:
    CDataReportChangeName():m_stCommon(),m_iGuildID(0),m_strGuildOldName(""),m_strGuildNewName(""){}

    MSGPACK_DEFINE(m_stCommon,m_iGuildID,m_strGuildOldName,m_strGuildNewName);

    CDataReportCommon m_stCommon;
    int32_t m_iGuildID;
    std::string m_strGuildOldName;
    std::string m_strGuildNewName;
};

class CDataReportGetGuildInfo
{
public:
    CDataReportGetGuildInfo():m_stCommon(),m_iGuildID(0),m_iChiefUid(0),m_iGuildExp(0),m_iActiveness(0){}

    MSGPACK_DEFINE(m_stCommon,m_iGuildID,m_iChiefUid,m_iGuildExp,m_iActiveness);

    CDataReportCommon m_stCommon;
    int32_t m_iGuildID;
    int32_t m_iChiefUid;
    int32_t m_iGuildExp;
    int32_t m_iActiveness;
};

class CDataReportDonateCard
{
public:
    CDataReportDonateCard():m_stCommon(),m_iDestUin(0),m_iCardID(){}

    MSGPACK_DEFINE(m_stCommon,m_iDestUin,m_iCardID);

    CDataReportCommon m_stCommon;
    int32_t m_iDestUin;
    int32_t m_iCardID;
};

class CDataReportSetWishCard
{
public:
    CDataReportSetWishCard():m_stCommon(),m_iCardID(){}

    MSGPACK_DEFINE(m_stCommon,m_iCardID);

    CDataReportCommon m_stCommon;
    int32_t m_iCardID;
};

class CDataReportBuyGuildItem
{
public:
    CDataReportBuyGuildItem():m_stCommon(),m_iGuildID(),m_strGuildName(""),m_iType(0),m_iIndex(0){}

    MSGPACK_DEFINE(m_stCommon,m_iGuildID,m_strGuildName,m_iType,m_iIndex);

    CDataReportCommon m_stCommon;
    int32_t m_iGuildID;
    std::string m_strGuildName;
    int32_t m_iType;
    int32_t m_iIndex;
};

class CDataReportPayChoose
{
public:
    CDataReportPayChoose():m_stCommon(),m_iIndex(0){}

    MSGPACK_DEFINE(m_stCommon,m_iIndex);

    CDataReportCommon m_stCommon;
    int32_t m_iIndex;
};


//玩家充值
class CDataReportPay
{
public:
    CDataReportPay():m_stCommon(),m_iDepositID(0),m_iProductID(0),m_iChannelID(0),m_strOrderID(""),
          m_iPayTime(0),m_strPayPrice(""),m_strPayMethod(""),m_bIsGM(false),m_bFirst(false){}

    MSGPACK_DEFINE(m_stCommon,m_iDepositID,m_iProductID,m_iChannelID,m_strOrderID,m_iPayTime,m_strPayPrice,m_strPayMethod,m_bIsGM,m_bFirst);

    CDataReportCommon m_stCommon;
    int32_t m_iDepositID;		//充值ID
    int32_t m_iProductID;		//产品ID
    int32_t m_iChannelID;		//渠道ID
    std::string m_strOrderID;	//订单号
    int32_t m_iPayTime;			//购买时间
    std::string m_strPayPrice;	//购买价格
    std::string m_strPayMethod;	//购买方式
    bool m_bIsGM;               //是否是GM充值
    bool m_bFirst;              //是否首次充值
};

class CDataReportItemChange
{
public:
    CDataReportItemChange():m_stCommon(),m_iItemID(0),m_iGrade(0),m_iType(0),m_iCount(0),m_iCmd(0),m_iTotal(0){}

    MSGPACK_DEFINE(m_stCommon,m_iItemID,m_iGrade,m_iType,m_iCount,m_iCmd,m_iTotal);

    CDataReportCommon m_stCommon;
    int32_t m_iItemID;
    int32_t m_iGrade;
    int32_t m_iType;
    int32_t m_iCount;
    int32_t m_iCmd;
    int32_t m_iTotal;
};

class CDataReportOffline
{
public:
    CDataReportOffline():m_stCommon(),m_iRegisterDate(0),m_iOnlineTime(0){}

    MSGPACK_DEFINE(m_stCommon,m_iRegisterDate,m_iOnlineTime);

    CDataReportCommon m_stCommon;
    int32_t m_iRegisterDate;
    int32_t m_iOnlineTime;
};
