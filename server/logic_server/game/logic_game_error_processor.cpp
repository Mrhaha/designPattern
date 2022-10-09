//
// Created by hzl on 2019/2/20.
//

#include "logic_game_error_processor.h"

CLogicErrorProcessor::CLogicErrorProcessor(uint16_t usCmd, const std::string &strCmdName) : CLogicProcessorBase(usCmd, strCmdName)
{}

bool CLogicErrorProcessor::DoUserRun(const msgpack::unpacked &stFormatData)
{
    GetConnectorSender()->NotifyCloseClient(m_iUin,
                                            m_stUserConnectorData.m_usConnectionIndex,
                                            m_stUserConnectorData.m_iClientIP,
                                            m_stUserConnectorData.m_usClientPort);

    return (true);
}
