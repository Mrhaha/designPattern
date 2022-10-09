//
// Created by hzl on 2019/4/20.
//

#include "QueueServerAlert.h"
#include "QueueServerConfig.h"
#include "common_log_utility.h"
#include "common_logengine.h"
#include <curl/curl.h>
#include <sstream>
// #include "common_smtp163_socket.h"

int32_t curlCallBackToString(char* data, size_t size, size_t nmemb, std::string* writerData)
{
    unsigned long totalSize = size * nmemb;

    if (!writerData) return 0;

    writerData->append(data, totalSize);

    return (int)totalSize;
}

std::string CQueueServerAlert::SendAlertNotice(const std::string& id, const std::map<std::string, std::string>& params)
{
    std::string sResult;
    if (!CheckAlertNotice(id, params))
    {
        return sResult;
    }

    curl_global_init(CURL_GLOBAL_ALL);
    if (CURL *curl = curl_easy_init(); curl)
    {
        std::shared_ptr<CQueueServerConfig> queue_config;
        CQueueServerConfig::GetConfig(queue_config);
        const auto &config = queue_config->GetQueuePushoverConfig().GetPushoverConfig(id);

        for(auto& admin : queue_config->GetQueuePushoverConfig().m_stPushoverAdminList)
        {
            std::string token;
            auto iter = admin.m_stPlatformTokenMap.find(get_platform_info<QUEUE_BUILD_PLATFORM>());
            if(iter != admin.m_stPlatformTokenMap.end())
            {
                token = iter->second;
            }
            else
            {
                token = admin.m_strDevToken;
            }

            curl_easy_setopt(curl, CURLOPT_URL, admin.m_strUrl.c_str());

            curl_httppost* post= nullptr;
            curl_httppost* last = nullptr;

            std::string message = config.m_strMessage.Replace(params);
            std::string priority = std::to_string(config.m_iPriority);
            std::string retry = std::to_string(config.m_iRetry);
            std::string expire = std::to_string(config.m_iExpire);
            std::string timestamp = std::to_string(now_sec());
            curl_formadd(&post, &last, CURLFORM_COPYNAME, "token", CURLFORM_COPYCONTENTS, token.c_str(), CURLFORM_END);
            curl_formadd(&post, &last, CURLFORM_COPYNAME, "user", CURLFORM_COPYCONTENTS, admin.m_strUserKey.c_str(), CURLFORM_END);
            curl_formadd(&post, &last, CURLFORM_COPYNAME, "message", CURLFORM_COPYCONTENTS, message.c_str(), CURLFORM_END);
            curl_formadd(&post, &last, CURLFORM_COPYNAME, "title", CURLFORM_COPYCONTENTS, config.m_strTitle.c_str(), CURLFORM_END);
            curl_formadd(&post, &last, CURLFORM_COPYNAME, "priority", CURLFORM_COPYCONTENTS, priority.c_str(), CURLFORM_END);
            curl_formadd(&post, &last, CURLFORM_COPYNAME, "retry", CURLFORM_COPYCONTENTS, retry.c_str(), CURLFORM_END);
            curl_formadd(&post, &last, CURLFORM_COPYNAME, "expire", CURLFORM_COPYCONTENTS, expire.c_str(), CURLFORM_END);
            curl_formadd(&post, &last, CURLFORM_COPYNAME, "sound", CURLFORM_COPYCONTENTS, config.m_strSound.c_str(), CURLFORM_END);
            curl_formadd(&post, &last, CURLFORM_COPYNAME, "timestamp", CURLFORM_COPYCONTENTS, timestamp.c_str(), CURLFORM_END);
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallBackToString);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sResult);
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                TRACESVR(log_mask_system_error, "[CQueueServerAlert:%s] curl_easy_perform fail! url:%s  |result:%s | code:%d |error:%s\n",
                         __FUNCTION__, admin.m_strUrl.c_str(), sResult.c_str(), res, curl_easy_strerror(res));
            }
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return sResult;
}

std::string CQueueServerAlert::SendAlertApiMailNotice(const std::string& id, const std::map<std::string, std::string>& params)
{
    std::string sResult = "";
    std::shared_ptr<CQueueServerConfig> queue_config;
    CQueueServerConfig::GetConfig(queue_config);
    const auto& stPushoverConfig = queue_config->GetQueuePushoverConfig();

    if(false == stPushoverConfig.m_stMailNotify.m_bSwitch)
        return sResult;

    const auto &config = stPushoverConfig.GetPushoverConfig(id);

    if(!config.m_stPosVec.empty())
    {
        std::string message = config.m_strMessage.Replace(params);
        std::string strAddrList;
        for(auto& pos : config.m_stPosVec)
        {
            auto iter = stPushoverConfig.m_stOtherMember.lower_bound(pos);
            auto iterEnd = stPushoverConfig.m_stOtherMember.upper_bound(pos);
            for(; iter != iterEnd; ++iter)
            {
                if(strAddrList.empty())
                    strAddrList = iter->second.m_strMailAddr;
                else
                    strAddrList += ("|" + iter->second.m_strMailAddr);
            }
        }

        curl_global_init(CURL_GLOBAL_ALL);
        if (CURL *curl = curl_easy_init(); curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, queue_config->GetZooQueueConfig().m_stQueueConfig.m_strApiMailUrl.c_str());

            curl_httppost* post= nullptr;
            curl_httppost* last = nullptr;

            curl_formadd(&post, &last, CURLFORM_COPYNAME, "message", CURLFORM_COPYCONTENTS, message.c_str(), CURLFORM_END);
            curl_formadd(&post, &last, CURLFORM_COPYNAME, "addrs", CURLFORM_COPYCONTENTS, strAddrList.c_str(), CURLFORM_END);
            curl_formadd(&post, &last, CURLFORM_COPYNAME, "title", CURLFORM_COPYCONTENTS, config.m_strTitle.c_str(), CURLFORM_END);
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallBackToString);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sResult);
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                TRACESVR(log_mask_system_error, "[CQueueServerAlert:%s] curl_easy_perform fail! result:%s |code:%d |error:%s\n",
                         __FUNCTION__, sResult.c_str(), res, curl_easy_strerror(res));
            }

            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
    }

    return sResult;
}

/*
std::string CQueueServerAlert::SendAlertMailNotice(const std::string& id, const std::map<std::string, std::string>& params)
{
    std::string sResult = "";
    std::shared_ptr<CQueueServerConfig> queue_config;
    CQueueServerConfig::GetConfig(queue_config);
    const auto& stPushoverConfig = queue_config->GetQueuePushoverConfig();

    if(false == stPushoverConfig.m_stMailNotify.m_bSwitch)
        return sResult;

    const auto &config = stPushoverConfig.GetPushoverConfig(id);

    if(!config.m_stPosVec.empty())
    {
        std::string message = config.m_strMessage.Replace(params);
        CSMTPSocket stSmtpSocket;

        // 登录mail服务器
        sResult = stSmtpSocket.ConnectEmailServer(stPushoverConfig.m_stMailNotify.m_strMailServerUrl,
                                                  stPushoverConfig.m_stMailNotify.m_strMailUserName, stPushoverConfig.m_stMailNotify.m_strMailUserPw);
        if(!sResult.empty())
        {
            stSmtpSocket.CloseEmailConnect();
            return sResult;
        }

        std::vector<std::string> stAddrVec;
        // 抄送自己，避免可能出现的垃圾邮件拦截
        stAddrVec.push_back(stPushoverConfig.m_stMailNotify.m_strMailUserName);
        for(auto& pos : config.m_stPosVec)
        {
            auto iter = stPushoverConfig.m_stOtherMember.lower_bound(pos);
            auto iterEnd = stPushoverConfig.m_stOtherMember.upper_bound(pos);
            for(; iter != iterEnd; ++iter)
            {
                stAddrVec.push_back(iter->second.m_strMailAddr);
            }
        }

        sResult = stSmtpSocket.SendOneEmail(stPushoverConfig.m_stMailNotify.m_strMailUserName, stAddrVec, config.m_strTitle, message);
        if(!sResult.empty())
        {
            // 发送失败
            TRACESVR(log_mask_system_error, "[CQueueServerAlert:%s] SendOneEmail fail! id:%s |error:%s\n", __FUNCTION__, id.c_str(), sResult.c_str());
        }

        // 登出
        stSmtpSocket.CloseEmailConnect();
    }

    return sResult;
}
*/

bool CQueueServerAlert::CheckAlertNotice(const std::string& id, const std::map<std::string, std::string>& params)
{
    return true;
}
