//
// Created by lxhbroser on 2019/4/24.
//

#pragma once

#ifdef _WIN32
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

#define GY_CLIENT_SESSION_KEY_MAX_LEN 255

#pragma pack(push, 1)
struct TClientProtoStatic
{
    TClientProtoStatic()
    {
        Clear();
    }

    void Clear()
    {
        m_uiPacketLen   = 0;
        m_iUin          = -1;
        m_uiVersion     = 0;
        m_uiSequence    = 0;
        m_iGroupID      = 0;
        m_iChannelID   = 0;
        m_usCmd         = 0;
        m_iSvrRsp       = 0;
        m_ucEncrpytFlag = 0;
        m_ucSessionLen  = 0;
    }

    uint32_t        m_uiPacketLen;
    int32_t         m_iUin;
    uint32_t        m_uiVersion;
    uint32_t        m_uiSequence;
    int32_t         m_iGroupID;
    int32_t         m_iChannelID;
    uint16_t        m_usCmd;
    int16_t         m_iSvrRsp;
    uint8_t         m_ucEncrpytFlag;
    uint8_t         m_ucSessionLen;
};
#pragma pack(pop)

class CMsgHeadClient
{
public:
    enum EnumClientToConnectorCMD   // 客户端发往Connector，并需要Connector处理的协议
    {                               // 此CMD与SERVER_CMD_LOGIC_DEFINE属于共用协议体
        ECTCC_HEART_BEAT_CMD = 1    // 不过，因为目前Client与Connector只有一个心跳CMD，所以单独定义
    };

    CMsgHeadClient();

    int32_t Encode(char* pBuffer, uint32_t uiLength, bool bIsNeedSession = true) const;

    int32_t Decode(const char* pBuffer, uint32_t uiLength);

    TClientProtoStatic      m_stStaticData;
    char                    m_szSessionKey[GY_CLIENT_SESSION_KEY_MAX_LEN];
};
