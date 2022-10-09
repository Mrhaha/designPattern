#pragma once
#include "Markup.h"
#include "server_base_protocol.h"
#include "common_fmtstring.h"
#include "common_datetime.h"

struct TLogicCardbagInfoElem
{
	TLogicCardbagInfoElem() : m_iWeight(0), m_iDiscount(0) {}
	int             m_iWeight;
	int             m_iDiscount;

	CPackGameItem   m_stCommodityItem;
	CPackGameItem   m_stConsumeItem;
};

struct TLogicCardbagRateElem
{
	int             m_iRate;
	std::string     m_strCardBag;
};
typedef std::vector<TLogicCardbagRateElem>  LiLogicCardbagRateVector;

struct TLogicMailElem
{
	std::string m_strSender;
	std::string m_strTitle;
	CFormatString m_stContent;
	std::vector<CPackGameItem> m_astItem;
};

//活动时间信息
struct TLogicActivityRefElem
{
	TLogicActivityRefElem()
			: m_iTimeType(0), m_iStartDay(0), m_iEndDay(0), m_iServerDay(0), m_iCreateDay(0), m_iStartTime(0), m_iEndTime(0), m_iType(0), m_iLevel(0), m_iCustomCreateTime(0) {}

	int					m_iTimeType;
	int                 m_iStartDay;
	int                 m_iEndDay;
	int					m_iServerDay;
	int                 m_iCreateDay;
	int                 m_iStartTime;
	int                 m_iEndTime;
	int                 m_iType;
	int					m_iLevel;
	int					m_iCustomCreateTime;
};

struct TLogicRandBonusItem
{
    TLogicRandBonusItem() : m_iOrder(0), m_iItemType(0), m_iCardID(0), m_iWeight(0), m_iNum(0), m_iNumMax(0), m_iNotify(0), m_bIsBigPrize(false) {}

    int				m_iOrder;
    int             m_iItemType;
    int             m_iCardID;
    int             m_iWeight;
    int             m_iNum;
    int             m_iNumMax;          // 如果有值，表示数量随机
    int				m_iNotify;
    bool            m_bIsBigPrize;      // 标记当前奖品是否是大奖，用于红包系统或其他
};

// 权重概率
struct TLogicWeightElem
{
    TLogicWeightElem() : m_stWeight(), m_iTotalWeight(0) {}

    std::map<int32_t, int32_t> m_stWeight;      // [id, weight]
    int32_t     m_iTotalWeight;
};

// 活动开启时间
struct TLogicAvyOpeningElem
{
    TLogicAvyOpeningElem() : m_iOpeningType(0), m_iOpeningPara1(0), m_iOpeningPara2(0), m_iCustomCreateTime(0) {}

    int             m_iOpeningType;
    int             m_iOpeningPara1;
    int             m_iOpeningPara2;
    int			    m_iCustomCreateTime;
};

class CLogicConfigCommonFunc
{
public:
    static bool ParserAvyOpeningElem(const CMarkup& stXML, TLogicAvyOpeningElem& stElem);
};

class CLogicConfigUtil
{
public:
    static void CombineGameItem(std::vector<CPackGameItem>& stItemList, const CPackGameItem& stItem);
};
