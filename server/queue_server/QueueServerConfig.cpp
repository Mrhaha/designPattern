#include "QueueAppCommon.h"
#include "QueueServerConfig.h"

std::shared_ptr<CQueueServerConfig> CQueueServerConfig::m_pConfig;

std::mutex CQueueServerConfig::m_stLock;

CQueueServerConfig::config_loader_t CQueueServerConfig::CONFIG_LOADER_LIST[] =
{
	{ "zoo_queue_config", CQueueServerConfig::GetZooQueueConfigParser },
    { "pushover", CQueueServerConfig::GetQueuePushoverConfigParser },
};

size_t CQueueServerConfig::s_iConfigSize = sizeof(CQueueServerConfig::CONFIG_LOADER_LIST) / sizeof(CQueueServerConfig::config_loader_t);

void CQueueServerConfig::GetConfig(std::shared_ptr<CQueueServerConfig>& pConfig)
{
	pConfig.reset();
	{
		std::unique_lock<std::mutex> locker(m_stLock);
		pConfig = m_pConfig;
	}
}

void CQueueServerConfig::SetConfig(std::shared_ptr<CQueueServerConfig> pConfig)
{
	{
		std::unique_lock<std::mutex> locker(m_stLock);
		m_pConfig.swap(pConfig);
	}
}
