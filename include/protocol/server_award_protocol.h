#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"

class CPackBankPrice
{
public:
	CPackBankPrice(int32_t iTimeStamp = 0, int32_t iWeiPrice = 0, int32_t iShuPrice = 0, int32_t iWuPrice = 0)
		: m_iTimeStamp(iTimeStamp), m_iWeiPrice(iWeiPrice), m_iShuPrice(iShuPrice), m_iWuPrice(iWuPrice) {}

	MSGPACK_DEFINE(m_iTimeStamp, m_iWeiPrice, m_iShuPrice, m_iWuPrice);

	int32_t m_iTimeStamp;	//节点的时间戳
	int32_t m_iWeiPrice;	//魏国金价
	int32_t m_iShuPrice;	//蜀国金价
	int32_t m_iWuPrice;		//吴国金价	
};

class CPackBankCountry
{
public:
	CPackBankCountry(int32_t iCountry = 0, int32_t iBuyNum = 0, int32_t iAvgPrice = 0)
		: m_iCountry(iCountry), m_iBuyNum(iBuyNum), m_iAvgPrice(iAvgPrice) {}

	MSGPACK_DEFINE(m_iCountry, m_iBuyNum, m_iAvgPrice);

	int32_t m_iCountry;			//国家 1-魏 2-蜀 3-吴
	int32_t m_iBuyNum;			//买入数量
	int32_t m_iAvgPrice;		//买入均价
};

class CPackBuyBack
{
public:
	CPackBuyBack()
		: m_iBackType(0), m_iUseBackCount(0), m_iBuyBackCount(0), m_iCanBackCount(0), m_iLeftBasicCount(0) {}

	MSGPACK_DEFINE(m_iBackType, m_iUseBackCount, m_iBuyBackCount, m_iCanBackCount, m_iLeftBasicCount);

	int32_t m_iBackType;
	int32_t m_iUseBackCount;
	int32_t m_iBuyBackCount;
	int32_t m_iCanBackCount;
	int32_t m_iLeftBasicCount;
};

//NOTIFY_USER_DAILY_DATA = 10258,				//通知玩家每日数据
class CResponseNotifyUserDailyData : public CPackUserSyncData
{
public:
	CResponseNotifyUserDailyData() : m_iTreasureOnlineIndex(0), m_iTreasureOnlineTimestamp(0), m_iTreasureLeftDigCount(0), m_iFreeTurnTableCount(0),
		m_iLeftTurnCardCount(0), m_strTurnCardAwardBitmap(), m_iTurnOpenCardAmount(0), m_iMultiColorLanternVec(), m_iLastOpenLaternVec(),
		m_strVipDailyAwardBitmap(), m_strRecommendedLineupBitmap(), m_iLoginInsureOpenTime(0), m_iHeroInsureOpenTime(0), m_iHomeInsureOpenTime(0), 
		m_iCountDownHero(0), m_iWorshipWarGodCount(0), m_iBuyActionCoinCount(0), m_iCanJoinGuildTime(0), m_iPetInsureOpenTime(0), m_iCharm(0), m_iMergeLevelCount(0),
		m_iLuckyHammerLogCount(0), m_iLoginFund1OpenTime(0), m_iLoginFund2OpenTime(0), m_iLoginFund3OpenTime(0), m_iLoginFund4OpenTime(0), m_iLoginFund5OpenTime(0),
		m_iSpeGiftWeekActID(0), m_iSpeGiftWeekLevel(0), m_iSpeGiftMonthActID(0), m_iSpeGiftMonthLevel(0), m_stCardDialogLimitAvyStat(), m_iCardDialogNextTime(0),
		m_iCardDialogTimes(0), m_iGrowFoundationOpenTime(0), m_stVipDailyTaskList(), m_iDailyQuestLiveness(0), m_strDailyQuestLivenessBitmap(),
		m_iQuarterVipAwardLevel(0), m_iHeartMapTimes(0), m_strDailyAwardState(""), m_iHeartDialogCard(0), m_stDatingPlaceTimes(), m_stSimpleGameTimes(),
		m_iHeartInsureOpenTime(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iTreasureOnlineIndex, m_iTreasureOnlineTimestamp, m_iTreasureLeftDigCount, m_iFreeTurnTableCount,
		m_iLeftTurnCardCount, m_strTurnCardAwardBitmap, m_iTurnOpenCardAmount, m_iMultiColorLanternVec, m_iLastOpenLaternVec,
		m_strVipDailyAwardBitmap, m_strRecommendedLineupBitmap, m_iLoginInsureOpenTime, m_iHeroInsureOpenTime, m_iHomeInsureOpenTime, 
		m_iCountDownHero, m_iWorshipWarGodCount, m_iBuyActionCoinCount, m_iCanJoinGuildTime, m_iPetInsureOpenTime, m_iCharm, m_iMergeLevelCount,
		m_iLuckyHammerLogCount, m_iLoginFund1OpenTime, m_iLoginFund2OpenTime, m_iLoginFund3OpenTime, m_iLoginFund4OpenTime, m_iLoginFund5OpenTime,
		m_iSpeGiftWeekActID, m_iSpeGiftWeekLevel, m_iSpeGiftMonthActID, m_iSpeGiftMonthLevel, m_stCardDialogLimitAvyStat, m_iCardDialogNextTime,
		m_iCardDialogTimes, m_iGrowFoundationOpenTime, m_stVipDailyTaskList, m_iDailyQuestLiveness, m_strDailyQuestLivenessBitmap,
		m_iQuarterVipAwardLevel, m_iHeartMapTimes, m_strDailyAwardState, m_iHeartDialogCard, m_stDatingPlaceTimes, m_stSimpleGameTimes,
		m_iHeartInsureOpenTime);

	int32_t m_iTreasureOnlineIndex;			//时光宝藏在线奖励index         // TODO
	int32_t m_iTreasureOnlineTimestamp;		//时光宝藏奖励时间戳            // TODO
	int32_t m_iTreasureLeftDigCount;		//时光宝藏剩余挖宝次数           // TODO
	int32_t m_iFreeTurnTableCount;			//免费转盘次数
	int32_t m_iLeftTurnCardCount;			//剩余翻卡次数
	std::string m_strTurnCardAwardBitmap;	//翻卡奖励领取状态
	int32_t m_iTurnOpenCardAmount;			//翻开牌的数量
	std::vector<int32_t> m_iMultiColorLanternVec;  //选择的缤纷彩灯
	std::vector<int32_t> m_iLastOpenLaternVec;		//上一期彩灯开奖
	std::string m_strVipDailyAwardBitmap;		//vip每日奖励状态
	std::string m_strRecommendedLineupBitmap;	//推荐阵容bitmap
	int32_t m_iLoginInsureOpenTime;				//登录险开启时间           // TODO
	int32_t m_iHeroInsureOpenTime;				//伙伴险开启时间           // TODO
	int32_t m_iHomeInsureOpenTime;				//家园险开启时间           // TODO
	int32_t m_iCountDownHero;				//限时神将每日免费次数
	int32_t m_iWorshipWarGodCount;			//膜拜战神次数
	int32_t m_iBuyActionCoinCount;			//已经购买行动力次数
	int32_t m_iCanJoinGuildTime;			//可加入公会时间
	int32_t m_iPetInsureOpenTime;			//灵宠险开启时间               // TODO
	int32_t m_iCharm;						//魅力值
	int32_t m_iMergeLevelCount;				//合服副本次数
	int32_t m_iLuckyHammerLogCount;			//幸运魔罐日志今日已经蹭好运次数
    int32_t m_iLoginFund1OpenTime;				//登录基金1开启时间         // TODO
    int32_t m_iLoginFund2OpenTime;				//登录基金2开启时间         // TODO
    int32_t m_iLoginFund3OpenTime;				//登录基金3开启时间         // TODO
    int32_t m_iLoginFund4OpenTime;				//登录基金4开启时间         // TODO
    int32_t m_iLoginFund5OpenTime;				//登录基金5开启时间         // TODO

    int32_t m_iSpeGiftWeekActID;                // 特权周礼活动ID
    int32_t m_iSpeGiftWeekLevel;                // 特权周礼玩家等级
    int32_t m_iSpeGiftMonthActID;               // 特权月礼活动ID
    int32_t m_iSpeGiftMonthLevel;               // 特权月礼玩家等级

    std::vector<int32_t>    m_stCardDialogLimitAvyStat;     // 小人互动，限时活动领取奖励状态
    int32_t m_iCardDialogNextTime;              // 小人互动，下一次可互动时间
    int32_t m_iCardDialogTimes;                 // 小人互动，已互动次数

    int32_t m_iGrowFoundationOpenTime;          // 成长计划开启时间
    std::set<int>   m_stVipDailyTaskList;       // VIP每日任务              // TODO
    int32_t m_iDailyQuestLiveness;              // 每日任务活跃度            // TODO
    std::string m_strDailyQuestLivenessBitmap;  // 每日任务活跃度奖励领取状态  // TODO
    int32_t m_iQuarterVipAwardLevel;            // 至尊卡每日VIP奖励领取等级
    int32_t m_iHeartMapTimes;                   // 好感度副本每日次数         // TODO
    std::string m_strDailyAwardState;           // 每日奖励标志位
    int32_t m_iHeartDialogCard;                 // 每日好感度互动伙伴
    std::map<unsigned char, unsigned char> m_stDatingPlaceTimes;    // 约会地点每日次数
    std::map<unsigned char, unsigned char> m_stSimpleGameTimes;     // 小游戏每日次数
    int32_t m_iHeartInsureOpenTime;             // 好感度基金
};

//GET_BANK_INFO = 10260,						//获取钱庄信息
class CResponseGetBankInfo : public CPackUserSyncData
{
public:
	CResponseGetBankInfo() : m_iRefreshTimeStamp(0), m_strEvent(), m_stBankPriceVec(), m_stBankCountryVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_iRefreshTimeStamp, m_strEvent, m_stBankPriceVec, m_stBankCountryVec);

	int32_t m_iRefreshTimeStamp;
	std::string m_strEvent;			//事件
	std::vector<CPackBankPrice> m_stBankPriceVec;
	std::vector<CPackBankCountry> m_stBankCountryVec;
};

//BUY_BANK_GOLD = 10261, //购买钱庄金券
class CRequestBuyBankGold
{
public:
	CRequestBuyBankGold() : m_iCountry(0), m_iPrice(0), m_iBuyNum(0) {}

	MSGPACK_DEFINE(m_iCountry, m_iPrice, m_iBuyNum);

	int32_t m_iCountry;
	int32_t m_iPrice;
	int32_t m_iBuyNum;
};

class CResponseBuyBankGold : public CPackUserSyncData
{
public:
	CResponseBuyBankGold() : m_iPrice(0), m_iBuyNum(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iPrice, m_iBuyNum, m_stBankCountry);

	int32_t m_iPrice;
	int32_t m_iBuyNum;
	CPackBankCountry m_stBankCountry;
};

//SELL_BANK_GOLD = 10262, //卖出钱庄金券
class CRequestSellBankGold
{
public:
	CRequestSellBankGold() : m_iCountry(0), m_iPrice(0), m_iSellNum(0) {}

	MSGPACK_DEFINE(m_iCountry, m_iPrice, m_iSellNum);

	int32_t m_iCountry;
	int32_t m_iPrice;
	int32_t m_iSellNum;
};

class CResponseSellBankGold : public CPackUserSyncData
{
public:
	CResponseSellBankGold() : m_iPrice(0), m_iSellNum(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iPrice, m_iSellNum, m_stBankCountry);

	int32_t m_iPrice;
	int32_t m_iSellNum;
	CPackBankCountry m_stBankCountry;
};

//GET_HAPPY_TURN_CARD = 10280,				//获取翻拍数据
class CResponseGetHappyTurnCard : public CPackUserSyncData
{
public:
	CResponseGetHappyTurnCard() : m_stItemVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_stItemVec);

	std::vector<CPackGameItem> m_stItemVec;
};

//GET_TURN_CARD_AWARD = 10281,				//获取翻派奖励
class CRequestGetTurnCardAward
{
public:
	CRequestGetTurnCardAward() : m_iCount(0) {}

	MSGPACK_DEFINE(m_iCount);

	int32_t m_iCount;
};

class CResponseGetTurnCardAward : public CPackUserSyncData
{
public:
	CResponseGetTurnCardAward() : m_iCount() {}

	MSGPACK_DEFINE(m_stSyncData, m_iCount);

	int32_t m_iCount;
	CPackGameItem m_stBonus;
};

//TURNING_HAPPY_CARD = 10282,				//翻拍
class CRequestTurningHappyCard
{
public:
	CRequestTurningHappyCard() : m_iIndex(0) {}

	MSGPACK_DEFINE(m_iIndex);

	int32_t m_iIndex;
};

class CResponseTurningHappyCard : public CPackUserSyncData
{
public:
	CResponseTurningHappyCard() : m_iIndex(0), m_bIsPair(false), m_stCard() {}

	MSGPACK_DEFINE(m_stSyncData, m_iIndex, m_bIsPair, m_stCard);

	int32_t m_iIndex;
	bool m_bIsPair;		//是否配对，配对才会获得奖励
	CPackGameItem m_stCard;
}; 

//SELECT_MULTI_COLOR_LANTERN = 10284,					//选择缤纷彩灯
class CRequestSelectMultiColorLantern
{
public:
	CRequestSelectMultiColorLantern() : m_iLanternVec() {}

	MSGPACK_DEFINE(m_iLanternVec);

	std::vector<int32_t> m_iLanternVec;
};

class CResponseSelectMultiColorLantern : public CPackUserSyncData
{
public:
	CResponseSelectMultiColorLantern() : m_iLanternVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_iLanternVec);

	std::vector<int32_t> m_iLanternVec;
};


//GET_VIP_DAILY_AWARD = 10287,				//获取VIP每日奖励
class CRequestGetVipDailyAward
{
public:
	CRequestGetVipDailyAward() : m_iVipLevel(), m_bIsQuarterAward(false) {}

	MSGPACK_DEFINE(m_iVipLevel, m_bIsQuarterAward);

	int32_t     m_iVipLevel;
	bool        m_bIsQuarterAward;          // 是否是领取至尊卡奖励 true:至尊卡奖励, false:每日祈祷奖励
};

class CResponseGetVipDailyAward : public CPackUserSyncData
{
public:
	CResponseGetVipDailyAward() : m_iVipLevel(0), m_stBonusVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_iVipLevel, m_stBonusVec);

	int32_t m_iVipLevel;
	std::vector<CPackGameItem> m_stBonusVec;
};

//NOTIFY_BUY_BACK_INFO = 10288, //通知回购信息
class CNotifyBuyBackInfo : public CPackUserSyncData
{
public:
	CNotifyBuyBackInfo() : m_stBuyBackVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_stBuyBackVec);

	std::vector<CPackBuyBack> m_stBuyBackVec;
};

//GET_BUY_BACK_COUNT = 10289, //购买回购次数
class CRequestGetBuyBackCount
{
public:
	CRequestGetBuyBackCount() : m_iBuyBackType(0), m_iBuyBackCount(0) {}

	MSGPACK_DEFINE(m_iBuyBackType, m_iBuyBackCount);

	int32_t m_iBuyBackType;
	int32_t m_iBuyBackCount;
};

class CResponseGetBuyBackCount : public CPackUserSyncData
{
public:
	CResponseGetBuyBackCount() : m_iBuyBackType(0), m_iBuyBackCount(0), m_stConsumeVec(), m_stBonusVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_iBuyBackType, m_iBuyBackCount, m_stConsumeVec, m_stBonusVec);

	int32_t m_iBuyBackType;
	int32_t m_iBuyBackCount;
	std::vector<CPackGameItem> m_stConsumeVec;
	std::vector<CPackGameItem> m_stBonusVec;
};

//GET_RECOMMENDED_LINEUP_AWARD = 10290,		//获取推荐阵容奖励
class CRequestGetRecommendedLineupAward
{
public:
	CRequestGetRecommendedLineupAward() : m_iIndex(0) {}

	MSGPACK_DEFINE(m_iIndex);

	int32_t m_iIndex;
};

class CResponseGetRecommendedLineupAward : public CPackUserSyncData
{
public:
	CResponseGetRecommendedLineupAward() : m_iIndex(0), m_stBonusVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_iIndex, m_stBonusVec);

	int32_t m_iIndex;
	std::vector<CPackGameItem> m_stBonusVec;
};


class CResponseGetFirstDepositBonus : public CPackUserSyncData
{
public:
	CResponseGetFirstDepositBonus() : m_stBonusVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_stBonusVec);

	std::vector<CPackGameItem>  m_stBonusVec;
};

class CResponseGetAddUpDepositBonus : public CPackUserSyncData
{
public:
    CResponseGetAddUpDepositBonus() : m_stBonusVec() {}

    MSGPACK_DEFINE(m_stSyncData, m_stBonusVec);

    std::vector<CPackGameItem>  m_stBonusVec;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
class CPackRedBagReceiver
{
public:
	MSGPACK_DEFINE(m_iUid, m_iGroupID, m_strNick, m_iAvatarID, m_iAvatarBorder, m_stReward, m_bIsBest);

	int32_t         m_iUid = 0;
	int32_t         m_iGroupID = 0;
	std::string     m_strNick;
	int32_t         m_iAvatarID = 0;
	int32_t         m_iAvatarBorder = 0;
	CPackGameItem   m_stReward = {};
    bool            m_bIsBest = false;
};

class CPackRedBagBrief
{
public:
	MSGPACK_DEFINE(m_iRedBagID, m_iUid, m_iGroupID, m_strNick, m_iAvatarID, m_iAvatarBorder, m_iTalkChannel, m_iTotalYuanbao, m_iTotalCount, m_iRedBagItemID);

	int32_t m_iRedBagID = 0;
	int32_t m_iUid = 0;
	int32_t m_iGroupID = 0;
	std::string m_strNick;
	int32_t m_iAvatarID = 0;
	int32_t m_iAvatarBorder = 0;
	int32_t m_iTalkChannel = 0;
	int32_t m_iTotalYuanbao = 0;
	int32_t m_iTotalCount = 0;
	int32_t m_iRedBagItemID = 0;
};

//SEND_RED_BAG = 10363,						//发红包
class CRequestSendRedBag
{
public:
	MSGPACK_DEFINE(m_iRedBagItemID);

	int32_t m_iRedBagItemID = 0;
};

class CResponseSendRedBag : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_stConsumeItem);

	CPackGameItem m_stConsumeItem;
};

//RECEIVE_RED_BAG = 10364,					//领红包
class CRequestReceiveRedBag
{
public:
	MSGPACK_DEFINE(m_iRedBagID);

	int32_t m_iRedBagID = 0;
};

class CResponseReceiveRedBag : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_stReward, m_stRedBagInfo, m_stReceiverVec, m_bIsBest);

	CPackGameItem       m_stReward;
	CPackRedBagBrief    m_stRedBagInfo;
	std::vector<CPackRedBagReceiver> m_stReceiverVec;
    bool                m_bIsBest;          // 是否大奖
};

//GET_RED_BAG_LIST = 10365,					//获取红包列表
class CResponseGetRedBagList : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_stRedBagVec, m_stRedBagEarnings, m_stWorldQuestRedBag);

	std::vector<CPackRedBagBrief>   m_stRedBagVec;
    std::vector<CPackGameItem>      m_stRedBagEarnings;     // 每日红包收益
    std::vector<int32_t>            m_stWorldQuestRedBag;   // 已发放的世界任务红包，任务ID列表
};

//CHECK_RED_BAG_VALID = 10366,				//检查红包是否有效
class CRequestCheckRedBagValid
{
public:
	MSGPACK_DEFINE(m_iRedBagID);

	int32_t m_iRedBagID = 0;
};

class CResponseCheckRedBagValid : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iRedBagID, m_iStatus, m_stRedBagInfo);

	int32_t m_iRedBagID = 0;
	int32_t m_iStatus = 0;  //1不存在  2已领完  3过期
	CPackRedBagBrief m_stRedBagInfo;
};

//MSG_LOGIC_SYS_OTHER_BUY = 10329,          // 其他通用购买功能(豪华大餐补吃，还原功能道具快速购买)
class CRequestSysOtherBuy
{
public:
  MSGPACK_DEFINE(m_iType, m_iPara1, m_iPara2);

  int32_t   m_iType;
  int32_t   m_iPara1;
  int32_t   m_iPara2;
};

class CResponseSysOtherBuy : public CPackUserSyncData
{
public:
  MSGPACK_DEFINE(m_stSyncData, m_iType, m_iPara1, m_iPara2, m_stConsumeVec, m_stBonusVec);

  int32_t   m_iType;
  int32_t   m_iPara1;
  int32_t   m_iPara2;
  std::vector<CPackGameItem> m_stConsumeVec;
  std::vector<CPackGameItem> m_stBonusVec;
};

////////////////////////////////////////////////////////////////////////////////////////
// 小人互动
class CRequestCardDialog
{
public:
    CRequestCardDialog() : m_iCardID(0), m_iOption(0), m_bIsLimitAvy(false) {}

    MSGPACK_DEFINE(m_iCardID, m_iOption, m_bIsLimitAvy);

    int32_t     m_iCardID;      // 互动的伙伴ID
    int32_t     m_iOption;      // 对话选项
    bool        m_bIsLimitAvy;  // true:限时活动类伙伴互动, false:普通互动
};

class CResponseCardDialog : public CPackUserSyncData
{
public:
    CResponseCardDialog() : m_iCardDialogNextTime(0), m_iCardDialogTimes(0), m_stCardDialogLimitAvyStat(), m_stBonusVec() {}

    MSGPACK_DEFINE(m_stSyncData, m_iCardDialogNextTime, m_iCardDialogTimes, m_stCardDialogLimitAvyStat, m_stBonusVec);

    int32_t                     m_iCardDialogNextTime;          // 小人互动，下一次可互动时间
    int32_t                     m_iCardDialogTimes;             // 小人互动，已互动次数
    std::vector<int32_t>        m_stCardDialogLimitAvyStat;     // 小人互动，限时活动领取奖励状态
    std::vector<CPackGameItem>  m_stBonusVec;                   // 奖励列表
};

class CResponseDailySign : public CPackUserSyncData
{
public:
    CResponseDailySign() : m_stDailySignItem(), m_stTotalSignItems(), m_stItemData(), m_iFlag(0),m_bSignSuccess(false),m_stAtlasSignItems() {}

    MSGPACK_DEFINE(m_stSyncData, m_stDailySignItem, m_stTotalSignItems, m_stItemData, m_iFlag,m_bSignSuccess,m_stAtlasSignItems);

    CPackGameItem       m_stDailySignItem;              // 每日签到奖励
    std::vector<CPackGameItem>  m_stTotalSignItems;     // 累计签到奖励
    CPackTotalItemData  m_stItemData;
    int32_t             m_iFlag;
    bool                m_bSignSuccess;
    CPackGameItem       m_stAtlasSignItems;             // 全图鉴签到奖励
};

// 修改充值年龄限制
class CRequestChangeAgeLimit
{
public:
    CRequestChangeAgeLimit() : m_iAgeLimitIndex(0) {}

    MSGPACK_DEFINE(m_iAgeLimitIndex);

    int32_t m_iAgeLimitIndex;
};

class CResponseChangeAgeLimit : public CPackUserSyncData
{
public:
    CResponseChangeAgeLimit() : m_iAgeLimitIndex(){}

    MSGPACK_DEFINE(m_stSyncData, m_iAgeLimitIndex);

    int32_t m_iAgeLimitIndex;
};