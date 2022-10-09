#pragma once

#include "logic_config_grow.h"
#include "logic_game_cache_table.h"
#include "logic_game_publisher.h"

class CLogicMultiStepTaskBase
{
public:
    struct multi_step_task_info_t
    {
        int         m_iGroupID;
        int         m_iUin;
        int         m_iTaskStep;
        int         m_iTargetNum;
    };
    
    typedef std::map<int, multi_step_task_info_t> changed_muti_step_task_info_map_type;
    
public:
    static int GetCompletedMutiTaskAmount(user_data_table_ptr_type pUserData);

    static TMutiStepTaskTableValueType InitMutiStepTaskTableValue(int iGroupID, int iUin, int iTaskID, int iTargetNum);
    
    static changed_muti_step_task_info_map_type& GetChangedMutiStepTask();
    
    static changed_muti_step_task_info_map_type GetChangedMutiStepTaskAndClear();

    static void SubscribeGameLevelInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_level_channel_data_t& stData);

    static void SubscribeGameCardInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_card_channel_data_t& stData);

    static void SubscribeGameItemAddInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_item_add_channel_data_t& stData);

    static void SubscribeGameUserLevelUpInfoCallBack(user_data_table_ptr_type pUserData);

    static void SubscribeGameCombatInfoCallBack(user_data_table_ptr_type pUserData);

	static void SubscribeGameUserActionInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_user_action_channel_data_t& stData);

	static void SubscribeClimbTowerOrderCallBack(user_data_table_ptr_type pUserData, const CLogicClimbTowerOrderChannel::data_type& stData);

	static void SubscribePvpTerritoryOrderCallBack(user_data_table_ptr_type pUserData, const CLogicPvpTerritoryOrderChannel::data_type& stData);

    static void SubscribeGameMallInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_mall_channel_data_t& stData);

public:
    CLogicMultiStepTaskBase(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData);
    
    virtual ~CLogicMultiStepTaskBase() {  };
    
    TMutiStepTaskTable* GetTaskInfo();
    
    bool IsInReceiveBonusStatus(const TMutiStepTaskTable* pTaskInfo) const;
    
    void CheckTask(int iNum = 1);
    
    user_data_table_t::muti_step_task_container_type::iterator ReceiveTask(const TLogicMutiStepTaskConfigStepDetail& stMutiTaskStepConfig, int iNum);
    
    std::pair<CLogicMultiStepTaskBase::multi_step_task_info_t, std::vector<CPackGameItem>> GoToNextStepAndGetBonusConfig(TMutiStepTaskTable& stTaskInfo);
    
protected:    
    virtual int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) = 0;
    
protected:
    void ChangeBonusState(TMutiStepTaskTable& stTaskInfo);
    
    bool IsInReceiveBonusStatus(const TMutiStepTaskTable& stUserTaskInfo) const;
    
    const TLogicMutiStepTaskConfigStepDetail* GetTaskStepConfig(int iTaskStep) const;
    
    bool IsTaskCanOpen() const;
    
    void NotifyClientTaskChanged(const TMutiStepTaskTable& stTaskInfo);
    
protected:
    const TLogicMutiStepTaskConfigElem&    m_stTaskConfig;
    user_data_table_t&                           m_stUserData;
    
protected:
    static changed_muti_step_task_info_map_type     m_stChangedMutiStepTaskList;

};

class CRkLogicMutiStepTaskFactory
{
public:
    enum MUTI_STEP_TASK_TYPE
    {
        MUTI_STEP_TASK_TYPE_START = 0,
        PASS_NORMAL_CHAPTER_TASK_TYPE = 1,
        PASS_ELITE_CHAPTER_TASK_TYPE = 2,
        PASS_DEVIL_CHAPTER_TASK_TYPE = 3,
        HAVE_MANY_HERO_CARDS_TASK_TYPE = 4,
        HAVE_MANY_SPECIFIED_COLOR_HERO_CARDS_TASK_TYPE = 5,
        HAVE_MANY_SPECIFIED_STAR_HERO_CARDS_TASK_TYPE = 6,
        UPGRADE_ROLE_LEVEL_TASK_TYPE = 7,
		TOTAL_ONLINE_TASK_TYPE = 8,
		PVP_TERRITORY_ORDER_TASK_TYPE = 9,
		TACTICS_COUNT_TASK_TYPE = 10,
		CLIMB_TOWER_ORDER_TASK_TYPE = 11,
        SIGN_TOTAL_TIMES_TASK_TYPE = 12,            // 累计签到N天
        GUILD_TASK_STAR_TASK_TYPE = 13,             // 接取5星公会委托 target_amount 次
        PASS_GOLD_LEVEL_TASK_TYPE = 14,             // 通关金币副本 target_amount 次
        FRIEND_MEME_TASK_TYPE = 15,                 // 好友么么N次
        UPGRADE_ATK_HERO_COLOR_TASK_TYPE = 16,      // N个攻击型伙伴进阶到M阶
        UPGRADE_DEF_HERO_COLOR_TASK_TYPE = 17,      // N个防御型伙伴进阶到M阶
        UPGRADE_SUP_HERO_COLOR_TASK_TYPE = 18,      // N个辅助型伙伴进阶到M阶
        PVP_SUCCESS_TIMES_TASK_TYPE = 19,           // 竞技场胜利 target_amount 次
        YUANBAO_COMMODITY_TASK_TYPE = 20,           // 进行 target_amount 次砖石召唤

        ROLE_COMBAT_TASK_TYPE = 100,
        PASS_NORAML_CHAPTER_TIMES_TASK_TYPE = 101,
        MUTI_STEP_TASK_TYPE_END
    };
    
public:
    typedef std::shared_ptr<CLogicMultiStepTaskBase> (*CREATE_MUTI_STEP_TASK_FUNC_TYPE)(user_data_table_t&, const TLogicMutiStepTaskConfigElem&);

public:
    static std::shared_ptr<CLogicMultiStepTaskBase> CreateTask(user_data_table_t& stUserData, enum CRkLogicMutiStepTaskFactory::MUTI_STEP_TASK_TYPE iTaskID);
    
private:
    template<class T> static std::shared_ptr<CLogicMultiStepTaskBase> CreateTaskIMP(user_data_table_t& stUserData, const TLogicMutiStepTaskConfigElem& stTaskConfigElem)
    {
        return (std::shared_ptr<CLogicMultiStepTaskBase>(new T(stTaskConfigElem, stUserData)));
    }
    
private:
    static std::map<enum MUTI_STEP_TASK_TYPE, CREATE_MUTI_STEP_TASK_FUNC_TYPE>      MUTI_STEP_TASK_CREATOR_MAP;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
class CRkLogicPassNormalChapterMutiStepTask : public CLogicMultiStepTaskBase
{
public:
    friend class CRkLogicMutiStepTaskFactory;
    
private:
    CRkLogicPassNormalChapterMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData);
    
protected:    
    int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};

class CRkLogicPassEliteChapterMutiStepTask : public CLogicMultiStepTaskBase
{
public:
    friend class CRkLogicMutiStepTaskFactory;
    
private:
    CRkLogicPassEliteChapterMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData);
    
protected:    
    int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};

class CRkLogicPassDevilChapterMutiStepTask : public CLogicMultiStepTaskBase
{
public:
    friend class CRkLogicMutiStepTaskFactory;
    
private:
    CRkLogicPassDevilChapterMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData);
    
protected:
    int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};

class CRkLogicHaveManyHeroCardsMutiStepTask : public CLogicMultiStepTaskBase
{
public:
    friend class CRkLogicMutiStepTaskFactory;
    
private:
    CRkLogicHaveManyHeroCardsMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData);
    
protected:
    int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};

class CRkLogicHaveManySpecifiedColorHeroCardsMutiStepTask : public CLogicMultiStepTaskBase
{
public:
    friend class CRkLogicMutiStepTaskFactory;
    
private:
    CRkLogicHaveManySpecifiedColorHeroCardsMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData);
    
protected:
    int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};

class CRkLogicHaveManySpecifiedStarHeroCardsMutiStepTask : public CLogicMultiStepTaskBase
{
public:
    friend class CRkLogicMutiStepTaskFactory;
    
private:
    CRkLogicHaveManySpecifiedStarHeroCardsMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData);
    
protected:
    int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};

class CRkLogicUpgradeRoleLevelMutiStepTask : public CLogicMultiStepTaskBase
{
public:
    friend class CRkLogicMutiStepTaskFactory;
    
private:
    CRkLogicUpgradeRoleLevelMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData);
    
protected:
    int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};

class CRkLogicRoleCombatMutiStepTask : public CLogicMultiStepTaskBase
{
public:
    friend class CRkLogicMutiStepTaskFactory;
    
private:
    CRkLogicRoleCombatMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData);
    
protected:
    int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};

class CRkLogicClimbTowerOrderMutiStepTask : public CLogicMultiStepTaskBase
{
public:
	friend class CRkLogicMutiStepTaskFactory;

private:
	CRkLogicClimbTowerOrderMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData);

protected:
	int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};

class CRkLogicPvpTerritoryOrderMutiStepTask : public CLogicMultiStepTaskBase
{
public:
	friend class CRkLogicMutiStepTaskFactory;

private:
	CRkLogicPvpTerritoryOrderMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData);

protected:
	int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};

class CRkLogicTacticsCountMutiStepTask : public CLogicMultiStepTaskBase
{
public:
	friend class CRkLogicMutiStepTaskFactory;

private:
	CRkLogicTacticsCountMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData);

protected:
	int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};

class CRkLogicTotalOnlineMutiStepTask : public CLogicMultiStepTaskBase
{
public:
	friend class CRkLogicMutiStepTaskFactory;

private:
	CRkLogicTotalOnlineMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData);

protected:
	int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};

// 累加类成就任务通用
class CRkLogicAddUpTypeMutiStepTask : public CLogicMultiStepTaskBase
{
public:
    friend class CRkLogicMutiStepTaskFactory;

private:
    CRkLogicAddUpTypeMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
        : CLogicMultiStepTaskBase(stConfig, stUserData) {}

protected:
    int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};

// 统计 攻击/防御/辅助型伙伴进阶到指定阶的数量
class CRkLogicUpgradeSpecialHeroColorMutiStepTask : public CLogicMultiStepTaskBase
{
public:
    friend class CRkLogicMutiStepTaskFactory;

private:
    CRkLogicUpgradeSpecialHeroColorMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
        : CLogicMultiStepTaskBase(stConfig, stUserData) {}

protected:
    int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};

class CRkLogicPvpSuccessTimesMutiStepTask : public CLogicMultiStepTaskBase
{
public:
    friend class CRkLogicMutiStepTaskFactory;

private:
    CRkLogicPvpSuccessTimesMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
        : CLogicMultiStepTaskBase(stConfig, stUserData) {}

protected:
    int CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum = 1) override;
};
