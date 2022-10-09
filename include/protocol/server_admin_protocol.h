#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"

class CGMResponseBody
{
public:
    CGMResponseBody() : m_iCode(0) {}

    MSGPACK_DEFINE(m_iCode);

    int32_t m_iCode;
};

//MSG_ADMIN_RECHARGE = 9000,              //GM充值
class CGMRequestRecharge
{
public:
    CGMRequestRecharge() :
            m_iDepositID(0), m_iProductID(0), m_iChannelID(0), m_strOrderID(), m_iPayTime(0)
            , m_strPayPrice(), m_strPayMethod(), m_bIsTestOrder(false), m_strAdditional() {}

    MSGPACK_DEFINE(m_iDepositID, m_iProductID, m_iChannelID, m_strOrderID, m_iPayTime,
                   m_strPayPrice, m_strPayMethod, m_bIsTestOrder, m_strAdditional);


    int32_t m_iDepositID;		//充值ID
    int32_t m_iProductID;		//产品ID
    int32_t m_iChannelID;		//渠道ID
    std::string m_strOrderID;	//订单号
    int32_t m_iPayTime;			//购买时间
    std::string m_strPayPrice;	//购买价格
    std::string m_strPayMethod;	//购买方式
    bool m_bIsTestOrder;		//是否测试订单
    std::string m_strAdditional;//额外数据
};

class CGMResponseRecharge
{
public:
    CGMResponseRecharge() : m_iCode(0), m_bIsNew(0) {}

    MSGPACK_DEFINE(m_iCode, m_bIsNew);

    int32_t         m_iCode;
    int32_t			m_bIsNew;
};

//MSG_LOGIC_USER_RECHARGE_NOTIFY = 10091,			//玩家充值通知
class CGMNotifyRecharge : public CPackUserSyncData
{
public:
    MSGPACK_DEFINE(m_stSyncData, m_iDepositID, m_iPrice, m_iTotalAddupRmb, m_strOrderID, m_iPayGamecoin, m_stItemData);

    int         m_iDepositID;
    int         m_iPrice;
    int         m_iTotalAddupRmb;
    std::string m_strOrderID;
    int32_t		m_iPayGamecoin;
    CPackTotalItemData m_stItemData;
};

//MSG_ADMIN_MODIFY_ITEM = 9001,           //GM修改玩家物品
class CGMRequestModifyItem
{
public:
    MSGPACK_DEFINE(m_stItems);

    std::vector<CPackGameItem> m_stItems;
};

class CGMResponseModifyItem
{
public:
    CGMResponseModifyItem() : m_iCode(0), m_stSuccess(), m_stFail() {}

    MSGPACK_DEFINE(m_iCode, m_stSuccess, m_stFail);

    int					m_iCode;
    std::vector<int>	m_stSuccess;
    std::vector<int>	m_stFail;
};

//MSG_LOGIC_NOTIFY_MODIFY_ITEM_EXTERNAL = 10255,		//通知外部修改道具
class CGMNotifyModifyItem : public CPackUserSyncData
{
public:
    CGMNotifyModifyItem() : m_stItems(), m_stFishVec() {}

    MSGPACK_DEFINE(m_stSyncData, m_stItems, m_stFishVec);

    CPackTotalItemData m_stItems;
    std::vector<CPackHomeFishElem> m_stFishVec;
};

//MSG_ADMIN_ADD_SYSTEM_MAIL = 9002,		//GM发系统邮件
class CGMRequestAddSystemMail
{
public:
    MSGPACK_DEFINE(m_iVipLevel,
                   m_iBeginTime,
                   m_iEndTime,
                   m_strTitle,
                   m_strSender,
                   m_strContent,
                   m_stAttachment,
                   m_stChannelVec);

    int32_t m_iVipLevel = 0;
    int32_t m_iBeginTime = 0;
    int32_t m_iEndTime = 0;
    std::string                 m_strTitle;
    std::string                 m_strSender;
    std::string                 m_strContent;
    std::vector<CPackGameItem>  m_stAttachment;
    std::set<int32_t>           m_stChannelVec;

};

using CGMResponseAddSystemMail = CGMResponseBody;


//MSG_ADMIN_DELETE_SYSTEM_MAIL = 9003,	//GM删除系统邮件
class CGMRequestDeleteSystemMail
{
public:
    CGMRequestDeleteSystemMail() : m_iMailID(0), m_iBeginTime(0) {}

    MSGPACK_DEFINE(m_iMailID, m_iBeginTime);

    int             m_iMailID;
    int             m_iBeginTime;
};

using CGMResponseDeleteSystemMail = CGMResponseBody;


//MSG_ADMIN_GIFT_CARD_AWARD = 9004,		//GM通知礼包卡
class CGMRequestGiftCardAward
{
public:
    MSGPACK_DEFINE(m_strCardCode, m_stItems);

    std::string m_strCardCode;
    std::vector<CPackGameItem> m_stItems;
};

using CGMResponseGiftCardAward = CGMResponseBody;


//MSG_ADMIN_SEND_ACTIVE_MAIL = 9005,		//GM发送活跃邮件
class CGMRequestSendActiveMail
{
public:
    MSGPACK_DEFINE(m_strSender,
                   m_strTitle,
                   m_strContent,
                   m_stBonusVec);

    std::string m_strSender;
    std::string m_strTitle;
    std::string m_strContent;
    std::vector<CPackGameItem> m_stBonusVec;
};

using CGMResponseSendActiveMail = CGMResponseBody;


//MSG_ADMIN_SET_USER_BAN = 9006,		    //GM封禁玩家
class CGMRequestSetUserBan
{
public:
    CGMRequestSetUserBan() : m_iBanType(0), m_iDuration(0), m_strComment() {}

    MSGPACK_DEFINE(m_iBanType, m_iDuration, m_strComment);

    int32_t		m_iBanType;
    int32_t     m_iDuration;
    std::string		m_strComment;

};

using CGMResponseSetUserBan = CGMResponseBody;

//MSG_ADMIN_MODIFY_CHAPTER = 9009,		//GM修改玩家的章节进度
class CGMRequestModifyChapter
{
public:
    MSGPACK_DEFINE(m_iChapterType,
                   m_iLevelID);

    int32_t				m_iChapterType;
    int32_t				m_iLevelID;
};

using CGMResponseModifyChapter = CGMResponseBody;

//MSG_ADMIN_MODIFY_HERO_CARD = 9010,		//GM修改玩家的武将卡牌
class CGMRequestModifyHeroCard
{
public:
    MSGPACK_DEFINE(m_iHeroID, m_iHeroLevel, m_iHeroColor, m_iHeroStar, m_iSkillLevel
    , m_iEquipLevel, m_iEquipStar, m_iEquipColor, m_iJewelLevel1, m_iJewelLevel2);

    int32_t				m_iHeroID;
    int32_t				m_iHeroLevel;
    int32_t				m_iHeroColor;
    int32_t				m_iHeroStar;
    int32_t				m_iSkillLevel;
    int32_t				m_iEquipLevel;
    int32_t				m_iEquipStar;
    int32_t				m_iEquipColor;
    int32_t				m_iJewelLevel1;
    int32_t				m_iJewelLevel2;
};

using CGMResponseModifyHeroCard = CGMResponseBody;


//MSG_ADMIN_MODIFY_CREATE_TIME = 9011,	//GM修改玩家创角时间
class CGMRequestModifyCreateTime
{
public:
    MSGPACK_DEFINE(m_iCreateTime);

    int				m_iCreateTime;
};

using CGMResponseModifyCreateTime = CGMResponseBody;



//MSG_ADMIN_MODIFY_GUIDE = 9012,		    //GM修改玩家引导
class CGMRequestModifyGuide
{
public:
    MSGPACK_DEFINE(m_iMajorStep, m_iMinorStep);

    int32_t m_iMajorStep;
    int32_t m_iMinorStep;
};

using CGMResponseModifyGuide = CGMResponseBody;

//MSG_ADMIN_DELETE_HERO = 9014,		    //GM删除伙伴
class CGMRequestDeleteHero
{
public:
    MSGPACK_DEFINE(m_iHeroID);

    int32_t m_iHeroID;
};

using CGMResponseDeleteHero = CGMResponseBody;


//MSG_ADMIN_DELETE_RANK = 9015,		    //GM删除排行
class CGMRequestDeleteRank
{
public:
    MSGPACK_DEFINE(m_strRank);

    std::string m_strRank;
};

using CGMResponseDeleteRank = CGMResponseBody;

//MSG_ADMIN_CLEAR_CACHE = 9016,		    //清除缓存
using CGMResponseClearCache = CGMResponseBody;

//MSG_ADMIN_SET_GUILD_BULLETIN = 9017,	//GM设置公会公告
class CGMRequestSetGuildBulletin
{
public:
    MSGPACK_DEFINE(m_iGuildID);

    int32_t m_iGuildID;
};

using CGMResponseSetGuildBulletin = CGMResponseBody;

//MSG_ADMIN_JUBAO_AWARD = 9018,		    //GM举报奖励
class CGMRequestJubaoAward
{
public:
    MSGPACK_DEFINE(m_strDstNick, m_strMailID);

    std::string m_strDstNick;
    std::string m_strMailID;
};

using CGMResponseJubaoAward = CGMResponseBody;

//MSG_ADMIN_USER_RANK = 9019,		        //GM玩家排行
class CGMResponseUserRank
{
public:
    MSGPACK_DEFINE(m_stPVPRankVec, m_stPowerRankVec, m_stUnlimitRankVec, m_stTowerRankVec, m_stCountdownRankVec,
                   m_stTerritoryRankVec, m_stMilitarkRankVec, m_stHomeExpRankVec, m_stEntireWarRankVec, m_stCharmRankVec, m_stMatchPvpRankVec);

    std::vector<CPackUserRank> m_stPVPRankVec;
    std::vector<CPackUserRank> m_stPowerRankVec;
    std::vector<CPackUserRank> m_stUnlimitRankVec;      // TODO
    std::vector<CPackUserRank> m_stTowerRankVec;
    std::vector<CPackUserRank> m_stCountdownRankVec;
    std::vector<CPackUserRank> m_stTerritoryRankVec;    // TODO
    std::vector<CPackUserRank> m_stMilitarkRankVec;
    std::vector<CPackUserRank> m_stHomeExpRankVec;
    std::vector<CPackUserRank> m_stEntireWarRankVec;
    std::vector<CPackUserRank> m_stCharmRankVec;
    std::vector<CPackUserRank> m_stMatchPvpRankVec;
};

//MSG_ADMIN_CROSS_DAY = 9020,		        //GM玩家跨天
using CGMResponseCrossDay = CGMResponseBody;

class CGMRequestAddFish
{
public:
    CGMRequestAddFish() : m_iFishID(0), m_iGrade(0), m_iNum(0) {}

    MSGPACK_DEFINE(m_iFishID, m_iGrade, m_iNum);

    int32_t     m_iFishID;
    int32_t     m_iGrade;
    int32_t     m_iNum;
};

class CGMRequestAddEquip
{
public:
    CGMRequestAddEquip() : m_iGrade(0), m_iEquipTag(0), m_iIndex(0) {}

    MSGPACK_DEFINE(m_iGrade, m_iEquipTag, m_iIndex);

    int32_t     m_iGrade;
    int32_t     m_iEquipTag;
    int32_t     m_iIndex;
};

class CGMRequestDeleteConstel
{
public:
    MSGPACK_DEFINE(m_iUniqueID);

    int32_t m_iUniqueID = 0;
};

class CGMRequestAddConstel
{
public:
    CGMRequestAddConstel() : m_iGrade(0), m_iEquipTag(0), m_iSuit(0), m_iLevel(0), m_iMainAttrType(0), m_iAttrType1(0), m_iAttrValue1(0),
        m_iAttrType2(0), m_iAttrValue2(0), m_iAttrType3(0), m_iAttrValue3(0), m_iAttrType4(0), m_iAttrValue4(0) {}

    MSGPACK_DEFINE(m_iGrade, m_iEquipTag, m_iSuit, m_iLevel, m_iMainAttrType, m_iAttrType1, m_iAttrValue1,
                   m_iAttrType2, m_iAttrValue2, m_iAttrType3, m_iAttrValue3, m_iAttrType4, m_iAttrValue4);

    int32_t     m_iGrade;
    int32_t     m_iEquipTag;
    int32_t     m_iSuit;
    int32_t     m_iLevel;

    int32_t     m_iMainAttrType;
    int32_t     m_iAttrType1;
    int32_t     m_iAttrValue1;
    int32_t     m_iAttrType2;
    int32_t     m_iAttrValue2;
    int32_t     m_iAttrType3;
    int32_t     m_iAttrValue3;
    int32_t     m_iAttrType4;
    int32_t     m_iAttrValue4;
};

//MSG_ADMIN_GET_CAN_DEPOSIT = 9026,       //GM请求可以充值的ID
class CGMResponseGetCanDeposit
{
public:
    MSGPACK_DEFINE(m_stDepositIDVec);

    std::vector<int32_t> m_stDepositIDVec;
};

class CGMRequestSetEntireWarPass
{
public:
    MSGPACK_DEFINE(m_iThemeID, m_iWarID, m_iCurCount);

    int32_t m_iThemeID = 0;
    int32_t m_iWarID = 0;
    int32_t m_iCurCount = 0;
};

using CGMResponseSetEntireWarPass = CGMResponseBody;

class CGMRequestCopyUser
{
  public:
    CGMRequestCopyUser() : m_iSourceGroupID(0), m_iSourceUin(0) {}

    MSGPACK_DEFINE(m_iSourceGroupID, m_iSourceUin);

    int32_t     m_iSourceGroupID;
    int32_t     m_iSourceUin;
};

class CGMRequestChangeAgeLimit
{
public:
    MSGPACK_DEFINE(iAgeLimitIndex);

    int32_t iAgeLimitIndex = 0;
};