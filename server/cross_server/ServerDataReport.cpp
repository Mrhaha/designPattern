#include "ServerDataReport.h"
#include <curl/curl.h>
#include "ServerConfig.h"
#include "ServerAlert.h"
#include "server_data_report_protocol.h"

int32_t dataReportCurlCallBackToString(char* data, size_t size, size_t nmemb, std::string* writerData)
{
    unsigned long totalSize = size * nmemb;

    if (!writerData) return 0;

    writerData->append(data, totalSize);

    return (int)totalSize;
}

std::string CServerDataReport::SendDataReport(int iServerID,std::string roleID,CDataReportCommon& common,std::string strParam)
{
    std::string sResult = "";
    char sendData[1024];
    char reportData[1024];
    memset(sendData, 0, sizeof(sendData));
    memset(reportData, 0, sizeof(reportData));
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl != 0)
    {
        std::shared_ptr<CCrossServerConfig> cross_config;
        CCrossServerConfig::GetConfig(cross_config);
        const auto &dataReportConfig = cross_config->GetDataReportConfig();

        curl_easy_setopt(curl, CURLOPT_URL, dataReportConfig.m_strURL.c_str());

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type:application/json");
        headers = curl_slist_append(headers, "charset=UTF-8");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1); // POST

        std::string data = R"([{"appid":"%s","event":"%s","udid":"%s","server_id":%d,"uuid":"%s","status": {"roleid":"%s"},"params":%s}])";
        sprintf(reportData,data.c_str(),dataReportConfig.m_strAppID.c_str(),common.m_strEven.c_str(),common.m_strUdid.c_str(),iServerID,common.m_strAccount.c_str(),roleID.c_str(),strParam.c_str());
        strcpy(sendData, reportData);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS,sendData);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dataReportCurlCallBackToString);//设置回调函数
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sResult);
        CURLcode res = curl_easy_perform(curl);
        common.m_strEven = std::string(reportData);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return sResult;
}



