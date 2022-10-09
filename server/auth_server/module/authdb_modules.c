#include "authdb_define.h"
#include "authdb_modules.h"

extern module_t authdb_sqlexec;
extern module_t authdb_test;
extern module_t authdb_auth_req;
extern module_t authdb_auth_renew;
extern module_t authdb_make_sign;

module_col_t authdb_module_col = {
    authdb_modules_init,
    authdb_modules_get
};
static module_t* authdb_modules[] = {
    &authdb_test,
    &authdb_auth_req,
    &authdb_auth_renew,
    &authdb_make_sign,
    NULL
};
int32_t authdb_modules_init(void* data)
{
    int16_t i;
    module_t* mod = NULL;
    for (i = 0; authdb_modules[i]; ++i)
    {
        mod = authdb_modules[i];
        if (NULL != mod->init_module)
        {
            mod->init_module(data);
        }
    }
    return success;
}

module_t* authdb_modules_get(int32_t idx)
{
    module_t* mod = NULL;
    switch (idx)
    {
    case MSG_SQL_EXECUTE:
        mod = &authdb_sqlexec;
    	break;
    case MSG_REQUEST_TEST:
        mod = &authdb_test;
        break;
    case MSG_REQUEST_AUTH:
        mod = &authdb_auth_req;
        break;
    case MSG_REQUEST_AUTH_RENEW:
        mod = &authdb_auth_renew;
        break;
    case MSG_REQ_MAKE_SIGN:
        mod = &authdb_make_sign;
        break;
    default:
        mod = NULL;
        break;
    }
    return mod;
}
