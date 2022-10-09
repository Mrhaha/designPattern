#pragma once
#include "ServerConfigParser.h"
#include <memory>
#include <mutex>

///////////////////////////////////////////////////////////////////////////////////////////////
#define DECLARE_CONFIG_PARSER(class_name, func_name, member_name) \
public: \
    const class_name& func_name() const { return (member_name); } \
    static CCrossConfigParser* func_name##Parser(std::shared_ptr<CCrossServerConfig>& pConfig) { return (pConfig->func_name##Parser()); } \
    class_name* func_name##Parser() { return (&(member_name)); } \
private: \
    class_name member_name; \

class CCrossServerConfig
{
public:
	typedef CCrossConfigParser* (*CCrossParseConfigFunc)(std::shared_ptr<CCrossServerConfig>& pConfig);

	struct config_loader_t
	{
		const char*                     m_pFileName;
		CCrossParseConfigFunc      m_pParserGetFunc;
	};

	static void GetConfig(std::shared_ptr<CCrossServerConfig>& pConfig);
	static void SetConfig(std::shared_ptr<CCrossServerConfig> pConfig);

	static config_loader_t* GetConfigLoader() { return (CONFIG_LOADER_LIST); }
	static size_t GetConfigSize() { return (s_iConfigSize); }

	DECLARE_CONFIG_PARSER(CZooCrossConfigParser, GetZooCrossConfig, m_stZooCrossConfig)
	DECLARE_CONFIG_PARSER(CZooActivityConfigParser, GetZooActivityConfig, m_stZooActivityConfig)
	DECLARE_CONFIG_PARSER(CMatchPvpConfigParser, GetMatchPvpConfig, m_stMatchPvpConfig)
	DECLARE_CONFIG_PARSER(CMatchPvpRobotConfigParser, GetMatchPvpRobotConfig, m_stMatchPvpRobotConfig)
	DECLARE_CONFIG_PARSER(CGuildWarConfigParser, GetGuildWarConfig, m_stGuildWarConfig)
    DECLARE_CONFIG_PARSER(CPushoverConfigParser, GetPushoverConfig, m_sPushoverConfig)
    DECLARE_CONFIG_PARSER(CCrossConfigGuildParser, GetGuildConfig, m_stGuildConfig)
    DECLARE_CONFIG_PARSER(CCrossConfigDataReportParser, GetDataReportConfig, m_stDataReportConfig)

private:
	static std::shared_ptr<CCrossServerConfig> m_pConfig;

	static std::mutex	m_stLock;

	static size_t s_iConfigSize;

	static config_loader_t CONFIG_LOADER_LIST[];
};

