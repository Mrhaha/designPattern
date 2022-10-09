//
// Created by luoxiaohu on 2020/9/24.
//

#include "logic_game_common_publisher.h"
#include "logic_game_common_observer.h"

CLogicCommonPublisher::CLogicCommonPublisher()
{
    /******************************************世  界  任  务*****************************************************/
    RegisterObserver(CLogicConfigDefine::ECET_RoleLevelUp,          CLogicCommonObserver::NotifyWorldQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_RoleCombatUp,         CLogicCommonObserver::NotifyWorldQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_Tower,                CLogicCommonObserver::NotifyWorldQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_PvELevelFirstFight,   CLogicCommonObserver::NotifyWorldQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_CardCommodityGradeNum, CLogicCommonObserver::NotifyWorldQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_GoldEggTimes,         CLogicCommonObserver::NotifyWorldQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_XunBaoTimes,          CLogicCommonObserver::NotifyWorldQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_LuckyTurnTableTimes,  CLogicCommonObserver::NotifyWorldQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_LuckyHammerTimes,     CLogicCommonObserver::NotifyWorldQuestEvent);
    /************************************************************************************************************/

    /******************************************红  包  事  件*****************************************************/
    RegisterObserver(CLogicConfigDefine::ECET_CardCommodityGradeNum,    CLogicCommonObserver::NotifyNewRedBagEvent);
    RegisterObserver(CLogicConfigDefine::ECET_LuckyHammerFinalAward,    CLogicCommonObserver::NotifyNewRedBagEvent);
    RegisterObserver(CLogicConfigDefine::ECET_LuckyTurnTableFinalAward, CLogicCommonObserver::NotifyNewRedBagEvent);
    RegisterObserver(CLogicConfigDefine::ECET_XunBaoFinalAward,         CLogicCommonObserver::NotifyNewRedBagEvent);
    RegisterObserver(CLogicConfigDefine::ECET_MultiColorSpecialAward,   CLogicCommonObserver::NotifyNewRedBagEvent);
    /************************************************************************************************************/

    /****************************************调查问卷部分事件实时**************************************************/
    RegisterObserver(CLogicConfigDefine::ECET_RoleLevelUp,          CLogicCommonObserver::NotifyQuestionnaireEvent);
    /************************************************************************************************************/

    /********************************************** 家园经验事件   *******************************************************/
    RegisterObserver(CLogicConfigDefine::ECET_GetHomeLevelItem,         CLogicCommonObserver::NotifyHomeLevelChangeEvent);
    RegisterObserver(CLogicConfigDefine::ECET_GetHomeLevelFish,         CLogicCommonObserver::NotifyHomeLevelChangeEvent);
    RegisterObserver(CLogicConfigDefine::ECET_GetHomeBuffItem,         CLogicCommonObserver::NotifyHomeLevelChangeEvent);


    /********************************************** 任 务 *******************************************************/
    RegisterObserver(CLogicConfigDefine::ECET_CardCommodityGradeNum,    CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_CardCommoditySameCardNum, CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_RoleLevelUp,              CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_EquipGradeNum,            CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_ConstelGradeNum,          CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_PvELevelFirstFight,       CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_Login,                    CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FightPvPTimes,            CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FightPvPWinTimes,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_LevelTypeTimes,           CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_UpgradeCardLevel,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_HomeExploreTimes,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_HomeExploreCompleteTimes, CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_HomeVisitTimes,           CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_UpgradeCardTimes,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_BuyEnergyTimes,           CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_QuestCondIndexTimes,      CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_UpgradeCardColor,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FishingSuccessTimes,      CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_CardHeartLevel,           CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_ConstelLevelUp,           CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_DivConstelTimes,          CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_PvPRank,                  CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_CardGradeNum,             CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_RuneCardNum,              CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_UpgradeCardStar,          CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_CardCommodityNum,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_HomeOrnamentNum,          CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_UninterruptedCardCommodityNoGrade,    CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_ConstelURSuitNum,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_ChapterAchieveCompleteNum,CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_CurGoldNum,               CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_ExploreLevel,             CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FishingBigFish,           CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FishingKingFish,          CLogicCommonObserver::NotifyQuestEvent);

    RegisterObserver(CLogicConfigDefine::ECET_WipeFightTotalNum,        CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_WipeFightOnceNum,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_JoinGuild,                CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_RefreshLevelTimes,        CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_UseTalentPoint,           CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_PresentGuildWish,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_UpLevelConstelMainAtr,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_CreateBanquetUseItem,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_JoinFriendBanquet,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_GuildWish,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_CreateBanquet,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FishingTargetFish,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FishingTotalBigFish,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FishingTotalKingFish,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FightPvPContinueWinTimes,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FightPvpNotJoinType,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FightPvpLeftAliveWin,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FightPvpAllSameType,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FightPvpWinInSecond,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FightPvpNotWinInSecond,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_FightPvpLeftAliveLose,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_UserBindEmail,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_ConsumeGoldCoin,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_ConsumeDiamond,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_TotalMonster,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_HarvestCount,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_HotSpringCount,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_PutOnConstelSuit,         CLogicCommonObserver::NotifyQuestEvent);

    RegisterObserver(CLogicConfigDefine::ECET_GetTargetHeroCard,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_UpStarsHeroCard,         CLogicCommonObserver::NotifyQuestEvent);
    RegisterObserver(CLogicConfigDefine::ECET_UpGradeHeroCard,         CLogicCommonObserver::NotifyQuestEvent);


    /************************************************************************************************************/
}
