#pragma once

#include "logic_config_base.h"
#include "logic_game_processor_base.h"

class CLogicEquipmentProcessor : public CLogicProcessorBase
{
public:
	CLogicEquipmentProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;

private:
	bool DoCombineGameItem(const msgpack::unpacked& stFormatData);

    bool DoConstelDiv(const msgpack::unpacked& stFormatData);
    bool DoConstelLevelUp(const msgpack::unpacked& stFormatData);
    bool DoConstelEquip(const msgpack::unpacked& stFormatData);
    bool DoConstelOnekeyUnload(const msgpack::unpacked& stFormatData);
    bool DoConstelLockOrDiscard(const msgpack::unpacked& stFormatData);

    bool DoConstelPresetSave(const msgpack::unpacked& stFormatData);
    bool DoConstelPresetDel(const msgpack::unpacked& stFormatData);
    bool DoConstelPresetEquip(const msgpack::unpacked& stFormatData);
    bool DoConstelPresetList(const msgpack::unpacked& stFormatData);

    bool DoEquipmentEquip(const msgpack::unpacked& stFormatData);
    bool DoEquipmentDiv(const msgpack::unpacked& stFormatData);
    bool DoEquipmentLock(const msgpack::unpacked& stFormatData);

    bool DoEquipmentAddExp(const msgpack::unpacked& stFormatData);
    bool DoEquipmentUpgrade(const msgpack::unpacked& stFormatData);
    bool DoEquipmentRefine(const msgpack::unpacked& stFormatData);
    bool DoEquipmentRefineOp(const msgpack::unpacked& stFormatData);


private:
    int ConstelOnekeyUnload(int iCardID);
    int ConstelEquip(int iCardID, int iConstelID);
};
