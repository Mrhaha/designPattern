#include "server_cmd_logic_define.h"
#include "logic_game_error_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_bill.h"

CLogicCmdMap* CLogicCmdMap::m_pLogicCmdMap = nullptr;

CLogicCmdMap::CLogicCmdMap()
{
}

CLogicCmdMap&	
CLogicCmdMap::GetInstance()
{
	if(nullptr == m_pLogicCmdMap)
	{
		m_pLogicCmdMap = new CLogicCmdMap();
	}

	return (*m_pLogicCmdMap);
}

processor_base_ptr_type	
CLogicCmdMap::GetProcessor(int iCmd)
{
    if (const auto iter = m_stCmdProcessFactoryMap.find(iCmd); iter != m_stCmdProcessFactoryMap.end() && iter->second)
    {
        return iter->second();
    }

    return CLogicProcessorFactory<CLogicErrorProcessor>::Create(iCmd, "ERROR_CMD_" + std::to_string(iCmd));
}

void CLogicCmdMap::RegisterProcessorFactory(uint16_t usCmd, const std::string& strCmdName, logic_processor_factory_type pFactory)
{
    m_stCmdProcessFactoryMap[usCmd] = std::bind(pFactory, usCmd, strCmdName);
}
