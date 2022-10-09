//
// Created by lxhbroser on 2019/4/25.
//

#pragma once

#include "common_base.h"
#include "common_socket.h"
#include "common_entity.h"
#include "common_ipv4.h"

template <typename SocketPolicy = CSocketEmptyPolicy>
class CTCPConnectionHandle
{
public:
    CTCPConnectionHandle() = default;
    virtual ~CTCPConnectionHandle() = default;

    DISALLOW_COPY_AND_ASSIGN(CTCPConnectionHandle);

public:
    //
    void Init(EnumEntityTypes entity_type, int32_t entity_id, const std::string& str_ip, uint16_t port);

    bool CheckAlive();
    void Alive();

    int32_t ConnectTo(const char* localIP = nullptr, uint16_t localPort = 0, bool nonblock = true, int32_t msecond = 100);

    // CTCPSocket methods
    int32_t                 GetFD() const           { return m_socket.get_fd(); }
    enmTCPConnectionStatus  GetStatus() const       { return m_socket.get_status(); }
    bool                    BeConnected() const     { return m_socket.be_connected(); }
    void                    Set4ByteSize(bool flag) { m_socket.set_4_byte_size(flag); }

    //get one completed code from receive buffer
    //return value: >0 -- success, <= 0  -- some error
    //size: 应该传入data的大小,传出得到的实际数据大小（目前size没有传入data的大小，仅传出数据的实际大小）
    EnumMsgHeadPackERROR GetOneCode(char* data, size_t& size, int32_t max_code_size = max_package_size);

    //send data to remote peer
    int32_t SendData(const char* data, size_t size);

    //receive data from socket stream, and store recv buffer, wait for processing
    int32_t RecvData();

    int32_t AcceptFd(int32_t fd);

    void CloseSocket();

    // CTCPEntity methods
    EnumEntityTypes     GetEntityType() const   { return m_stEntity.get_entity_type(); }
    int32_t             GetEntityId() const     { return m_stEntity.get_entity_id(); }
    const std::string&  GetStrIp() const        { return m_stEntity.get_str_ip(); }
    uint32_t            GetIp() const           { return m_stEntity.get_ip(); }
    uint16_t            GetPort() const         { return m_stEntity.get_port(); }

private:
    CTCPSocket<SocketPolicy> m_socket;
    CTCPEntity m_stEntity;

    time_t m_tmLastActiveTime;              // active time
};

//////////////////////////////////////////////////////////////////////////
// CTCPConnectionHandle Imp
//////////////////////////////////////////////////////////////////////////
template <typename SocketPolicy>
void CTCPConnectionHandle<SocketPolicy>::Init(EnumEntityTypes entity_type, int32_t entity_id, const std::string& str_ip, uint16_t port)
{
    m_stEntity.set_entity_type(entity_type);
    m_stEntity.set_entity_id(entity_id);
    m_stEntity.set_str_ip(str_ip);

    const uint32_t uip = inet_addr(str_ip.c_str());

    m_stEntity.set_ip(uip);
    m_stEntity.set_port(port);
}

template <typename SocketPolicy>
bool CTCPConnectionHandle<SocketPolicy>::CheckAlive()
{
    const time_t tmNow = time(nullptr);
    if (!BeConnected())
    {
        return false;
    }

    if (tmNow - m_tmLastActiveTime >= MAX_UNACTIVE_INTERVAL)
    {
        CloseSocket();
        return false;
    }

    return true;
}

template <typename SocketPolicy>
void CTCPConnectionHandle<SocketPolicy>::Alive()
{
    m_tmLastActiveTime = time(nullptr);
}

template <typename SocketPolicy>
int32_t CTCPConnectionHandle<SocketPolicy>::ConnectTo(const char* localIP, uint16_t localPort, bool nonblock, int32_t msecond)
{
    if (GetEntityType() == Enum_EET_EntityTypes_Null || GetEntityId() < 0)
    {
        return -1;
    }

    int32_t ret = m_socket.open_as_client(localIP, localPort);
    if (0 != ret)
    {
        return -2;
    }

    ret = m_socket.connect_to(GetIp(), GetPort(), nonblock, msecond);
    if (0 != ret)
    {
        return -3;
    }

    Alive();

    return ret;
}

template <typename SocketPolicy>
EnumMsgHeadPackERROR CTCPConnectionHandle<SocketPolicy>::GetOneCode(char* data, size_t& size, int32_t max_code_size/* = max_package_size*/)
{
    return m_socket.get_one_code(data, size, max_code_size);
}

template <typename SocketPolicy>
int32_t CTCPConnectionHandle<SocketPolicy>::SendData(const char* data, size_t size)
{
    return m_socket.send_data(data, size);
}

template <typename SocketPolicy>
int32_t CTCPConnectionHandle<SocketPolicy>::RecvData()
{
    return m_socket.recv_data();
}

template <typename SocketPolicy>
int32_t CTCPConnectionHandle<SocketPolicy>::AcceptFd(int32_t fd)
{
    return m_socket.accept_fd(fd);
}

template <typename SocketPolicy>
void CTCPConnectionHandle<SocketPolicy>::CloseSocket()
{
    m_socket.close_socket();
}
