#pragma once
#include "logic_config_base.h"
#include "server_base_protocol.h"

/////////////////////////////////////////////////////////////////////////////////
//奇遇地图结点
struct stQiyuMapNode
{
	int32_t		m_iID;
	int32_t		m_iPreID;
};

//奇遇事件结点
struct stQiyuEventNode
{
	std::string						m_strID;
	char							m_cType;
	int32_t							m_iEventID;
	int32_t							m_iCount;
	std::vector<int32_t>			m_aMapNodes;
};

//一个奇遇地图信息
struct stQiyuMapData
{
	int32_t														m_iID;
	int32_t														m_iLifeTime;
	int32_t														m_iJingjieHeroNum;
	int32_t														m_iCangbaotuID;
	std::unordered_map<int32_t, stQiyuMapNode>					m_mpMapNodes;
	std::map<int32_t, std::vector<stQiyuEventNode> >			m_mpEventNodes;	//first:level 
};

/////////////////////////////////////////////////////////////////////////////////
//基础奖励组
struct stRewardGroup
{
	//std::string												m_strRewardGroupName;
	std::vector<CPackGameItem>			m_aRewards;
	int32_t													m_iExParam;
	std::string												m_strBadRewardID;
};

//随机奖池
struct stRewardPool
{
	//std::string												m_strRewardPoolName;
	std::vector<std::pair<std::string, int32_t> >			m_aRewardGroup;
};

//等级奖池配置
struct stLevelRewardItem
{
	int32_t													m_iBeginLevel;
	int32_t													m_iEndLevel;
	std::string												m_strRewardPoolName;
};

struct stLevelRewardPool
{
	//std::string												m_strLevelRewardName;
	std::vector<stLevelRewardItem>							m_aLevelReward;
};

/////////////////////////////////////////////////////////////////////////////////
//基础交易对
struct stJiaoyiItem
{
	//std::string										m_strID;

	std::vector<CPackGameItem>  m_aSourceItemVec;
	std::vector<CPackGameItem>  m_aDesItemVec;
};

//交易池
struct stJiaoyiPool
{
	//std::string												m_strJiaoyiPoolName;
	std::vector<std::pair<std::string, int32_t> >			m_aJiaoyiGroup;
};

//等级交易池配置
struct stLevelJiaoyiItem
{
	int32_t													m_iBeginLevel;
	int32_t													m_iEndLevel;
	std::string												m_strJiaoyiPoolName;
};

struct stJiaoyiLevelPool
{
	//std::string												m_strJiaoyiLevelPoolName;
	std::vector<stLevelJiaoyiItem>							m_aLevelJiaoyi;
};
/////////////////////////////////////////////////////////////////////////////////
//基础消耗组
struct stConsumeGroup
{
	//int														m_iID;
	std::vector<CPackGameItem>			m_aConsumes;
};


//等级奖池配置
struct stLevelConsumeItem
{
	int32_t													m_iBeginLevel;
	int32_t													m_iEndLevel;
	int32_t													m_iConsumeGroupID;
};

struct stLevelConsumePool
{
	int32_t													m_iID;
	std::vector<stLevelConsumeItem>							m_aConsume;
};

/////////////////////////////////////////////////////////////////////////////////
struct stMingyun
{
	std::string									m_strLevelRewardPool1;
	//std::string									m_strLevelRewardPool2;
};

struct stMiti
{
	//int											m_iID;
	std::string									m_strLevelRewardPool;
	int											m_iBuffType;
	int											m_iBuffPercent;
};

struct stZhiyu
{
	//int											m_iID;
	int											m_iBuffType;
	int											m_iBuffPercent;
};

struct stZhanzheng
{
	//int											m_iID;
	int											m_iBuffType;
	int											m_iBuffPercent;
};

struct stJiaoyisuo
{
	//int											m_iID;
	std::vector<std::string>						m_aJiaoyiLevelPool;
};

struct stGanwuHero
{
	//int											m_iID;
	CPackGameItem			m_stConsumeItem1;
	CPackGameItem			m_stReward1;
	CPackGameItem			m_stConsumeItem2;
	CPackGameItem			m_stReward2;
};

struct stRonglian
{
	//int											m_iID;
	int											m_iRonglianValue;
	std::string									m_strLevelRewardPool;
};

struct stMibaoku
{
	//int											m_iID;
	int											m_iBox2Cash;
	int											m_iBox3Cash;
	std::string									m_strLevelRewardPool1;
	std::string									m_strLevelRewardPool2;
	std::string									m_strLevelRewardPool3;
	int											m_iCoef1;
	int											m_iCoef2;
	int											m_iCoef3;
};

struct stGuanqiaItem
{
	int											m_iCoef;
	int											m_iSex;
	bool										m_bPVEOrPVP;
};

struct stGuanqiaPool
{
	//int											m_iID;
	std::string									m_strType;
	std::unordered_map<int, stGuanqiaItem>		m_mpGuanqia;
};

struct stLevelGuanqiaItem
{
	int											m_iBeginLevel;
	int											m_iEndLevel;
	float										m_fRatio;
	int											m_iGuanqiaPoolId;
};

struct stLevelGuanqiaPool
{
	//int											m_iID;
	std::vector<stLevelGuanqiaItem>				m_aLevelGuanqia;
};

struct stQiangzhetiaozhan
{
	//int											m_iID;
	//std::string									m_strLevelRewardPool;
	int											m_iLevelGuanqiaPool;
};

struct stBaozangshouwei
{
	//int											m_iID;
	std::string									m_strLevelRewardPool;
	int											m_iLevelGuanqiaPool;
	std::vector<CPackGameItem>	m_aConsumeItem;
};

struct stMaozei
{
	int											m_iID;
	std::string									m_strLevelRewardPool1;
	std::string									m_strLevelRewardPool2;
	int											m_iLevelGuanqiaPool;
	int											m_iLevelConsumePool;
};

struct stPvpBot
{
	std::string									m_strRewardGroup;
	std::string									m_strCardName;
};

/////////////////////////////////////////////////////////////////////////////////

class CLogicConfigQiyuParser : public CLogicConfigParserBase
{
public:
	CLogicConfigQiyuParser()
		: CLogicConfigParserBase()
	{}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;
	
	int												m_iOpenLevel;
	std::unordered_map<int, std::vector<int> >		m_mpDayQiyu;

	std::unordered_map<int32_t, stQiyuMapData>		m_mpQiyuMap;

	std::unordered_map<std::string, stRewardGroup>	m_mpRewardGroup;
	std::unordered_map<std::string, stRewardPool>	m_mpRewardPool;
	std::unordered_map<std::string, stLevelRewardPool>	m_mpLevelRewardPool;

	std::unordered_map<std::string, stJiaoyiItem>	m_mpJiaoyiItem;
	std::unordered_map<std::string, stJiaoyiPool>	m_mpJiaoyiPool;
	std::unordered_map<std::string, stJiaoyiLevelPool>	m_mpJiaoyiLevelPool;

	std::unordered_map<int32_t, stConsumeGroup>		m_mpConsumeGroup;
	std::unordered_map<int, stLevelConsumePool>		m_mpLevelConsumePool;

	std::unordered_map<int, std::string>			m_mpNormalReward;

	std::unordered_map<int, std::string>			m_mpHongyun;
	std::vector<int>								m_aHongyunContent;

	std::unordered_map<int, std::string>			m_mpHideHongyun;
	std::vector<int>								m_aHideHongyunContent;

	std::unordered_map<int, stMingyun>				m_mpMingyun;
	std::set<int>									m_aMingyunContent;

	std::unordered_map<int, stMiti>					m_mpMiti;
	std::unordered_map<int, int>					m_mpMitiContent;

	std::unordered_map<int, stZhiyu>				m_mpZhiyu;
	std::unordered_map<int, stZhanzheng>			m_mpZhanzheng;

	std::unordered_map<int, std::string>			m_mpBaozang;

	std::unordered_map<int, stJiaoyisuo>			m_mpJiaoyisuo;

	std::unordered_map<int, stGanwuHero>			m_mpGanwuHero;
	std::vector<int>								m_aMaleGanwuContent;
	std::vector<int>								m_aFemaleGanwuContent;

	std::map<std::tuple<int, int>, int>	m_mpItemRonglianValue;//type,id,ronglianvalue
	std::unordered_map<int, stRonglian>				m_mpRonglian;

	std::unordered_map<int, stMibaoku>				m_mpMibaoku;

	std::unordered_map<int, stGuanqiaPool>			m_mpGuanqiaPool;
	std::unordered_map<int, stLevelGuanqiaPool>		m_mpGuanqiaLevelPool;

	std::unordered_map<int, stQiangzhetiaozhan>		m_mpQiangzhetiaozhan;
	std::vector<int>								m_aMaleQiangzheContent;
	std::vector<int>								m_aFemaleQiangzheContent;

	std::unordered_map<int, stBaozangshouwei>		m_mpBaozangshouwei;

	std::unordered_map<int, stMaozei>				m_mpMaozei;
	std::vector<int>								m_aMaozeiContent;

	std::vector<int>								m_aMubeiContent;
};

/////////////////////////////////////////////////////////////////////////////////

class CLogicConfigQiyuBotParser : public CLogicConfigParserBase
{
public:
	CLogicConfigQiyuBotParser()
		: CLogicConfigParserBase()
	{}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	std::unordered_map<int, stPvpBot>				m_mpPVPBot;
};
