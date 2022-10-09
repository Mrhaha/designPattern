#pragma once

#include "tc_epoller.h"
#include "tc_singleton.h"

#define SERVICE_POLLER_INS taf::TC_Singleton<taf::TC_Epoller, CCreateLogicPoller>::getInstance()

template<typename T>
class CCreateLogicPoller
{
public:
    static taf::TC_Epoller* create()
    {
        taf::TC_Epoller* pPoller = new taf::TC_Epoller(false);
        pPoller->create(4098);

        return (pPoller);
    }

    static void destroy(taf::TC_Epoller* pPoller)
    {
        delete pPoller;
    }
};


