#pragma once

#include "logic_link_define.h"
#include "logic_game_cache_table.h"

class CLogicAdminProcessor
{
public:
    using ADMIN_PROCESSOR_MAP_VALUE = std::pair<std::string, std::function<bool(int32_t, int16_t, stLogicSoAdminClient&)>>;
    using ADMIN_PROCESSOR_MAP_TYPE = std::map<int32_t, ADMIN_PROCESSOR_MAP_VALUE>;
    static ADMIN_PROCESSOR_MAP_TYPE ADMIN_PROCESSOR_MAP;

public:
	static bool ProcessMessage(int32_t iUin, int16_t nGroupID, uint16_t usCmd, stLogicSoAdminClient &stClient);

    static bool ProcessRecharge(int32_t iUin, int16_t nGroupID, uint16_t usCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessModifyItem(int32_t iUin, int16_t nGroupID, uint16_t usCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

    static bool ProcessAddSystemMail(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

    static bool ProcessSetUserBan(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

    static bool ProcessDeleteSystemMail(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessGiftCardAward(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

    static bool ProcessSendActiveMail(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessModifyChapter(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessModifyHeroCard(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessModifyCreateTime(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessModifyGuide(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessModifyGuild(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessDeleteHero(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessDeleteRank(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessClearCache(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessSetGuildBulletin(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessJubaoAward(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessUserRank(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessUserCrossDay(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

    static bool ProcessAddFish(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

    static bool ProcessAddEquip(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessDismissGuild(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

	static bool ProcessSetGuildChief(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

    static bool ProcessGetCanDeposit(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

    static bool ProcessSetEntireWarPass(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

    static bool ProcessChangeGuildName(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

    static bool ProcessCopyUser(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

    static bool ProcessAddConstel(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

    static bool ProcessDelConstel(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);

    static bool ProcessChangeAgeLimit(int32_t iUin, int16_t nGroupID, int32_t iCmd, const char *pProtoName, stLogicSoAdminClient &stClient);


    template<typename T>
    static void ResponseMessage(int32_t iFd, uint16_t usCmd, const T& stRsp);

private:
    template <typename T>
    static bool DecodeClient(stLogicSoAdminClient& stClient, const char* pProtoName, T& stMsgPackObj);

	static bool ThreeStarPassLevel(user_data_table_ptr_type pUserData, int32_t iCmd, int iLevelID);
};
