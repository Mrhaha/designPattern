#include <iostream>

#include "mysql_global_info.h"

int CGlobalConfig::s_iDiv = 0;

std::string CGlobalConfig::s_strHost;
std::string CGlobalConfig::s_strPort;
std::string CGlobalConfig::s_strUser;
std::string CGlobalConfig::s_strPasswd;
std::string CGlobalConfig::s_strDatabase;
std::string CGlobalConfig::s_strStorageInstance;

taf::TC_Mysql* CGlobalConfig::s_pMysqlHandle = NULL;

void
CGlobalConfig::Initialize(const std::string& strPath)
{
    std::string strFile = strPath + std::string("/") + cszGlobalConfigFile;

    tinyxml2::XMLDocument doc;
    doc.LoadFile(strFile.c_str());

    tinyxml2::XMLElement* pRoot = doc.RootElement();

    s_iDiv = ::atoi(pRoot->Attribute("div_num"));

    s_strHost = pRoot->Attribute("host");
    s_strPort = pRoot->Attribute("port");
    s_strUser = pRoot->Attribute("user");
    s_strPasswd = pRoot->Attribute("passwd");
    s_strDatabase = pRoot->Attribute("database");
    s_strStorageInstance = pRoot->Attribute("storage_instance");

}

void
CGlobalConfig::InitMysqlConn()
{
    taf::TC_DBConf stDbCfg;
    stDbCfg._host = s_strHost;
    stDbCfg._port = ::atoi(s_strPort.c_str());
    stDbCfg._user = s_strUser;
    stDbCfg._password = s_strPasswd;
    stDbCfg._database = s_strDatabase;
    stDbCfg._charset = "utf8";
    stDbCfg._flag = 0;

    s_pMysqlHandle = new taf::TC_Mysql();
    try
    {
        s_pMysqlHandle->init(stDbCfg);    
        s_pMysqlHandle->connect();        
    }
    catch(const taf::TC_Mysql_Exception& stEx)
    {
        std::cout << "MYSQL CREATE CONNECTION ERROR|MSG:" << stEx.what() << std::endl;
        delete s_pMysqlHandle;
        s_pMysqlHandle = NULL;
    }
}
