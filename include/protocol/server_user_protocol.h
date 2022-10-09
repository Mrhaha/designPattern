#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"
#include "server_common_protocol.h"
#include "server_role_protocol.h"

class CPackUserBaseInfo
{
public:
	MSGPACK_DEFINE(m_iUin, m_iGroupID, m_bIsMale, m_strNick, m_iVipLevel, m_iMajorGuideStep, m_iMinorGuideStep,
		m_iLevel, m_iExp, m_ulSpecialGuideStep, m_iAddupRmb, m_iLastLoginTime,
		m_iCreateRoleTime, m_iCurrentFightLevelID, m_iLastGetScoreRankBonusTime,
		m_iLastGetCombatRankBonusTime, m_iConstelExtraTimes, m_iUnlimitLevelLastPlayTime,
		m_iUnlimitLevelMaxScore, m_iCompleteTaskNum, m_strNormalChapterBonusBitmap,
		m_strEliteChapterBonusBitmap, m_strDevilChapterBonusBitmap, m_strEventChapterBonusBitmap,
		m_iHufuStoreLastRefreshTime, m_iHufuStoreRefreshTimes, m_iDailySignPos,
		m_iUnlimitLevelTotalScoreToday, m_strUnlimitLevelPhaseBonus,
		m_iUnReadMailAmount, m_bCanSign, m_iVipExp, m_iUnlimitLevelMaxPass, m_strUnlimitLevelMaxPhaseBonus,
		m_iChangeNickCount, m_iRuneCardID, m_iLastSignTime, m_iTotalSignTimes,
        m_strTotalSignStat, m_iOrnamentHallExpD, m_strOrnamentHallAwardStat, m_iTotalHeartLevel, m_iTotalHeartExp,
        m_iThemeSkinID);

	CPackUserBaseInfo()
		: m_iUin(0), m_iGroupID(0), m_strNick(), m_iVipLevel(0), m_iMajorGuideStep(0),
		m_iMinorGuideStep(0), m_iLevel(0), m_iExp(0), m_ulSpecialGuideStep(0), m_iAddupRmb(0),
		m_iLastLoginTime(0), m_iCreateRoleTime(0), m_iCurrentFightLevelID(0),
		m_iLastGetScoreRankBonusTime(0), m_iLastGetCombatRankBonusTime(0), m_iConstelExtraTimes(0),
		m_iUnlimitLevelLastPlayTime(0), m_iUnlimitLevelMaxScore(0), m_iCompleteTaskNum(0),
		m_strNormalChapterBonusBitmap(), m_strEliteChapterBonusBitmap(), m_strDevilChapterBonusBitmap(),
		m_strEventChapterBonusBitmap(), m_iHufuStoreLastRefreshTime(0), m_iHufuStoreRefreshTimes(0),
		m_iDailySignPos(0), m_iUnlimitLevelTotalScoreToday(0), m_strUnlimitLevelPhaseBonus(),
		m_iUnReadMailAmount(0), m_bCanSign(false), m_iVipExp(0), m_iUnlimitLevelMaxPass(0),
		m_strUnlimitLevelMaxPhaseBonus(), m_iChangeNickCount(0), m_iRuneCardID(0),
        m_iLastSignTime(0), m_iTotalSignTimes(0), m_strTotalSignStat(""), m_iOrnamentHallExpD(0),
        m_strOrnamentHallAwardStat(), m_iTotalHeartLevel(0), m_iTotalHeartExp(0), m_iThemeSkinID(0) {}

	int             m_iUin;
	int             m_iGroupID;
	bool            m_bIsMale;                      // TODO
	std::string     m_strNick;
	int             m_iVipLevel;
	int             m_iMajorGuideStep;
	int             m_iMinorGuideStep;
	int             m_iLevel;
	int             m_iExp;
    uint64_t        m_ulSpecialGuideStep;
	int             m_iAddupRmb;
	int             m_iLastLoginTime;
	int             m_iCreateRoleTime;
	int             m_iCurrentFightLevelID;
	int             m_iLastGetScoreRankBonusTime;
	int             m_iLastGetCombatRankBonusTime;
	int             m_iConstelExtraTimes;               // 念写卡每日高额奖励次数
	int             m_iUnlimitLevelLastPlayTime;        // TODO
	int             m_iUnlimitLevelMaxScore;            // TODO
	int             m_iCompleteTaskNum;
	std::string     m_strNormalChapterBonusBitmap;      // TODO
	std::string     m_strEliteChapterBonusBitmap;       // TODO
	std::string     m_strDevilChapterBonusBitmap;       // TODO
	std::string     m_strEventChapterBonusBitmap;       // TODO
	int             m_iHufuStoreLastRefreshTime;        // TODO
	int             m_iHufuStoreRefreshTimes;           // TODO
	int             m_iDailySignPos;                    // TODO
	int             m_iUnlimitLevelTotalScoreToday;     // TODO
	std::string     m_strUnlimitLevelPhaseBonus;        // TODO
	int             m_iUnReadMailAmount;
	bool            m_bCanSign;                         // TODO
	int             m_iVipExp;
	int32_t			m_iUnlimitLevelMaxPass;             // TODO
	std::string     m_strUnlimitLevelMaxPhaseBonus;     // TODO
	int32_t         m_iChangeNickCount;
	int32_t         m_iRuneCardID;
    int32_t         m_iLastSignTime;
    int32_t         m_iTotalSignTimes;
    std::string     m_strTotalSignStat;                 // TODO
    int32_t         m_iOrnamentHallExpD;                // TODO
    std::string     m_strOrnamentHallAwardStat;         // TODO
    int32_t         m_iTotalHeartLevel;
    int32_t         m_iTotalHeartExp;
    int32_t         m_iThemeSkinID;
};

class CPackCurrency
{
public:
	MSGPACK_DEFINE(m_iYuanBao, m_iGold, m_iZhuHaiScore, m_iEvilBrand, m_iPayYuanBao,
        m_iExploreExp, m_iEnergy, m_iLastAutoAddEnergyTime, m_iEnergyStore,
		m_iLastAutoAddSkillPointTime, m_iAwakeStone, m_iCrossFight,
		m_iSeal, m_iVariableSoul, m_iVitality, m_iAddVitalityTime, m_iGuildCoin, m_iTacticsCoin,
		m_iEntireWarContribution, m_iEntireWarMilitarkLevel);

	CPackCurrency()
		: m_iYuanBao(0), m_iGold(0), m_iZhuHaiScore(0), m_iEvilBrand(0), m_iPayYuanBao(0),
        m_iExploreExp(0), m_iEnergy(0), m_iLastAutoAddEnergyTime(0), m_iEnergyStore(0),
		m_iLastAutoAddSkillPointTime(0), m_iAwakeStone(0), m_iCrossFight(0), m_iSeal(0),
		m_iVariableSoul(0), m_iVitality(0), m_iAddVitalityTime(0), m_iGuildCoin(0), m_iTacticsCoin(0),
        m_iEntireWarContribution(0), m_iEntireWarMilitarkLevel(0) {}

	int             m_iYuanBao;
	int             m_iGold;
	int             m_iZhuHaiScore;                 //竹海积分
	int             m_iEvilBrand;
	int             m_iPayYuanBao;
	int             m_iExploreExp;
	int             m_iEnergy;
	int             m_iLastAutoAddEnergyTime;
	int             m_iEnergyStore;
	int             m_iLastAutoAddSkillPointTime;   // TODO
	int             m_iAwakeStone;                  // TODO
	int             m_iCrossFight;                  // TODO
	int             m_iSeal;                        // TODO
	int             m_iVariableSoul;                // TODO
	int             m_iVitality;
	int             m_iAddVitalityTime;
	int				m_iGuildCoin;
	int				m_iTacticsCoin;                 // TODO
	int				m_iEntireWarContribution;
	int				m_iEntireWarMilitarkLevel;
};

class CPackHeroCard
{
public:
	MSGPACK_DEFINE(m_iHeroCardID, m_iFashionID, m_iColor, m_iStar, m_iCreateTime,
		m_iLevel, m_iExp, m_iHeroSoul, m_iCardSkill1Level,
		m_iCardSkill2Level, m_iCardSkill3Level, m_iActionPoint,
        m_iKnowledgePoint, m_iFishPoint, m_iCookPoint, m_iGardenPoint, m_iTalentPoint1,
        m_iTalentPoint, m_iTalentTotalConsume, m_stTalentMap,
		m_iEquipmentStar4, m_iEquipmentStar5, m_iEquipmentStar6,
		m_iEquipmentLevel1, m_iEquipmentLevel2, m_iEquipmentLevel3,
		m_iEquipmentLevel4, m_iEquipmentLevel5, m_iEquipmentLevel6,
		m_iEquipmentColor1, m_iEquipmentColor2, m_iEquipmentColor3,
		m_iEquipmentColor4, m_iEquipmentColor5, m_iEquipmentColor6,
		m_iUpgradeColor1, m_iUpgradeColor2, m_iUpgradeColor3, m_iUpgradeColor4,
		m_stHeroFate, m_iCrossFightCostHp,
		m_iEquipmentJewel1Slot1, m_iEquipmentJewel1Slot2,
		m_iEquipmentJewel2Slot1, m_iEquipmentJewel2Slot2,
		m_iEquipmentJewel3Slot1, m_iEquipmentJewel3Slot2,
		m_iEquipmentJewel4Slot1, m_iEquipmentJewel4Slot2,
		m_iEquipmentJewel5Slot1, m_iEquipmentJewel5Slot2,
		m_iEquipmentJewel6Slot1, m_iEquipmentJewel6Slot2,
		m_iTrainAttr1, m_iTrainAttr2, m_iTrainAttr3, m_iTrainAttr4,
		m_iTrainNewAttr1, m_iTrainNewAttr2, m_iTrainNewAttr3, m_iTrainNewAttr4,
		m_stCharacterVec, m_iPetIDVec, m_strMysteryBitmap, m_iMysteryFinalType, m_iMysteryFinalLevel,
		m_iAmuletUniqueIDVec, m_iHeartLevel, m_iHeartExp, m_iDailyGiftNum, m_iLastGiftTime,
		m_iGradeLevel, m_stImpression, m_iSkillCopyCard, m_iTrainBreakNum1, m_iTrainBreakNum2,
        m_iTrainBreakNum3, m_iTrainBreakNum4, m_iTrainBreakAttr1, m_iTrainBreakAttr2, m_iTrainBreakAttr3, m_iTrainBreakAttr4,
        m_iEquipConstellation1, m_iEquipConstellation2, m_iEquipConstellation3, m_iEquipConstellation4,
        m_iEquip1, m_iEquip2, m_iEquip3, m_iEquip4);

	CPackHeroCard()
		: m_iHeroCardID(0), m_iFashionID(0), m_iColor(0), m_iStar(0), m_iCreateTime(0),
		m_iLevel(0), m_iExp(0), m_iHeroSoul(0), m_iCardSkill1Level(0), m_iCardSkill2Level(0),
		m_iCardSkill3Level(0), m_iActionPoint(0), m_iKnowledgePoint(0), m_iFishPoint(0),
        m_iCookPoint(0), m_iGardenPoint(0), m_iTalentPoint1(0), m_iTalentPoint(0), m_iTalentTotalConsume(0), m_stTalentMap(),
		m_iEquipmentStar4(0), m_iEquipmentStar5(0), m_iEquipmentStar6(0),
		m_iEquipmentLevel1(0), m_iEquipmentLevel2(0), m_iEquipmentLevel3(0),
		m_iEquipmentLevel4(0), m_iEquipmentLevel5(0), m_iEquipmentLevel6(0),
		m_iEquipmentColor1(0), m_iEquipmentColor2(0), m_iEquipmentColor3(0),
		m_iEquipmentColor4(0), m_iEquipmentColor5(0), m_iEquipmentColor6(0),
		m_iUpgradeColor1(0), m_iUpgradeColor2(0), m_iUpgradeColor3(0), m_iUpgradeColor4(0),
		m_iCrossFightCostHp(0), m_iEquipmentJewel1Slot1(0), m_iEquipmentJewel1Slot2(0),
		m_iEquipmentJewel2Slot1(0), m_iEquipmentJewel2Slot2(0),
		m_iEquipmentJewel3Slot1(0), m_iEquipmentJewel3Slot2(0),
		m_iEquipmentJewel4Slot1(0), m_iEquipmentJewel4Slot2(0),
		m_iEquipmentJewel5Slot1(0), m_iEquipmentJewel5Slot2(0),
		m_iEquipmentJewel6Slot1(0), m_iEquipmentJewel6Slot2(0),
		m_iTrainAttr1(0), m_iTrainAttr2(0), m_iTrainAttr3(0), m_iTrainAttr4(0),
		m_iTrainNewAttr1(0), m_iTrainNewAttr2(0), m_iTrainNewAttr3(0), m_iTrainNewAttr4(0),
		m_stCharacterVec(), m_iPetIDVec(), m_strMysteryBitmap(), m_iMysteryFinalType(0), m_iMysteryFinalLevel(0),
		m_iAmuletUniqueIDVec(), m_iHeartLevel(0), m_iHeartExp(0), m_iDailyGiftNum(0), m_iLastGiftTime(0),
		m_iGradeLevel(0), m_stImpression(), m_iSkillCopyCard(0), m_iTrainBreakNum1(0), m_iTrainBreakNum2(0),
        m_iTrainBreakNum3(0), m_iTrainBreakNum4(0), m_iTrainBreakAttr1(0), m_iTrainBreakAttr2(0), m_iTrainBreakAttr3(0),
        m_iTrainBreakAttr4(0), m_iEquipConstellation1(0), m_iEquipConstellation2(0), m_iEquipConstellation3(0),
        m_iEquipConstellation4(0), m_iEquip1(0), m_iEquip2(0), m_iEquip3(0), m_iEquip4(0) {}

	int32_t              m_iHeroCardID;
	int32_t              m_iFashionID;
	int32_t              m_iColor;
	int32_t              m_iStar;
	int32_t              m_iCreateTime;
	int32_t              m_iLevel;
	int32_t              m_iExp;
	int32_t              m_iHeroSoul;               // TODO
	int32_t              m_iCardSkill1Level;        // TODO
	int32_t              m_iCardSkill2Level;        // TODO
	int32_t              m_iCardSkill3Level;        // TODO
	int32_t              m_iActionPoint;            // TODO
	int32_t              m_iKnowledgePoint;         // TODO
	int32_t              m_iFishPoint;              // TODO
	int32_t              m_iCookPoint;              // TODO
	int32_t              m_iGardenPoint;            // TODO
	int32_t              m_iTalentPoint1;           // 专有天赋点数1
	int32_t              m_iTalentPoint;            // 专有天赋点数
	int32_t              m_iTalentTotalConsume;     // 天赋总消耗
	std::map<int, int>   m_stTalentMap;             // 天赋
	int32_t              m_iEquipmentStar4;         // TODO
	int32_t              m_iEquipmentStar5;         // TODO
	int32_t              m_iEquipmentStar6;         // TODO
	int32_t              m_iEquipmentLevel1;        // TODO
	int32_t              m_iEquipmentLevel2;        // TODO
	int32_t              m_iEquipmentLevel3;        // TODO
	int32_t              m_iEquipmentLevel4;        // TODO
	int32_t              m_iEquipmentLevel5;        // TODO
	int32_t              m_iEquipmentLevel6;        // TODO
	int32_t              m_iEquipmentColor1;        // TODO
	int32_t              m_iEquipmentColor2;        // TODO
	int32_t              m_iEquipmentColor3;        // TODO
	int32_t              m_iEquipmentColor4;        // TODO
	int32_t              m_iEquipmentColor5;        // TODO
	int32_t              m_iEquipmentColor6;        // TODO
	int32_t              m_iUpgradeColor1;
	int32_t              m_iUpgradeColor2;
	int32_t              m_iUpgradeColor3;
	int32_t              m_iUpgradeColor4;
	std::vector<int32_t> m_stHeroFate;              // TODO
	int32_t              m_iCrossFightCostHp;

	int32_t              m_iEquipmentJewel1Slot1;   // TODO
	int32_t              m_iEquipmentJewel1Slot2;   // TODO
	int32_t              m_iEquipmentJewel2Slot1;   // TODO
	int32_t              m_iEquipmentJewel2Slot2;   // TODO
	int32_t              m_iEquipmentJewel3Slot1;   // TODO
	int32_t              m_iEquipmentJewel3Slot2;   // TODO
	int32_t              m_iEquipmentJewel4Slot1;   // TODO
	int32_t              m_iEquipmentJewel4Slot2;   // TODO
	int32_t              m_iEquipmentJewel5Slot1;   // TODO
	int32_t              m_iEquipmentJewel5Slot2;   // TODO
	int32_t              m_iEquipmentJewel6Slot1;   // TODO
	int32_t              m_iEquipmentJewel6Slot2;   // TODO

	int32_t              m_iTrainAttr1;         // TODO
	int32_t              m_iTrainAttr2;         // TODO
	int32_t              m_iTrainAttr3;         // TODO
	int32_t              m_iTrainAttr4;         // TODO

	int32_t              m_iTrainNewAttr1;      // TODO
	int32_t              m_iTrainNewAttr2;      // TODO
	int32_t              m_iTrainNewAttr3;      // TODO
	int32_t              m_iTrainNewAttr4;      // TODO

	std::vector<int> m_stCharacterVec;  // TODO
	std::vector<int32_t> m_iPetIDVec;   // TODO

	std::string m_strMysteryBitmap;     // TODO
	int32_t m_iMysteryFinalType;        // TODO
	int32_t m_iMysteryFinalLevel;       // TODO

	std::vector<int32_t> m_iAmuletUniqueIDVec;  // TODO

	// 好感度
	int32_t             m_iHeartLevel;
	int32_t             m_iHeartExp;
	int32_t             m_iDailyGiftNum;
	int32_t             m_iLastGiftTime;
    int32_t             m_iGradeLevel;              // SSR阶级
    std::vector<int>    m_stImpression;             // 印象分
    int32_t             m_iSkillCopyCard;           // TODO

    // 培养突破
    int32_t             m_iTrainBreakNum1;          // TODO
    int32_t             m_iTrainBreakNum2;          // TODO
    int32_t             m_iTrainBreakNum3;          // TODO
    int32_t             m_iTrainBreakNum4;          // TODO
    int32_t             m_iTrainBreakAttr1;         // TODO
    int32_t             m_iTrainBreakAttr2;         // TODO
    int32_t             m_iTrainBreakAttr3;         // TODO
    int32_t             m_iTrainBreakAttr4;         // TODO

    // 星纹
    int32_t              m_iEquipConstellation1;
    int32_t              m_iEquipConstellation2;
    int32_t              m_iEquipConstellation3;
    int32_t              m_iEquipConstellation4;

    // 装备
    int32_t              m_iEquip1;
    int32_t              m_iEquip2;
    int32_t              m_iEquip3;
    int32_t              m_iEquip4;
};

class CPackBuyResourceInfo
{
public:
	CPackBuyResourceInfo()
		: m_iBuyGoldTimes(0), m_iBuyEnergyTimes(0), m_iBuySkillPointTimes(0),
		m_iBuyUpgradeWushenTimes(0), m_iBuyRefreshPVPTimes(0),
		m_iBuyRefreshUnlimitTimes(0), m_iBuyVitalityTimes(0),
		m_iBreakGoldEggTimes(0), m_iBuyGoldTotalTimes(0) {}

	MSGPACK_DEFINE(m_iBuyGoldTimes, m_iBuyEnergyTimes, m_iBuySkillPointTimes,
		m_iBuyUpgradeWushenTimes, m_iBuyRefreshPVPTimes,
		m_iBuyRefreshUnlimitTimes, m_iBuyVitalityTimes, m_iBreakGoldEggTimes,
		m_iBuyGoldTotalTimes);

	int             m_iBuyGoldTimes;
	int             m_iBuyEnergyTimes;
	int             m_iBuySkillPointTimes;      // TODO
	int             m_iBuyUpgradeWushenTimes;   // TODO
	int             m_iBuyRefreshPVPTimes;
	int             m_iBuyRefreshUnlimitTimes;  // TODO
	int             m_iBuyVitalityTimes;
	int				m_iBreakGoldEggTimes;       // TODO
	int             m_iBuyGoldTotalTimes;
};

class CPackUserBaseInfoSection1
{
public:
	MSGPACK_DEFINE(m_iAvatarIconID, m_iAvatarBorderID, m_iBlackMarketOpenTime,
		m_iBlackMarketRefreshTimes, m_iMonthCardOpenTime, m_iWeekCardOpenTime,
		m_iFirstDepositBonus, m_iGroceryStoreLastRefreshTime, m_iGroceryStoreRefreshTimes,
		m_iQuickPassFlag, m_iPlayerLVAwardStep, m_bHasActiveHongDian, m_strVipBagBitmap,
		m_iCrossFightResetTime, m_iCrossFightResetTimes, m_iRedemptionRoad,
		m_iCrossFightStoreLastRefreshTime, m_iCrossFightStoreRefreshTimes,
		m_iCrossFightRefreshResetTimesTimes, m_iCrossFightLastRefreshResetTimesTime,
		m_iDailyDeposit, m_iDailyDepositBonus, m_iOnlineBonusIndex, m_iOnlineBonusTime,
		m_strFightPowerRankStageBitmap, m_iAddUpLoginDay, m_iQuarterCardOpenTime,
		m_strTalentAwardBitmap, m_iSilverMonthCardOpenTime, m_iGoldMonthCardOpenTime, m_iTalkQipao,
		m_stCardDialogLimitAvyStat, m_iCardDialogNextTime, m_iCardDialogTimes, m_iAddUpDepositBonus,
        m_ulOnceEventStat, m_iConstelExpPool, m_stMainBgm);

	int         m_iAvatarIconID;
	int         m_iAvatarBorderID;
	int         m_iBlackMarketOpenTime;             // TODO
	int         m_iBlackMarketRefreshTimes;         // TODO
	int         m_iMonthCardOpenTime;               // TODO
	int         m_iWeekCardOpenTime;                // TODO
	int         m_iFirstDepositBonus;
	int         m_iGroceryStoreLastRefreshTime;     // TODO
	int         m_iGroceryStoreRefreshTimes;        // TODO
	int         m_iQuickPassFlag;                   // TODO
	int         m_iPlayerLVAwardStep;               // TODO
	bool        m_bHasActiveHongDian;               // TODO
	std::string m_strVipBagBitmap;
	int         m_iCrossFightResetTime;
	int         m_iCrossFightResetTimes;
	int         m_iRedemptionRoad;                  // TODO
	int         m_iCrossFightStoreLastRefreshTime;  // TODO
	int         m_iCrossFightStoreRefreshTimes;     // TODO
	int         m_iCrossFightRefreshResetTimesTimes;
	int         m_iCrossFightLastRefreshResetTimesTime;
	int         m_iDailyDeposit;
	int         m_iDailyDepositBonus;
	int         m_iOnlineBonusIndex;                    // TODO
	int         m_iOnlineBonusTime;                     // TODO
	std::string m_strFightPowerRankStageBitmap;
	int         m_iAddUpLoginDay;
	int         m_iQuarterCardOpenTime;                 // TODO
	std::string m_strTalentAwardBitmap;                 // TODO
	int			m_iSilverMonthCardOpenTime;             // TODO
	int			m_iGoldMonthCardOpenTime;               // TODO
	int			m_iTalkQipao;                           // TODO
	std::vector<int32_t>    m_stCardDialogLimitAvyStat;     // 小人互动，限时活动领取奖励状态
	int         m_iCardDialogNextTime;                      // 小人互动，下一次可互动时间
	int         m_iCardDialogTimes;                         // 小人互动，已互动次数
	int         m_iAddUpDepositBonus;                       // 首次累充奖励是否已领取
    uint64_t    m_ulOnceEventStat;
    int32_t     m_iConstelExpPool;                          // 星纹经验池
    std::list<int> m_stMainBgm;                             // 主城BGM
};

//玩家设备统计信息
class CPackUserStatInfo
{
public:
	CPackUserStatInfo()
		: m_strAccount("default_account"), m_strDeviceID("default_uuid"), m_strDeviceModel("default_model"),
		m_strDeviceOS("ios"), m_strDeviceRatio("640x960"), m_strDeviceNet("2G"),
		m_strDeviceCarrier("net"), m_strPlatformID("default_id"), m_strPlatformSign("default_sign"), m_iResVer(0) {}

	MSGPACK_DEFINE(m_strAccount, m_strDeviceID, m_strDeviceModel, m_strDeviceOS,
		m_strDeviceRatio, m_strDeviceNet, m_strDeviceCarrier,
		m_strPlatformID, m_strPlatformSign, m_iResVer);

	std::string             m_strAccount;           //用户帐号 
	std::string             m_strDeviceID;          //设备唯一标识
	std::string             m_strDeviceModel;       //设备型号
	std::string             m_strDeviceOS;          //设备操作系统
	std::string             m_strDeviceRatio;       //设备分辨率
	std::string             m_strDeviceNet;         //设备联网方式(2G 3G 4G WIFI)
	std::string             m_strDeviceCarrier;     //设备运营商
	std::string             m_strPlatformID;        //平台ID
	std::string             m_strPlatformSign;      //平台标示
	int32_t					m_iResVer;				//资源版本号
};

class CResponseGetUserData : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_stUserBaseInfo, m_stCurrencyInfo, m_stHeroCardLandTeam,
        m_stItemNumMap, m_stHeroCardVec, m_stCherryTeam,
		m_iTimestamp, m_stAllWushenInfo, m_stBuyResourceInfo,
		m_stUserBaseInfoSection1, m_iOpenServerDay, m_iRefreshDayHour,
		m_stCrossFightTeam, m_stItemAttrVec,
		m_stClimbTowerInfo, m_iSerialPayAwarded, m_iOpenSvrTime, m_stSelfGuild,
		m_stPvpTerritoryInfo, m_stTeamVec, m_stFashionVec, m_stUserTactics, m_stPetVec, m_iXunchongLevel,
		m_stAmuletInfoVec, m_stHonorInfoVec, m_iMergeServerTime, m_stThemeVec, m_stConstellationVec,
        m_stEquipVec,m_iMonthAddUp,m_iAgeLimitIndex,m_iLastChangeAgeLimit);

	CResponseGetUserData()
		: m_stUserBaseInfo(), m_stCurrencyInfo(), m_stHeroCardLandTeam(),
        m_stItemNumMap(), m_stHeroCardVec(), m_stCherryTeam(),
		m_iTimestamp(0), m_stAllWushenInfo(), m_stBuyResourceInfo(),
		m_stUserBaseInfoSection1(), m_iOpenServerDay(0), m_iRefreshDayHour(0),
		m_stCrossFightTeam(), m_stItemAttrVec(),
		m_stClimbTowerInfo(), m_iSerialPayAwarded(0), m_iOpenSvrTime(0), m_stSelfGuild(),
		m_stPvpTerritoryInfo(), m_stTeamVec(), m_stFashionVec(), m_stUserTactics(), m_stPetVec(), m_iXunchongLevel(0),
		m_stAmuletInfoVec(), m_stHonorInfoVec(), m_iMergeServerTime(0), m_stThemeVec(), m_stConstellationVec(),
        m_stEquipVec() ,m_iMonthAddUp(0),m_iAgeLimitIndex(0),m_iLastChangeAgeLimit(0){}

	CPackUserBaseInfo             	m_stUserBaseInfo;
	CPackCurrency                   m_stCurrencyInfo;
	CPackCardTeam                  	m_stHeroCardLandTeam;   // TODO
	std::map<int, int>       		m_stItemNumMap;
	std::vector<CPackHeroCard>      m_stHeroCardVec;
	std::vector<int>                m_stCherryTeam;         // TODO
	int                             m_iTimestamp;
	std::vector<int>                m_stAllWushenInfo;        // TODO
	CPackBuyResourceInfo            m_stBuyResourceInfo;
	CPackUserBaseInfoSection1       m_stUserBaseInfoSection1;
	int                             m_iOpenServerDay;
	int                             m_iRefreshDayHour;
	CPackCardTeam                  	m_stCrossFightTeam;     // TODO
	std::vector<CPackItemAttr>      m_stItemAttrVec;
	CPackProtoClimbTower			m_stClimbTowerInfo;
	int                             m_iSerialPayAwarded;
	int								m_iOpenSvrTime;
	std::vector<int>				m_stSelfGuild;          // TODO
	std::vector<int>			    m_stPvpTerritoryInfo;   // TODO
    std::vector<CPackCardTeam>		m_stTeamVec;
	std::vector<CPackFashionInfo>	m_stFashionVec;
    std::vector<int>			m_stUserTactics;            // TODO
	std::vector<int>			m_stPetVec;                 // TODO
	int								m_iXunchongLevel;       // TODO
	std::vector<int>	m_stAmuletInfoVec;                  // TODO
	std::vector<CPackHonorInfo>		m_stHonorInfoVec;
	int32_t							m_iMergeServerTime;
    std::vector<int>	            m_stThemeVec;           // TODO
    std::vector<CPackConstellationInfo> m_stConstellationVec;
    std::vector<CPackEquipInfo>     m_stEquipVec;
    int32_t                         m_iMonthAddUp;           // 每月充值金额
    int32_t                         m_iAgeLimitIndex;        // 充值年龄限制序号
    int32_t                         m_iLastChangeAgeLimit;   // 上次修改年龄限制时间戳
};

/////////////////////////////////////////////////////////////////////////////////////////
//创建角色
class CRequestCreateRole
{
public:
	MSGPACK_DEFINE(m_strNick, m_ucRoleGender, m_strPid, m_iDeviceType, m_strMac, m_strIDFa, m_stStatInfo);

	CRequestCreateRole()
		: m_strNick(), m_ucRoleGender(0), m_strPid(""), m_iDeviceType(2), m_strMac(),
		m_strIDFa(), m_stStatInfo() {}

	std::string                     m_strNick;              //主角昵称
	unsigned char                   m_ucRoleGender;         //主角性别
	std::string                     m_strPid;               //Pid
															//stat info
	int                             m_iDeviceType;          //(1-安卓,2-ios,3-wp)
	std::string                     m_strMac;               //网卡地址
	std::string                     m_strIDFa;              //ios专用
	CPackUserStatInfo  				m_stStatInfo;           //统计信息
};

class CResponseCreateRole : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iGroupID, m_iUin, m_strNick);

	CResponseCreateRole() : m_iGroupID(0), m_iUin(0), m_strNick() {}

	int             m_iGroupID;
	int             m_iUin;
    std::string     m_strNick;              //主角昵称
};

class CRequestCheckNickCorrect
{
public:
	MSGPACK_DEFINE(m_strNick);

	CRequestCheckNickCorrect() : m_strNick("") {}

	std::string     m_strNick;
};

///////////////////////////////////////////////////////////////////////////////////////////////
//断线重连
class CResponseReconnect : public CPackUserSyncData
{
public:
	CResponseReconnect() : m_iSvrTimestamp(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iSvrTimestamp);

	int             m_iSvrTimestamp;
};

///////////////////////////////////////////////////////////////////////////////////////////////
//同步服务器时间戳
class CResponseSyncTimestamp : public CPackUserSyncData
{
public:
	CResponseSyncTimestamp() : m_iSvrTimestamp(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iSvrTimestamp);

	int             m_iSvrTimestamp;
};

class CResponseCheckUserExist : public CPackUserSyncData
{
public:
	CResponseCheckUserExist() : m_iCode(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iCode);

	int                 m_iCode;
};

class CResponseUserBanError : public CPackUserSyncData
{
public:
	CResponseUserBanError() : m_iBanBeginTime(0), m_iBanEndTime(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iBanBeginTime, m_iBanEndTime);

	int         m_iBanBeginTime;
	int         m_iBanEndTime;
};

//CHECK_CAN_DEPOSIT = 10304,					//检查是否可以充值
class CRequestCheckCanDeposit
{
public:
	MSGPACK_DEFINE(m_iDepositID);

	CRequestCheckCanDeposit() : m_iDepositID(0) {}

	int32_t m_iDepositID;
};

class CResponseCheckCanDeposit : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iDepositID, m_bCanBuy);

	CResponseCheckCanDeposit() : m_iDepositID(0), m_bCanBuy(false) {}

	int32_t m_iDepositID;
	bool m_bCanBuy;
};

// 免费充值
class CRequestFreeDeposit
{
public:
    CRequestFreeDeposit() : m_iDepositID(0) {}

    MSGPACK_DEFINE(m_iDepositID);

    int32_t     m_iDepositID;
};

class CResponseFreeDeposit : public CPackUserSyncData
{
public:
    CResponseFreeDeposit() : m_iDepositID(0), m_iVipExp(0), m_stItemList() {}

    MSGPACK_DEFINE(m_stSyncData, m_iDepositID, m_iVipExp, m_stItemList);

    int32_t     m_iDepositID;
    int32_t     m_iVipExp;
    std::vector<CPackGameItem>  m_stItemList;
};

//NOTIFY_SERVER_CONFIG = 10307,				//通知服务器配置
class CNotifyServerConfigNoThree : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_strNormalService, m_strSuperService, m_iH5PaySwitch, m_strH5PayLink
		, m_strEvaluateUrl, m_iEvaluateLevel, m_iEvaluateRatio, m_iMergeTime, m_iMergeCount
		);

    CNotifyServerConfigNoThree() : m_strNormalService(), m_strSuperService(), m_iH5PaySwitch(0), m_strH5PayLink()
		, m_strEvaluateUrl(), m_iEvaluateLevel(0), m_iEvaluateRatio(0), m_iMergeTime(0), m_iMergeCount(0)
		, m_iThreePaySwitch(0), m_strThreePayLink() {}

	std::string 	m_strNormalService;
	std::string 	m_strSuperService;
	int32_t 		m_iH5PaySwitch;
	std::string 	m_strH5PayLink;
	std::string 	m_strEvaluateUrl;
	int32_t 		m_iEvaluateLevel;
	int32_t 		m_iEvaluateRatio;
	int32_t 		m_iMergeTime;
	int32_t 		m_iMergeCount;
    int32_t 		m_iThreePaySwitch;
    std::string 	m_strThreePayLink;
};

class CNotifyServerConfig : public CNotifyServerConfigNoThree
{
public:
    MSGPACK_DEFINE(m_stSyncData, m_strNormalService, m_strSuperService, m_iH5PaySwitch, m_strH5PayLink
    , m_strEvaluateUrl, m_iEvaluateLevel, m_iEvaluateRatio, m_iMergeTime, m_iMergeCount
    , m_iThreePaySwitch, m_strThreePayLink);
};

// 防沉迷，更新玩家年龄
class CNotifyUpdateUserAge : public CPackUserSyncData
{
public:
    CNotifyUpdateUserAge() {}

    MSGPACK_DEFINE(m_stSyncData);
};

class CRequestUpdateUserAge
{
public:
    MSGPACK_DEFINE(m_iAge);

    CRequestUpdateUserAge() : m_iAge(0) {}

    int32_t m_iAge;     // 0 表示是游客
};

class CRequestGetHeroHistoryReward
{
public:
    CRequestGetHeroHistoryReward() : m_iCardID(0), m_iIndex(0) {}

    MSGPACK_DEFINE(m_iCardID, m_iIndex);

    int32_t     m_iCardID;
    int32_t     m_iIndex;
};

class CResponseGetHeroHistoryReward : public CPackUserSyncData
{
public:
    CResponseGetHeroHistoryReward() : m_iCardID(0), m_stReward(), m_stHistory() {}

    MSGPACK_DEFINE(m_stSyncData, m_iCardID, m_stReward, m_stHistory);

    int32_t                     m_iCardID;
    std::vector<CPackGameItem>  m_stReward;
    CPackHeroHistory            m_stHistory;
};

class CPackQuestionnaireAnswer
{
public:
    CPackQuestionnaireAnswer() : m_stIndexVec(), m_strContent("") {}

    MSGPACK_DEFINE(m_stIndexVec, m_strContent);

    std::vector<int32_t>        m_stIndexVec;               // 选择题答案索引，从1开始
    std::string                 m_strContent;               // 自由输入文本
};

class CRequestQuestionnaire
{
public:
    CRequestQuestionnaire() : m_iID(0), m_stAnswers() {}

    MSGPACK_DEFINE(m_iID, m_stAnswers);

    int32_t         m_iID;      // 问卷ID
    std::vector<CPackQuestionnaireAnswer> m_stAnswers;
};

class CResponseQuestionnaire : public CPackUserSyncData
{
public:
    CResponseQuestionnaire() : m_iID(0), m_stRewards() {}

    MSGPACK_DEFINE(m_stSyncData, m_iID, m_stRewards);

    int32_t         m_iID;
    std::vector<CPackGameItem> m_stRewards;
};

