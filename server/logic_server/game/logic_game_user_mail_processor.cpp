#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_item_protocol.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "logic_game_user_mail_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_cache_common.h"
#include "logic_game_global.h"
#include "logic_game_common_publisher.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_USER_MAIL, CLogicUserMailProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_READ_USER_MAIL, CLogicUserMailProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_USER_MAIL_BONUS, CLogicUserMailProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_DELETE_MAIL, CLogicUserMailProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_RECORD_USER_BIND_EMAIL, CLogicUserMailProcessor)


CLogicUserMailProcessor::CLogicUserMailProcessor(uint16_t usCmd, const std::string &strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicUserMailProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
	CLogicGlobal::AddAsyncUserGlobalMail(m_iUin, m_iGroupID);

    if ((int)m_usCmd == MSG_LOGIC_GET_USER_MAIL)
    {
        CPackGameItem stItem;
        CResponseGetUserMail stRsp;
        CResponseGetUserMail::user_mail_info_t stUserMailInfo;

        auto stUserMailItBeg = m_pUserInfo->m_stUserMailMap.Begin();
        auto stUserMailItEnd = m_pUserInfo->m_stUserMailMap.End();

        int iAmount = 0;
        int iNowSec = CLogicCommon::GetSec();

        for ( ; stUserMailItBeg != stUserMailItEnd; ++iAmount)
        {
            if (stUserMailItBeg->second->GetExpiredTime() < iNowSec && stUserMailItBeg->second->GetExpiredTime() != 0)
            {
                //删除设置过期时间的过期邮件
                m_pUserInfo->m_stUserMailMap.Erase(stUserMailItBeg++);
                continue;
            }
            stUserMailInfo.m_iMailID = stUserMailItBeg->first;
            stUserMailInfo.m_cIsNew = stUserMailItBeg->second->GetIsNew();
            stUserMailInfo.m_cIsReceived = stUserMailItBeg->second->GetIsReceived();
            stUserMailInfo.m_iCreateTime = stUserMailItBeg->second->GetCreateTime();
            stUserMailInfo.m_strTitle = stUserMailItBeg->second->GetTitle();
            stUserMailInfo.m_strSender = stUserMailItBeg->second->GetSender();
            stUserMailInfo.m_strContent = stUserMailItBeg->second->GetContent();
			stUserMailInfo.m_iAwardMultiple = stUserMailItBeg->second->GetAwardMultiple();
			stUserMailInfo.m_stAttachment = stUserMailItBeg->second->GetAttachment();
            stUserMailInfo.m_iExpiredTime = stUserMailItBeg->second->GetExpiredTime();
            stRsp.m_stUserMailVec.push_back(stUserMailInfo);
            ++stUserMailItBeg;
            stUserMailInfo.m_stAttachment.clear();
        }

        PROCESSOR_LOG_DEBUG << "SUCCESS|TOTAL_USER_MAIL:" << iAmount << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
    else if ((int)m_usCmd == MSG_LOGIC_READ_USER_MAIL)
    {
        CResponseReadUserMail stRsp;
        CRequestReadUserMail stParam;

        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);

        auto stUserMailIter = m_pUserInfo->m_stUserMailMap.Find(stParam.m_iMailID);
        if (stUserMailIter != m_pUserInfo->m_stUserMailMap.End())
        {
            stUserMailIter->second->SetIsNew(0);
            stUserMailIter->second->SetReadTime(CLogicCommon::GetSec());

            stRsp.m_iMailID = stParam.m_iMailID;

            PROCESSOR_LOG_DEBUG << "SUCCESS|MAIL_KEY:" << stParam.m_iMailID << std::endl;

            SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CAN_NOT_FOUND_MAIL_ID << "|MSG:" << GET_ERRMSG_BY_ERRCODE(CAN_NOT_FOUND_MAIL_ID)
                                << "|MAIL_KEY:" << stParam.m_iMailID << std::endl;

            SEND_ERRCODE_AND_RET(CAN_NOT_FOUND_MAIL_ID)
        }
    }
    else if ((int)m_usCmd == MSG_LOGIC_GET_USER_MAIL_BONUS)
    {
        CResponseGetUserMailBonus stRsp;
        CRequestGetUserMailBonus stParam;

        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);

        int32_t iNum = 1;
		std::string strAllMailKey = "";
        std::string strBonus = "";
		for (auto iMailID : stParam.m_iMailIDVec)
		{
            if(iNum > 100) break;

			const auto stUserMailIter = m_pUserInfo->m_stUserMailMap.Find(iMailID);
			if (stUserMailIter == m_pUserInfo->m_stUserMailMap.End())
			{
				continue;
			}

            ++iNum;

            // 未读邮件设置已读
            if(stUserMailIter->second->GetIsNew())
            {
                stUserMailIter->second->SetIsNew(0);
                stUserMailIter->second->SetReadTime(CLogicCommon::GetSec());
            }

            //邮件有未领取附件
            if (!stUserMailIter->second->GetAttachment().empty() && !stUserMailIter->second->GetIsReceived())
            {
                stUserMailIter->second->SetIsReceived(1);

                std::vector<CPackGameItem> items;
                for (const auto& bonus : stUserMailIter->second->GetAttachment())
                {
                    // 重复获得兑换道具
                    int iMissNum = 0;
                    int32_t iAddUpNum = 0;
                    auto m_pConfig = CLogicConfig::GetItemConfig().GetConfig(bonus.m_iCardID);
                    if (m_pConfig != nullptr)
                    {
                        auto stIter = m_pUserInfo->m_stItemMap.Find(m_pConfig->m_iItemID);
                        if (stIter != m_pUserInfo->m_stItemMap.End())
                        {
                            iAddUpNum = stIter->second->GetAddUpAmount();
                        }
                        // 如果是限制获取数量的道具
                        if(m_pConfig->m_iUniqueCount > 0)
                        {
                            if (iAddUpNum <= m_pConfig->m_iUniqueCount)
                            {
                                int iLastCount = m_pConfig->m_iUniqueCount - iAddUpNum;
                                int iCanAddNum = std::min(bonus.m_iNum, iLastCount);
                                iMissNum       = bonus.m_iNum - iCanAddNum;
                            }
                        }
                    }

                    std::string strOneItem;
                    if(CLogicCacheCommon::AddGameItem(m_pUserInfo, bonus, strOneItem, m_usCmd))
                    {
                        if (m_pConfig != nullptr)
                        {
                            if (m_pConfig->m_stRepeatExchangeItem.m_iItemType > 0 && iMissNum > 0)
                            {
                                auto stExchangeItem = m_pConfig->m_stRepeatExchangeItem;
                                stExchangeItem.m_iNum *= iMissNum;
                                stRsp.m_stAttachment.emplace_back(stExchangeItem.m_iItemType,
                                                                  stExchangeItem.m_iCardID,
                                                                  stExchangeItem.m_iNum);
                                items.emplace_back(stExchangeItem.m_iItemType, stExchangeItem.m_iCardID, stExchangeItem.m_iNum);
                            }
                        }

                        if (bonus.m_iNum - iMissNum > 0)
                        {
                            strBonus += strOneItem;
                            stRsp.m_stAttachment.emplace_back(bonus.m_iItemType, bonus.m_iCardID, bonus.m_iNum - iMissNum);
                            items.emplace_back(bonus.m_iItemType, bonus.m_iCardID, bonus.m_iNum);
                        }
                    }
                }

                // bill record
                CLogicBill::SaveMailBill(m_pUserInfo->m_iChannelID, CLogicBill::ESMBT_GET_MAIL_BONUS, m_strCmdName, stUserMailIter->second->m_stData, items);
            }

            stRsp.m_iMailIDVec.push_back(iMailID);
            strAllMailKey += std::to_string(iMailID) + ",";
		}

        /*
	    if (stRsp.m_stAttachment.empty())
	    {
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << MAIL_ATTACHMENT_IS_EMPTY_BAD_CMD
				<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(MAIL_ATTACHMENT_IS_EMPTY_BAD_CMD)
	    		<< "|MAIL_KEY" << strAllMailKey << std::endl;

			SEND_ERRCODE_AND_RET(MAIL_ATTACHMENT_IS_EMPTY_BAD_CMD)
	    }
        */

		PROCESSOR_LOG_DEBUG << "SUCCESS|MAIL_KEY:" << strAllMailKey << "|BONUS:" << strBonus << std::endl;

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
	else if ((int)m_usCmd == MSG_LOGIC_DELETE_MAIL)
	{
        CResponseDelUserMail stRsp;
        CRequestDelUserMail stParam;

		if (!DecodeMsgPack(stFormatData, stParam)) return (false);

		std::string strDelMailIDs = "";
		std::string strErrorNoFindMailIDs = "";
		std::string strErrorNoReadMailIDs = "";
		std::string strErrorAttachmentMailIDs = "";
        for (auto iMailID : stParam.m_iMailIDList)
        {
            auto stUserMailIter = m_pUserInfo->m_stUserMailMap.Find(iMailID);
            if (stUserMailIter != m_pUserInfo->m_stUserMailMap.End())
            {
                if (stUserMailIter->second->GetIsNew())
                {
                    strErrorNoReadMailIDs += std::to_string(iMailID) + ",";
                    continue;
                }

                if (!stUserMailIter->second->GetAttachment().empty() && !stUserMailIter->second->GetIsReceived())
                {
                    strErrorAttachmentMailIDs += std::to_string(iMailID) + ",";
                    continue;
                }

                m_pUserInfo->m_stUserMailMap.Erase(stUserMailIter);

                strDelMailIDs += std::to_string(iMailID) + ",";
                stRsp.m_iMailIDList.push_back(iMailID);
            }
            else
            {
                strErrorNoFindMailIDs += std::to_string(iMailID) + ",";
            }
        }

        if(!strErrorNoFindMailIDs.empty() || !strErrorNoReadMailIDs.empty() || !strErrorAttachmentMailIDs.empty())
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:Can Not Del MailIDs" << "|NotFindIDs:" << strErrorNoFindMailIDs << "|NoReadIDs:"
                                << strErrorNoReadMailIDs << "|NoAttachmentIDs:" << strErrorAttachmentMailIDs << std::endl;
        }

        if(strDelMailIDs.empty())
        {
            using namespace SERVER_ERRCODE;

            int32_t errorCode = CAN_NOT_FOUND_MAIL_ID;
            std::string strErrorCode = GET_ERRMSG_BY_ERRCODE(CAN_NOT_FOUND_MAIL_ID);
            if(!strErrorNoFindMailIDs.empty())
            {
                errorCode = CAN_NOT_FOUND_MAIL_ID;
                strErrorCode = GET_ERRMSG_BY_ERRCODE(CAN_NOT_FOUND_MAIL_ID);
            }
            else if(!strErrorNoReadMailIDs.empty())
            {
                errorCode = MAIL_NOT_READ;
                strErrorCode = GET_ERRMSG_BY_ERRCODE(MAIL_NOT_READ);
            }
            else if(!strErrorAttachmentMailIDs.empty())
            {
                errorCode = ATTACHMENT_MAIL_NOT_DELETE;
                strErrorCode = GET_ERRMSG_BY_ERRCODE(ATTACHMENT_MAIL_NOT_DELETE);
            }

            PROCESSOR_LOG_ERROR << "ERRCODE:" << errorCode << "|MSG:" << strErrorCode << std::endl;

            SEND_ERRCODE_AND_RET(errorCode)
        }
        else
        {
            PROCESSOR_LOG_DEBUG << "SUCCESS|MAIL_KEYS:" << strDelMailIDs << std::endl;

            SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        }
	}
    else if ((int)m_usCmd == MSG_LOGIC_RECORD_USER_BIND_EMAIL)
    {
        //如果已经绑定忽略
        if (m_pUserInfo->m_stUserInfoTable.GetIsBindEmail() != 0)
        {
            return true;
        }
        m_pUserInfo->m_stUserInfoTable.SetIsBindEmail(1);
        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_UserBindEmail;
        stEventData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
    }
    return (true);
}


