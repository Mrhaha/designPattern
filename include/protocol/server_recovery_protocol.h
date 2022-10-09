#pragma once

#include "msgpack.hpp"
#include "server_base_protocol.h"

class CPackInCacheUserHeader
{
public:
	MSGPACK_DEFINE(m_strVersion, m_strMD5Checksum);

	std::string             m_strVersion;
	std::string             m_strMD5Checksum;
};

//招财猫历史记录
class CPackLuckyCatHistory
{
public:
	CPackLuckyCatHistory() : m_iGroupID(0), m_iUID(0), m_iNum(0), m_strName("") {}

	MSGPACK_DEFINE(m_iGroupID, m_iUID, m_iNum, m_strName);

	int32_t m_iGroupID;//服
	int32_t m_iUID;//uid
	int32_t m_iNum;//获取数量
	std::string m_strName;//名称
};

class CPackLevelRaceRankElem
{
public:
	CPackLevelRaceRankElem() : m_iTotalAchieve(0) {}

	//MSGPACK_DEFINE(m_iTotalAchieve, m_stLevelRaceRank);
	MSGPACK_DEFINE(m_iTotalAchieve);

	int                                     m_iTotalAchieve;
	//std::vector<level_race_rank_elem_t>     m_stLevelRaceRank;
};


class CPackGlobalRecoveryData
{
public:
    CPackGlobalRecoveryData()
        : m_stRecoveryUser()
        , m_stLevelRaceActive()
        , m_iLastMultiColorOpenTime(0)
        , m_iLastMultiColorOpenVec()
        , m_stMultiColorUserData()
        , m_stLuckyCatHistoryList()
    {}

    MSGPACK_DEFINE(m_stRecoveryUser
            , m_stLevelRaceActive
            , m_iLastMultiColorOpenTime
            , m_iLastMultiColorOpenVec
            , m_stMultiColorUserData
            , m_stLuckyCatHistoryList
   );

    std::list<std::pair<int32_t, int32_t>>        	m_stRecoveryUser;
    std::map<int32_t, CPackLevelRaceRankElem>       m_stLevelRaceActive;
    int32_t										    m_iLastMultiColorOpenTime;
    std::vector<int32_t>						    m_iLastMultiColorOpenVec;
    std::map<uint64_t, std::vector<int32_t>> 	    m_stMultiColorUserData;
    std::list<CPackLuckyCatHistory> 			    m_stLuckyCatHistoryList;    //招财猫记录
};


