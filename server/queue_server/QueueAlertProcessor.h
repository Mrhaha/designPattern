#pragma once

#include "QueueProcessor.h"

class CQueueAlertProcessor : public CQueueProcessor
{
public:
    int32_t ProcessMessages(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength) override;

protected:
    int32_t ProcessServiceAlert(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
};
