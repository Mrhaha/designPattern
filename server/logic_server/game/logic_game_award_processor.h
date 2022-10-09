#pragma once

#include "logic_game_processor_base.h"

class CLogicAwardProcessor : public CLogicProcessorBase
{
public:
	CLogicAwardProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
	bool DoUserRun(const msgpack::unpacked& stFormatData) override;

private:
	bool DoUserRunGetBankInfo(const msgpack::unpacked& stFormatData);

	bool DoUserRunBuyBankGold(const msgpack::unpacked& stFormatData);

	bool DoUserRunSellBankGold(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetHappyTurnCard(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetTurnCardAward(const msgpack::unpacked& stFormatData);

	bool DoUserRunTurningHappyCard(const msgpack::unpacked& stFormatData);

	bool DoUserRunSelectMultiColorLantern(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetVipDailyAward(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetVipQuarterDailyAward(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetBuyBackCount(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetRecommendedLineupAward(const msgpack::unpacked& stFormatData);

	bool DoUserRunSendRedBag(const msgpack::unpacked& stFormatData);

	bool DoUserRunReceiveRedBag(const msgpack::unpacked& stFormatData);

    bool DoUserRunWorldQuestReward(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetRedBagList(const msgpack::unpacked& stFormatData);

	bool DoUserRunCheckRedBagValid(const msgpack::unpacked& stFormatData);

	bool DoUserRunSysOtherBuy(const msgpack::unpacked& stFormatData);

    bool DoUserRunChangeAgeLimit(const msgpack::unpacked& stFormatData);

private:
	bool HaveAllHero(const std::vector<int32_t>& iHeroVec) const;
	bool DinnerFixBuy(user_data_table_ptr_type pUserInfo, int32_t iActivityID, int32_t iIndex, std::vector<CPackGameItem>& stBonusList);
};


