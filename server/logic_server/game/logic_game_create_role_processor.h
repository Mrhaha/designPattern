#pragma once

#include "logic_game_processor_base.h"

class CLogicCreateRoleProcessor : public CLogicProcessorBase
{
public:
	CLogicCreateRoleProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData);
    
private:
	std::string GenServerNickTableSQL(const std::string& strNick) const;
    user_data_table_ptr_type CreateRole(int iCardID, const std::string& strNick) const;

    bool DebugRefreshRoleData();
};
