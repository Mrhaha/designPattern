//
// Created by hzl on 2019/4/20.
//

#pragma once

#include <map>
#include <string>

class CQueueServerAlert
{
public:
    static std::string SendAlertNotice(const std::string& id, const std::map<std::string, std::string>& params);

    static std::string SendAlertApiMailNotice(const std::string& id, const std::map<std::string, std::string>& params);

    // static std::string SendAlertMailNotice(const std::string& id, const std::map<std::string, std::string>& params);

private:
    static bool CheckAlertNotice(const std::string& id, const std::map<std::string, std::string>& params);
};


