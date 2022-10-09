//
// Created by lxhbroser on 2019/4/24.
//

#include "string.h"
#include "arpa/inet.h"
#include "common_msg_head_connector.h"
#include "common_base.h"

CMsgHeadConnector::CMsgHeadConnector()
{
    m_iPackageSize      = 0;
    m_byControlType     = head_type_invalid;
    m_usConnectionIndex = 0;

    ::memset(&m_stControlInfo, 0, sizeof(m_stControlInfo));
}

CMsgHeadConnector::~CMsgHeadConnector()
{
    //do nothing
}

int32_t CMsgHeadConnector::Encode(char* pBuffer, int32_t iBufferSize)
{
    if((nullptr == pBuffer) || iBufferSize < (int32_t)sizeof(CMsgHeadConnector))
    {
        return Enum_MHPE_BUFFER_TOO_SMALL;
    }

    uint32_t uiTotalLength = 0;

    uint32_t iPackageSize = htonl(m_iPackageSize);
    uint16_t usIndex = htons(m_usConnectionIndex);

    *((uint32_t*)(pBuffer + uiTotalLength)) = iPackageSize;
    uiTotalLength += sizeof(iPackageSize);

    *(pBuffer + uiTotalLength) = m_byControlType;
    uiTotalLength += sizeof(m_byControlType);

    *((uint16_t*)(pBuffer + uiTotalLength)) = usIndex;
    uiTotalLength += sizeof(usIndex);

    int32_t iSourceIP, iTime, iUin;
    uint16_t usSourcePort;
    switch(m_byControlType)
    {
        case head_type_start_request :
            iSourceIP = (int32_t)htonl((uint32_t)m_stControlInfo.m_stSourceIPInfo.m_iSourceIP);
            *((int32_t*)(pBuffer + uiTotalLength)) = iSourceIP;
            uiTotalLength += sizeof(iSourceIP);

            usSourcePort = htons(m_stControlInfo.m_stSourceIPInfo.m_usSourcePort);
            *((uint16_t*)(pBuffer + uiTotalLength)) = usSourcePort;
            uiTotalLength += sizeof(usSourcePort);

            break;
        case head_type_transform_request :
            iTime = (int32_t)htonl((uint32_t)m_stControlInfo.m_iTime);
            *((int32_t*)(pBuffer + uiTotalLength)) = iTime;
            uiTotalLength += sizeof(iTime);

            break;
        case head_type_force_close_connection :
            *(pBuffer + uiTotalLength) = m_stControlInfo.m_stDisconnectInfo.m_byDisconnectType;
            uiTotalLength += sizeof(m_stControlInfo.m_stDisconnectInfo.m_byDisconnectType);

            iSourceIP = (int32_t)htonl((uint32_t)m_stControlInfo.m_stDisconnectInfo.m_iSourceIP);
            *((int32_t*)(pBuffer + uiTotalLength)) = iSourceIP;
            uiTotalLength += sizeof(iSourceIP);

            usSourcePort = htons(m_stControlInfo.m_stDisconnectInfo.m_usSourcePort);
            *((uint16_t*)(pBuffer + uiTotalLength)) = usSourcePort;
            uiTotalLength += sizeof(usSourcePort);

            break;
        case head_type_disconnect_notify :
            iUin = (int32_t)htonl((uint32_t)m_stControlInfo.m_stClientDisconnect.m_iUin);
            *((int32_t*)(pBuffer + uiTotalLength)) = iUin;
            uiTotalLength += sizeof(iUin);

            *(pBuffer + uiTotalLength) = m_stControlInfo.m_stClientDisconnect.m_byDisconnectReason;
            uiTotalLength += sizeof(m_stControlInfo.m_stClientDisconnect.m_byDisconnectReason);

            int16_t sTemp = (int16_t)htons((uint16_t)m_stControlInfo.m_stClientDisconnect.m_iPlatformID);
            *((int16_t*)(pBuffer + uiTotalLength)) = sTemp;
            uiTotalLength += sizeof(sTemp);

            sTemp = (int16_t)htons((uint16_t)m_stControlInfo.m_stClientDisconnect.m_iGroupID);
            *((int16_t*)(pBuffer + uiTotalLength)) = sTemp;
            uiTotalLength += sizeof(sTemp);

            break;
    }

    return ((int32_t)uiTotalLength);
}

int32_t CMsgHeadConnector::Decode(const char* pBuffer, const int32_t& iBufferSize)
{
    int32_t buffSize = iBufferSize - MiniSize();
    if((nullptr == pBuffer) || (buffSize <= 0))
    {
        return Enum_MHPE_BUFFER_TOO_SMALL;
    }

    uint32_t uiLength = 0;
    m_iPackageSize = ntohl(*((uint32_t*)(pBuffer + uiLength)));
    uiLength += sizeof(m_iPackageSize);

    m_byControlType = *(pBuffer + uiLength);
    uiLength += sizeof(m_byControlType);

    m_usConnectionIndex = ::ntohs(*((uint16_t*)(pBuffer + uiLength)));
    uiLength += sizeof(m_usConnectionIndex);

    switch(m_byControlType)
    {
        case head_type_start_request :
            buffSize -= sizeof(m_stControlInfo.m_stSourceIPInfo.m_iSourceIP);
            buffSize -= sizeof(m_stControlInfo.m_stSourceIPInfo.m_usSourcePort);
            if(buffSize < 0)
            {
                return Enum_MHPE_BUFFER_TOO_SMALL;
            }

            m_stControlInfo.m_stSourceIPInfo.m_iSourceIP = (int32_t)ntohl(*((uint32_t*)(pBuffer + uiLength)));
            uiLength += sizeof(m_stControlInfo.m_stSourceIPInfo.m_iSourceIP);

            m_stControlInfo.m_stSourceIPInfo.m_usSourcePort = ntohs(*((uint16_t*)(pBuffer + uiLength)));
            uiLength += sizeof(m_stControlInfo.m_stSourceIPInfo.m_usSourcePort);

            break;
        case head_type_transform_request :
            buffSize -= sizeof(m_stControlInfo.m_iTime);
            if(buffSize < 0)
            {
                return Enum_MHPE_BUFFER_TOO_SMALL;
            }

            m_stControlInfo.m_iTime = (int32_t)ntohl(*((uint32_t*)(pBuffer + uiLength)));
            uiLength += sizeof(m_stControlInfo.m_iTime);

            break;
        case head_type_force_close_connection:
            buffSize -= sizeof(m_stControlInfo.m_stDisconnectInfo.m_byDisconnectType);
            buffSize -= sizeof(m_stControlInfo.m_stDisconnectInfo.m_iSourceIP);
            buffSize -= sizeof(m_stControlInfo.m_stDisconnectInfo.m_usSourcePort);
            if(buffSize < 0)
            {
                return Enum_MHPE_BUFFER_TOO_SMALL;
            }

            m_stControlInfo.m_stDisconnectInfo.m_byDisconnectType = *(pBuffer + uiLength);
            uiLength += sizeof(m_stControlInfo.m_stDisconnectInfo.m_byDisconnectType);

            m_stControlInfo.m_stDisconnectInfo.m_iSourceIP = (int32_t)ntohl(*((uint32_t*)(pBuffer + uiLength)));
            uiLength += sizeof(m_stControlInfo.m_stDisconnectInfo.m_iSourceIP);

            m_stControlInfo.m_stDisconnectInfo.m_usSourcePort = ntohs(*((uint16_t*)(pBuffer + uiLength)));
            uiLength += sizeof(m_stControlInfo.m_stDisconnectInfo.m_usSourcePort);

            break;
        case head_type_disconnect_notify :
            buffSize -= sizeof(m_stControlInfo.m_stClientDisconnect.m_iUin);
            buffSize -= sizeof(m_stControlInfo.m_stClientDisconnect.m_byDisconnectReason);
            buffSize -= sizeof(m_stControlInfo.m_stClientDisconnect.m_iPlatformID);
            buffSize -= sizeof(m_stControlInfo.m_stClientDisconnect.m_iGroupID);
            if(buffSize < 0)
            {
                return Enum_MHPE_BUFFER_TOO_SMALL;
            }

            m_stControlInfo.m_stClientDisconnect.m_iUin = (int32_t)ntohl(*((uint32_t*)(pBuffer + uiLength)));
            uiLength += sizeof(m_stControlInfo.m_stClientDisconnect.m_iUin);

            m_stControlInfo.m_stClientDisconnect.m_byDisconnectReason = *(pBuffer + uiLength);
            uiLength += sizeof(m_stControlInfo.m_stClientDisconnect.m_byDisconnectReason);

            m_stControlInfo.m_stClientDisconnect.m_iPlatformID = (int16_t)ntohs(*((uint16_t*)(pBuffer + uiLength)));
            uiLength += sizeof(m_stControlInfo.m_stClientDisconnect.m_iPlatformID);

            m_stControlInfo.m_stClientDisconnect.m_iGroupID = (int16_t)ntohs(*((uint16_t*)(pBuffer + uiLength)));
            uiLength += sizeof(m_stControlInfo.m_stClientDisconnect.m_iGroupID);

            break;
        default :
            return Enum_MHPE_DATA_ERROR;
    }

    return ((int32_t)uiLength);
}

int32_t CMsgHeadConnector::Size() const
{
    uint32_t uiLength = sizeof(m_iPackageSize) + sizeof(m_byControlType) + sizeof(m_usConnectionIndex);
    switch(m_byControlType)
    {
        case head_type_start_request :
            uiLength += sizeof(m_stControlInfo.m_stSourceIPInfo.m_iSourceIP);
            uiLength += sizeof(m_stControlInfo.m_stSourceIPInfo.m_usSourcePort);

            break;
        case head_type_transform_request :
            uiLength += sizeof(m_stControlInfo.m_iTime);

            break;
        case head_type_force_close_connection:
            uiLength += sizeof(m_stControlInfo.m_stDisconnectInfo.m_byDisconnectType);
            uiLength += sizeof(m_stControlInfo.m_stDisconnectInfo.m_iSourceIP);
            uiLength += sizeof(m_stControlInfo.m_stDisconnectInfo.m_usSourcePort);

            break;
        case head_type_disconnect_notify :
            uiLength += sizeof(m_stControlInfo.m_stClientDisconnect.m_iUin);
            uiLength += sizeof(m_stControlInfo.m_stClientDisconnect.m_byDisconnectReason);
            uiLength += sizeof(m_stControlInfo.m_stClientDisconnect.m_iPlatformID);
            uiLength += sizeof(m_stControlInfo.m_stClientDisconnect.m_iGroupID);

            break;
    }

    return ((int)uiLength);
}

int32_t CMsgHeadConnector::MiniSize()
{
    return (sizeof(m_iPackageSize) + sizeof(m_byControlType) + sizeof(m_usConnectionIndex));
}
