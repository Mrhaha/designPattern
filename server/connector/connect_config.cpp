#include "connect_config.h"
#include "connector_log.h"
#include "common_ipv4.h"
#include "common_datetime.h"
#include "common_tinyxml2.h"

using namespace tinyxml2;

int32_t toint( const char* pch, int32_t* pint )
{
	int iVaule = 0;
	if(NULL == pch)
	{
		return fail;
	}
	sscanf(pch, "%i", &iVaule);
	*pint = (int32_t)iVaule;
	return success;
}

CConnectConfig::CConnectConfig()
{
	Initialize();
}
CConnectConfig::~CConnectConfig()
{
	//do nothing
}
int32_t CConnectConfig::Initialize()
{
	m_tmLastCheckConfigTime  = 0;	//最后一次检查配置文件的时间

	m_iOpenedPortCount = 0;
	m_nRouterCount = 0;
	memset(m_aiServerIndex, -1, sizeof(m_aiServerIndex));

	return success;
}

int32_t CConnectConfig::LoadConfig(const char* pszFileName)
{
	if (NULL == pszFileName)
	{
		TRACESVR(log_mask_system_config, "[CConnectConfig::%s] connect config file error!\n", __FUNCTION__);
		return fail;
	}

	struct stat stFileStat;
	if (0 == stat(pszFileName, &stFileStat))
	{
		m_tmConfigLastModifyTime = stFileStat.st_mtime;
	}

	XMLDocument document;
	if (document.LoadFile(pszFileName))
	{
		TRACESVR(log_mask_system_config, "[CConnectConfig::%s] LoadFile(%s) error!\n", __FUNCTION__, pszFileName);
		return fail;
	}

	Initialize();

	XMLElement* pstElement = document.RootElement();
    if (NULL == pstElement)
    {
        TRACESVR(log_mask_system_config, "[CConnectConfig::%s] LoadFile(%s) do not have (connectorsvrd) node!\n", __FUNCTION__, pszFileName);
        return fail;
    }

    XMLElement* pstChildElement = NULL;
	XMLElement* pstGrandElement = NULL;
	const char *pszValue = NULL;

	//session key
	const char* szSessionKey = pstElement->Attribute("session_key");
	if (NULL == szSessionKey)
	{
		TRACESVR(log_mask_system_config, "[CConnectConfig::%s] LoadFile(%s) do not have session_key attribute!\n",
				 __FUNCTION__, pszFileName);

		return fail;
	}
	else
	{
		::strncpy(m_szSessionKey, szSessionKey, sizeof(m_szSessionKey));
		m_szSessionKey[sizeof(m_szSessionKey) - 1] = '\0';
	}

	//log_level
	pszValue = pstElement->Attribute("log_level");
	if (NULL == pszValue)
	{
		m_iLogLevel = -1;
	}
	else
	{
		m_iLogLevel = std::atoi(pszValue);
	}

	//check_socket_interval
	pszValue = pstElement->Attribute("check_socket_interval");
	if (NULL == pszValue)
	{
		m_iCheckSocketInterval = 10;
	}
	else
	{
		m_iCheckSocketInterval = std::atoi(pszValue);
	}

	//sokcet_unalive_time
	pszValue = pstElement->Attribute("sokcet_unalive_time");
	if (NULL == pszValue)
	{
		m_iMaxSocketUnaliveTime = 120;
	}
	else
	{
		m_iMaxSocketUnaliveTime = std::atoi(pszValue);
	}

	//wait_firt_package_time
	pszValue = pstElement->Attribute("wait_firt_package_time");
	if (NULL == pszValue)
	{
		m_iMaxWaitForFirstPackageTime = 30;
	}
	else
	{
		m_iMaxWaitForFirstPackageTime = std::atoi(pszValue);
	}

	//statistic_interval
	pszValue = pstElement->Attribute("statistic_interval");
	if (NULL == pszValue)
	{
		m_iLogStatisticInterval = 90;
	}
	else
	{
		m_iLogStatisticInterval = std::atoi(pszValue);
	}

	//socket_buffer
	pszValue = pstElement->Attribute("socket_buffer");
	if (NULL == pszValue)
	{
		m_iSocketBufferSize = 131072;
	}
	else
	{
		m_iSocketBufferSize = std::atoi(pszValue);
	}

	//signature_valid_period
	pszValue = pstElement->Attribute("signature_valid_period");
	if (NULL == pszValue)
	{
        m_nSignatureValidPeriod = 86400;//one day
	}
	else
	{
		m_nSignatureValidPeriod = std::atoi(pszValue);
	}

	//allow_no_signature
	pszValue = pstElement->Attribute("allow_no_signature");
	if (NULL == pszValue)
	{
		m_iIsAllowNoSignature = 0;
	}
	else
	{
		m_iIsAllowNoSignature = std::atoi(pszValue);
	}

	//max_fd
	pszValue = pstElement->Attribute("max_fd");
	if (NULL == pszValue)
	{
		m_nMaxfd = open_max_socket_count;
	}
	else
	{
		m_nMaxfd = std::atoi(pszValue);
	}
	if (m_nMaxfd > max_socket_count)
	{
		m_nMaxfd = max_socket_count;
	}
	else if (m_nMaxfd < (int32_t)open_min_socket_count)
	{
		m_nMaxfd = open_min_socket_count;
	}

	//local_ip
	pszValue = pstElement->Attribute("local_ip");
	if (NULL == pszValue)
	{
		m_szLocalIP[0] = '\0';
	}
	else
	{
		STRNCPY(m_szLocalIP, pszValue, sizeof(m_szLocalIP));
	}

	//Listen Port
    int32_t iValue = 0;
	pstChildElement = pstElement->FirstChildElement("listen_port");
	while (NULL != pstChildElement)
	{
		pszValue = pstChildElement->GetText();
		if (NULL != pszValue && success == toint(pszValue, &iValue))
		{
			m_unOpenedPort[m_iOpenedPortCount] = (uint16_t)iValue;
			++ m_iOpenedPortCount;
			if (m_iOpenedPortCount >= max_listen_port_count)
			{
				break;
			}//if
		}//if

		//next step
		pstChildElement = pstChildElement->NextSiblingElement("listen_port");
	}//while

	//
	pstChildElement = pstElement->FirstChildElement("router");
	while (NULL != pstChildElement)
	{
		//logic_ip="192.168.1.51"
		pszValue = pstChildElement->Attribute("logic_ip");
		if (NULL == pszValue)
		{
			TRACESVR(log_mask_system_config, "[CConnectConfig::%s] LoadFile(%s) node(router) do not have (logic_ip) attribute!\n", __FUNCTION__, pszFileName);
			return fail;
		}
		STRNCPY(m_astRouterServer[m_nRouterCount].m_szLogicIP, pszValue,
			sizeof(m_astRouterServer[m_nRouterCount].m_szLogicIP));

		uint32_t iLogicIP = 0;
		ip_string_to_addr(m_astRouterServer[m_nRouterCount].m_szLogicIP, iLogicIP);
		m_astRouterServer[m_nRouterCount].m_uiLogicIP = iLogicIP;

		//logic_listen_port="20001"
		pszValue = pstChildElement->Attribute("logic_listen_port");
		if (NULL == pszValue)
		{
			TRACESVR(log_mask_system_config, "[CConnectConfig::%s] LoadFile(%s) node(router) do not have (logic_listen_port) attribute!\n", __FUNCTION__, pszFileName);
			return fail;
		}
		m_astRouterServer[m_nRouterCount].m_nLogicListenPort = (uint16_t)std::atoi(pszValue);

		//logic_port="10010"
		pszValue = pstChildElement->Attribute("logic_port");
		if (NULL == pszValue)
		{
			TRACESVR(log_mask_system_config, "[CConnectConfig::%s] LoadFile(%s) node(router) do not have (logic_port) attribute!\n", __FUNCTION__, pszFileName);
			return fail;
		}
		m_astRouterServer[m_nRouterCount].m_nLogicPort = (uint16_t)std::atoi(pszValue);

		//local_port="10015"
		pszValue = pstChildElement->Attribute("local_port");
		if (NULL == pszValue)
		{
			TRACESVR(log_mask_system_config, "[CConnectConfig::%s] LoadFile(%s) node(router) do not have (local_port) attribute!\n", __FUNCTION__, pszFileName);
			return fail;
		}
		m_astRouterServer[m_nRouterCount].m_nLocalPort = (uint16_t)std::atoi(pszValue);

		//服务器组ID
		pstGrandElement = pstChildElement->FirstChildElement("group_id");
		bool bIsValidRouter = false;
		while (NULL != pstGrandElement)
		{
			pszValue = pstGrandElement->GetText();
			if (NULL != pszValue && success == toint(pszValue, &iValue))
			{
				if (iValue > 0 && iValue < max_logic_group_id)
				{
					bIsValidRouter = true;
					m_aiServerIndex[iValue] = m_nRouterCount;
				}
			}//if

			//next step
			pstGrandElement = pstGrandElement->NextSiblingElement("group_id");
		}//while

		if (true == bIsValidRouter)
		{
			++ m_nRouterCount;
		}

		//next step
		pstChildElement = pstChildElement->NextSiblingElement("router");
	}//while

	return success;
}

//获得服务器索引
int32_t CConnectConfig::GetServerIndex(int32_t iGroupID)
{
	if (iGroupID > 0 && iGroupID < max_logic_group_id)
	{
		return m_aiServerIndex[iGroupID];
	}
	return -1;
}

//获得客户端的连接索引
int32_t CConnectConfig::GetClientIndex(uint32_t uIP, uint16_t nPort)
{
	for (int16_t i = 0; i < m_nRouterCount; ++ i)
	{
		if (uIP == m_astRouterServer[i].m_uiLogicIP && nPort == m_astRouterServer[i].m_nLogicPort)
		{
			return i;
		}//if
	}//for
	return -1;
}

int32_t CConnectConfig::ReloadConfig(const char* pszFileName)
{
	if (NULL == pszFileName)
	{
		return fail;
	}

	time_t tmNowTime = time(NULL);
	if (tmNowTime - m_tmLastCheckConfigTime < (long)1_min)
	{
		//1分钟检查一次配置文件
		return success;
	}
	m_tmLastCheckConfigTime = tmNowTime;

	struct stat stFileStat;
	if (0 == stat(pszFileName, &stFileStat))
	{
		if (m_tmConfigLastModifyTime != stFileStat.st_mtime)
		{
			//保存最后修改时间
			m_tmConfigLastModifyTime = stFileStat.st_mtime;

			CConnectConfig* pstConfig = new CConnectConfig();
			if (NULL != pstConfig && success == pstConfig->LoadConfig(pszFileName))
			{
				LoadConfig(pszFileName);
				TRACESVR(log_mask_system_warning, "[CConnectConfig::%s] reload connect resource from (%s) success!\n", __FUNCTION__, pszFileName);
			}
			else
			{
				TRACESVR(log_mask_system_warning, "[CConnectConfig::%s] reload connect resource from (%s) failed!\n", __FUNCTION__, pszFileName);
			}

			if (NULL != pstConfig)
			{
				delete pstConfig;
			}
		}
	}

	return success;
}

//关闭socket链接
int32_t CConnectConfig::CloseServer(int32_t iGroupID)
{
    XMLDocument document;
	if (document.LoadFile(CONNECT_CONFIG))
	{
		TRACESVR(log_mask_system_config, "[CConnectConfig::%s] LoadFile(%s) error!\n", __FUNCTION__, CONNECT_CONFIG);
		return fail;
	}

	XMLElement* root = document.RootElement();
	if(root == nullptr)
    {
        TRACESVR(log_mask_system_config, "[CConnectConfig::%s] RootElement()(%s) error!\n", __FUNCTION__, CONNECT_CONFIG);
        return fail;
    }

	XMLElement* pstElement = root->FirstChildElement("router");
	XMLElement* pstChildElement = NULL;
	int32_t iValue = 0;
	const char* pszValue = NULL;
	bool bIsRemove = false;

	while (NULL != pstElement)
	{
		pstChildElement = pstElement->FirstChildElement("group_id");
		while (NULL != pstChildElement)
		{
			pszValue = pstChildElement->GetText();
			if (NULL != pszValue && success == toint(pszValue, &iValue))
			{
				if (iValue == iGroupID)
				{
					pstElement->DeleteChild(pstChildElement);
					bIsRemove = true;
					break;
				}//if
			}//if

			//next step
			pstChildElement = pstChildElement->NextSiblingElement("group_id");
		}//while

		if (true == bIsRemove && NULL == pstElement->FirstChildElement("group_id"))
		{
			root->DeleteChild(pstElement);
			break;
		}

		//next step
		pstElement = pstElement->NextSiblingElement("router");
	}//while

	if(document.SaveFile(CONNECT_CONFIG))
    {
        TRACESVR(log_mask_system_config, "[CConnectConfig::%s] SaveFile(%s) error!\n", __FUNCTION__, CONNECT_CONFIG);
        return fail;
    }

	return success;
}

//合服
int32_t CConnectConfig::MergeServer(int32_t iSrcGroupID, int32_t iDstGroupID)
{
	XMLDocument document;
	if (document.LoadFile(CONNECT_CONFIG))
	{
		TRACESVR(log_mask_system_config, "[CConnectConfig::%s] LoadFile(%s) error!\n", __FUNCTION__, CONNECT_CONFIG);
		return fail;
	}

	XMLElement* root = document.RootElement();
    if(root == nullptr)
    {
        TRACESVR(log_mask_system_config, "[CConnectConfig::%s] RootElement()(%s) error!\n", __FUNCTION__, CONNECT_CONFIG);
        return fail;
    }

	XMLElement* pstElement = root->FirstChildElement("router");
	XMLElement* pstChildElement = NULL;
	int32_t iValue = 0;
	const char* pszValue = NULL;
	bool bIsFind = false;

	while (NULL != pstElement)
	{
		pstChildElement = pstElement->FirstChildElement("group_id");
		while (NULL != pstChildElement)
		{
			pszValue = pstChildElement->GetText();
			if (NULL != pszValue && success == toint(pszValue, &iValue))
			{
				if (iValue == iSrcGroupID)
				{
					XMLElement* newGroupID = document.NewElement("group_id");
					char szString[32] = {0};
					memset(szString, 0x0, sizeof(szString));
					sprintf(szString, "%d", iDstGroupID);

					newGroupID->InsertEndChild(document.NewText(szString));
					pstElement->InsertEndChild(newGroupID);

					bIsFind = true;
					break;
				}//if
			}//if

			//next step
			pstChildElement = pstChildElement->NextSiblingElement("group_id");
		}//while

		if (true == bIsFind)
		{
			break;
		}

		//next step
		pstElement = pstElement->NextSiblingElement("router");
	}//while

	if(document.SaveFile(CONNECT_CONFIG))
    {
        TRACESVR(log_mask_system_config, "[CConnectConfig::%s] SaveFile(%s) error!\n", __FUNCTION__, CONNECT_CONFIG);
        return fail;
    }

	return success;
}

//加服
int32_t CConnectConfig::ConnectServer(int32_t iGroupID, const char* pszIP, int32_t iListenPort, int32_t iLogicPort, int32_t iLocalPort)
{
    XMLDocument document;
	if (document.LoadFile(CONNECT_CONFIG))
	{
		TRACESVR(log_mask_system_config, "[CConnectConfig::%s] LoadFile(%s) error!\n", __FUNCTION__, CONNECT_CONFIG);
		return fail;
	}

	XMLElement* root = document.RootElement();
	if(root == nullptr)
    {
        TRACESVR(log_mask_system_config, "[CConnectConfig::%s] RootElement()(%s) error!\n", __FUNCTION__, CONNECT_CONFIG);
        return fail;
    }

	XMLElement* newRouter = document.NewElement("router");
	newRouter->SetAttribute("logic_ip", pszIP);
	newRouter->SetAttribute("logic_listen_port", iListenPort);
	newRouter->SetAttribute("logic_port", iLogicPort);
	newRouter->SetAttribute("local_port", iLocalPort);

    char szString[32] = {0};
    memset(szString, 0x0, sizeof(szString));
    sprintf(szString, "%d", iGroupID);
	XMLElement* newGroupID = document.NewElement("group_id");
	newGroupID->InsertEndChild(document.NewText(szString));
	newRouter->InsertEndChild(newGroupID);

	root->InsertEndChild(newRouter);

    if(document.SaveFile(CONNECT_CONFIG))
    {
        TRACESVR(log_mask_system_config, "[CConnectConfig::%s] SaveFile(%s) error!\n", __FUNCTION__, CONNECT_CONFIG);
        return fail;
    }

	return success;
}
