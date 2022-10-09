#pragma once
#include "QueueServerConfigParser.h"
#include <memory>
#include <mutex>

///////////////////////////////////////////////////////////////////////////////////////////////
#define DECLARE_CONFIG_PARSER(class_name, func_name, member_name) \
public: \
    const class_name& func_name() const { return (member_name); } \
    static CQueueConfigParser* func_name##Parser(std::shared_ptr<CQueueServerConfig>& pConfig) { return (pConfig->func_name##Parser()); } \
    class_name* func_name##Parser() { return (&(member_name)); } \
private: \
    class_name member_name; \

class CQueueServerConfig
{
public:
	typedef CQueueConfigParser* (*CCrossParseConfigFunc)(std::shared_ptr<CQueueServerConfig>& pConfig);

	struct config_loader_t
	{
		const char*                     m_pFileName;
		CCrossParseConfigFunc      m_pParserGetFunc;
	};

	static void GetConfig(std::shared_ptr<CQueueServerConfig>& pConfig);
	static void SetConfig(std::shared_ptr<CQueueServerConfig> pConfig);

	static config_loader_t* GetConfigLoader() { return (CONFIG_LOADER_LIST); }
	static size_t GetConfigSize() { return (s_iConfigSize); }

	DECLARE_CONFIG_PARSER(CZooQueueConfigParser, GetZooQueueConfig, m_stZooQueueConfig)
    DECLARE_CONFIG_PARSER(CQueuePushoverConfigParser, GetQueuePushoverConfig, m_sQueuePushoverConfig)

private:
	static std::shared_ptr<CQueueServerConfig> m_pConfig;

	static std::mutex	m_stLock;

	static size_t s_iConfigSize;

	static config_loader_t CONFIG_LOADER_LIST[];
};

