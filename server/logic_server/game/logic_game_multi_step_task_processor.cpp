#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "server_task_protocol.h"
#include "logic_game_cache_common.h"
#include "logic_game_multi_step_task.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_multi_step_task_processor.h"
#include "server_cmd_data_report_define.h"
#include "server_data_report_protocol.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_MUTI_STEP_TASK_DETAIL, CLogicMultiStepTaskProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_MUTI_STEP_TASK_GIFT, CLogicMultiStepTaskProcessor)


CLogicMultiStepTaskProcessor::CLogicMultiStepTaskProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicMultiStepTaskProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    if((int)m_usCmd == MSG_LOGIC_GET_MUTI_STEP_TASK_DETAIL)
    {
        std::string strAllTaskInfoDump = "{";
        char szLogBuffer[128];
        
        CResponseMutiStepTaskStatus stRspProto;
        for(auto stIter = m_pUserInfo->m_stMutiStepTaskTableMap.Begin(); 
            stIter != m_pUserInfo->m_stMutiStepTaskTableMap.End(); stIter ++)
        {
            CResponseMutiStepTaskStatus::muti_step_task_detail stDetail;
            stDetail.m_iTaskID = stIter->second->m_stData.m_iTaskID;
            stDetail.m_iTaskStep = stIter->second->GetTaskStep();
            stDetail.m_iTargetNum = stIter->second->GetTargetNum();
            
            stRspProto.m_sMutiStepTaskDetailVec.push_back(stDetail);
            
            if(stIter != m_pUserInfo->m_stMutiStepTaskTableMap.Begin())
            {
                snprintf(szLogBuffer, sizeof(szLogBuffer), ",[%d,%d,%d]", 
                         stDetail.m_iTaskID, stDetail.m_iTaskStep, stIter->second->GetTargetNum());
            }
            else
            {
                snprintf(szLogBuffer, sizeof(szLogBuffer), "[%d,%d,%d]", 
                         stDetail.m_iTaskID, stDetail.m_iTaskStep, stIter->second->GetTargetNum());
            }
            
            strAllTaskInfoDump += szLogBuffer;
        }
        strAllTaskInfoDump += "}";
        
        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|MUTI_TASK:" << strAllTaskInfoDump << std::endl;
        
        SendToClient(stRspProto, SERVER_ERRCODE::SUCCESS);
    }
    else if((int)m_usCmd == MSG_LOGIC_GET_MUTI_STEP_TASK_GIFT)
    {
        CRequestGetMutiStepTaskGift stParam;
        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);
        
        if((stParam.m_iTaskID <= CRkLogicMutiStepTaskFactory::MUTI_STEP_TASK_TYPE_START) ||
           (stParam.m_iTaskID >= CRkLogicMutiStepTaskFactory::MUTI_STEP_TASK_TYPE_END))
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << MUTI_STEP_TASK_ID_IS_INVALID << "|MSG:" << GET_ERRMSG_BY_ERRCODE(MUTI_STEP_TASK_ID_IS_INVALID)
                                << "|TASK_ID:" << stParam.m_iTaskID << std::endl;

            SEND_ERRCODE_AND_RET(MUTI_STEP_TASK_ID_IS_INVALID)
        }
        
        auto stTaskInfoPtr = CRkLogicMutiStepTaskFactory::CreateTask(*m_pUserInfo, (enum CRkLogicMutiStepTaskFactory::MUTI_STEP_TASK_TYPE)stParam.m_iTaskID);
        TMutiStepTaskTable* pMutiTaskInfoDetail = nullptr;
        if(!stTaskInfoPtr.get() || ((pMutiTaskInfoDetail = stTaskInfoPtr->GetTaskInfo()) == nullptr) || !stTaskInfoPtr->IsInReceiveBonusStatus(pMutiTaskInfoDetail))
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << MUTI_STEP_TASK_IS_NOT_IN_RECEIVE_BONUS_STATE << "|MSG:" << GET_ERRMSG_BY_ERRCODE(MUTI_STEP_TASK_IS_NOT_IN_RECEIVE_BONUS_STATE)
                                << "|TASK_ID:" << stParam.m_iTaskID << std::endl;

            SEND_ERRCODE_AND_RET(MUTI_STEP_TASK_IS_NOT_IN_RECEIVE_BONUS_STATE)
        }
        
        CResponseGetMutiStepTaskGift stRspProto;
        stRspProto.m_iTaskID = stParam.m_iTaskID;
        
        auto stStepAndBonusPair = stTaskInfoPtr->GoToNextStepAndGetBonusConfig(*pMutiTaskInfoDetail);
        stRspProto.m_iNextStep = stStepAndBonusPair.first.m_iTaskStep;
        stRspProto.m_iNextStepTargetNum = stStepAndBonusPair.first.m_iTargetNum;

        static std::string strBonus;
        if(CLogicCacheCommon::AddGameItem(m_pUserInfo, stStepAndBonusPair.second, strBonus, m_usCmd))
        {
            stRspProto.m_stBonus = stStepAndBonusPair.second;

            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|TASK_ID:" << stParam.m_iTaskID << "|CURRENT_TASK_STEP:" << stRspProto.m_iNextStep << "|CURRENT_TARGET_NUM:" 
                                << stRspProto.m_iNextStepTargetNum << "|BONUS_ITEMS:" << strBonus << std::endl;
            
            SendToClient(stRspProto, SUCCESS);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << MUTI_STEP_TASK_GIFT_ITEM_IS_NOT_IN_CONFIG << "|MSG:" << GET_ERRMSG_BY_ERRCODE(MUTI_STEP_TASK_GIFT_ITEM_IS_NOT_IN_CONFIG)
                                << "|TASK_ID:" << stParam.m_iTaskID << "|CURRENT_TASK_STEP:" << stRspProto.m_iNextStep << "|CURRENT_TARGET_NUM:" 
                                << stRspProto.m_iNextStepTargetNum << "|BONUS_ITEMS:" << strBonus << std::endl;
            
            SendToClient(MUTI_STEP_TASK_GIFT_ITEM_IS_NOT_IN_CONFIG);
        }


    }
    
    return (true);
}


