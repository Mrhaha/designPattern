#pragma once

#include "server_entire_war_protocol.h"
#include "logic_game_processor_base.h"

class CLogicEntireWarProcessor : public CLogicProcessorBase
{
public:
    enum
    {
        EPRO_Invalid = 0,
        EPRO_Boss = 1,
        EPRO_Box = 2,
        EPRO_Empty = 3,
    };

    enum
    {
        EAPT_Refuse = 0,
        EAPT_Accept = 1,
        EAPT_OneKeyRefuse = 2,
        EAPT_OneKeyAccept = 3,
    };

public:
	CLogicEntireWarProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
	bool DoUserRun(const msgpack::unpacked& stFormatData) override;

private:
	bool DoUserRunGetEntireWarInfo(const msgpack::unpacked& stFormatData);

	bool DoUserRunFightEntireWarStart(const msgpack::unpacked& stFormatData);

	bool DoUserRunFightEntireWarResult(const msgpack::unpacked& stFormatData);

	bool DoUserRunSweepEntireWar(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetEntireWarMarket(const msgpack::unpacked& stFormatData);

	bool DoUserRunBuyEntireWarMarket(const msgpack::unpacked& stFormatData);

	bool DoUserRunDigEntireWarTreasure(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetEntireWarQuestion(const msgpack::unpacked& stFormatData);

	bool DoUserRunAnswerEntireWarQuestion(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetEntireWarMapAward(const msgpack::unpacked& stFormatData);

	bool DoUserRunUpgradeEntireMilitaryLevel(const msgpack::unpacked &stFormatData);

	bool DoUserRunGetEntireWarFinalAward(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetEntireWarRank(const msgpack::unpacked& stFormatData);

	bool DoUserRunWorshipEntireWarGod(const msgpack::unpacked& stFormatData);

	bool DoUserRunBuyActionCoin(const msgpack::unpacked& stFormatData);

    bool DoUserSimpleGameEntireWar(const msgpack::unpacked& stFormatData);

    /////////////////////////////////////////////////////////////////////////
    bool DoUserRunLimitBossLevelBegin(const msgpack::unpacked& stFormatData);
    bool DoUserRunLimitBossLevelResult(const msgpack::unpacked& stFormatData);
    bool DoUserRunLimitBossInvite(const msgpack::unpacked& stFormatData);
    bool DoUserRunLimitBossOpenInviteList(const msgpack::unpacked& stFormatData);
    bool DoUserRunLimitBossAccept(const msgpack::unpacked& stFormatData);
    bool DoUserRunLimitBossOpenList(const msgpack::unpacked& stFormatData);
    bool DoUserRunLimitBossGetBox(const msgpack::unpacked& stFormatData);
    bool DoUserRunLimitBossOpenFriendList(const msgpack::unpacked& stFormatData);
    bool DoUserRunLimitBossRefreshOpenBoss(const msgpack::unpacked& stFormatData);

private:
    void RefreshSelfEntireWar();

    void AutoAddActionCoin();

    bool IsUserEntireWarTreasureDigOut(int32_t iWarID);

    bool IsUserEntireWarExtraAward(int32_t iWarID, int32_t iExtraID);

    void SetUserEntireWarExtraAward(int32_t iWarID, int32_t iExtraID);

    int32_t GetNewUserEntireWarQuestion(int32_t iWarID);

    bool CheckUserEntireWarTeamMember(int32_t iWarID, const std::vector<int32_t> &TeamVec);

    bool CheckUserEntireWarTeamMember(int32_t iType, int32_t iValue, const std::vector<int32_t>& iValidTeamVec);

    CPackEntireWarInfo GetUserEntireWarInfoPack();

    CPackEntireWarMap GetUserEntireWarMapPack(int32_t iWarID);

    std::vector<CPackMarket> GetUserEntireWarMarketPack(int32_t iWarID);

	//加日志
	void AddReport(int32_t iWarID, const std::vector<int> &params);

private:
    ////////////////////////////////////////////////限时BOSS/////////////////////////////////////////////////////
    // 限时BOSS随机判定
    void RandLimitBoss();
};

