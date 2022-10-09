#pragma once

#include "server_recovery_protocol.h"

class CLogicDataRecovery
{
public:
	static void CheckDumpDataTime();

	static void DumpRecoveryDataToFile();

	static int RecoveryDataFromFile(CPackGlobalRecoveryData &rstGlobalCache, const std::string& filename = DUMP_FILE, const std::string& version= DUMP_VERSION);

	static int ParseRecoveryDataFile(const std::string& filename);

	static void BackupRecoveryDataFile();

	static int SaveRecoveryDataToFile(const CPackGlobalRecoveryData &stRecoverData);

private:
	const static std::string DUMP_FILE;
	const static std::string DUMP_VERSION;
	static int32_t s_iLastDumpDataTime;
};



