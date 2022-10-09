#pragma once

#include <string>
#include "tc_base64.h"

struct stLogicSoClientInfo;
struct TUserMailTableValueType;
struct TUserConstellationTableValueType;
struct TUserEquipTableValueType;

class CLogicBill
{
public:
    enum SaveMailBillType
    {
        ESMBT_ADD = 1,                  // 新加邮件
        ESMBT_GET_MAIL_BONUS,           // 提取附件后删除
        ESMBT_TIMEOUT_DELETE,           // 超时删除
    };
public:
    static void RefreshBillRation(int32_t iCmd, const std::string& strCmdName);

    static const string& GetBillRation();

    static const std::string& GetBillCmdName();

private:
    static int32_t m_iBillRationCmd;
    static std::string m_strBillRationCmdName;
    static int32_t m_iBillRationSequence;
    static std::string m_strBillRation;

public:
    static void SaveGameItemBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd
            , int32_t iItemType, int32_t iItemID, int32_t iCurrentNum, int32_t iDeltaNum, const std::string& strDescription);

    static void SaveConstelBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd, int32_t iDeltaValue, const TUserConstellationTableValueType& value);

    static void SaveEquipBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd, int32_t iDeltaValue, const TUserEquipTableValueType& value);

    static void SaveFightPassBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd
            , int32_t iLevelID, int32_t iFightPower, const std::vector<int32_t>& iTeamVec, const std::string& strAddition);

    static void SaveFirstFightBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd
            , int32_t iLevelID, int32_t iLevelStep, int32_t iFightPower);

    static void SaveAntiScoreBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd
            , int32_t iPassDay, int32_t iScore,int32_t iScoreLimit, int32_t iFightPower, const std::vector<int32_t>& iTeamVec, const std::string& strAddition);

    static void SaveAntiCheatingBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd
            , int32_t iLevelID, int32_t iFightPower, int32_t iAntiFightPower, const std::vector<int32_t>& iTeamVec, const std::string& strAddition,int32_t iPassTime);

    static void SaveUserLoginBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd, const std::string& strCmdName);

    static void SaveChatBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iLevel
            , int32_t iVipLevel, const std::string& strNick, int32_t iChannel, const std::string& strMsg, int32_t iTalkUin);

    static void SaveJubaoBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, const std::string &strNick, int32_t iDstUin
            , int32_t iDstGroupID, const std::string &strDstNick, int32_t iDstLevel, int32_t iDstVipLevel, int32_t iDstFightPower, int32_t iType
            , const std::string &strMsg, const std::string &strFromUI, int32_t iIsAward, const std::string& strUser);

    static void SaveMailBill(int32_t iChannelID, SaveMailBillType iHandleType, const std::string& strCmdName, const TUserMailTableValueType& mail, const std::vector<CPackGameItem>& items);

    static void SaveGameQuestionnaire(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iQuestionnaireID, int32_t iLevel
            , int32_t iVipLevel, int32_t iCombat, const std::string& strNick, std::vector<std::pair<std::string, std::string>> stAnswers);

    static void SaveGuildWish(int iStatTime, int iUin, int iGroupID, int iDstUin, int iDstGroupID, int iCardID, int iSwapCardID, int iConsume, int iAccept);
};
