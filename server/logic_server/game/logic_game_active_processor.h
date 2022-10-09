#pragma once

#include "logic_game_processor_base.h"

class CLogicActiveProcessor : public CLogicProcessorBase
{
public:
	CLogicActiveProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;

private:
	bool DoUserRunGetActiveInfo(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetAllActiveInfo(const msgpack::unpacked& stFormatData);

	bool DoUserRunReceiveActiveBonus(const msgpack::unpacked& stFormatData);

	bool DoUserRunAwardActivityMultiTask(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetActivityTaskAward(const msgpack::unpacked& stFormatData);
    bool DoUserRunGetSequenceTaskReward(const msgpack::unpacked& stFormatData);
	bool DoUserRunGetActivityCountDownHero(const msgpack::unpacked& stFormatData);//抽取限时武将
	bool DoUserRunGetRankCountDownHero(const msgpack::unpacked& stFormatData);//获取限时武将排行
	bool DoUserRunGetLuckyCatHistory(const msgpack::unpacked& object_handle);//获取招财猫历史记录

	bool DoUserRunGetFlowerLanternLog(const msgpack::unpacked& stFormatData);

	bool DoUserRunMergeActivityTaskAward(const msgpack::unpacked& stFormatData);

	bool DoUserRunMergeActivityExchange(const msgpack::unpacked& stFormatData);

	bool DoUserRunFightMergeActivity(const msgpack::unpacked& stFormatData);

	bool DoUserRunFightMergeActivityResult(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetDaySingleRechargeInfo(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetDaySingleRechargeReward(const msgpack::unpacked& stFormatData);

    bool DoUserRunGetWorldQuestInfo(const msgpack::unpacked& stFormatData);

    // 新活动系统
    bool DoUserRunGetAvyReward(const msgpack::unpacked& stFormatData);

private:
	CPackRankCountdownHero GetCountdownHeroPlayerInfo(int iGroupID, int iUin, int iScore, int iRankAmount);

	TMergeActivityTable* FetchMergeActivityTable(int32_t iTaskID);
};
