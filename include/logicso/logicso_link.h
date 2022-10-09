//
// Created by hzl on 2019/1/31.
//

#pragma once
#include "logicso_game_api.h"

#ifdef _cplusplus
extern "C" {
#endif

CLogicSoGameApi* CreateGameApiPtr();

void DestroyGameApiPtr(CLogicSoGameApi* pstGameApi);

#ifdef _cplusplus
}
#endif

