#pragma once

#include "logic_game_processor_base.h"

class CLogicGameItemProcessor : public CLogicProcessorBase
{
public:
	CLogicGameItemProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
	bool DoUserRun(const msgpack::unpacked& stFormatData) override;

private:
	bool DoUserRunEquipFashion(const msgpack::unpacked& stFormatData);

	bool DoUserRunUnlockHonor(const msgpack::unpacked& stFormatData);

	bool DoUserRunEquipHonor(const msgpack::unpacked& stFormatData);

    bool DoUserRunPhyStoreExtract(const msgpack::unpacked& stFormatData);

	bool DoUserRunUseItem(const msgpack::unpacked& stFormatData);

private:
    int32_t UseFashionItem(const int32_t iItemID, const int32_t iNum);
    int32_t UseGiftBagItem(const int32_t iItemID, const int32_t iNum);
    int32_t UseBuffItem(const int32_t iItemID, const int32_t iNum, const int32_t iBuffID);
    int32_t UseExploreCangBaoTuItem(const int32_t iItemID, const int32_t iNum, const int32_t iQuestID);
};
