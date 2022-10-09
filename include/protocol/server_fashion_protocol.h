#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"

//EQUIP_FASHION = 10279,						//装备时装
class CRequestEquipFashion
{
public:
	CRequestEquipFashion() : m_iFashionID(0), m_iCardID(0) {}

	MSGPACK_DEFINE(m_iFashionID, m_iCardID);

	int32_t m_iFashionID;           // 0 表示脱掉
	int32_t m_iCardID;
};

class CResponseEquipFashion : public CPackUserSyncData
{
public:
	CResponseEquipFashion() : m_iFashionID(0), m_iCardID(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iFashionID, m_iCardID);

	int32_t m_iFashionID;
	int32_t m_iCardID;
};

////////////////////////////////////////////////////////////////////
//NOTIFY_HONOR_UPDATE = 10390,				//通知称号更新
class CNotifyHonorUpdate : public CPackUserSyncData
{
public:
	CNotifyHonorUpdate() : m_stHonorInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_stHonorInfo);

	CPackHonorInfo m_stHonorInfo;
};

//UNLOCK_HONOR = 10391,						//解锁称号
class CRequestUnlockHonor
{
public:
	CRequestUnlockHonor() : m_iHonorID(0) {}

	MSGPACK_DEFINE(m_iHonorID);

	int32_t m_iHonorID;
};

class CResponseUnlockHonor : public CPackUserSyncData
{
public:
	CResponseUnlockHonor() : m_iHonorID(0), m_stConsumeItem() {}

	MSGPACK_DEFINE(m_stSyncData, m_iHonorID, m_stConsumeItem);

	int32_t m_iHonorID;
	CPackGameItem m_stConsumeItem;
};

//EQUIP_HONOR = 10392,						//装备称号
class CRequestEquipHonor
{
public:
	CRequestEquipHonor() : m_iHonorID(0) {}

	MSGPACK_DEFINE(m_iHonorID);

	int32_t m_iHonorID;
};

class CResponseEquipHonor : public CPackUserSyncData
{
public:
	CResponseEquipHonor() : m_iHonorID(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iHonorID);

	int32_t m_iHonorID;
};

class CResponsePhyStoreExtract : public CPackUserSyncData
{
public:
    CResponsePhyStoreExtract() : m_iAddPhy(0), m_iPhyStore(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iAddPhy, m_iPhyStore);

    int32_t     m_iAddPhy;
    int32_t     m_iPhyStore;
};
