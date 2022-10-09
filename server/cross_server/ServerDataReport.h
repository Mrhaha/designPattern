#pragma once
#include "AppCommon.h"
#include "server_data_report_protocol.h"
#include "server_cmd_data_report_define.h"
#include <map>
#include <string>

class CServerDataReport
{
public:
    static std::string SendDataReport(int iServerID,std::string roleID,CDataReportCommon& common,std::string strParam);


};


