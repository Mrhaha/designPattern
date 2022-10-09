#include "tc_md5.h"
#include "common_datetime.h"
#include "logic_link_common.h"
#include "logic_game_data_recovery.h"
#include "logic_game_active_info.h"
#include "logic_config.h"
#include "logic_game_global_frame.h"
#include "logic_game_global.h"
#include "logic_game_rank_list.h"

const std::string CLogicDataRecovery::DUMP_FILE = "./dump_data.log";
const std::string CLogicDataRecovery::DUMP_VERSION = "1.0";
int32_t CLogicDataRecovery::s_iLastDumpDataTime = time(nullptr);

void CLogicDataRecovery::CheckDumpDataTime()
{
	struct tm stCurrentDate = CLogicCommon::GetDate();
	stCurrentDate.tm_hour = 0;
	stCurrentDate.tm_min = 0;
	stCurrentDate.tm_sec = 0;

	int iNow = CLogicCommon::GetSec();
	int iTodayDumpTime = mktime(&stCurrentDate);
	if ((iNow >= iTodayDumpTime) && (s_iLastDumpDataTime < iTodayDumpTime))
	{
		DumpRecoveryDataToFile();
		BackupRecoveryDataFile();
		s_iLastDumpDataTime = iTodayDumpTime + 1;
	}
}

void CLogicDataRecovery::DumpRecoveryDataToFile()
{
    CPackGlobalRecoveryData stRecoveryData = CLogicGlobal::GetGlobalCache()->m_stRecoveryData;
    stRecoveryData.m_stRecoveryUser = CLogicCacheCommon::GetCacheUserID(500);

	CLogicDataRecovery::SaveRecoveryDataToFile(stRecoveryData);

	LOGIC_LOG_TRACE_DEBUG << "EVERY DAY|DUMP_TO_FILE_USER_NUM:" << stRecoveryData.m_stRecoveryUser.size() << std::endl;
}

int CLogicDataRecovery::RecoveryDataFromFile(CPackGlobalRecoveryData &rstGlobalCache, const std::string& filename, const std::string& version)
{
    std::string strData = taf::TC_File::load2str(filename);
    if(!strData.empty())
    {
        size_t uiOffset = 0;
        msgpack::unpacked stResult1, stResult2;
        try
        {
            msgpack::unpack(&stResult1, strData.c_str(), strData.size(), &uiOffset);
        }
        catch(const std::runtime_error& stEx)
        {
            return (-1);
        }

        CPackInCacheUserHeader stHeader;
        const msgpack::object& stHeaderMsgObj = stResult1.get();
        try
        {
            stHeaderMsgObj.convert(&stHeader);
        }
        catch(const std::bad_cast& stEx)
        {
            return (-2);
        }

        if(uiOffset >= strData.size())
        {
            return (-3);
        }

        std::string strRecoverData = strData.substr(uiOffset);

        std::string strMD5 = taf::TC_MD5::md5str(strRecoverData);
        if(strMD5 != stHeader.m_strMD5Checksum)
        {
            return (-4);
        }

        try
        {
            msgpack::unpack(&stResult2, strRecoverData.c_str(), strRecoverData.size());
        }
        catch(const std::runtime_error& stEx)
        {
            return (-5);
        }

        const msgpack::object& stDataMsgObj = stResult2.get();
        try
        {
            stDataMsgObj.convert(&rstGlobalCache);
        }
        catch(const std::bad_cast& stEx)
        {
            return (-6);
        }
    }

    return (0);

}

int CLogicDataRecovery::ParseRecoveryDataFile(const std::string& filename)
{
	CPackGlobalRecoveryData stRecoveryData;
	const int iRet = RecoveryDataFromFile(stRecoveryData, filename);
	if (0 != iRet)
	{
		return iRet;
	}

	std::cout << "[LEVEL_RACE]" << std::endl;
	for (const auto& rank : stRecoveryData.m_stLevelRaceActive)
	{
		std::cout << "UIN:" << rank.first << " |TOTAL_ACHIEVE:" << rank.second.m_iTotalAchieve << std::endl;
	}
	std::cout << std::endl;

	std::cout << "[MULTI_COLOR_USER_DATA]" << std::endl;
	std::cout << "LAST_OPEN_TIME:" << get_datetime_string(stRecoveryData.m_iLastMultiColorOpenTime) << " LAST_OPEN COLOR:[ ";
	for (const auto& id : stRecoveryData.m_iLastMultiColorOpenVec)
	{
		std::cout << id << ' ';
	}
	std::cout << ']' << std::endl;
	for (const auto& user : stRecoveryData.m_stMultiColorUserData)
	{
		const user_info_key_t stKey(user.first);
		std::cout << "UIN:" << stKey.m_stRoleIDDetail.m_iUin << " |GROUP:" << stKey.m_stRoleIDDetail.m_iGroupID << " |GUESS MULTI COLOR:[ ";
		for (const auto& id : user.second)
		{
			std::cout << id << ' ';
		}
		std::cout << ']' << std::endl;
	}
	std::cout << std::endl;

	return 0;
}

void CLogicDataRecovery::BackupRecoveryDataFile()
{
	std::string backup = "backup";
	if (create_directory(backup.c_str()))
	{
		const std::string backup_filename = backup + "/dump_backup_" + CLogicCommon::GetDateString() + ".log";
		copy_file(CLogicDataRecovery::DUMP_FILE.c_str(), backup_filename.c_str());
	}
}

int CLogicDataRecovery::SaveRecoveryDataToFile(const CPackGlobalRecoveryData &stRecoverData)
{
	CPackInCacheUserHeader stDumpData;
	stDumpData.m_strVersion = DUMP_VERSION;

	msgpack::sbuffer stHeaderBuffer, stDataBuffer;
	msgpack::pack(stDataBuffer, stRecoverData);

	std::string strData = std::string(stDataBuffer.data(), stDataBuffer.size());
	stDumpData.m_strMD5Checksum = taf::TC_MD5::md5str(strData);

	msgpack::pack(stHeaderBuffer, stDumpData);

	std::string strHeader = std::string(stHeaderBuffer.data(), stHeaderBuffer.size());

	ofstream stFileStream(DUMP_FILE.c_str(), std::ofstream::out | std::ofstream::binary);
	std::string strTotalData = strHeader + strData;
	stFileStream << strTotalData;
	stFileStream.close();

	return (0);
}



