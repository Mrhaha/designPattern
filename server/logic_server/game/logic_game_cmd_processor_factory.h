#pragma once

#include <string>

#include "logic_game_processor_base.h"

#define _REGISTER_CMD_FACTORY_WITH_LINE_NUM(CMD, CMD_NAME, CLASS_NAME, LINE_NUM)          \
struct _static_##CLASS_NAME##LINE_NUM                                                                                  \
{                                                                                                                      \
    _static_##CLASS_NAME##LINE_NUM()                                                                                   \
    {                                                                                                                  \
        CLogicCmdMap::GetInstance().RegisterProcessorFactory((int)CMD, CMD_NAME, &CLogicProcessorFactory<CLASS_NAME>::Create); \
    }                                                                                                                  \
};                                                                                                                     \
static _static_##CLASS_NAME##LINE_NUM g_stInstance##CLASS_NAME##LINE_NUM;

#define REGISTER_CMD_FACTORY_WITH_LINE_NUM(CMD, CLASS_NAME, LINE_NUM) _REGISTER_CMD_FACTORY_WITH_LINE_NUM(CMD, #CMD, CLASS_NAME, LINE_NUM)

#define REGISTER_CMD_FACTORY(CMD, CLASS_NAME) REGISTER_CMD_FACTORY_WITH_LINE_NUM(CMD, CLASS_NAME, __LINE__)

#define LOGIC_CMD_CONTAINER CLogicCmdMap::GetInstance()

class CLogicCmdMap
{
public:
	typedef processor_base_ptr_type (*logic_processor_factory_type)(uint16_t usCmd, const std::string& strCmdName);
public:
	static CLogicCmdMap&			GetInstance();

	processor_base_ptr_type			GetProcessor(int iCmd);
    
    void                            RegisterProcessorFactory(uint16_t usCmd, const std::string& strCmdName, logic_processor_factory_type pFactory);

private:
	CLogicCmdMap();

private:
	static CLogicCmdMap*                          m_pLogicCmdMap;

private:
    std::unordered_map<uint32_t, std::function<processor_base_ptr_type()>>    m_stCmdProcessFactoryMap;
};
