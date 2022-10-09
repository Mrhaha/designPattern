#include "logic_game_publisher.h"
#include "logic_game_multi_step_task.h"
#include "logic_game_user_stat.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_global.h"
#include "logic_game_active_info.h"

CLogicPublisher::CLogicPublisher()
{
    // Muti Step Task
    SubscribeGameItemAddChannel(CLogicMultiStepTaskBase::SubscribeGameItemAddInfoCallBack);
    SubscribeGameLevelChannel(CLogicMultiStepTaskBase::SubscribeGameLevelInfoCallBack);
    SubscribeGameCardChannel(CLogicMultiStepTaskBase::SubscribeGameCardInfoCallBack);
    SubscribeGameMallChannel(CLogicMultiStepTaskBase::SubscribeGameMallInfoCallBack);
    SubscribeGameCombatChannel(CLogicMultiStepTaskBase::SubscribeGameCombatInfoCallBack);
    SubscribeGameUserLevelUpChannel(CLogicMultiStepTaskBase::SubscribeGameUserLevelUpInfoCallBack);
	SubscribeGameUserActionChannel(CLogicMultiStepTaskBase::SubscribeGameUserActionInfoCallBack);
	SubscribeClimbTowerOrderChannel(CLogicMultiStepTaskBase::SubscribeClimbTowerOrderCallBack);
	SubscribePvpTerritoryOrderChannel(CLogicMultiStepTaskBase::SubscribePvpTerritoryOrderCallBack);

    // Daily Task
    SubscribeGameLevelChannel(CLogicDailyTaskSystem::SubscribeGameLevelInfoCallBack);
    SubscribeGameCardChannel(CLogicDailyTaskSystem::SubscribeGameCardInfoCallBack);
    SubscribeGameMallChannel(CLogicDailyTaskSystem::SubscribeGameMallInfoCallBack);
    SubscribeGameUserActionChannel(CLogicDailyTaskSystem::SubscribeGameUserActionInfoCallBack);
    SubscribeGameHeroEquipmentChannel(CLogicDailyTaskSystem::SubscribeGameHeroEquipmentCallBack);
	SubscribeFunctionalGameItemChannel(CLogicDailyTaskSystem::SubscribeFunctionalGameItemInfoCallBack);
	SubscribeAwardActivityMultiTaskChannel(CLogicDailyTaskSystem::SubscribeAwardActivityMultiTaskCallBack);

    // Active
    SubscribeGameDepositChannel(CLogicActiveBase::SubscribeGameDepositInfoCallBack);
    SubscribeGameUserLevelUpChannel(CLogicActiveBase::SubscribeGameUserLevelUpInfoCallBack);
    SubscribeGameLevelChannel(CLogicActiveBase::SubscribeGameLevelInfoCallBack);
    SubscribeGamePVPRankChannel(CLogicActiveBase::SubscribeGamePVPRankCallBack);
    SubscribeGameUnlimitLevelScoreChannel(CLogicActiveBase::SubscribeGameUnlimitLevelScoreCallBack);
    SubscribeGameAddLevelStarChannel(CLogicActiveBase::SubscribeGameAddLevelStarCallBack);
    SubscribeGameHeroEquipmentChannel(CLogicActiveBase::SubscribeGameHeroEquipmentCallBack);
    SubscribeGameCardChannel(CLogicActiveBase::SubscribeGameCardInfoCallBack);
    SubscribeGameItemAddChannel(CLogicActiveBase::SubscribeGameItemAddInfoCallBack);
    SubscribeGameUserActionChannel(CLogicActiveBase::SubscribeGameUserActionInfoCallBack);
    SubscribeGameCombatChannel(CLogicActiveBase::SubscribeGameCombatInfoCallBack);
    SubscribeGameMallChannel(CLogicActiveBase::SubscribeGameMallChannelInfoCallBack);
	SubscribeFunctionalGameItemChannel(CLogicActiveBase::SubscribeFunctionalGameItemInfoCallBack);
	SubscribeGameEnergyChannel(CLogicActiveBase::SubscribeGameEnergyCallBack);
	SubscribeClimbTowerOrderChannel(CLogicActiveBase::SubscribeClimbTowerOrderCallBack);
	SubscribePvpTerritoryOrderChannel(CLogicActiveBase::SubscribePvpTerritoryOrderCallBack);

	//stat
	SubscribeGameUserLevelUpChannel(CLogicUserStat::SubscribeGameUserLevelUpInfoCallBack);
	SubscribeFunctionalGameItemChannel(CLogicUserStat::SubscribeFunctionalGameItemInfoCallBack);
}
