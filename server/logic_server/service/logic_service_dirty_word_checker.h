#pragma once

#include "tc_singleton.h"

#include "logic_service_ac_automaton.h"

typedef taf::TC_Singleton<CACAutomaton<uint32_t>> SERVICE_DIRTY_WORD_FILTER_TYPE;

#define SERVICE_DIRTY_WORD_FILTER_INS SERVICE_DIRTY_WORD_FILTER_TYPE::getInstance()



