#include "logicso_link.h"
#include "logic_link_define.h"

#ifdef _cplusplus
extern "C" {
#endif

	CLogicSoGameApi* CreateGameApiPtr()
	{
		g_pstGameApi = new CLogicSoGameApi();
		return g_pstGameApi;
	}

	void DestroyGameApiPtr(CLogicSoGameApi* pstGameApi)
	{
		delete pstGameApi;
	}

#ifdef _cplusplus
}
#endif
