#pragma once

#define cszDefaultEngine "InnoDB"
#define cszDefaultCharSet "utf8"
#define cszGlobalConfigFile "sql_global_info.xml"

#include "common_tinyxml2.h"

#include "tc_mysql.h"

#include <string>

class CGlobalConfig
{
public:
    static void Initialize(const std::string& strPath);
    static void InitMysqlConn();

    static int s_iDiv;
    static std::string s_strHost;
    static std::string s_strPort;
    static std::string s_strUser;
    static std::string s_strPasswd;
    static std::string s_strDatabase;
    static std::string s_strStorageInstance;

    static taf::TC_Mysql* s_pMysqlHandle;
};
