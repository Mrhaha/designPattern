#pragma once

#include <map>
#include <string>
#include "logicso_define.h"

#include "msgpack.hpp"
#include "logic_service_logger.h"

class CLogicAdminContainer
{
public:
    CLogicAdminContainer();

    ~CLogicAdminContainer();

    void AddAdminClient(int iFd, const std::string& strClientIP, unsigned short usClientPort);

    bool RecvData(int iFd);

private:
    void ProcessClient(stLogicSoAdminClient& stClient);
    
    void DelClient(stLogicSoAdminClient& stClient);
    
private:
    std::map<int, stLogicSoAdminClient>       m_stAdminContainer;
};

