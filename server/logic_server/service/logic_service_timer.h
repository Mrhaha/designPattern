#pragma once

#include "tc_singleton.h"
#include "tc_timer.h"

typedef taf::TC_Singleton<taf::CTCTimer> SERVICE_TIMER_TYPE;
#define SERVICE_TIMER SERVICE_TIMER_TYPE::getInstance()


