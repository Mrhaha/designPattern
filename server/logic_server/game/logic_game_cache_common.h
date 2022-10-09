#pragma once

#include <string>
#include <vector>

#include "server_home_protocol.h"
#include "logic_game_cache_table.h"
#include "logic_link_common.h"

class CLogicCacheCommon
{
public:
    static list<pair<int, int>> GetCacheUserID(int iCount);
    
    static user_data_table_ptr_type LoadUserData(int iGroupID, int iUin,bool bNotMove=false);
    
    static user_data_table_ptr_type GetInCacheUserData(int iGroupID, int iUin);

	static bool IsBanAccount(user_data_table_ptr_type pUserData);
    static bool IsBanDelUser(user_data_table_ptr_type pUserData);
	static bool IsBanTalk(user_data_table_ptr_type pUserData);

    static int32_t GetUserPrimaryKeyIndex(user_data_table_ptr_type pUserData);

	static CPackUserMatch GetUserCrossInfo(user_data_table_ptr_type pUserData);

	static CPackUserRank GetUserRankInfo(user_data_table_ptr_type pUserData, int32_t iRank, int32_t iRankScore);

	static CPackHonorInfo GetHonorInfo(const TUserHonorTableValueType& value);

    static CPackConstellationInfo GetConstellationInfo(const TUserConstellationTableValueType& value);
    static CPackEquipInfo GetEquipInfo(const TUserEquipTableValueType& value);
    static CPackFashionInfo GetFashionInfo(const TFashionInfoTableValueType& value);
    static CPackHomeExploreInfo GetExploreQuestInfo(const TUserHomeExploreTableValueType& value);
    static CPackAvatarInfo GetAvatarInfo(const TUserAvatarTableValueType& value);

    static CPackAvyInfo GetUserAvyInfoPack(const TUserAvyTableValueType& stValue);
    static CPackLevelInfo GetUserLevelInfo(const TLevelInfoTableValueType& stValue);

	static int32_t GetEquipHonorID(user_data_table_ptr_type pUserData);

	static CPackPvpChallengerInfo GetPvpChallengerInfo(unsigned int uiRank);

    static CPackUserBrief GetUserBriefInfo(user_data_table_ptr_type pUserData);
    static CPackUserBriefCard GetCardBriefInfo(user_data_table_ptr_type pUserData, int32_t iCardID,int32_t iPositions);
	static CPackPvpChallengerInfo GetPvpChallengerInfo(int iGroupID, int iUin);
    static CPackPvpUserInfo GetPvpUserInfo(int iGroupID, int iUin);
    static CPackUserDetailInfo GetUserDetailInfo(int iGroupID, int iUin, int iTeamType = 0);

	static CPackPvpChallengerCardInfo GetPvpChallengerAttackCard(user_data_table_ptr_type pUserInfo, int iCardID, int iPosition = 0);

    static void AutoAddPhyPower(user_data_table_ptr_type pUserData);

    static void AutoAddVitality(user_data_table_ptr_type pUserData);

    static bool AddGameItem(user_data_table_ptr_type pUserData, const std::vector<CPackGameItem>& stBonusItemList, std::string& strBonusList, int32_t iCmd, const std::map<std::string, std::string>& info = {}, bool isMonitor = true);

    static bool AddGameItem(user_data_table_ptr_type pUserData, const std::vector<CPackGameItem>& stBonusItemList, int32_t iCmd, const std::map<std::string, std::string>& info = {}, bool isMonitor = true);

    static bool AddGameItem(user_data_table_ptr_type pUserData, const CPackGameItem& stBonusItem, std::string& strBonus, int32_t iCmd, const std::map<std::string, std::string>& info = {}, bool isMonitor = true);

    static bool AddGameItem(user_data_table_ptr_type pUserData, const CPackGameItem& stBonusItem, int32_t iCmd, const std::map<std::string, std::string>& info = {}, bool isMonitor = true);

    static bool AddGameItem(user_data_table_ptr_type pUserData, int iItemType, int iCardID, int iNum, int32_t iCmd, const std::map<std::string, std::string>& info = {}, bool isMonitor = true);

    static void PushGameItem(std::vector<CPackGameItem>& stBonusItemList, const CPackGameItem& stBonusItem);

	static bool CanConsume(user_data_table_ptr_type pUserData, int iItemType, int iCardID, int iNum, int32_t iCmd);

    static bool ConsumeGameItem(user_data_table_ptr_type pUserData, const CPackGameItem& stConsumeItem, int32_t iCmd, const std::map<std::string, std::string>& info = {});

    static bool ConsumeGameItem(user_data_table_ptr_type pUserData, const std::vector<CPackGameItem>& stConsumeItemVec, int32_t iCmd, const std::map<std::string, std::string>& info = {});

    static bool ConsumeGameItem(user_data_table_ptr_type pUserData, int iItemType, int iCardID, int iNum, int32_t iCmd, const std::map<std::string, std::string>& info = {});

    static bool SellGameItem(user_data_table_ptr_type pUserData, int iItemType, int iCardID, int iNum, int32_t iCmd);

	static bool ChangeRoleNick(user_data_table_ptr_type pUserData, const std::string& strNick, CLogicSQLTableRecordBase::sql_operation_t iOperation);

    static int GetMainRoleCardID(user_data_table_ptr_type pUserData);

	static bool IsMainRoleCardID(int32_t iCardID);

	static int32_t ResultMatchPVPScore(user_data_table_ptr_type pUserData, bool is_win);

	static int32_t ResultFairPVPScore(user_data_table_ptr_type pUserData, bool is_win);
    
    static int GetLeaderCardID(user_data_table_ptr_type pUserData);

    static std::string GetGameItemString(const std::vector<CPackGameItem>& stGameItems);

	static void AddCharm(user_data_table_ptr_type pUserInfo, int32_t iAddCharm);

    static bool AddUserFightReport(user_data_table_ptr_type pUser, CPackFightReportItem &stReportItem);

    static bool ClearUserFightReport(user_data_table_ptr_type pUser, int32_t iFightReportType);

    // 变长二进制位图
    static bool         VecBitmapCheckState(const CVecBitmap& stData, int32_t iIndex);
    static void         VecBitmapSetState(CVecBitmap& stData, int32_t iIndex);
    static std::string  VecBitmap2Bin(const CVecBitmap& stData);

    // 其他
    static int32_t  GetItemNum(user_data_table_ptr_type pUserData, int32_t iItemType, int32_t iItemID);
    static bool     HasEnoughItem(user_data_table_ptr_type pUserData, const CPackGameItem& stNeedItem);
    // stConsume -- [itemID, <itemType, itemNum>]
    static bool     CombineOneKey(user_data_table_ptr_type pUserData, int32_t iItemID, int32_t iNum, std::map<int, std::pair<int, int>>& stConsume);

    // 星纹
    static bool AddConstelItem(user_data_table_ptr_type pUserData, int32_t iGrade, int32_t iEquipTag, int32_t iSuit);

    // 家园
    static void GetHomeExploreReward(user_data_table_ptr_type pUserData, int iQuestID, const CPackCardTeam& stTeam, std::vector<CPackGameItem>& stRewards);

    // 队伍阵容
    static bool CheckTeamMemberValid(user_data_table_ptr_type pUserData, const CPackCardTeam& stTeam);
    static bool SetTeam(user_data_table_ptr_type pUserData, int32_t iType, CPackCardTeam& stTeam);
    static CPackCardTeam GetTeam(user_data_table_ptr_type pUserData, int32_t iType);

    static int CheckGuildName(std::string& strGuildName);
    static int CheckNickValid(std::string& strName);
    static int CheckAliasNickValid(std::string& strName);

    //刷新念写卡商店
    static void RefreshConstelShop(user_data_table_ptr_type pUserData);


    // list<[key, weight]>
    template<class Cont>
    static int32_t RandStlCont(const Cont& stVec, int32_t iTotalWeight = 0)
    {
        if(iTotalWeight == 0)
        {
            for(auto& iter : stVec)
                iTotalWeight += iter.second;
        }

        if(iTotalWeight <= 0)
            return (-1);

        int32_t iRandomWeight = CLogicCommon::GetRandNum(1,iTotalWeight,true);
        for(auto& iter : stVec)
        {
            if(iter.second >= iRandomWeight)
            {
                return iter.first;
            }

            iRandomWeight -= iter.second;
        }

        return (-1);
    }

private:
    static user_data_table_ptr_type GetUserDataFromDB(int iGroupID, int iUin);
};
