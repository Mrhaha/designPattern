//
// Created by lxhbroser on 2019/4/24.
//
#pragma once

#include "stdint.h"

enum EnumClientCloseReason
{
    enmClosedReason_Service             = 0,		//Service主动关闭
    enmClosedReason_Client              = 1,		//client主动关闭
    enmClosedReason_IOError             = 2,		//IO读写错误
    enmClosedReason_DataError           = 3,		//无法识别收到客户端的消息错误
    enmClosedReason_QueueError          = 4,		//无法识别收到客户端的消息错误
    enmClosedReason_RecvBuffError       = 5,		//接收缓冲错误(满)
    enmClosedReason_SendBuffError       = 6,		//发送缓冲错误(满)

    enmClosedReason_ClientTimeOut       = 7,	    //客户端超时(已经与service进程交换过数据)
    enmClosedReason_PushServiceChannelError = 8,	//写内存管道错误
    enmClosedReason_AppError            = 9,		//系统错误
    enmClosedReason_InvalidConnection   = 10,		//连接无效（没有与后端server有过数据交换，不需要通知service进程进行清理动作）

    enmClosedReason_InvalidSignature    = 11,		//无效的数字签名
};

enum EnumForceCloseType
{
    enmForceCloseType_Now =1,
    enmForceCloseType_Delay         //等数据全部发送成功后后关闭该连接.
};

enum EnumCtrMsgType
{
    head_type_invalid                   = (int8_t)-1,
    head_type_start_request             = (int8_t)0x01,   //方向:client --> Service, client首次发送消息给service.
    head_type_transform_request         = (int8_t)0x02,   //方向:client <--> Service, client与service之间互相传递消息(备:若client第一次发送消息，则用前一条指条发送
    head_type_force_close_connection    = (int8_t)0x03,   //方向:client <-- Service, Service主动关闭client的链接.
    head_type_disconnect_notify         = (int8_t)0x04,   //方向:client --> Service, client主动断线
    head_type_group_transform_request   = (int8_t)0x05,   //方向:client <-- Service, Service给一组用户发送消息(组播)。 暂不支持
};

class CMsgHeadConnector
{
public:
    CMsgHeadConnector();
    ~CMsgHeadConnector();

    /*
	* @method	: Encode编码
	* @param	: char*,传出编码后的buffer
	* @param	: int,传入buffer的最大长度
	* @return	: int,>0 success,<=0 failed.返回值的意义是编码后的长度
	*/
    int32_t     Encode(char* pBuffer, int32_t iBufferSize);

    /*
	* @method	: Decode解码
	* @param	: const char*,传入待解码的buffer
	* @param	: int,待解码buffer的长度
	* @return	: int,>0 success,<=0 failed.返回值的意义是头的偏移量
	*/
    int32_t     Decode(const char* pBuffer, const int32_t& iBufferSize);

    /*
	* @method	: 计算赋值后的router head的实际长度
	* @return	: 返回头部长度
	*/
    int32_t     Size() const;

    /*
	* @method	: 计算除去union结构的最小长度（可以计算赋值前的长度）
	* @return	: 返回头部最小长度
	*/
    static int32_t MiniSize();

    // 头部数据
    uint32_t    m_iPackageSize;         // 整个包的长度
    int8_t      m_byControlType;        // EnumCtrMsgType
    uint16_t    m_usConnectionIndex;    // 客户端对应Connector服务器中的Socket FD

    union __U
    {
        struct stSourceIPInfo
        {
            int32_t     m_iSourceIP;
            uint16_t    m_usSourcePort;
        }m_stSourceIPInfo;				//head_type_start_request

        int32_t         m_iTime;		//head_type_transform_request
                                        //( c->s :表示socket创建的时间, s->c: 是控制connect server是否断开连接, 0 表示不断开连接， >0 表示断开连接)

        struct __stDisconnectInfo__
        {
            int8_t      m_byDisconnectType;	//取值EnumForceCloseType
            int32_t     m_iSourceIP;
            uint16_t    m_usSourcePort;
        }m_stDisconnectInfo;			//head_type_force_close_connection .

        struct stClientRequestDisconnect
        {
            int32_t     m_iUin;
            int8_t      m_byDisconnectReason;
            int16_t     m_iPlatformID;
            int16_t     m_iGroupID;
        }m_stClientDisconnect;			//head_type_disconnect_notify

    }m_stControlInfo;
};
