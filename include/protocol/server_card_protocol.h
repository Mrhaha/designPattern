#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"

////////////////////////////////////////////////////////////////////////////////////////
//卡牌操作通用请求数据
class CRequestHeroCardOp
{
public:
  CRequestHeroCardOp() : m_iHeroCardID(0), m_iCardOpCount(0) {};

	MSGPACK_DEFINE(m_iHeroCardID, m_iCardOpCount);

	int             m_iHeroCardID;
	int             m_iCardOpCount;
};

////////////////////////////////////////////////////////////////////////////////////////
//卡牌从新变旧
class CResponseSetHeroCardOld : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iHeroCardID);

	int             m_iHeroCardID;
};

//////////////////////////////////////////////////////////////////////////////////////////
//吃经验卡升级英雄卡
class CRequestUpgradeHeroCardLevel
{
public:
	MSGPACK_DEFINE(m_iHeroCardID, m_stExpCardMap);

	int                     m_iHeroCardID;
	std::map<int, int>      m_stExpCardMap;
};

class CResponseUpgradeHeroCardLevel : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iHeroCardID, m_stExpCardMap, m_iHeroCardLevel, m_iHeroCardExp);

	int                 m_iHeroCardID;
	std::map<int, int>  m_stExpCardMap;
	int                 m_iHeroCardLevel;
	int                 m_iHeroCardExp;
};

/////////////////////////////////////////////////////////////////////////////////////////////
class CResponseUpgradeHeroCardColor : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iHeroCardID, m_iHeroCardColor, m_iHeroTalentPoint,m_iHeroTalentPoint1);

	int                 m_iHeroCardID;
	int                 m_iHeroCardColor;
    int                 m_iHeroTalentPoint;
    int                 m_iHeroTalentPoint1;
};

///////////////////////////////////////////////////////////////////////////////////////////////
//英雄卡升星
//request protocol:CRequestHeroCardOp

class CResponseUpgradeHeroCardStar : public CPackUserSyncData
{
public:
	CResponseUpgradeHeroCardStar() : m_iHeroCardID(0), m_iHeroCardStar(0), m_stCacheItem() {}

	MSGPACK_DEFINE(m_stSyncData, m_iHeroCardID, m_iHeroCardStar, m_stCacheItem);

	int                 m_iHeroCardID;
	int                 m_iHeroCardStar;
    CPackTotalItemData  m_stCacheItem;
};

///////////////////////////////////////////////////////////////////////////////////////////////
//英雄卡吃进阶材料

class CRequestUpgradeHeroCardColorMaterial
{
public:
	CRequestUpgradeHeroCardColorMaterial() : m_iCardID(0), m_stPosVec() {}

	MSGPACK_DEFINE(m_iCardID, m_stPosVec);

	int                 m_iCardID;
	std::vector<int>    m_stPosVec;
};

class CResponseUpgradeHeroCardColorMaterial : public CPackUserSyncData
{
public:
	CResponseUpgradeHeroCardColorMaterial() : m_iCardID(0), m_stPos(), m_stConsumeItemVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_iCardID, m_stPos, m_stConsumeItemVec);

	int                 m_iCardID;
	std::vector<int>    m_stPos;
	std::vector<CPackGameItem> m_stConsumeItemVec;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
class CRequestUpdateFightPower
{
public:
	CRequestUpdateFightPower()
		: m_iFightPower1(0), m_iFightPower2(0)
		, m_iPVPAttackFightPower1(0), m_iPVPAttackFightPower2(0) {}

	MSGPACK_DEFINE(m_iFightPower1, m_iFightPower2, m_iPVPAttackFightPower1, m_iPVPAttackFightPower2);

	int         m_iFightPower1;
	int         m_iFightPower2;
	int         m_iPVPAttackFightPower1;
	int         m_iPVPAttackFightPower2;
};

//////////////////////////////////////////////////////////////////////////////////////////
//少女好感度赠送礼物升级
class CRequestHeroHeartGifts
{
public:
    MSGPACK_DEFINE(m_iHeroCardID, m_stGiftsMap);

    int                     m_iHeroCardID;
    std::map<int, int>      m_stGiftsMap;           // 礼物ID与数量列表
};

class CResponseHeroHeartGifts : public CPackUserSyncData
{
public:
    MSGPACK_DEFINE(m_stSyncData, m_iHeroCardID, m_stGiftsMap, m_iHeartLevel, m_iHeartExp, m_iDailyGiftNum
                 , m_iLastGiftTime, m_iTotalHeartLevel, m_iTotalHeartExp, m_stReward, m_stExpMap);

    int32_t                 m_iHeroCardID;
    std::map<int32_t, int32_t>   m_stGiftsMap;
    int32_t                 m_iHeartLevel;
    int32_t                 m_iHeartExp;
    int32_t                 m_iDailyGiftNum;
    int32_t                 m_iLastGiftTime;
    int32_t                 m_iTotalHeartLevel;         // 总好感度等级
    int32_t                 m_iTotalHeartExp;           // 总好感度当前经验
    std::vector<CPackGameItem>  m_stReward;             // 伙伴好感度升级奖励
    std::map<int32_t, int32_t>  m_stExpMap;             // 经验列表
};

class CRequestUpgradeCardTalent
{
public:
    CRequestUpgradeCardTalent() : m_iCardID(0), m_iTalentID(0), m_iSelfPointNum(0), m_iSelfPointNum1(0),m_iCommonPointNum(0) {}

    MSGPACK_DEFINE(m_iCardID, m_iTalentID, m_iSelfPointNum, m_iSelfPointNum1,m_iCommonPointNum);

    int32_t     m_iCardID;
    int32_t     m_iTalentID;
    int32_t     m_iSelfPointNum;            // 消耗专用点数
    int32_t     m_iSelfPointNum1;           // 消耗专用点数一
    int32_t     m_iCommonPointNum;          // 消耗通用点数
};

class CResponseUpgradeCardTalent: public CPackUserSyncData
{
public:
    CResponseUpgradeCardTalent() : m_iCardID(0), m_iTalentID(0), m_iSelfPointNum(0),m_iSelfPointNum1(0) ,m_iCommonPointNum(0) {}

    MSGPACK_DEFINE(m_stSyncData,m_iCardID, m_iTalentID, m_iSelfPointNum, m_iSelfPointNum1,m_iCommonPointNum);

    int32_t     m_iCardID;
    int32_t     m_iTalentID;
    int32_t     m_iSelfPointNum;            // 消耗专用点数
    int32_t     m_iSelfPointNum1;           // 消耗专用点数一
    int32_t     m_iCommonPointNum;          // 消耗通用点数
};



// 星纹
class CRequestConstelCommonOpt
{
  public:
    CRequestConstelCommonOpt() : m_stUniqueIDs(), m_bDiv(false) {}

    MSGPACK_DEFINE(m_stUniqueIDs, m_bDiv);

    std::vector<int32_t>        m_stUniqueIDs;
    bool                        m_bDiv;             // true:分解，false:吞噬
};

class CResponseConstelDiv : public CPackUserSyncData
{
  public:
    CResponseConstelDiv() : m_stUniqueIDs(), m_stItemData() {}

    MSGPACK_DEFINE(m_stSyncData, m_stUniqueIDs, m_stItemData);

    std::vector<int32_t>        m_stUniqueIDs;
    CPackTotalItemData          m_stItemData;
};

class CRequestConstelLevelUp
{
public:
    CRequestConstelLevelUp() : m_iLevelUpUniqueID(0), m_iConsumeExpPool(0), m_stExpUniqueIDs() {}

    MSGPACK_DEFINE(m_iLevelUpUniqueID, m_iConsumeExpPool, m_stExpUniqueIDs);

    int32_t         m_iLevelUpUniqueID;
    int32_t         m_iConsumeExpPool;
    std::vector<int32_t>    m_stExpUniqueIDs;
};

class CResponseConstelLevelUp : public CPackUserSyncData
{
public:
    CResponseConstelLevelUp() : m_stLevelUpConstel(), m_stExpUniqueIDs(), m_iExpPool(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_stLevelUpConstel, m_stExpUniqueIDs, m_iExpPool);

    CPackConstellationInfo  m_stLevelUpConstel;
    std::vector<int32_t>    m_stExpUniqueIDs;
    int32_t                 m_iExpPool;
};

class CRequestConstelEquip
{
  public:
    CRequestConstelEquip() : m_iCardID(0), m_iConstelID(0) {}

    MSGPACK_DEFINE(m_iCardID, m_iConstelID);

    int32_t     m_iCardID;          // 操作的伙伴ID，有值表示装备，无值表示卸下
    int32_t     m_iConstelID;
};

class CResponseConstelEquip : public CPackUserSyncData
{
  public:
    CResponseConstelEquip() : m_iCardID(0), m_iConstelID(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iCardID, m_iConstelID);

    int32_t     m_iCardID;                  // 操作的伙伴ID，有值表示装备，无值表示卸下
    int32_t     m_iConstelID;               // 操作的星纹ID
};

class CRequestConstelOnekeyUnload
{
  public:
    CRequestConstelOnekeyUnload() : m_iCardID(0) {}

    MSGPACK_DEFINE(m_iCardID);

    int32_t     m_iCardID;
};

class CResponseConstelOnekeyUnload : public CPackUserSyncData
{
  public:
    CResponseConstelOnekeyUnload() : m_iCardID(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iCardID);

    int32_t     m_iCardID;
};

class CRequestConstelLockOrDiscard
{
  public:
    CRequestConstelLockOrDiscard() : m_iUniqueID(0), m_bIsLock(false), m_bIsSign(false) {}

    MSGPACK_DEFINE(m_iUniqueID, m_bIsLock, m_bIsSign);

    int32_t     m_iUniqueID;
    bool        m_bIsLock;              // true 加锁，false 弃置
    bool        m_bIsSign;              // true 标记，false 取消标记
};

class CResponseConstelLockOrDiscard : public CPackUserSyncData
{
  public:
    CResponseConstelLockOrDiscard() : m_iUniqueID(0), m_bIsLock(false), m_bIsSign(false) {}

    MSGPACK_DEFINE(m_stSyncData, m_iUniqueID, m_bIsLock, m_bIsSign);

    int32_t     m_iUniqueID;
    bool        m_bIsLock;
    bool        m_bIsSign;
};

class CRequestConstelPresetSave
{
public:
    CRequestConstelPresetSave() : m_iPresetID(0), m_strName(""), m_stConstelID() {}

    MSGPACK_DEFINE(m_iPresetID, m_strName, m_stConstelID);

    int32_t         m_iPresetID;        // 0 表示新增
    std::string     m_strName;
    std::vector<int32_t> m_stConstelID;
};

class CResponseConstelPresetSave : public CPackUserSyncData
{
public:
    CResponseConstelPresetSave() : m_stPresetInfo() {}

    MSGPACK_DEFINE(m_stSyncData, m_stPresetInfo);

    CPackConstelPresetInfo m_stPresetInfo;
};

class CRequestConstelPresetOp
{
public:
    CRequestConstelPresetOp() : m_iPresetID(0), m_iCardID(0) {}

    MSGPACK_DEFINE(m_iPresetID, m_iCardID);

    int32_t     m_iPresetID;
    int32_t     m_iCardID;
};

class CResponseConstelPresetOp : public CPackUserSyncData
{
public:
    CResponseConstelPresetOp() : m_iPresetID(0), m_iCardID(0), m_stEquipConstel() {}

    MSGPACK_DEFINE(m_stSyncData, m_iPresetID, m_iCardID, m_stEquipConstel);

    int32_t     m_iPresetID;
    int32_t     m_iCardID;
    std::map<int, int> m_stEquipConstel;    // 成功装备的礼装部位和ID  [装备位，礼装ID]
};

class CResponseConstelPresetList : public CPackUserSyncData
{
public:
    CResponseConstelPresetList() : m_stPresetList() {}

    MSGPACK_DEFINE(m_stSyncData, m_stPresetList);

    std::vector<CPackConstelPresetInfo> m_stPresetList;
};

class CRequestEquipmentEquip
{
public:
    CRequestEquipmentEquip() : m_iCardID(0), m_iEquipTagIndex(0), m_iEquipUniqueID(0) {}

    MSGPACK_DEFINE(m_iCardID, m_iEquipTagIndex, m_iEquipUniqueID);

    int32_t     m_iCardID;
    int32_t     m_iEquipTagIndex;
    int32_t     m_iEquipUniqueID;       // 0 表示卸下
};

class CResponseEquipmentEquip : public CPackUserSyncData
{
public:
    CResponseEquipmentEquip() : m_iCardID(0), m_iEquipTagIndex(0), m_iEquipUniqueID(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iCardID, m_iEquipTagIndex, m_iEquipUniqueID);

    int32_t     m_iCardID;
    int32_t     m_iEquipTagIndex;
    int32_t     m_iEquipUniqueID;
};

class CRequestEquipmentDiv
{
public:
    CRequestEquipmentDiv() : m_stUniqueIDs() {}

    MSGPACK_DEFINE(m_stUniqueIDs);

    std::vector<int32_t>        m_stUniqueIDs;
};

class CResponseEquipmentDiv : public CPackUserSyncData
{
public:
    CResponseEquipmentDiv() : m_stUniqueIDs(), m_iRewardGold(0), m_stRewardItem() {}

    MSGPACK_DEFINE(m_stSyncData, m_stUniqueIDs, m_iRewardGold, m_stRewardItem);

    std::vector<int32_t>        m_stUniqueIDs;
    int32_t                     m_iRewardGold;              // 奖励金币
    CPackGameItem               m_stRewardItem;             // 奖励代币
};

class CRequestEquipmentLock
{
public:
    CRequestEquipmentLock() : m_iUniqueID(0), m_bIsLock(false) {}

    MSGPACK_DEFINE(m_iUniqueID, m_bIsLock);

    int32_t     m_iUniqueID;
    bool        m_bIsLock;              // true 加锁
};

class CResponseEquipmentLock : public CPackUserSyncData
{
public:
    CResponseEquipmentLock() : m_iUniqueID(0), m_bIsLock(false) {}

    MSGPACK_DEFINE(m_stSyncData, m_iUniqueID, m_bIsLock);

    int32_t     m_iUniqueID;
    bool        m_bIsLock;
};

class CRequestCardSoulExchange
{
public:
    CRequestCardSoulExchange() : m_iCardID(0), m_iSoulNum(0) {}

    MSGPACK_DEFINE(m_iCardID, m_iSoulNum);

    int32_t     m_iCardID;
    int32_t     m_iSoulNum;
};

class CRequestEquipmentAddExp
{
public:
    CRequestEquipmentAddExp() : m_iUniqueID(0), m_stConsumeItem() {}

    MSGPACK_DEFINE(m_iUniqueID, m_stConsumeItem);

    int32_t     m_iUniqueID;
    std::vector<CPackGameItem> m_stConsumeItem;
};

class CResponseEquipmentAddExp : public CPackUserSyncData
{
public:
    CResponseEquipmentAddExp() : m_iUniqueID(0),m_iTotalExp(0), m_stConsumeItem() {}

    MSGPACK_DEFINE(m_stSyncData,m_iTotalExp, m_stConsumeItem);

    int32_t     m_iUniqueID;
    int32_t     m_iTotalExp;
    CPackTotalItemData m_stConsumeItem;

};

class CRequestEquipmentUpgrade
{
public:
    CRequestEquipmentUpgrade() : m_iUniqueID(0),m_iUpgradeType(0), m_iUpgradeID(0) {}

    MSGPACK_DEFINE(m_iUniqueID,m_iUpgradeType, m_iUpgradeID);

    int32_t     m_iUniqueID;
    int32_t     m_iUpgradeType;
    int32_t     m_iUpgradeID;
};

class CResponseEquipmentUpgrade : public CPackUserSyncData
{
public:
    CResponseEquipmentUpgrade() : m_iUniqueID(0), m_iUpgradeID(0),m_iTotalItem() {}

    MSGPACK_DEFINE(m_stSyncData, m_iUpgradeID,m_iTotalItem);

    int32_t     m_iUniqueID;
    int32_t     m_iUpgradeID;
    CPackTotalItemData m_iTotalItem;
};

class CRequestEquipmentRefine
{
public:
    CRequestEquipmentRefine() : m_iUniqueID(0), m_iRefineItem() {}

    MSGPACK_DEFINE(m_iUniqueID, m_iRefineItem);

    int32_t     m_iUniqueID;
    std::vector<CPackGameItem> m_iRefineItem;
};

class CResponseEquipmentRefine : public CPackUserSyncData
{
public:
    CResponseEquipmentRefine() : m_iUniqueID(0),m_iAttrIndex(0),m_iCostItem() {}

    MSGPACK_DEFINE(m_stSyncData,m_iUniqueID,m_iAttrIndex,m_iCostItem);

    int32_t     m_iUniqueID;
    int32_t     m_iAttrIndex;
    CPackTotalItemData m_iCostItem;
};


class CRequestEquipmentRefineOp
{
public:
    CRequestEquipmentRefineOp() : m_iUniqueID(0), m_bChoose() {}

    MSGPACK_DEFINE(m_iUniqueID, m_bChoose);

    int32_t     m_iUniqueID;
    bool        m_bChoose;
};

class CResponseEquipmentRefineOp : public CPackUserSyncData
{
public:
    CResponseEquipmentRefineOp() : m_iUniqueID(0),m_iAttrIndex(0) {}

    MSGPACK_DEFINE(m_stSyncData,m_iUniqueID,m_iAttrIndex);

    int32_t     m_iUniqueID;
    int32_t     m_iAttrIndex;
};