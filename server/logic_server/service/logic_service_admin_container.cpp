#include <errno.h>
#include <unistd.h>
#include "common_msg_head_admin.h"
#include "logic_service_logger.h"
#include "logic_service_admin_container.h"
#include "logic_service_api.h"

CLogicAdminContainer::CLogicAdminContainer() : m_stAdminContainer()
{
}

CLogicAdminContainer::~CLogicAdminContainer()
{
    std::map<int, stLogicSoAdminClient>::const_iterator stIter;
    for(stIter = m_stAdminContainer.begin(); stIter != m_stAdminContainer.end(); stIter ++)
    {
        if(stIter->first >= 0)
        {
            ::close(stIter->first);
        }
    }
}

void 
CLogicAdminContainer::AddAdminClient(int iFd, const std::string& strClientIP, unsigned short usClientPort)
{
    stLogicSoAdminClient stClient;
    stClient.m_iFd = iFd;
    stClient.m_strClientIP = strClientIP;
    stClient.m_usClientPort = usClientPort;
    stClient.m_uiLength = 0;
    stClient.m_uiOffset = 0;

    m_stAdminContainer[iFd] = stClient;
}

void 
CLogicAdminContainer::DelClient(stLogicSoAdminClient& stClient)
{
    stClient.Clear();
    m_stAdminContainer.erase(stClient.m_iFd);
}

void CLogicAdminContainer::ProcessClient(stLogicSoAdminClient& stClient)
{
    while(stClient.m_uiLength >= CMsgHeadAdmin::SIZE)
    {
        CMsgHeadAdmin stAdminHead;
        stAdminHead.decode(stClient.GetBuffer(), stClient.m_uiLength);
        if(stAdminHead.m_iPackageLen > max_admin_recv_pkg_size)
        {
            SERVICE_LOG_TRACE_ERROR << "ADMIN CLIENT|CLIENT:" << stClient.m_strClientIP << ":" << stClient.m_usClientPort 
                            << "|SERVER CLOSE CONNECTION|PACKAGE TOO LARGE|SIZE:" << stAdminHead.m_iPackageLen << std::endl;

            DelClient(stClient);
            return;
        }
        else if(stAdminHead.m_iPackageLen > stClient.m_uiLength)
        {
            break;
        }
        else
        {
            stClient.m_uiOffset += CMsgHeadAdmin::SIZE;
            stClient.m_uiLength -= CMsgHeadAdmin::SIZE;

            SERVICE_API_INS->GetLogicSoGameApi()->ProcessAdminMessage(stAdminHead.m_iUin, stAdminHead.m_nGroupID, stAdminHead.m_usCmd, stClient);
            if(stClient.m_iFd < 0)
            {
                DelClient(stClient);
                return;
            }
        }
    }

    if((stClient.m_uiLength > 0) && (0 != stClient.m_uiOffset))
    {
        ::memmove(stClient.m_pBuffer, stClient.m_pBuffer + stClient.m_uiOffset, stClient.m_uiLength);
    }
}

bool CLogicAdminContainer::RecvData(int iFd)
{
    auto iterAdmin = m_stAdminContainer.find(iFd);
    if (iterAdmin == m_stAdminContainer.end() || iterAdmin->second.m_iFd != iFd)
    {
        SERVICE_LOG_TRACE_DEBUG << "ADMIN CLIENT|CAN NOT FIND ADMIN CLIENT|FD:" << iFd << std::endl;
        return false;
    }

    auto& admin = iterAdmin->second;

    size_t uiBufferSize = sizeof(admin.m_pBuffer);
    size_t uiOffset = 0;
    while(1)
    {
        ssize_t iReadLen = ::read(admin.m_iFd, admin.m_pBuffer + uiOffset, uiBufferSize - uiOffset);
        if(iReadLen < 0)
        {
            if(EINTR == errno) continue;

            char* szErrMsg = ::strerror(errno);
            if(szErrMsg != nullptr)
            {
                SERVICE_LOG_TRACE_ERROR << "ADMIN CLIENT|CLIENT:" << admin.m_strClientIP << ":" << admin.m_usClientPort
                                << "|READ ERROR|SERVER CLOSE CONNECTION NOW|MSG:" << szErrMsg << std::endl;
            }
            else
            {
                SERVICE_LOG_TRACE_ERROR << "ADMIN CLIENT|CLIENT:" << admin.m_strClientIP << ":" << admin.m_usClientPort
                                << "|READ ERROR|SERVER CLOSE CONNECTION NOW|MSG:NO ERROR MESSAGE" << std::endl;
            }

            return (false);
        }
        else if(0 == iReadLen)
        {
            SERVICE_LOG_TRACE_DEBUG << "ADMIN CLIENT|CLIENT:" << admin.m_strClientIP << ":" << admin.m_usClientPort
                            << "|CLIENT REQUEST CLOSE CONNECTION" << std::endl;

            admin.Clear();

            break;
        }
        else
        {
            admin.m_uiLength += (size_t)iReadLen;
            break;
        }
    }

    ProcessClient(admin);

    return (true);
}
