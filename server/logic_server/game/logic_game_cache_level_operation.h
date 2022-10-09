#pragma once

#include "server_fight_protocol.h"
#include "logic_config_fight.h"
#include "logic_game_cache_table.h"
#include "server_new_limit_boss_protocol.h"

class CLogicCacheLevelOperation
{
public:
    CLogicCacheLevelOperation(user_data_table_ptr_type pUserData);

    char CalcLevelStar(const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam);

    static std::pair<user_data_table_t::level_info_container_type::iterator, bool>
    GetUserLevelInfo(user_data_table_ptr_type pUserData, int iLevelID);

    TLevelInfoTable* GetOrCreateLevelInfo(int iLevelID);

    static std::pair<uint64_t, bool>
    CheckGetChapterBonus(user_data_table_ptr_type pUserData,
                        const TLogicGameChapterElem* pstChapterConfigElem,
                        int iChapterID, int iPosition);

    static const std::vector<CPackGameItem>*
    GetChapterBonus(const TLogicGameChapterElem* pstChapterConfigElem, int iPosition);

    static uint64_t
    GetChapterBonusBitmap(user_data_table_ptr_type pUserData, int iChapterID, int iPosition);

    static void
    SetChapterBonusBitmap(user_data_table_ptr_type pUserData, int iChapterID, int iPosition, uint64_t uiBitmap);

    bool UpdateLevelTimes(TLevelInfoTable* pLevelData, const TLogicLevelElem* pConfig, int iTimes = 1);

    bool CheckFightLevelID(int iLevelID);

    bool CheckCanFightLevel(int iLevelID, const TLogicLevelElem* pLevelInfoConfigElem);

    bool CheckLevelCommonLimit(TLevelInfoTable* pLevelData, const TLogicLevelElem* pConfig, int extraLevel = 0,int iTimes = 1);
    bool CheckLevelFightBefore(TLevelInfoTable* pLevelData, const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam);
    bool HandleLevelFightAfter(TLevelInfoTable* pLevelData, const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam);

    bool CheckRspLevelDrop(const std::vector<CPackGameItem>& stRspBonus);

    void FlushLevelTimes(int iLevelID);

    int32_t GetOtherLevelReward(int iLevelID, const TLogicLevelElem* pConfig, uint64_t ulPara, int extraLevelID,std::vector<CPackGameItem>& stRewards,std::vector<CPackGameItem>& stExtraReward);

    void CalcExtraLevelReward(int iLevelID, const TLogicLevelElem* pConfig,std::vector<CPackGameItem>& stExtraReward,CPackTotalItemData* totalItemData);

    bool IsRefreshLimitBoss(int levelID);

    void InitLimitBoss(CPackNewLimitBossInfo* bossInfo);




private:
    bool CalcGoldCollectingRate(const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam, const TLogicEvaluateElem& stEvaluate);
    bool CalcMonsterKillRate(const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam, const TLogicEvaluateElem& stEvaluate);
    bool CalcDeadRoleNum(const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam, const TLogicEvaluateElem& stEvaluate);
    bool CalcTeamRemainHPRate(const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam, const TLogicEvaluateElem& stEvaluate);
    bool CalcBOSSTimeLimit(const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam, const TLogicEvaluateElem& stEvaluate);
    bool CalcGreaterAmount(const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam, const TLogicEvaluateElem& stEvaluate);

private:
    user_data_table_ptr_type                 m_pUserData;
};
