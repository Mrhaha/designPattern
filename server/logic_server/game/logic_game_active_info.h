#pragma once

#include "server_active_protocol.h"
#include "logic_config_active.h"
#include "logic_game_cache_table.h"
#include "logic_game_publisher.h"

class CLogicActiveBase
{
public:
	struct active_info_t
    {
        active_info_t() : m_ulReceivedBitmap(0), m_ulCompleteBitmap(0), m_ulParaEx(0), m_iLastReceiveAwardTime(0), m_iTargetNum(0) { }
            
        uint64_t    m_ulReceivedBitmap;
        uint64_t    m_ulCompleteBitmap;
        uint64_t    m_ulParaEx;
        int         m_iLastReceiveAwardTime;
        int         m_iTargetNum;
    };

	using active_obj_ptr = std::shared_ptr<CLogicActiveBase>;

	using create_active_obj_func_type = active_obj_ptr(*)(user_data_table_ptr_type, int);

	using active_notify_func_type = void(*)(user_data_table_ptr_type, int);

    static std::map<int, create_active_obj_func_type> CREATE_ACTIVE_FUNC_MAP;

    static std::map<int, active_notify_func_type> ACTIVE_NOTIFY_FUNC_MAP;

public:
    static void RegisterActiveNotify(int iType, active_notify_func_type pFunc);

    static void Notify(user_data_table_ptr_type pUserData, int iType, int iNum);

    static void SubscribeGameDepositInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_deposit_channel_data_t& stData);

    static void SubscribeGameLevelInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_level_channel_data_t& stData);

    static void SubscribeGamePVPRankCallBack(user_data_table_ptr_type pUserData, const logic_pvp_rank_channel_data_t& stData);

    static void SubscribeGameUnlimitLevelScoreCallBack(user_data_table_ptr_type pUserData, const logic_game_unlimit_level_score_channel_data_t& stData);

    static void SubscribeGameAddLevelStarCallBack(user_data_table_ptr_type pUserData, const logic_game_add_level_star_channel_data_t& stData);

    static void SubscribeGameHeroEquipmentCallBack(user_data_table_ptr_type pUserData, const logic_game_hero_equipment_channel_data_t& stData);

    static void SubscribeGameCardInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_card_channel_data_t& stData);

    static void SubscribeGameItemAddInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_item_add_channel_data_t& stData);

    static void SubscribeGameUserActionInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_user_action_channel_data_t& stData);

    static void SubscribeGameMallChannelInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_mall_channel_data_t& stData);

    static void SubscribeFunctionalGameItemInfoCallBack(user_data_table_ptr_type pUserData, const logic_functional_game_item_channel_data_t& stData);

	static void SubscribeGameEnergyCallBack(user_data_table_ptr_type pUserData, const CLogicGameEnergyChannel::data_type& stData);

	static void SubscribeClimbTowerOrderCallBack(user_data_table_ptr_type pUserData, const CLogicClimbTowerOrderChannel::data_type& stData);

	static void SubscribePvpTerritoryOrderCallBack(user_data_table_ptr_type pUserData, const CLogicPvpTerritoryOrderChannel::data_type& stData);

    static void SubscribeGameUserLevelUpInfoCallBack(user_data_table_ptr_type pUserData);

    static void SubscribeGameCombatInfoCallBack(user_data_table_ptr_type pUserData);

public:
    CLogicActiveBase(user_data_table_ptr_type pUserData, int iActivityID)
    : m_pUserData(pUserData), m_iActivityID(iActivityID) { }
    
    virtual ~CLogicActiveBase() {};
    
    virtual active_info_t GetActiveInfo();

    virtual int UpdateActiveInfo(int iNum) { return 0; };
    
    virtual bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) = 0;

    std::map<std::string, std::string> GetParamMap() { return m_stParamMap; }
    
    template<typename T> static active_obj_ptr CreateActiveFunc(user_data_table_ptr_type pUserData, int iActivityID);
    
    static active_obj_ptr CreateActiveObj(user_data_table_ptr_type pUserData, int iActivityID);

    static void GetAllActiveInfo(user_data_table_ptr_type pUserData, std::vector<CPackActiveInfo>& stActiveInfo);

	static bool IsActiveOpen(user_data_table_ptr_type pUserData, int iActivityID);
    
protected:
    user_data_table_ptr_type         m_pUserData;

    int                                 m_iActivityID;

    std::map<std::string, std::string>  m_stParamMap;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicGetEnergyActive : public CLogicActiveBase
{
public:
    CLogicGetEnergyActive(user_data_table_ptr_type pUserData, int iActivityID);
    
    active_info_t GetActiveInfo() override;
    
    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicBenefitStoreActive : public CLogicActiveBase
{
public:
    CLogicBenefitStoreActive(user_data_table_ptr_type pUserData, int iActivityID);
    
    active_info_t GetActiveInfo() override;
    
    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicCumulateLoginActive : public CLogicActiveBase
{
public:
    CLogicCumulateLoginActive(user_data_table_ptr_type pUserData, int iActivityID);
    
    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicCumulateDepositActive : public CLogicActiveBase
{
public:
    CLogicCumulateDepositActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);
    
    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicDeadlineLoginActive : public CLogicActiveBase
{
public:
    CLogicDeadlineLoginActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);
    
    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicBuyCommodityActive : public CLogicActiveBase
{
public:
    CLogicBuyCommodityActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);
    
    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicSingleDepositActive : public CLogicActiveBase
{
public:
    CLogicSingleDepositActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    active_info_t GetActiveInfo() override;
    
    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicDailyCumulateDepositActive : public CLogicActiveBase
{
public:
    CLogicDailyCumulateDepositActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    active_info_t GetActiveInfo() override;
    
    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicCornucopiaActive : public CLogicActiveBase
{
public:
    CLogicCornucopiaActive(user_data_table_ptr_type pUserData, int iActivityID);

    active_info_t GetActiveInfo() override;

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicFoundationActive : public CLogicActiveBase
{
public:
    CLogicFoundationActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicLevelRaceActive : public CLogicActiveBase
{
public:
    enum 
    {
        EXTRA_AWARD_TASK_COMPLETING = 0,
        EXTRA_AWARD_TASK_COMPLETED = 1,
        EXTRA_AWARD_TASK_RECEIVED = 2,
    };

public:
    CLogicLevelRaceActive(user_data_table_ptr_type pUserData, int iActivityID);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;

public:

    static void Notify(user_data_table_ptr_type pUserData, int iNum);
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicHeroFlashSaleActive : public CLogicActiveBase
{
    enum 
    {
        PURCHASE_LIMIT_TASK_COMPLETING = 0,
        PURCHASE_LIMIT_TASK_COMPLETED = 1,
        PURCHASE_LIMIT_TASK_RECEIVED = 2,
    };

public:
    CLogicHeroFlashSaleActive(user_data_table_ptr_type pUserData, int iActivityID);

    active_info_t GetActiveInfo() override;

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;

    static void Notify(user_data_table_ptr_type pUserData, int iNum);
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicOpenServerLoginActive : public CLogicActiveBase
{
public:
    CLogicOpenServerLoginActive(user_data_table_ptr_type pUserData, int iActivityID);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicOpenServerBuyActive : public CLogicActiveBase
{
public:
    CLogicOpenServerBuyActive(user_data_table_ptr_type pUserData, int iActivityID);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicNormalLevelActive : public CLogicActiveBase
{
public:
    CLogicNormalLevelActive(user_data_table_ptr_type pUserData, int iActivityID);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;

    static void Notify(user_data_table_ptr_type pUserData, int iNum);
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicEliteLevelActive : public CLogicActiveBase
{
public:
    CLogicEliteLevelActive(user_data_table_ptr_type pUserData, int iActivityID);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;

    static void Notify(user_data_table_ptr_type pUserData, int iNum);
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicPVPRankActive : public CLogicActiveBase
{
public:
    CLogicPVPRankActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicUnlimitLevelScoreActive : public CLogicActiveBase
{
public:
    CLogicUnlimitLevelScoreActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicPassLevelStarActive : public CLogicActiveBase
{
public:
    CLogicPassLevelStarActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicCumulateConsumeYuanBaoActive : public CLogicActiveBase
{
public:
    CLogicCumulateConsumeYuanBaoActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicDailyConsumeYuanBaoActive : public CLogicActiveBase
{
public:
	CLogicDailyConsumeYuanBaoActive(user_data_table_ptr_type pUserData, int iActivityID);

	active_info_t GetActiveInfo() override;

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicPassLevelTimesActive : public CLogicActiveBase
{
public:
    CLogicPassLevelTimesActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicBuyEnergyTimesActive : public CLogicActiveBase
{
public:
    CLogicBuyEnergyTimesActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicBuyGoldTimesActive : public CLogicActiveBase
{
public:
    CLogicBuyGoldTimesActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicBuyBlackStoreTimesActive : public CLogicActiveBase
{
public:
    CLogicBuyBlackStoreTimesActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicCrossFightPassLevelTimesActive : public CLogicActiveBase
{
public:
    CLogicCrossFightPassLevelTimesActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicUnlimitLevelFightTimesActive : public CLogicActiveBase
{
public:
    CLogicUnlimitLevelFightTimesActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicEliteLevelFightTimesActive : public CLogicActiveBase
{
public:
    CLogicEliteLevelFightTimesActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicNormalLevelFightTimesActive : public CLogicActiveBase
{
public:
    CLogicNormalLevelFightTimesActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicFightThemeActiveLevelActive : public CLogicActiveBase
{
public:
    CLogicFightThemeActiveLevelActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicPVPFightTimesActive : public CLogicActiveBase
{
public:
    CLogicPVPFightTimesActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicHeroEquipmentUpgradeLevelActive : public CLogicActiveBase
{
public:
    CLogicHeroEquipmentUpgradeLevelActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicHeroEquipmentUpgradeJewelActive : public CLogicActiveBase
{
public:
    CLogicHeroEquipmentUpgradeJewelActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicHeroEquipmentUpgradeStarActive : public CLogicActiveBase
{
public:
    CLogicHeroEquipmentUpgradeStarActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicHeroUpgradeColorActive : public CLogicActiveBase
{
public:
    CLogicHeroUpgradeColorActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicHeroUpgradeLevelActive : public CLogicActiveBase
{
public:
    CLogicHeroUpgradeLevelActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicHeroUpgradeStarActive : public CLogicActiveBase
{
public:
    CLogicHeroUpgradeStarActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicTeamUpgradeLevelActive : public CLogicActiveBase
{
public:
    CLogicTeamUpgradeLevelActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicUpgradeSkillActive : public CLogicActiveBase
{
public:
    CLogicUpgradeSkillActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicFightPowerActive : public CLogicActiveBase
{
public:
    CLogicFightPowerActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicHeroAchieveActive : public CLogicActiveBase
{
public:
    CLogicHeroAchieveActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicExchangeStoreActive : public CLogicActiveBase
{
public:
    CLogicExchangeStoreActive(user_data_table_ptr_type pUserData, int iActivityID);
    
    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicSingleItemExchangeActive : public CLogicActiveBase
{
public:
    CLogicSingleItemExchangeActive(user_data_table_ptr_type pUserData, int iActivityID);
    
    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicUpgradeTacticsActive : public CLogicActiveBase
{
public:
	CLogicUpgradeTacticsActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicHeroTrainActive : public CLogicActiveBase
{
public:
	CLogicHeroTrainActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicBreakEggActive : public CLogicActiveBase
{
public:
	CLogicBreakEggActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicYuanbaoRecruitActive : public CLogicActiveBase
{
public:
	CLogicYuanbaoRecruitActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicConsumeEnergyActive : public CLogicActiveBase
{
public:
	CLogicConsumeEnergyActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicClimbTowerActive : public CLogicActiveBase
{
public:
	CLogicClimbTowerActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicPvpTerritoryActive : public CLogicActiveBase
{
public:
	CLogicPvpTerritoryActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicTacticsCountActive : public CLogicActiveBase
{
public:
	CLogicTacticsCountActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicGuildDonateActive : public CLogicActiveBase
{
public:
	CLogicGuildDonateActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicGuildTaskActive : public CLogicActiveBase
{
public:
	CLogicGuildTaskActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicLuckyBoxActive : public CLogicActiveBase
{
public:
	CLogicLuckyBoxActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicTalentScoreActive : public CLogicActiveBase
{
public:
	CLogicTalentScoreActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	active_info_t GetActiveInfo() override;

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicTotalHeroTrainActive : public CLogicActiveBase
{
public:
	CLogicTotalHeroTrainActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	active_info_t GetActiveInfo() override;

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
////////////////// 限时神将
class CLogicCountDownHeroActive : public CLogicActiveBase
{
public:
	CLogicCountDownHeroActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};



/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicTurnTableScoreActive : public CLogicActiveBase
{
public:
	CLogicTurnTableScoreActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicLoginInsureActive : public CLogicActiveBase
{
public:
	CLogicLoginInsureActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicHeroInsureActive : public CLogicActiveBase
{
public:
	CLogicHeroInsureActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicHomeInsureActive : public CLogicActiveBase
{
public:
	CLogicHomeInsureActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicWishCharacterActive : public CLogicActiveBase
{
public:
	CLogicWishCharacterActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicFlowerLanternActive : public CLogicActiveBase
{
public:
	CLogicFlowerLanternActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	static void AddCandle(user_data_table_ptr_type pUserData, int32_t iEvent, int32_t iEventValue);

	static int32_t AddCandleLog(user_data_table_ptr_type pUserData, const TLogicActiveFlowerLanternInfoElem& rstInfo, int32_t iActivityID, int32_t iEvent, int32_t iEventValue);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicPetInsureActive : public CLogicActiveBase
{
public:
	CLogicPetInsureActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicHeartInsureActive : public CLogicActiveBase
{
public:
    CLogicHeartInsureActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicSendFlowerActive : public CLogicActiveBase
{
public:
	CLogicSendFlowerActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};


/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicLuckyHammerActive : public CLogicActiveBase
{
public:
	CLogicLuckyHammerActive(user_data_table_ptr_type pUserData, int iActivityID);

	static void Notify(user_data_table_ptr_type pUserData, int iNum);

	bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

	int UpdateActiveInfo(int iNum) override;
};

/////////////////////////////////////////////////////////////////////
//////////////////
class CLogicLoginFundActive : public CLogicActiveBase
{
public:
    CLogicLoginFundActive(user_data_table_ptr_type pUserData, int iActivityID);

    static void Notify(user_data_table_ptr_type pUserData, int iNum);

    bool GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes = 1) override;

    int UpdateActiveInfo(int iNum) override;
};


///////////////////////////////////////////////////////////////////////////////////////////////

class CLogicActivityChangeList
{
public:

    static CLogicActivityChangeList* GetInstance();

    void NotifyClientUser(int iGroupID, int iUin);

    inline std::map<int, int>& GetActivityChangeMap() { return m_stActivityChangeMap; }

private:
    //key:      iActivityID
    //value:    iRedPoint
    std::map<int, int>          m_stActivityChangeMap;

    static CLogicActivityChangeList*  s_pInstance;
};

#define LOGIC_ACTIVITY_CHANGE_LIST_INS CLogicActivityChangeList::GetInstance()
