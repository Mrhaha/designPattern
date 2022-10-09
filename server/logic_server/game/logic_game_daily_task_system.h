#pragma once

#include "logic_config_active.h"
#include "logic_config_grow.h"
#include "logic_game_cache_table.h"
#include "logic_game_publisher.h"

class CLogicDailyTaskSystem
{
public:
    enum DAILY_TASK_STATUS
    {
        TASK_UNDER_WAY = 0,
        TASK_HAS_ACHIEVE_GIFT = 1,
    };

    enum DAILY_TASK_TYPE
    {
        CHAPTER_TYPE_TASK = 2,
        UPGRADE_CARD_TASK = 3,
        BUY_CARD_TASK = 4,
        UPGRADE_SKILL_TASK = 5,
        BENEFITS_CARD_TASK = 6,
        USER_GAME_ACTION_TASK = 7,
        FIGHT_LEVEL_TASK = 8,
        EQUIPMENT_UPGRADE_LEVEL_TASK = 9,
        UPGRADE_CARD_COLOR_TASK = 10,
        CONSUME_GAME_ITEM_TASK = 11,
        LOGIN_TASK = 12,
        WORLD_TALK_TASK = 13,
		ACTIVITY_MULTI_TASK_AWARD = 14,
		HERO_STAR = 15,
		TEAM_LEVEL = 16,
		SEND_TARGET_ITEM = 17,
		USER_GAME_STATUS_TASK = 18,
		GET_GAME_ITEM_TASK = 19,
		CONSUME_TARGET_ITEM_TASK = 20,
		ONCE_SEND_TARGET_ITEM = 21,
		ONCE_RECV_TARGET_ITEM = 22,
		// SUBMIT_ITEMS = 23,          // 提交指定道具
		GET_VIRTUAL_ITEM = 24,      // 获得指定虚拟道具
        UPGRADE_CARD_LEVEL = 25,    // 伙伴升级
        HERO_HEART_LEVEL = 26,      // 伙伴好感度等级
        HERO_HEART_GIFT = 27,       // 伙伴好感度赠送礼物
        HERO_HEART_CHAPTER = 28,    // 伙伴好感度副本
        HERO_HEART_DATING = 29,     // 伙伴约会
        UPGRADE_CARD_HEART_LEVEL = 30,  // 伙伴好感度升级
    };

	enum USER_TASK_STATUS
	{
		TASK_STATUS_CHARM = 1,				//魅力值
		TASK_STATUS_HOME_ENV_EXP = 2,		//家园环境值
		TASK_STATUS_TOWER_MAX = 3,			//镇魂之塔最高层数
		TASK_STATUS_TERRITORY_MAX = 4,		//步步为营最高层数
		TASK_STATUS_SUM_GUILD_SKILL = 5,	//公会技能等级之和
		TASK_STATUS_SUM_GEM_LEVEL = 6,		//任意角色镶嵌宝石等级之和
		TASK_STATUS_FRIEND_COUNT = 7,		//好友数量
		TASK_STATUS_TALENT_VALUE = 8,		//图鉴评分
		TASK_STATUS_MATCH_PVP_LEVEL = 9,	//巅峰对决段位等级
		TASK_STATUS_CARD_COUNT = 10,		//拥有伙伴数量
		TASK_STATUS_PET_COUNT = 11,			//拥有灵宠数量
		TASK_STATUS_FASHION_COUNT = 12,		//拥有时装数量
		TASK_STATUS_HONOR_COUNT = 13,		//拥有永久称号数量
		TASK_STATUS_ENTIRE_MILITARK_LEVEL = 14,	//全民战役军衔等级
		TASK_STATUS_XUNCHONG_LEVEL = 15,		//驯宠等级
		TASK_STATUS_EQUIP1_MAX_STAR = 16,		//任意角色神兵星级
		TASK_STATUS_EQUIP2_MAX_STAR = 17,		//任意角色战甲星级
		TASK_STATUS_AMULET_MAX_LEVEL = 18,		//命符最大等级
		TASK_STATUS_TACTICS_SUM_LEVEL = 19,		//战术总等级
		TASK_STATUS_WUSHEN_LEVEL20_COUNT = 20,	//星魂等级20级以上的星魂个数
		TASK_STATUS_TACTICS_COUNT = 21,			//战术的个数
		TASK_STATUS_AVATAR_BORDER_COUNT = 22,	//头像框的个数
        TASK_STATUS_CARD_EQUIP_STAR = 23,		//伙伴装备升级到指定星数
	};

public:
    static bool NotifyTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum);

	static bool SetProgress(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum);

    static void SubscribeGameLevelInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_level_channel_data_t& stData);

    static void SubscribeGameCardInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_card_channel_data_t& stData);

    static void SubscribeGameMallInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_mall_channel_data_t& stData);

    static void SubscribeGameUserActionInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_user_action_channel_data_t& stData);

    static void SubscribeGameHeroEquipmentCallBack(user_data_table_ptr_type pUserData, const logic_game_hero_equipment_channel_data_t& stData);

    static void SubscribeFunctionalGameItemInfoCallBack(user_data_table_ptr_type pUserData, const logic_functional_game_item_channel_data_t& stData);

	static void SubscribeAwardActivityMultiTaskCallBack(user_data_table_ptr_type pUserData, const logic_award_activity_multi_task_channel_data_t& stData);

private:
	static bool NotifyActivityTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum);

	static bool NotifyActivityMultiTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum);

	static bool SetActivityTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum);

	static bool NotifyMergeActivityTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum);

	static bool SetMergeActivityTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum);

	static bool NotifyHonorTaskProgress(user_data_table_ptr_type pUserData, bool bIsSetProgress, int iTaskType, int iTargetID, int iNum);

	static bool SetThemeActivityChargeTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum);

	static bool NotifyThemeActivityChargeTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum);

	static bool NotifyIBusyTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum);

    static bool SetMonthPassTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum);

    static bool NotifyMonthPassTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum);

private:
	static void AddActivityTaskProgress(user_data_table_ptr_type pUserData, int32_t iActivityType, int32_t iActivityID, int32_t iTaskID, int32_t iAddNum, const TLogicActivityTaskElem& stTaskElem);

};
