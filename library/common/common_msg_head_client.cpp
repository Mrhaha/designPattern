//
// Created by lxhbroser on 2019/4/24.
//

#include <string.h>
#include "common_msg_head_client.h"
#include "common_base.h"

CMsgHeadClient::CMsgHeadClient() : m_stStaticData()
{

}

int32_t CMsgHeadClient::Encode(char* pBuffer, uint32_t uiLength, bool bIsNeedSession/*= true*/) const
{
    uint32_t uiSessionKeyLen = (uint32_t)m_stStaticData.m_ucSessionLen;
    if(bIsNeedSession == true)
    {
        if(uiSessionKeyLen >= GY_CLIENT_SESSION_KEY_MAX_LEN)
        {
            return Enum_MHPE_DATA_ERROR;
        }

        ::memcpy(pBuffer + sizeof(m_stStaticData), &m_szSessionKey, (size_t)uiSessionKeyLen);
    }
    else
    {
        uiSessionKeyLen = 0;
    }

    uint32_t uiTotalLen = sizeof(m_stStaticData) + uiSessionKeyLen;

    if(uiLength < uiTotalLen)
    {
        return Enum_MHPE_BUFFER_TOO_SMALL;
    }

    TClientProtoStatic stStaticData = m_stStaticData;
    stStaticData.m_uiPacketLen = htonl(stStaticData.m_uiPacketLen);
    stStaticData.m_iUin = (int32_t)htonl((uint32_t)stStaticData.m_iUin);
    stStaticData.m_uiVersion = htonl(stStaticData.m_uiVersion);
    stStaticData.m_uiSequence = htonl(stStaticData.m_uiSequence);
    stStaticData.m_iGroupID = (int32_t)htonl((uint32_t)stStaticData.m_iGroupID);
    stStaticData.m_iChannelID = (int32_t)htonl((uint32_t)stStaticData.m_iChannelID);
    stStaticData.m_usCmd = htons(stStaticData.m_usCmd);
    stStaticData.m_iSvrRsp = (int16_t)htons((uint16_t)stStaticData.m_iSvrRsp);
    stStaticData.m_ucSessionLen = (uint8_t)uiSessionKeyLen;

    ::memcpy(pBuffer, &stStaticData, sizeof(stStaticData));

    return ((int32_t)uiTotalLen);
}

int32_t CMsgHeadClient::Decode(const char* pBuffer, uint32_t uiLength)
{
    if(uiLength < sizeof(m_stStaticData))
    {
        return Enum_MHPE_BUFFER_TOO_SMALL;
    }

    ::memcpy(&m_stStaticData, pBuffer, sizeof(m_stStaticData));
    m_stStaticData.m_uiPacketLen = ntohl(m_stStaticData.m_uiPacketLen);
    m_stStaticData.m_iUin = (int32_t)ntohl((uint32_t)m_stStaticData.m_iUin);
    m_stStaticData.m_uiVersion = ntohl(m_stStaticData.m_uiVersion);
    m_stStaticData.m_uiSequence = ntohl(m_stStaticData.m_uiSequence);
    m_stStaticData.m_iGroupID = (int32_t)ntohl((uint32_t)m_stStaticData.m_iGroupID);
    m_stStaticData.m_iChannelID = (int32_t)ntohl((uint32_t)m_stStaticData.m_iChannelID);
    m_stStaticData.m_usCmd = ntohs(m_stStaticData.m_usCmd);
    m_stStaticData.m_iSvrRsp = (int16_t)ntohs((uint16_t)m_stStaticData.m_iSvrRsp);

    if((uiLength < m_stStaticData.m_ucSessionLen + sizeof(m_stStaticData)) || (m_stStaticData.m_ucSessionLen >= GY_CLIENT_SESSION_KEY_MAX_LEN))
    {
        return Enum_MHPE_BUFFER_TOO_SMALL;
    }

    ::memcpy(m_szSessionKey, pBuffer + sizeof(m_stStaticData), m_stStaticData.m_ucSessionLen);

    return (int32_t)(m_stStaticData.m_ucSessionLen + sizeof(m_stStaticData));
}
