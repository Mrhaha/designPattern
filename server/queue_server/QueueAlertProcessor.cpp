#include "QueueAlertProcessor.h"
#include "QueueServerAlert.h"
#include "QueueServerConfig.h"
#include "server_cmd_cross_define.h"
#include "server_cross_protocol.h"

int32_t
CQueueAlertProcessor::ProcessMessages(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    int32_t iReturn = success;

    return iReturn;
}

int32_t CQueueAlertProcessor::ProcessServiceAlert(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CSRequestServiceAlert stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    // pushover告警
    auto result = CQueueServerAlert::SendAlertNotice(stReqBody.m_strID, stReqBody.m_stParams);
    Log(log_mask_detail, "[CQueueAlertProcessor::%s]send alert notice. id:%s|result:%s\n", __FUNCTION__, stReqBody.m_strID.c_str(), result.c_str());

    // 邮件告警
    auto apiResult = CQueueServerAlert::SendAlertApiMailNotice(stReqBody.m_strID, stReqBody.m_stParams);
    Log(log_mask_detail, "[CQueueAlertProcessor::%s]send alert api mail notice. id:%s|result:%s\n", __FUNCTION__, stReqBody.m_strID.c_str(), apiResult.c_str());

    return success;
}
