#include "common_datetime.h"
#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "server_home_protocol.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "logic_game_home_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"
#include "logic_game_item_factory.h"
#include "logic_game_common_publisher.h"
#include "logic_game_buff.h"
#include "logic_game_global.h"
#include "common_datetime.h"

//消息ID
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_HOME_INFO, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_UNEQUIP_GOODS, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_EQUIP_GOODS, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_BUILD, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_CHANGE_HERO, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_PUT_BACK_HERO, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_FISHING_BEGIN, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_FISHING_END, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_FISHING_CHANGE_ROD, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_COOKBOOK, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_GOODS_MOVE, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_ENTER, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_EXPLORE_INFO, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_EXPLORE_START, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_EXPLORE_REWARD, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_GARDEN_PLANT, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_GARDEN_OP, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_GARDEN_UPDATE, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_VISIT, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_SAVE_HOME, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_HOME_VISIT_OTHER, CLogicHomeProcessor)

REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_GET_SIMPLE_GAME_INFO, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_PLAY_SIMPLE_GAME, CLogicHomeProcessor)

REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_LEVEL_INFO, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_GET_LEVEL_REWARD, CLogicHomeProcessor)



REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_HOT_SPRING_CREATE, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_HOT_SPRING_INVITE, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_HOT_SPRING_JOIN, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_HOT_SPRING_GET_REWARD, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_HOT_SPRING_INFO, CLogicHomeProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_HOME_HOT_SPRING_OTHER_JOIN, CLogicHomeProcessor)


REGISTER_CMD_FACTORY(MSG_LOGIC_HOME_TOPIC_SAVE, CLogicHomeProcessor)



CLogicHomeProcessor::CLogicHomeProcessor(uint16_t usCmd, const std::string &strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicHomeProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
	bool bRet = false;

	switch (m_usCmd)
	{
    case MSG_LOGIC_GET_HOME_INFO:
        bRet = DoUserRunGetHomeInfo(stFormatData);
        break;
    case MSG_LOGIC_HOME_ENTER:
        bRet = DoUserRunEnterHome(stFormatData);
        break;
    case MSG_LOGIC_HOME_UNEQUIP_GOODS:
        bRet = DoUserRunUnequipGoods(stFormatData);
        break;
    case MSG_LOGIC_HOME_EQUIP_GOODS:
        bRet = DoUserRunEquipGoods(stFormatData);
        break;
    case MSG_LOGIC_HOME_BUILD:
        bRet = DoUserRunHomeBuild(stFormatData);
        break;
    case MSG_LOGIC_HOME_CHANGE_HERO:
        bRet = DoUserRunHomeChangeHero(stFormatData);
        break;
    case MSG_LOGIC_HOME_PUT_BACK_HERO:
        bRet = DoUserRunHomePutBackHero(stFormatData);
        break;
    case MSG_LOGIC_HOME_FISHING_BEGIN:
        bRet = DoUserRunHomeFishingBegin(stFormatData);
        break;
    case MSG_LOGIC_HOME_FISHING_END:
        bRet = DoUserRunHomeFishingEnd(stFormatData);
        break;
    case MSG_LOGIC_HOME_FISHING_CHANGE_ROD:
        bRet = DoUserRunHomeFishingChangeRod(stFormatData);
        break;
    case MSG_LOGIC_HOME_COOKBOOK:
        bRet = DoUserRunHomeCookBook(stFormatData);
        break;
    case MSG_LOGIC_HOME_GOODS_MOVE:
        bRet = DoUserRunHomeGoodsMove(stFormatData);
        break;
    case MSG_LOGIC_HOME_EXPLORE_INFO:
        bRet = DoUserRunHomeExploreInfo(stFormatData);
        break;
    case MSG_LOGIC_HOME_EXPLORE_START:
        bRet = DoUserRunHomeExploreStart(stFormatData);
        break;
    case MSG_LOGIC_HOME_EXPLORE_REWARD:
        bRet = DoUserRunHomeExploreReward(stFormatData);
        break;
    case MSG_LOGIC_HOME_GARDEN_PLANT:
        bRet = DoUserRunHomeGardenPlant(stFormatData);
        break;
    case MSG_LOGIC_HOME_GARDEN_OP:
        bRet = DoUserRunHomeGardenOp(stFormatData);
        break;
    case MSG_LOGIC_HOME_GARDEN_UPDATE:
        bRet = DoUserRunHomeGardenUpdate(stFormatData);
        break;
    case MSG_LOGIC_HOME_VISIT:
        bRet = DoUserRunHomeVisit(stFormatData);
        break;

	case MSG_LOGIC_TOUHOU_GET_SIMPLE_GAME_INFO:
	    bRet = DoUserRunGetSimpleGameState(stFormatData);
        break;
	case MSG_LOGIC_TOUHOU_PLAY_SIMPLE_GAME:
	    bRet = DoUserRunPlaySimpleGame(stFormatData);
        break;
    case MSG_LOGIC_HOME_SAVE_HOME:
        bRet = DoUserRunHomeSave(stFormatData);
        break;

    case MSG_LOGIC_HOME_LEVEL_INFO:
        bRet = DoUserRunHomeGetLevelInfo(stFormatData);
        break;
    case MSG_LOGIC_HOME_GET_LEVEL_REWARD:
        bRet = DoUserRunHomeGetLevelReward(stFormatData);
        break;

    case MSG_LOGIC_HOME_HOT_SPRING_INFO:
        bRet = DoUserRunHomeHotSpringInfo(stFormatData);
        break;
    case MSG_LOGIC_HOME_HOT_SPRING_CREATE:
        bRet = DoUserRunHomeHotSpringCreate(stFormatData);
        break;
    case MSG_LOGIC_HOME_HOT_SPRING_INVITE:
        bRet = DoUserRunHomeHotSpringInvite(stFormatData);
        break;
    case MSG_LOGIC_HOME_HOT_SPRING_JOIN:
        bRet = DoUserRunHomeHotSpringJoin(stFormatData);
        break;
    case MSG_LOGIC_HOME_HOT_SPRING_GET_REWARD:
        bRet = DoUserRunHomeHotSpringGetReward(stFormatData);
        break;
    case MSG_LOGIC_HOME_TOPIC_SAVE:
        bRet = DoUserRunHomeTopicSave(stFormatData);
        break;
	default:
		PROCESSOR_LOG_ERROR << "NOT FOUND PROCESSOR WITH CMD:" << m_usCmd << std::endl;
		break;
	}

	return bRet;
}

bool CLogicHomeProcessor::DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    bool bRet = false;

    switch (stMsgHead.m_usCmd)
    {
    case MSG_CROSS_HOME_VISIT_OTHER:
        bRet = DoServerRunHomeVisitOther(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_HOME_VISIT:
        bRet = DoServerRunHomeVisit(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_HOME_HOT_SPRING_INVITE:
        bRet = DoServerRunHomeHotSpringInvite(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_HOME_HOT_SPRING_JOIN:
        bRet = DoServerRunHomeHotSpringJoin(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_HOME_HOT_SPRING_OTHER_JOIN:
        bRet = DoServerRunHomeHotSpringOtherJoin(stMsgHead, stFormatData);
        break;
    default:
        break;
    }

    return bRet;
}

bool CLogicHomeProcessor::DoUserRunGetHomeInfo(const msgpack::unpacked& stFormatData)
{
    CResponseGetHomeInfo stRsp;

    // 初始化
    if(m_pUserInfo->m_stHomeMap.Empty())
    {
        const auto& stConfig = CLogicConfig::GetHomeGoodsConfig().m_stHomeMap;
        for(auto& config : stConfig)
        {
            if(false == config.second.m_bIsInit) continue;

            TUserHomeTableValueType stData;
            stData.m_iUid = m_iUin;
            stData.m_iGroupID = m_iGroupID;
            stData.m_iHomeID = m_pUserInfo->m_stHomeMap.Size() + 1;
            stData.m_iHomeType = config.first;
            stData.m_iPos = config.second.m_iInitID;
            stData.m_iPageID = config.second.m_iInitPageID;
            stData.m_iFloorPageID = config.second.m_iInitFloorPageID;
            const auto stRet = m_pUserInfo->m_stHomeMap.Insert(std::make_pair(stData.m_iHomeID, stData));
            if (!stRet.second)
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << INTERN_ERROR << "|MSG:m_stHomeMap Init Error! |HOME_TYPE:" << config.first << std::endl;

                SEND_ERRCODE_AND_RET(INTERN_ERROR);
            }

            // 默认房间布置
            for (const int initItemsId : config.second.m_iInitGoodsVec)
            {
                auto initItemConfig = CLogicConfig::GetHomeGoodsConfig().GetHomeInitGoodsConfig(initItemsId);

                if(initItemConfig == nullptr)
                {
                    SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
                }
                auto itemConfig  = CLogicConfig::GetItemConfig().GetConfig(initItemConfig->m_iItemId);

                if(itemConfig == nullptr)
                {
                    SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
                }

                if (itemConfig->m_iSubType == CLogicConfigDefine::EIST_HomeOrnament)
                {
                    TUserHomeGoodsTableValueType stGoodsData;
                    stGoodsData.m_iUid = m_iUin;
                    stGoodsData.m_iGroupID = m_iGroupID;
                    stGoodsData.m_iHomeID = stData.m_iHomeID;
                    stGoodsData.m_iGoodsIndex = CLogicCacheCommon::GetUserPrimaryKeyIndex(m_pUserInfo);
                    stGoodsData.m_iGoodsID = initItemConfig->m_iItemId;
                    stGoodsData.m_iPosX = initItemConfig->m_iInitPosX;
                    stGoodsData.m_iPosY = initItemConfig->m_iInitPosY;
                    stGoodsData.m_cStat = initItemConfig->m_cStat;
                    stGoodsData.m_cQuadrant = initItemConfig->m_cQuadrant;
                    stGoodsData.m_iSeatIndex = initItemConfig->m_iSeatId;
                    const auto stGoodsRet = m_pUserInfo->m_stHomeGoodsMap[stData.m_iHomeID].Insert(std::make_pair(stGoodsData.m_iGoodsIndex, stGoodsData));
                    if (!stGoodsRet.second)
                    {
                        PROCESSOR_LOG_ERROR << "INSERT NEW HOME_GOODS ERROR!" << "|INDEX:" << stGoodsData.m_iGoodsIndex << std::endl;

                        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR)
                    }
                }
                else if (itemConfig->m_iSubType == CLogicConfigDefine::EIST_HomeFootcloth)
                {
                    auto &stFootsMap = m_pUserInfo->m_stHomeFootclothMap[stData.m_iHomeID];
                    auto iterFoot = stFootsMap.Find(initItemConfig->m_iItemId);
                    if(iterFoot == stFootsMap.End())
                    {
                        TUserHomeFootclothTableValueType stFootClothData;
                        stFootClothData.m_iUid = m_iUin;
                        stFootClothData.m_iGroupID = m_iGroupID;
                        stFootClothData.m_iHomeID = stData.m_iHomeID;
                        stFootClothData.m_iFootclothID = initItemConfig->m_iItemId;
                        auto stFootRet = stFootsMap.Insert(std::make_pair(stFootClothData.m_iFootclothID, stFootClothData));
                        if (!stFootRet.second)
                        {
                            PROCESSOR_LOG_ERROR << "INSERT NEW HOME_FOOT_CLOTH ERROR!" << "|ITEM_ID:" << initItemConfig->m_iItemId << std::endl;

                            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR)
                        }

                        iterFoot = stFootRet.first;
                    }

                    int32_t iPos = CLogicConfigHomeGoodsParser::GetUniqueIDByPos(initItemConfig->m_iInitPosX, initItemConfig->m_iInitPosY);
                    auto stFootData = iterFoot->second->GetPosVec();
                    if(stFootData.find(iPos) != stFootData.end())
                    {
                        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::POS_ERROR)
                    }

                    stFootData[iPos] = initItemConfig->m_cStat;
                    iterFoot->second->SetPosVec(stFootData);
                }
            }
        }

        // 默认鱼竿
        CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, CLogicConfig::GetHomeGoodsConfig().m_iDefaultFishingRod, 1, m_usCmd);
        m_pUserInfo->m_stIBusyData.SetFishingRod(CLogicConfig::GetHomeGoodsConfig().m_iDefaultFishingRod);
    }

    for(auto iter = m_pUserInfo->m_stHomeMap.Begin(); iter != m_pUserInfo->m_stHomeMap.End(); ++iter)
    {
        CPackHomeElem stHome;
        stHome.m_iHomeID = iter->first;
        stHome.m_iHomeType = iter->second->GetHomeType();
        stHome.m_iPos = iter->second->GetPos();
        stHome.m_iPageID = iter->second->GetPageID();
        stHome.m_iFloorPageID = iter->second->GetFloorPageID();
        stHome.m_stHeroVec = iter->second->GetHeroVec();
        stRsp.m_stHomeVec.emplace_back(std::move(stHome));
    }

    for(auto iterFish = m_pUserInfo->m_stFishItemMap.Begin(); iterFish != m_pUserInfo->m_stFishItemMap.End(); ++iterFish)
    {
        CPackHomeFishElem stFish;
        stFish.m_iFishIndex = iterFish->first;
        stFish.m_iFishID = iterFish->second->GetFishID();
        stFish.m_iValue = iterFish->second->GetValue();
        stFish.m_cGrade = iterFish->second->GetGrade();
        stRsp.m_stFishVec.emplace_back(stFish);
    }

    stRsp.m_iFishingRod = m_pUserInfo->m_stIBusyData.GetFishingRod();

    for(auto& iterGarden : m_pUserInfo->m_stHomeGardenMap)
    {
        for(auto iter = iterGarden.second.Begin(); iter != iterGarden.second.End(); ++iter)
        {
            CPackHomeGardenElem stElem;
            stElem.m_iLandID = iter->first;
            stElem.m_iSeedID = iter->second->GetSeedID();
            stElem.m_iSeedStat = iter->second->GetSeedStat();
            stElem.m_iStatTime = iter->second->GetStatTime();
            stRsp.m_stGardenMap[iterGarden.first].emplace_back(std::move(stElem));
        }
    }

    for(auto& iterGoods : m_pUserInfo->m_stHomeGoodsMap)
    {
        for(auto iter = iterGoods.second.Begin(); iter != iterGoods.second.End(); ++iter)
        {
            CPackHomeGoodsElem stGoods;
            stGoods.m_iGoodsIndex = iter->first;
            stGoods.m_iGoodsID = iter->second->GetGoodsID();
            stGoods.m_cQuadrant = iter->second->GetQuadrant();
            stGoods.m_iPosX = iter->second->GetPosX();
            stGoods.m_iPosY = iter->second->GetPosY();
            stGoods.m_cStat = iter->second->GetStat();
            stGoods.m_iSeatIndex = iter->second->GetSeatIndex();
            stRsp.m_stEquipGoodsMap[iterGoods.first].emplace_back(std::move(stGoods));
        }

    }

    for(auto& iterFoot : m_pUserInfo->m_stHomeFootclothMap)
    {
        for(auto iter = iterFoot.second.Begin(); iter != iterFoot.second.End(); ++iter)
        {
            CPackHomeFootclothElem stFoot;
            stFoot.m_iFootclothID = iter->first;
            stFoot.m_stPosVec = iter->second->GetPosVec();
            stRsp.m_stFootclothMap[iterFoot.first].emplace_back(std::move(stFoot));
        }
    }

    stRsp.m_stHomeBgm = m_pUserInfo->m_stIBusyData.GetHomeBgm();
    stRsp.m_stUid2HomeVisit = m_pUserInfo->m_stOnlyInCacheData.m_stUid2HomeVisit;
    stRsp.m_iFishingTimes = m_pUserInfo->m_stUserDailyTable.GetFishingTimes();


    for(auto iter = m_pUserInfo->m_stUserHomeTopicMap.Begin(); iter != m_pUserInfo->m_stUserHomeTopicMap.End(); ++iter)
    {
        stRsp.m_stHomeTopicInfo.emplace_back(iter->second->GetHomeTopicInfo());
    }

    stRsp.m_stHomeGoodBuff = m_pUserInfo->m_stIBusyData.GetHomeBuffID();
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoUserRunEnterHome(const msgpack::unpacked& stFormatData)
{
    CRequestHomeEnter stReq;
    CResponseHomeEnter stRsp;
    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    auto iterHome = m_pUserInfo->m_stHomeMap.Find(stReq.m_iHomeID);
    if(iterHome == m_pUserInfo->m_stHomeMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_NONEXISTENT);
    }

    stRsp.m_stHomeInfo.m_iHomeID = iterHome->first;
    stRsp.m_stHomeInfo.m_iHomeType = iterHome->second->GetHomeType();
    stRsp.m_stHomeInfo.m_iPos = iterHome->second->GetPos();
    stRsp.m_stHomeInfo.m_iPageID = iterHome->second->GetPageID();
    stRsp.m_stHomeInfo.m_iFloorPageID = iterHome->second->GetFloorPageID();
    stRsp.m_stHomeInfo.m_stHeroVec = iterHome->second->GetHeroVec();

    auto& stHomeGoodsMap = m_pUserInfo->m_stHomeGoodsMap[stReq.m_iHomeID];
    for(auto iterGoods = stHomeGoodsMap.Begin(); iterGoods != stHomeGoodsMap.End(); ++iterGoods)
    {
        CPackHomeGoodsElem stGoods;
        stGoods.m_iGoodsIndex = iterGoods->first;
        stGoods.m_iGoodsID = iterGoods->second->GetGoodsID();
        stGoods.m_cQuadrant = iterGoods->second->GetQuadrant();
        stGoods.m_iPosX = iterGoods->second->GetPosX();
        stGoods.m_iPosY = iterGoods->second->GetPosY();
        stGoods.m_cStat = iterGoods->second->GetStat();
        stGoods.m_iSeatIndex = iterGoods->second->GetSeatIndex();
        stRsp.m_stEquipGoodsVec.emplace_back(std::move(stGoods));
    }

    auto& stHomeFootMap = m_pUserInfo->m_stHomeFootclothMap[stReq.m_iHomeID];
    for(auto iterFoot = stHomeFootMap.Begin(); iterFoot != stHomeFootMap.End(); ++iterFoot)
    {
        CPackHomeFootclothElem stFoot;
        stFoot.m_iFootclothID = iterFoot->first;
        stFoot.m_stPosVec = iterFoot->second->GetPosVec();
        stRsp.m_stFootclothVec.emplace_back(std::move(stFoot));
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoUserRunUnequipGoods(const msgpack::unpacked& stFormatData)
{
    CRequestHomeUnequipGoods stReq;
    CResponseHomeUnequipGoods stRsp;
    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }
    int32_t bRet = UnEquipGoods(stReq);
    if (bRet != SERVER_ERRCODE::SUCCESS)
    {
        SEND_ERRCODE_AND_RET(bRet)
    }

    stRsp.m_iHomeID = stReq.m_iHomeID;
    stRsp.m_iItemID = stReq.m_iItemID;
    stRsp.m_iPara = stReq.m_iPara;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoUserRunEquipGoods(const msgpack::unpacked& stFormatData)
{
    CRequestHomeEquipGoods stReq;
    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }
    int32_t bRet = EquipGoods(stReq);
    if (bRet != SERVER_ERRCODE::SUCCESS)
    {
        SEND_ERRCODE_AND_RET(bRet)
    }
    CResponseHomeEquipGoods stRsp;
    stRsp.m_iHomeID = stReq.m_iHomeID;
    stRsp.m_stGoods = stReq.m_stGoodsPara;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    return true;
}

bool CLogicHomeProcessor::DoUserRunHomeBuild(const msgpack::unpacked& stFormatData)
{
    CRequestHomeBuild stReq;
    CResponseHomeBuild stRsp;
    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    auto pConfig = CLogicConfig::GetHomeGoodsConfig().GetHomeConfig(stReq.m_iHomeType);
    if(nullptr == pConfig || !pConfig->m_bIsExtend)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    if(stReq.m_iPos <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    for(auto iter = m_pUserInfo->m_stHomeMap.Begin(); iter != m_pUserInfo->m_stHomeMap.End(); ++iter)
    {
        if(stReq.m_iHomeType == CLogicConfigDefine::EHT_Farmland)
        {
            if(iter->second->GetHomeType() == stReq.m_iHomeType && iter->second->GetPos() == stReq.m_iPos)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
            }
        }
        else
        {
            if(iter->second->GetPos() == stReq.m_iPos)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
            }
        }
    }

    if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, pConfig->m_stBuildConsume, m_usCmd))
    {
        PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode() << "|MSG:" << CLogicError::GetErrMsg() << std::endl;

        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
    }

    TUserHomeTableValueType stData;
    stData.m_iUid = m_iUin;
    stData.m_iGroupID = m_iGroupID;
    stData.m_iHomeID = m_pUserInfo->m_stHomeMap.Size() + 1;
    stData.m_iHomeType = stReq.m_iHomeType;
    stData.m_iPos = stReq.m_iPos;
    stData.m_iPageID = pConfig->m_iInitPageID;
    stData.m_iFloorPageID = pConfig->m_iInitFloorPageID;
    auto stRet = m_pUserInfo->m_stHomeMap.Insert(std::make_pair(stData.m_iHomeID, stData));
    if (!stRet.second)
    {
        PROCESSOR_LOG_ERROR << "BUILD HOME ERROR!" << "|HOME_TYPE:" << stReq.m_iHomeType << std::endl;

        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
    }

    stRsp.m_stHomeInfo.m_iHomeID = stRet.first->first;
    stRsp.m_stHomeInfo.m_iHomeType = stRet.first->second->GetHomeType();
    stRsp.m_stHomeInfo.m_iPos = stRet.first->second->GetPos();
    stRsp.m_stHomeInfo.m_iFloorPageID = stRet.first->second->GetFloorPageID();
    stRsp.m_stHomeInfo.m_iPageID = stRet.first->second->GetPageID();

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoUserRunHomeChangeHero(const msgpack::unpacked& stFormatData)
{
    CRequestHomeChangeHero stReq;
    CResponseHomeChangeHero stRsp;
    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    auto iterHome = m_pUserInfo->m_stHomeMap.Find(stReq.m_iHomeID);
    if(iterHome == m_pUserInfo->m_stHomeMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_NONEXISTENT)
    }

    auto pConfig = CLogicConfig::GetHomeGoodsConfig().GetHomeConfig(iterHome->second->GetHomeType());
    if(nullptr == pConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR)
    }

    auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(stReq.m_iCardID);
    if(iterCard == m_pUserInfo->m_stHeroCardTableMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED)
    }

    auto stData = iterHome->second->GetHeroVec();
    auto iterHero = stData.find(stReq.m_iCardID);
    if(iterHero != stData.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if(pConfig->m_iLimitHeroNum <= int(stData.size()))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_HERO_TOO_MUCH)
    }

    // 清除其他房间该伙伴数据
    for(auto iter = m_pUserInfo->m_stHomeMap.Begin(); iter != m_pUserInfo->m_stHomeMap.End(); ++iter)
    {
        if(iter->first == stReq.m_iHomeID) continue;

        auto iterCardInfo = iter->second->GetHeroVec().find(stReq.m_iCardID);
        if(iterCardInfo != iter->second->GetHeroVec().end())
        {
            if(iterCardInfo->second.m_iGoodsIndex > 0)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_BUSY)
            }

            // 移除该房间的伙伴
            auto stOldData = iter->second->GetHeroVec();
            stOldData.erase(stReq.m_iCardID);
            iter->second->SetHeroVec(stOldData);

            break;
        }
    }

    CPackHomeHeroInfo stInfo;
    stData.emplace(stReq.m_iCardID, stInfo);
    iterHome->second->SetHeroVec(stData);

    stRsp.m_iHomeID = stReq.m_iHomeID;
    stRsp.m_iCardID = stReq.m_iCardID;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoUserRunHomePutBackHero(const msgpack::unpacked& stFormatData)
{
    CRequestHomePutBackHero stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    auto iterHome = m_pUserInfo->m_stHomeMap.Find(stReq.m_iHomeID);
    if(iterHome == m_pUserInfo->m_stHomeMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_NONEXISTENT)
    }

    auto stHeroVec = iterHome->second->GetHeroVec();
    auto iterHomeCard = stHeroVec.find(stReq.m_iCardID);
    if(iterHomeCard == stHeroVec.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if(iterHomeCard->second.m_iGoodsIndex > 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_BUSY)
    }

    stHeroVec.erase(iterHomeCard);
    iterHome->second->SetHeroVec(stHeroVec);

    SendToClient(SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoUserRunHomeFishingBegin(const msgpack::unpacked& stFormatData)
{
    CRequestHomeFishingBegin stReq;
    CResponseHomeFishingBegin stRsp;
    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    auto& stConfig = CLogicConfig::GetHomeFuncConfig();
    auto pFishpondConfig = stConfig.GetFishpondConfig(stReq.m_iFishpondID);
    if(nullptr == pFishpondConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
    }


    // 判断背包
    if(m_pUserInfo->m_stFishItemMap.Size() >= CLogicConfigHomeFuncParser::E_MaxFishBag)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FISH_BAG_SPACE_NOT_ENOUGH)
    }

    auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(stReq.m_iCardID);
    if(iterCard == m_pUserInfo->m_stHeroCardTableMap.End())
    {
        SEND_ERRCODE_AND_RET1(SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED, stReq.m_iCardID)
    }

    auto stWeightMap = pFishpondConfig->m_stFishWeight;
    auto pRodConfig = stConfig.GetFishRodConfig(m_pUserInfo->m_stIBusyData.GetFishingRod());
    if(pRodConfig)
    {
        // 计算鱼竿对指定鱼的加成
        auto iter = pRodConfig->m_stRodFuncMap.lower_bound(CLogicConfigHomeFuncParser::ERFT_FishWeight);
        auto iterEnd = pRodConfig->m_stRodFuncMap.upper_bound(CLogicConfigHomeFuncParser::ERFT_FishWeight);
        for(; iter != iterEnd; ++iter)
        {
            auto iterFishWeight = stWeightMap.m_stWeight.find(iter->second.second);
            if(iterFishWeight != stWeightMap.m_stWeight.end())
            {
                iterFishWeight->second += iter->second.first;
                stWeightMap.m_iTotalWeight += iter->second.first;
            }
        }
    }

    // 随机鱼
    int32_t iFishID = CLogicCacheCommon::RandStlCont(stWeightMap.m_stWeight, stWeightMap.m_iTotalWeight);
    if(iFishID <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR)
    }

    // 钓鱼次数超了
    if(m_pUserInfo->m_stUserDailyTable.GetFishingTimes() > pFishpondConfig->m_iBagTimes)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishID = iFishID;
    m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishValue = 0;

    auto pFishConfig = stConfig.GetFishConfig(iFishID);
    if(nullptr == pFishConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR)
    }

    // 计算大鱼概率
    auto iterBigWeight = pFishpondConfig->m_stBigWeight.find(iFishID);
    int32_t iBigWeight = (iterBigWeight == pFishpondConfig->m_stBigWeight.end() ? 0 : iterBigWeight->second);
    // 计算鱼竿加成
    if(iBigWeight > 0 && pRodConfig)
    {
        auto iter = pRodConfig->m_stRodFuncMap.lower_bound(CLogicConfigHomeFuncParser::ERFT_BigFishWeight);
        auto iterEnd = pRodConfig->m_stRodFuncMap.upper_bound(CLogicConfigHomeFuncParser::ERFT_BigFishWeight);
        for(; iter != iterEnd; ++iter)
        {
            iBigWeight += iter->second.first;
        }
    }

    // 钓到大鱼
    if(CLogicCommon::GetRandNum() % CLogicConfigDefine::ESD_NormalPctWeight < iBigWeight)
    {
        m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishValue = CLogicCommon::GetRandNum(pFishConfig->m_iValueMin, pFishConfig->m_iValueMax, true);

        // 计算鱼竿对鱼王的加成
        if(pRodConfig)
        {
            int iKingWeight = 0;
            auto iter = pRodConfig->m_stRodFuncMap.lower_bound(CLogicConfigHomeFuncParser::ERFT_FishKingWeight);
            auto iterEnd = pRodConfig->m_stRodFuncMap.upper_bound(CLogicConfigHomeFuncParser::ERFT_FishKingWeight);
            for(; iter != iterEnd; ++iter)
            {
                iKingWeight += iter->second.first;
            }

            if(iKingWeight > 0 && CLogicCommon::GetRandNum() % CLogicConfigDefine::ESD_NormalPctWeight < iKingWeight)
            {
                m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishValue = pFishConfig->m_iValueMax;
            }
        }

        // 计算公会buff对于五星鱼的加成
        auto guildBuffIter = m_pUserInfo->m_stOnlyInCacheData.m_mapGuildBuff.find(EnumGuildBuffType::GuildBuffFishing);
        if (guildBuffIter != m_pUserInfo->m_stOnlyInCacheData.m_mapGuildBuff.end())
        {
            // 鱼品质
            auto iterGrade = pFishConfig->m_stValueToGradeMap.upper_bound(m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishValue);
            if(iterGrade != pFishConfig->m_stValueToGradeMap.begin())
                --iterGrade;

            if(iterGrade->second < guildBuffIter->second.m_iBuffParam[0]) //小于五星的鱼再摇一次
            {
                auto iFiveStarWeight = CLogicCommon::GetRandNum() % CLogicConfigDefine::ESD_NormalPctWeight;
                if (iFiveStarWeight < guildBuffIter->second.m_iBuffParam[1])
                {
                    m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishValue = CLogicCommon::GetRandNum(pFishConfig->m_iValueMin, pFishConfig->m_iValueMax, true);
                }
            }
        }
    }

    m_pUserInfo->m_stUserDailyTable.AddFishingTimes(1);
    stRsp.m_iFishID = m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishID;
    stRsp.m_iValue = m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishValue;
    stRsp.m_iFishingTimes = m_pUserInfo->m_stUserDailyTable.GetFishingTimes();
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoUserRunHomeFishingEnd(const msgpack::unpacked& stFormatData)
{
    CRequestHomeFishingEnd stReq;
    CResponseHomeFishingEnd stRsp;
    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    if(0 == m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishID)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    auto pFishpondConfig = CLogicConfig::GetHomeFuncConfig().GetFishpondConfig(stReq.m_iFishpondID);
    if(nullptr == pFishpondConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
    }

    // 判断背包
    if(m_pUserInfo->m_stFishItemMap.Size() >= CLogicConfigHomeFuncParser::E_MaxFishBag)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FISH_BAG_SPACE_NOT_ENOUGH)
    }

    auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(stReq.m_iCardID);
    if(iterCard == m_pUserInfo->m_stHeroCardTableMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR)
    }

    const auto& config = CLogicConfig::GetHomeFuncConfig();

    bool bKing = false;
    switch (stReq.m_iResultType)
    {
    case CRequestHomeFishingEnd::fish_failed:
        break;
    case CRequestHomeFishingEnd::fish_and_bag:
    case CRequestHomeFishingEnd::bag_success:
    {
        std::vector<CPackGameItem> stRewards;
        CLogicConfig::GetCardbagConfig().GetRandBonusByName(pFishpondConfig->m_strBag, stRewards);
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRewards, m_usCmd);

        if(stReq.m_iResultType == CRequestHomeFishingEnd::bag_success)
        {
            break;
        }
    }
    case CRequestHomeFishingEnd::fish_success:
    {
        auto pFishConfig = config.GetFishConfig(m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishID);
        if(nullptr == pFishConfig)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR)
        }

        // 大鱼
        if(m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishValue > 0)
        {
            TUserFishItemTableValueType stFishData;
            stFishData.m_iUid = m_iUin;
            stFishData.m_iGroupID = m_iGroupID;
            stFishData.m_iFishIndex = stRsp.m_stFish.m_iFishIndex = CLogicCacheCommon::GetUserPrimaryKeyIndex(m_pUserInfo);
            stFishData.m_iFishID = stRsp.m_stFish.m_iFishID = m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishID;
            stFishData.m_iValue = stRsp.m_stFish.m_iValue = m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishValue;

            // 鱼品质
            auto iterGrade = pFishConfig->m_stValueToGradeMap.upper_bound(stFishData.m_iValue);
            if(iterGrade != pFishConfig->m_stValueToGradeMap.begin())
                --iterGrade;

            if(iterGrade != pFishConfig->m_stValueToGradeMap.end())
            {
                stFishData.m_cGrade = stRsp.m_stFish.m_cGrade = iterGrade->second;
            }

            m_pUserInfo->m_stFishItemMap.Insert(std::make_pair(stFishData.m_iFishIndex, stFishData));

            // 鱼王
            bKing = (stRsp.m_stFish.m_iValue >= pFishConfig->m_iValueMax);
        }
        else
        {
            // 小鱼
            CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, pFishConfig->m_iItemID, 1, m_usCmd);
        }

        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_FishingSuccessTimes;
        stEventData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
        break;
    }
    default:
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }



    m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishID = 0;
    m_pUserInfo->m_stOnlyInCacheData.m_iCacheFishValue = 0;

    stRsp.m_iCardID = stReq.m_iCardID;
    stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    if(stRsp.m_stFish.m_iFishID > 0)
    {
        // 大鱼
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_FishingBigFish;
        stEventData.m_iPara = stRsp.m_stFish.m_iFishID;
        stEventData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

        stEventData.m_eType = CLogicConfigDefine::ECET_FishingTotalBigFish;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

        stEventData.m_eType = CLogicConfigDefine::ECET_FishingTargetFish;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

        // 鱼王
        if(bKing)
        {
            stEventData.m_eType = CLogicConfigDefine::ECET_FishingKingFish;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

            stEventData.m_eType = CLogicConfigDefine::ECET_FishingTotalKingFish;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
        }

        //发送增加家园经验事件
        stEventData.m_eType = CLogicConfigDefine::ECET_GetHomeLevelFish;
        stEventData.m_iPara = stRsp.m_stFish.m_iFishID;
        stEventData.m_iParaEx = stRsp.m_stFish.m_cGrade;
        stEventData.m_iCount = 1;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

        // 徽章
        CLogicBadgeManage::UpdateBadgeCond(m_pUserInfo, CLogicConfigDefine::EBCT_CollectFish, stRsp.m_stFish.m_iFishID, {});
    }

    return true;
}

bool CLogicHomeProcessor::DoUserRunHomeFishingChangeRod(const msgpack::unpacked& stFormatData)
{
    CRequestHomeFishingChangeRod stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    if(stReq.m_iItemID == m_pUserInfo->m_stIBusyData.GetFishingRod())
    {
        SendToClient(SERVER_ERRCODE::SUCCESS);
        return true;
    }

    auto pConfig = CLogicConfig::GetItemConfig().GetConfig(stReq.m_iItemID);
    if(nullptr == pConfig || pConfig->m_iSubType != CLogicConfigDefine::EIST_HomeFishingRod)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if(CLogicCacheCommon::GetItemNum(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, stReq.m_iItemID) <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    m_pUserInfo->m_stIBusyData.SetFishingRod(stReq.m_iItemID);

    SendToClient(SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoUserRunHomeCookBook(const msgpack::unpacked& stFormatData)
{
    CRequestHomeCookBook stReq;
    CResponseHomeCookBook stRsp;
    if(!DecodeMsgPack(stFormatData, stReq)) return false;

    auto pConfig = CLogicConfig::GetHomeFuncConfig().GetCookBookConfig(stReq.m_iCookBookID);
    if(!pConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
    }

    if(pConfig->m_iUnlockItem > 0 && CLogicCacheCommon::GetItemNum(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, pConfig->m_iUnlockItem) <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COOKBOOK_IS_LOCKED)
    }

    // 检查普通食材
    if(stReq.m_stConsumeFoodID.size() != pConfig->m_stFoodTypeVec.size())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FOOD_TYPE_ERROR)
    }

    std::map<int, int> stGradeWeight;
    std::vector<CPackGameItem> stConsumeVec;
    for(std::size_t index = 0; index < pConfig->m_stFoodTypeVec.size(); ++index)
    {
        auto pItemConfig = CLogicConfig::GetItemConfig().GetConfig(stReq.m_stConsumeFoodID[index]);
        if(!pItemConfig || pItemConfig->m_iFoodType != pConfig->m_stFoodTypeVec[index].first)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FOOD_TYPE_ERROR)
        }

        if(pConfig->m_stFoodTypeVec[index].second > 0 && pConfig->m_stFoodTypeVec[index].second != pItemConfig->m_cGrade)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FOOD_TYPE_ERROR)
        }

        stConsumeVec.emplace_back(CLogicConfigDefine::EXCHANGE_ITEM, pItemConfig->m_iItemID, 1);
        stGradeWeight[pItemConfig->m_cGrade] = CLogicConfigDefine::ESD_RandomWeight;
    }

    // 检查鱼
    if(!pConfig->m_stNeedFish.empty())
    {
        if(stReq.m_stConsumeFishIndex.size() != pConfig->m_stNeedFish.size())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FOOD_TYPE_ERROR)
        }

        std::set<int> stIndexSet;
        for(std::size_t index = 0; index < pConfig->m_stNeedFish.size(); ++index)
        {
            auto iterFish = m_pUserInfo->m_stFishItemMap.Find(stReq.m_stConsumeFishIndex[index]);
            if(iterFish == m_pUserInfo->m_stFishItemMap.End())
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FISH_NOT_EXIST)
            }

            if(pConfig->m_stNeedFish[index].first != iterFish->second->GetFishID())
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FOOD_TYPE_ERROR)
            }

            if(pConfig->m_stNeedFish[index].second > 0 && pConfig->m_stNeedFish[index].second != iterFish->second->GetGrade())
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FISH_VALUE_ERROR)
            }

            stIndexSet.insert(stReq.m_stConsumeFishIndex[index]);
            stGradeWeight[iterFish->second->GetGrade()] = CLogicConfigDefine::ESD_RandomWeight;
        }

        if(stIndexSet.size() != stReq.m_stConsumeFishIndex.size())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }
    }

    //判断是否存在指定的家园buff
    //料理添加最高星级合成概率
    auto userHomeBuff = m_pUserInfo->m_stIBusyData.GetHomeBuffID();
    auto targetBuffIter = userHomeBuff.find(EnumHomeGoodBuffType::HomeGoodBuff_Cook);
    auto hasTargetBuff = (targetBuffIter != userHomeBuff.end());

    if (hasTargetBuff)
    {
        stGradeWeight.begin()->second += targetBuffIter->second.m_fParam1;
    }

    // 随机奖励
    int iRandGrade = CLogicCacheCommon::RandStlCont(stGradeWeight);
    auto iterRewardConfig = pConfig->m_stRewardGradeToItemID.find(iRandGrade);
    if(iterRewardConfig == pConfig->m_stRewardGradeToItemID.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::CONFIG_LOGIC_ERROR)
    }

    // 消耗道具
    if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stConsumeVec, m_usCmd))
    {
        PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode() << "|MSG:" << CLogicError::GetErrMsg() << std::endl;

        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
    }

    // 消耗鱼
    if(!pConfig->m_stNeedFish.empty())
    {
        for(auto fish_index : stReq.m_stConsumeFishIndex)
        {
            m_pUserInfo->m_stFishItemMap.Erase(fish_index);
            stRsp.m_stConsumeFishIndex.push_back(fish_index);
        }
    }

    CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, iterRewardConfig->second, 1, m_usCmd);

    stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    // 徽章更新
    CLogicBadgeManage::UpdateBadgeCond(m_pUserInfo, CLogicConfigDefine::EBCT_CookBookTimes, 1, {});

    return true;
}

bool CLogicHomeProcessor::DoUserRunHomeGoodsMove(const msgpack::unpacked& stFormatData)
{
    CRequestHomeGoodsMove stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    int32_t bRet = MoveGoods(stReq);
    if (bRet != SERVER_ERRCODE::SUCCESS)
    {
        SEND_ERRCODE_AND_RET(bRet)
    }

    SendToClient(SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoUserRunHomeExploreInfo(const msgpack::unpacked& stFormatData)
{
    CResponseHomeExploreInfo stRsp;
    for(auto iter = m_pUserInfo->m_stHomeExploreMap.Begin(); iter != m_pUserInfo->m_stHomeExploreMap.End(); ++iter)
    {
        if(iter->second->GetState() == CLogicConfigHomeGardenExploreParser::EQS_Received)
            continue;

        stRsp.m_stInfoVec.push_back(CLogicCacheCommon::GetExploreQuestInfo(iter->second->m_stData));
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoUserRunHomeExploreStart(const msgpack::unpacked& stFormatData)
{
    CRequestHomeExploreStart stReq;
    CResponseHomeExploreStart stRsp;
    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    auto pConfig = CLogicConfig::GetHomeGardenExploreConfig().GetExploreQuestConfig(stReq.m_iQuestID);
    if(nullptr == pConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
    }

    auto iterData = m_pUserInfo->m_stHomeExploreMap.Find(stReq.m_iQuestID);
    if(iterData == m_pUserInfo->m_stHomeExploreMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if(iterData->second->GetState() != CLogicConfigHomeGardenExploreParser::EQS_CanAccept)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QUEST_STATE_ERROR)
    }

    // 检查队伍
    if(!CLogicCacheCommon::CheckTeamMemberValid(m_pUserInfo, stReq.m_stTeam))
    {
        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
    }

    // 检查探索任务伙伴限制
    if(!CheckExploreTeamLimit(pConfig, stReq.m_stTeam))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TEAM_MEMBER_ERROR)
    }

    // 判断是否已在其他探索队列中
    for(auto iter = m_pUserInfo->m_stHomeExploreMap.Begin(); iter != m_pUserInfo->m_stHomeExploreMap.End(); ++iter)
    {
        if(iter->second->GetState() != CLogicConfigHomeGardenExploreParser::EQS_Accepted)
            continue;

        auto& stExploreTeam = iter->second->GetTeam();
        for(auto cardID : stReq.m_stTeam)
        {
            if(cardID == 0) continue;

            if(std::find(stExploreTeam.begin(), stExploreTeam.end(), cardID) != stExploreTeam.end())
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_EXPLORE_CARD_OCCUPY)
            }
        }
    }

    iterData->second->SetState(CLogicConfigHomeGardenExploreParser::EQS_Accepted);
    iterData->second->SetTeam(stReq.m_stTeam);
    iterData->second->SetEndTime(CLogicCommon::GetSec() + pConfig->m_iTime);

    stRsp.m_stExploreInfo = CLogicCacheCommon::GetExploreQuestInfo(iterData->second->m_stData);
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    // 通用事件
    CLogicEventData stEventData;
    stEventData.m_eType = CLogicConfigDefine::ECET_HomeExploreTimes;
    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

    return true;
}

bool CLogicHomeProcessor::DoUserRunHomeExploreReward(const msgpack::unpacked& stFormatData)
{
    CRequestHomeExploreReward stReq;
    CResponseHomeExploreReward stRsp;
    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    auto pConfig = CLogicConfig::GetHomeGardenExploreConfig().GetExploreQuestConfig(stReq.m_iQuestID);
    if(nullptr == pConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
    }

    auto iterData = m_pUserInfo->m_stHomeExploreMap.Find(stReq.m_iQuestID);
    if(iterData == m_pUserInfo->m_stHomeExploreMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if(iterData->second->GetState() != CLogicConfigHomeGardenExploreParser::EQS_Accepted)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QUEST_STATE_ERROR)
    }

    if(iterData->second->GetEndTime() > CLogicCommon::GetSec())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::DAILY_TASK_NOT_COMPLETED)
    }

    std::vector<CPackGameItem> stRewards;
    CLogicCacheCommon::GetHomeExploreReward(m_pUserInfo, stReq.m_iQuestID, iterData->second->GetTeam(), stRewards);

    static std::string strBonus;
    CLogicCacheCommon::AddGameItem(m_pUserInfo, stRewards, strBonus, m_usCmd);

    iterData->second->SetState(CLogicConfigHomeGardenExploreParser::EQS_Received);
    iterData->second->SetEndTime(0);
    iterData->second->SetTeam({});

    stRsp.m_iQuestID = stReq.m_iQuestID;
    stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|EXPLORE_QUEST_ID:" << stReq.m_iQuestID << "|BONUS_ITEM:" << strBonus << std::endl;

    // 通用事件
    CLogicEventData stEventData;
    stEventData.m_eType = CLogicConfigDefine::ECET_HomeExploreCompleteTimes;
    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

    // 徽章
    if(pConfig->m_bIsCbt)
    {
        CLogicBadgeManage::UpdateBadgeCond(m_pUserInfo, CLogicConfigDefine::EBCT_CompletSpecExplore, 1, {});
    }

    return true;
}

bool CLogicHomeProcessor::DoUserRunHomeGardenPlant(const msgpack::unpacked& stFormatData)
{
    CRequestHomeGardenPlant stReq;
    CResponseHomeGardenPlant stRsp;
    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    auto iterHome = m_pUserInfo->m_stHomeMap.Find(stReq.m_iHomeID);
    if(iterHome == m_pUserInfo->m_stHomeMap.End() || iterHome->second->GetHomeType() != CLogicConfigDefine::EHT_Farmland)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_TYPE_LIMIT);
    }

    if(stReq.m_iIndex <= 0 || stReq.m_iIndex > CLogicConfig::GetHomeGardenExploreConfig().m_iFarmlandNum)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    auto pConfig = CLogicConfig::GetHomeGardenExploreConfig().GetGardenConfig(stReq.m_iSeedID);
    if(nullptr == pConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    auto iterLand = m_pUserInfo->m_stHomeGardenMap[stReq.m_iHomeID].Find(stReq.m_iIndex);
    if(iterLand == m_pUserInfo->m_stHomeGardenMap[stReq.m_iHomeID].End())
    {
        TUserHomeGardenTableValueType stValue;
        stValue.m_iUid = m_pUserInfo->m_iUin;
        stValue.m_iGroupID = m_pUserInfo->m_iGroupID;
        stValue.m_iHomeID = stReq.m_iHomeID;
        stValue.m_iLandID = stReq.m_iIndex;
        auto stRet = m_pUserInfo->m_stHomeGardenMap[stReq.m_iHomeID].Insert(std::make_pair(stValue.m_iLandID, stValue));
        if (stRet.second == false)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERROR:" << INTERN_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                                << "|HOME_ID:" << stReq.m_iHomeID << "|LAND_ID:" << stReq.m_iIndex << std::endl;

            SEND_ERRCODE_AND_RET(INTERN_ERROR);
        }

        iterLand = stRet.first;
    }

    if(iterLand->second->GetSeedID())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_BUILD_SLOT_NOT_EMPTY_ERROR);
    }

    auto stConsume = LOGIC_GAME_ITEM_FACTORY_INS->CreateGameItem(CLogicConfigDefine::EXCHANGE_ITEM, stReq.m_iSeedID, 1, m_usCmd, m_pUserInfo);
    if (!stConsume.get() || !stConsume->CanConsume())
    {
        using namespace SERVER_ERRCODE;
        PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                            << "|MSG:" << CLogicError::GetErrMsg()
                            << std::endl;

        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
    }

    stConsume->Consume();

    iterLand->second->SetSeedID(stReq.m_iSeedID);
    iterLand->second->SetSeedStat(CLogicConfigHomeGardenExploreParser::EOT_Harvest);
    iterLand->second->SetStatTime(CLogicCommon::GetSec() + pConfig->m_stStatVec[1].m_iTime);

    stRsp.m_iHomeID = stReq.m_iHomeID;
    stRsp.m_stPlant.m_iLandID = iterLand->first;
    stRsp.m_stPlant.m_iSeedID = iterLand->second->GetSeedID();
    stRsp.m_stPlant.m_iSeedStat = iterLand->second->GetSeedStat();
    stRsp.m_stPlant.m_iStatTime = iterLand->second->GetStatTime();

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoUserRunHomeGardenOp(const msgpack::unpacked& stFormatData)
{
    CRequestHomeGardenOp stReq;
    CResponseHomeGardenOp stRsp;
    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    auto iterHome = m_pUserInfo->m_stHomeMap.Find(stReq.m_iHomeID);
    if(iterHome == m_pUserInfo->m_stHomeMap.End() || iterHome->second->GetHomeType() != CLogicConfigDefine::EHT_Farmland)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_TYPE_LIMIT);
    }

    std::vector<int32_t> stIndex;
    auto& stGardenMap = m_pUserInfo->m_stHomeGardenMap[stReq.m_iHomeID];
    for (auto index : stReq.m_iIndex)
    {
        if(index <= 0 || index > CLogicConfig::GetHomeGardenExploreConfig().m_iFarmlandNum)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        for(auto iterPlant = stGardenMap.Begin(); iterPlant != stGardenMap.End(); ++iterPlant)
        {
            if(iterPlant->second->GetSeedID() <= 0) continue;

            if(index && index != iterPlant->first) continue;

            if(iterPlant->second->GetStatTime() > CLogicCommon::GetSec()) continue;

            auto pConfig = CLogicConfig::GetHomeGardenExploreConfig().GetGardenConfig(iterPlant->second->GetSeedID());
            if(pConfig == nullptr) continue;

            stIndex.push_back(index);
        }
    }


    for (auto index : stIndex)
    {
        for(auto iterPlant = stGardenMap.Begin(); iterPlant != stGardenMap.End(); ++iterPlant)
        {

            if(index && index != iterPlant->first) continue;

            auto pConfig = CLogicConfig::GetHomeGardenExploreConfig().GetGardenConfig(iterPlant->second->GetSeedID());
            for(auto& item : pConfig->m_stRewards)
            {
                stRsp.m_stRewards.emplace_back(item.m_iItemType, item.m_iCardID, item.m_iNum);
            }
            //判断是否存在指定的家园buff
            auto userHomeBuff = m_pUserInfo->m_stIBusyData.GetHomeBuffID();
            auto targetBuffIter = userHomeBuff.find(EnumHomeGoodBuffType::HomeGoodBuff_Garden);
            auto hasTargetBuff = (targetBuffIter != userHomeBuff.end());

            if (hasTargetBuff)
            {
                std::vector<std::pair<int32_t,int32_t>> weightPair;
                weightPair.emplace_back(1,targetBuffIter->second.m_fParam1);
                weightPair.emplace_back(2,CLogicConfigDefine::ESD_RandomWeight - targetBuffIter->second.m_fParam1);
                auto ret = CLogicCacheCommon::RandStlCont(weightPair);

                //添加额外收获
                if (1 == ret)
                {
                    for(auto& item : pConfig->m_stRewards)
                    {
                        stRsp.m_stRewards.emplace_back(item.m_iItemType, item.m_iCardID, item.m_iNum);
                    }
                }
            }

            iterPlant->second->SetSeedID(0);
            iterPlant->second->SetSeedStat(0);
            iterPlant->second->SetStatTime(0);

            // 通用事件
            CLogicEventData stEventData;
            stEventData.m_eType = CLogicConfigDefine::ECET_HarvestCount;
            stEventData.m_iCmd = m_usCmd;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
        }
    }

    static std::string strBonusItem;
    if(!stRsp.m_stRewards.empty())
    {
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stRewards, strBonusItem, m_usCmd);
    }

    stRsp.m_iHomeID = stReq.m_iHomeID;
    stRsp.m_stSuccessIndex = stIndex;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);


    PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|BONUS_ITEM:" << strBonusItem << std::endl;

    return true;
}
//废弃
bool CLogicHomeProcessor::DoUserRunHomeGardenUpdate(const msgpack::unpacked& stFormatData)
{
    CResponseHomeGardenUpdate stRsp;

    const int32_t iNowTime = CLogicCommon::GetSec();
    int32_t iOpTimes = 0;
    for(auto& stHomeGarden : m_pUserInfo->m_stHomeGardenMap)
    {
        auto iterHome = m_pUserInfo->m_stHomeMap.Find(stHomeGarden.first);
        if(iterHome == m_pUserInfo->m_stHomeMap.End() || iterHome->second->GetHeroVec().empty())
        {
            continue;
        }

        std::map<int, THeroCardInfoTable*> stHeroTable;
        for(auto& card : iterHome->second->GetHeroVec())
        {
            auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(card.first);
            if(iterCard == m_pUserInfo->m_stHeroCardTableMap.End())
            {
                continue;
            }

            stHeroTable.insert(std::make_pair(iterCard->first, iterCard->second));
        }

        if(stHeroTable.empty()) continue;

        CPackGardenUpdateElem stUpdateData;
        for(auto iterPlant = stHomeGarden.second.Begin(); iterPlant != stHomeGarden.second.End(); ++iterPlant)
        {
            if(iterPlant->second->GetSeedID() <= 0 || iterPlant->second->GetStatTime() > iNowTime) continue;

            auto pConfig = CLogicConfig::GetHomeGardenExploreConfig().GetGardenConfig(iterPlant->second->GetSeedID());
            if(nullptr == pConfig || int(pConfig->m_stStatVec.size()) <= iterPlant->second->GetSeedStat() + 1) continue;

            const TLogicSeedStatElem* pNowStatConfig = &(pConfig->m_stStatVec[iterPlant->second->GetSeedStat()]);
            const TLogicSeedStatElem* pNextStatConfig = nullptr;

            bool bRecord = false;
            const int iMaxSize = pConfig->m_stStatVec.size();
            // 如果没有下一阶段的话，则最后阶段不自动进行
            while(iMaxSize > iterPlant->second->GetSeedStat() + 1 && iterPlant->second->GetStatTime() <= iNowTime)
            {
                // 收获必须手动
                if(pNowStatConfig->m_iNeedOp == CLogicConfigHomeGardenExploreParser::EOT_Harvest)
                    break;

                pNextStatConfig = &(pConfig->m_stStatVec[iterPlant->second->GetSeedStat() + 1]);

                bRecord = true;

                int iRandIndex = CLogicCommon::GetRandNum() % stHeroTable.size();
                auto iterCard = stHeroTable.begin();
                while (iRandIndex-- > 0) ++iterCard;

                // 记录操作
                CPackGardenAutoOp stOpRecord;
                stOpRecord.m_iCardID = iterCard->first;
                stOpRecord.m_iLandID = iterPlant->first;
                stOpRecord.m_iOp = pNowStatConfig->m_iNeedOp;
                stUpdateData.m_stOpVec.emplace_back(stOpRecord);

                // 更新到下一阶段
                iterPlant->second->AddSeedStat(1);
                int32_t iOldEndTime = iterPlant->second->GetStatTime();
                iterPlant->second->SetStatTime(iOldEndTime + pNextStatConfig->m_iTime);

                pNowStatConfig = pNextStatConfig;
            }

            if(bRecord)
            {
                CPackHomeGardenElem stPlant;
                stPlant.m_iLandID = iterPlant->first;
                stPlant.m_iSeedID = iterPlant->second->GetSeedID();
                stPlant.m_iSeedStat = iterPlant->second->GetSeedStat();
                stPlant.m_iStatTime = iterPlant->second->GetStatTime();
                stUpdateData.m_stPlantVec.emplace_back(stPlant);
            }

            if(stHeroTable.empty()) break;
        }

        if(stUpdateData.m_stOpVec.empty())
            continue;

        iOpTimes += stUpdateData.m_stOpVec.size();
        stRsp.m_stUpdateData[stHomeGarden.first] = std::move(stUpdateData);
    }

    // 蚯蚓奖励
    if(iOpTimes > 0)
    {
        CLogicConfig::GetCardbagConfig().GetRandBonusByName(CLogicConfig::GetHomeGardenExploreConfig().m_strGardenOpBag, stRsp.m_stRewards, iOpTimes);
        if(!stRsp.m_stRewards.empty())
        {
            CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stRewards, m_usCmd);
        }
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoUserRunHomeVisit(const msgpack::unpacked& stFormatData)
{
    CResponseHomeVisit stRsp;
    CRequestHomeVisit stParam;
    if(DecodeMsgPack(stFormatData, stParam) == false) return (false);

    //记录拜访信息，如果homeID==0表示退出，清除内存记录
    m_pUserInfo->m_stOnlyInCacheData.m_iVisitedInfo = stParam;
    if (0 == stParam.m_iHomeID)
    {
        m_pUserInfo->m_stOnlyInCacheData.m_iVisitedInfo.m_iUid = 0;
    }

    CRequestHomeVisit stCross;
    stCross.m_iHomeID = stParam.m_iHomeID;
    stCross.m_iUid = m_iUin;
    stCross.m_iGroupID = m_iGroupID;
    stCross.m_iCardID = stParam.m_iCardID;
    stCross.m_strNickName = stParam.m_strNickName;
    stCross.m_iSkinID = stParam.m_iSkinID;
    SendToCrossServer(enmCmdType_cross, MSG_CROSS_HOME_VISIT_OTHER, stCross, stParam.m_iUid, stParam.m_iGroupID);
    return true;
}

bool CLogicHomeProcessor::DoUserRunGetSimpleGameState(const msgpack::unpacked& stFormatData)
{
    CResponseGetSimpleGameState stRsp;
    stRsp.m_stSimpleGamePoint = m_pUserInfo->m_stIBusyData.GetSimpleGamePoint();
    stRsp.m_strFlyArrayRewardState = int2bin(m_pUserInfo->m_stUserDailyTable.GetFlyArrayRewardState());
    for(auto i : m_pUserInfo->m_stUserDailyTable.GetFlyArrayReward())
        stRsp.m_stFlyArrayReward.push_back(i);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoUserRunPlaySimpleGame(const msgpack::unpacked& stFormatData)
{
    CRequestPlaySimpleGame stReq;
    CResponsePlaySimpleGame stRsp;
    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    const auto pGameConfig = CLogicConfig::GetSimpleGameConfig().GetSimpleGameConfig(stReq.m_iGameID);
    if(nullptr == pGameConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    auto stGameTimes = m_pUserInfo->m_stUserDailyTable.GetSimpleGameTimes();
    CLogicGameItemFactory::CRkLogicGameItemBasePtr stConsume;
    if(stReq.m_bIsConsumeItem)
    {
        stConsume = LOGIC_GAME_ITEM_FACTORY_INS->CreateGameItem(
                (CLogicConfigDefine::GAME_ITEM_COLLECTION)pGameConfig->m_stConsumeItem.m_iItemType,
                pGameConfig->m_stConsumeItem.m_iCardID,
                pGameConfig->m_stConsumeItem.m_iNum,
                m_usCmd,
                m_pUserInfo);

        if (!stConsume.get() || !stConsume->CanConsume())
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }
    }
    else
    {
        if(stGameTimes[stReq.m_iGameID] >= pGameConfig->m_iFreeTimes)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
        }

        stGameTimes[stReq.m_iGameID] += 1;
    }

    stRsp.m_stRewards = CLogicConfig::GetSimpleGameConfig().GetGameReward(stReq.m_iGameID, stReq.m_iGamePara);
    if(stRsp.m_stRewards.empty())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
    }

    // 飞天大炮弹阵列奖励
    if(stReq.m_iGameID == CLogicConfigDefine::ESGAMET_FlyCannonball && !stReq.m_stPosList.empty() && !CLogicConfig::GetSimpleGameConfig().m_stFlyArrayRewardBag.empty())
    {
        const int32_t iOpenServerDay = day_diff(CLogicConfig::GetSystemConfig().GetOpenSvrTime(), CLogicCommon::GetSec()) + 1;
        int32_t iCurRewardIndex = iOpenServerDay % CLogicConfig::GetSimpleGameConfig().m_stFlyArrayRewardBag.size();
        if(iCurRewardIndex <= 0) iCurRewardIndex = CLogicConfig::GetSimpleGameConfig().m_stFlyArrayRewardBag.size();
        const auto& stArrayRewardConfig = CLogicConfig::GetSimpleGameConfig().m_stFlyArrayRewardBag[iCurRewardIndex - 1];

        auto stArrayState = m_pUserInfo->m_stUserDailyTable.GetFlyArrayRewardState();
        const auto& stArrayReward = m_pUserInfo->m_stUserDailyTable.GetFlyArrayReward();

        for(auto iIndex : stReq.m_stPosList)
        {
            if(iIndex <= 0 || iIndex > int(stArrayReward.size())) continue;

            int32_t iRewardIndex = stArrayReward[iIndex - 1];
            if(iRewardIndex <= 0 || iRewardIndex > int(stArrayRewardConfig.size())) continue;

            if(stArrayState & (1ULL << iRewardIndex)) continue;

            stRsp.m_stRewards.push_back(stArrayRewardConfig[iRewardIndex - 1]);
            stArrayState |= (1ULL << iRewardIndex);
        }

        m_pUserInfo->m_stUserDailyTable.SetFlyArrayRewardState(stArrayState);
    }

    if(stConsume != nullptr && stConsume.get())
        stConsume->Consume();

    m_pUserInfo->m_stUserDailyTable.SetSimpleGameTimes(stGameTimes);

    // 记录最大积分
    auto stGamePoint = m_pUserInfo->m_stIBusyData.GetSimpleGamePoint();
    if(stGamePoint[stReq.m_iGameID] < stReq.m_iGamePara)
    {
        stGamePoint[stReq.m_iGameID] = stReq.m_iGamePara;
        m_pUserInfo->m_stIBusyData.SetSimpleGamePoint(stGamePoint);
    }

    static std::string strBonusItem;
    CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stRewards, strBonusItem, m_usCmd);

    stRsp.m_iGameID = stReq.m_iGameID;
    stRsp.m_iGamePara = stReq.m_iGamePara;
    stRsp.m_bIsConsumeItem = stReq.m_bIsConsumeItem;
    stRsp.m_iGameTimes = stGameTimes[stReq.m_iGameID];
    stRsp.m_strFlyArrayRewardState = int2bin(m_pUserInfo->m_stUserDailyTable.GetFlyArrayRewardState());

    PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|GAME_ID:" << stReq.m_iGameID << "|GAME_PARA:" << stReq.m_iGamePara
                        << "|IS_CONSUME:" << stReq.m_bIsConsumeItem << "|BONUS_ITEM:" << strBonusItem << std::endl;

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}
int32_t CLogicHomeProcessor::UnEquipGoods(CRequestHomeUnequipGoods& stReq)
{
    auto pConfig = CLogicConfig::GetItemConfig().GetConfig(stReq.m_iItemID);
    if(nullptr == pConfig)
    {
        return (SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    auto iterHome = m_pUserInfo->m_stHomeMap.Find(stReq.m_iHomeID);
    if(iterHome == m_pUserInfo->m_stHomeMap.End())
    {
        return (SERVER_ERRCODE::HOME_NONEXISTENT);
    }

    switch (pConfig->m_iSubType)
    {
    case CLogicConfigDefine::EIST_HomePage:
    {
        if(iterHome->second->GetPageID() != stReq.m_iItemID)
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        iterHome->second->SetPageID(0);

        break;
    }
    case CLogicConfigDefine::EIST_HomeFloorBG:
    {
        if(iterHome->second->GetFloorPageID() != stReq.m_iItemID)
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        iterHome->second->SetFloorPageID(0);

        break;
    }
    case CLogicConfigDefine::EIST_HomeOrnament:
    {
        auto iterGoods = m_pUserInfo->m_stHomeGoodsMap[stReq.m_iHomeID].Find(stReq.m_iPara);
        if(iterGoods == m_pUserInfo->m_stHomeGoodsMap[stReq.m_iHomeID].End() || iterGoods->second->GetGoodsID() != stReq.m_iItemID)
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        // 检查是否有伙伴在当期家具上
        auto stHeroVec = iterHome->second->GetHeroVec();
        for(auto iterHero = stHeroVec.begin(); iterHero != stHeroVec.end(); ++iterHero)
        {
            if(iterHero ->second.m_iGoodsIndex == iterGoods->first)
            {
                return (SERVER_ERRCODE::HOME_BUILD_SLOT_NOT_EMPTY_ERROR);
            }
        }

        m_pUserInfo->m_stHomeGoodsMap[stReq.m_iHomeID].Erase(iterGoods);

        break;
    }
    case CLogicConfigDefine::EIST_HomeFootcloth:
    {
        auto iterFoot = m_pUserInfo->m_stHomeFootclothMap[stReq.m_iHomeID].Find(stReq.m_iItemID);
        if(iterFoot == m_pUserInfo->m_stHomeFootclothMap[stReq.m_iHomeID].End())
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        auto stData = iterFoot->second->GetPosVec();
        if(stData.erase(stReq.m_iPara) <= 0)
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        iterFoot->second->SetPosVec(stData);

        break;
    }
    default:
    {
        return (SERVER_ERRCODE::PARAMETER_ERROR);
    }
    }

    static std::string strBonusItem;
    CPackGameItem stItem(CLogicConfigDefine::EXCHANGE_ITEM, stReq.m_iItemID, 1);
    CLogicCacheCommon::AddGameItem(m_pUserInfo, stItem, strBonusItem, m_usCmd);
    PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|HOME_ID:" << stReq.m_iHomeID << "|ITEM_ID:" << stReq.m_iItemID << "|PARAM:" << stReq.m_iPara << "|BONUS_ITEM:" << strBonusItem << std::endl;
    return SERVER_ERRCODE::SUCCESS;
}

int32_t CLogicHomeProcessor::EquipGoods(CRequestHomeEquipGoods& stReq)
{
    auto pConfig = CLogicConfig::GetHomeGoodsConfig().GetHomeGoodsConfig(stReq.m_stGoodsPara.m_iGoodsID);
    if(nullptr == pConfig)
    {
        return (SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    auto pItemConfig = CLogicConfig::GetItemConfig().GetConfig(stReq.m_stGoodsPara.m_iGoodsID);
    if(nullptr == pItemConfig)
    {
        return (SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    auto iterHome = m_pUserInfo->m_stHomeMap.Find(stReq.m_iHomeID);
    if(iterHome == m_pUserInfo->m_stHomeMap.End())
    {
        return (SERVER_ERRCODE::HOME_NONEXISTENT);
    }

    if(pConfig->m_iLimitHomeType != 0 && pConfig->m_iLimitHomeType != iterHome->second->GetHomeType())
    {
        return (SERVER_ERRCODE::HOME_TYPE_LIMIT);
    }

    auto pHomeConfig = CLogicConfig::GetHomeGoodsConfig().GetHomeConfig(iterHome->second->GetHomeType());
    if(nullptr == pHomeConfig)
    {
        return (SERVER_ERRCODE::INTERN_ERROR);
    }

    if(!pHomeConfig->m_bIsCanDress)
    {
        return (SERVER_ERRCODE::HOME_TYPE_LIMIT);
    }

    // 消耗道具
    auto stConsume = LOGIC_GAME_ITEM_FACTORY_INS->CreateGameItem(CLogicConfigDefine::EXCHANGE_ITEM, stReq.m_stGoodsPara.m_iGoodsID, 1, m_usCmd, m_pUserInfo);
    if (!stConsume.get() || !stConsume->CanConsume())
    {
        using namespace SERVER_ERRCODE;
        PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                            << "|MSG:" << CLogicError::GetErrMsg()
                            << "|ITEM_ID:" << stReq.m_stGoodsPara.m_iGoodsID
                            << std::endl;

        return (CLogicError::GetErrCode());
    }

    int32_t iUnequipItem = 0;
    switch (pItemConfig->m_iSubType)
    {
    case CLogicConfigDefine::EIST_HomePage:
    {
        if(iterHome->second->GetPageID() == stReq.m_stGoodsPara.m_iGoodsID)
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        iUnequipItem = iterHome->second->GetPageID();
        iterHome->second->SetPageID(stReq.m_stGoodsPara.m_iGoodsID);

        break;
    }
    case CLogicConfigDefine::EIST_HomeFloorBG:
    {
        if(iterHome->second->GetFloorPageID() == stReq.m_stGoodsPara.m_iGoodsID)
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        iUnequipItem = iterHome->second->GetFloorPageID();
        iterHome->second->SetFloorPageID(stReq.m_stGoodsPara.m_iGoodsID);

        break;
    }
    case CLogicConfigDefine::EIST_HomeOrnament:
    {
        if(!HomeEquipGoods(stReq.m_iHomeID, iterHome->second->GetHomeType(), stReq.m_stGoodsPara))
        {
            return (CLogicError::GetErrCode());
        }

        break;
    }
    case CLogicConfigDefine::EIST_HomeFootcloth:
    {
        if(!CLogicConfigHomeGoodsParser::IsValidPos(stReq.m_stGoodsPara.m_iPosX, stReq.m_stGoodsPara.m_iPosY) ||
            !CLogicConfigHomeGoodsParser::IsValidStat(stReq.m_stGoodsPara.m_cStat))
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        auto& stFootsMap = m_pUserInfo->m_stHomeFootclothMap[stReq.m_iHomeID];

        int iNum = 0;
        for(auto iter = stFootsMap.Begin(); iter != stFootsMap.End(); ++iter)
        {
            iNum += iter->second->GetPosVec().size();
        }

        if(iNum >= CLogicConfigHomeGoodsParser::E_MaxFootclothNum)
        {
            return (SERVER_ERRCODE::HOME_GOODS_TOO_MUCH);
        }

        auto iterFoot = stFootsMap.Find(stReq.m_stGoodsPara.m_iGoodsID);
        if(iterFoot == stFootsMap.End())
        {
            TUserHomeFootclothTableValueType stData;
            stData.m_iUid = m_iUin;
            stData.m_iGroupID = m_iGroupID;
            stData.m_iHomeID = stReq.m_iHomeID;
            stData.m_iFootclothID = stReq.m_stGoodsPara.m_iGoodsID;
            auto stRet = stFootsMap.Insert(std::make_pair(stData.m_iFootclothID, stData));
            if (!stRet.second)
            {
                PROCESSOR_LOG_ERROR << "INSERT NEW HOME_FOOTCLOTH ERROR!" << "|ITEM_ID:" << stReq.m_stGoodsPara.m_iGoodsID << std::endl;

                return (SERVER_ERRCODE::INTERN_ERROR);
            }

            iterFoot = stRet.first;
        }

        int32_t iPos = CLogicConfigHomeGoodsParser::GetUniqueIDByPos(stReq.m_stGoodsPara.m_iPosX, stReq.m_stGoodsPara.m_iPosY);
        auto stFootData = iterFoot->second->GetPosVec();
        if(stFootData.find(iPos) != stFootData.end())
        {
            return (SERVER_ERRCODE::POS_ERROR);
        }

        stFootData[iPos] = stReq.m_stGoodsPara.m_cStat;
        iterFoot->second->SetPosVec(stFootData);

        break;
    }
    default:
    {
        return (SERVER_ERRCODE::PARAMETER_ERROR);
    }
    }

    stConsume->Consume();

    if(iUnequipItem > 0) CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, iUnequipItem, 1, m_usCmd);

    return SERVER_ERRCODE::SUCCESS;
}

int32_t CLogicHomeProcessor::MoveGoods(CRequestHomeGoodsMove& stReq)
{
    auto iterHome = m_pUserInfo->m_stHomeMap.Find(stReq.m_iHomeID);
    if(iterHome == m_pUserInfo->m_stHomeMap.End())
    {
        return (SERVER_ERRCODE::HOME_NONEXISTENT);
    }

    if(!CLogicConfigHomeGoodsParser::IsValidPos(stReq.m_iNewX, stReq.m_iNewY) ||
        !CLogicConfigHomeGoodsParser::IsValidStat(stReq.m_cStat) ||
        !CLogicConfigHomeGoodsParser::IsValidStat(stReq.m_cQuadrant))
    {
        return (SERVER_ERRCODE::PARAMETER_ERROR);
    }

    // 地毯
    if(stReq.m_bFootcloth)
    {
        auto iterFoot = m_pUserInfo->m_stHomeFootclothMap[stReq.m_iHomeID].Find(stReq.m_iPara);
        if(iterFoot == m_pUserInfo->m_stHomeFootclothMap[stReq.m_iHomeID].End())
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        int32_t iIndex = CLogicConfigHomeGoodsParser::GetUniqueIDByPos(stReq.m_iCurX, stReq.m_iCurY);
        auto stPosVec = iterFoot->second->GetPosVec();
        auto iterCurPos = stPosVec.find(iIndex);
        if(iterCurPos == stPosVec.end())
        {
            return (SERVER_ERRCODE::POS_ERROR);
        }

        int32_t iNewIndex = CLogicConfigHomeGoodsParser::GetUniqueIDByPos(stReq.m_iNewX, stReq.m_iNewY);

        // 只是转方向
        if(iIndex == iNewIndex)
        {
            iterCurPos->second = stReq.m_cStat;
        }
        else
        {
            if(stPosVec.find(iNewIndex) != stPosVec.end())
            {
                return (SERVER_ERRCODE::POS_ERROR);
            }

            stPosVec.erase(iterCurPos);
            stPosVec[iNewIndex] = stReq.m_cStat;
        }

        iterFoot->second->SetPosVec(stPosVec);
    }
    else
    {
        // 家具
        auto iterGoods = m_pUserInfo->m_stHomeGoodsMap[stReq.m_iHomeID].Find(stReq.m_iPara);
        if(iterGoods == m_pUserInfo->m_stHomeGoodsMap[stReq.m_iHomeID].End())
        {
            return (SERVER_ERRCODE::PARAMETER_ERROR);
        }

        iterGoods->second->SetPosX(stReq.m_iNewX);
        iterGoods->second->SetPosY(stReq.m_iNewY);
        iterGoods->second->SetStat(stReq.m_cStat);
        iterGoods->second->SetQuadrant(stReq.m_cQuadrant);
    }
    return SERVER_ERRCODE::SUCCESS;
}

bool CLogicHomeProcessor::DoUserRunHomeSave(const msgpack::unpacked& stFormatData)
{
    CRequestHomeSave stReq;
    CResponseHomeSave stAllRsp;
    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }
    auto homeConfig = CLogicConfig::GetHomeGoodsConfig();

    for(auto unEquip: stReq.m_stUnEquipGoods)
    {
        auto iter = m_pUserInfo->m_stHomeMap.Find(unEquip.m_iHomeID);
        if (iter == m_pUserInfo->m_stHomeMap.End())
        {
            SendToClient(stAllRsp, SERVER_ERRCODE::PARAMETER_ERROR);
            return false;
        }
        auto unEquipConfig = homeConfig.GetHomeConfig(iter->second->GetHomeType());
        if (unEquipConfig == nullptr)
        {
            continue;
        }

        if (!unEquipConfig->m_bIsCanDress)
        {
            continue ;
        }
        int32_t bRet = UnEquipGoods(unEquip);
        if (bRet != SERVER_ERRCODE::SUCCESS)
        {
            SEND_ERRCODE_AND_RET(bRet)
        }
        CResponseHomeUnequipGoods stRsp;
        stRsp.m_iHomeID = unEquip.m_iHomeID;
        stRsp.m_iItemID = unEquip.m_iItemID;
        stRsp.m_iPara = unEquip.m_iPara;
        stAllRsp.m_stUnEquipGoods.emplace_back(stRsp);
    }

    for(auto equip: stReq.m_stEquipGood)
    {
        auto iter = m_pUserInfo->m_stHomeMap.Find(equip.m_iHomeID);
        if (iter == m_pUserInfo->m_stHomeMap.End())
        {
            SendToClient(stAllRsp, SERVER_ERRCODE::PARAMETER_ERROR);
            return false;
        }
        auto equipConfig = homeConfig.GetHomeConfig(iter->second->GetHomeType());
        if (equipConfig == nullptr || !equipConfig->m_bIsCanDress)
        {
            continue;
        }

        int32_t bRet = EquipGoods(equip);
        if (bRet != SERVER_ERRCODE::SUCCESS)
        {
            SendToClient(stAllRsp, bRet);
            return false;
        }

        CResponseHomeEquipGoods stRsp;
        stRsp.m_iHomeID = equip.m_iHomeID;
        stRsp.m_stGoods = equip.m_stGoodsPara;
        stAllRsp.m_stEquipGood.emplace_back(stRsp);
    }

    for(auto moveGoods: stReq.m_stMoveGoods)
    {
        auto iter = m_pUserInfo->m_stHomeMap.Find(moveGoods.m_iHomeID);
        if (iter == m_pUserInfo->m_stHomeMap.End())
        {
            SendToClient(stAllRsp, SERVER_ERRCODE::PARAMETER_ERROR);
            return false;
        }
        auto equipConfig = homeConfig.GetHomeConfig(iter->second->GetHomeType());
        if (equipConfig == nullptr || !equipConfig->m_bIsCanDress)
        {
            continue;
        }
        int32_t bRet = MoveGoods(moveGoods);
        if (bRet != SERVER_ERRCODE::SUCCESS)
        {
            SendToClient(stAllRsp, bRet);
            return false;
        }
        CRequestHomeGoodsMove stRsp;
        stRsp = moveGoods;
        stAllRsp.m_stMoveEquipGoods.emplace_back(stRsp);
    }
    SendToClient(stAllRsp, SERVER_ERRCODE::SUCCESS);
    return SERVER_ERRCODE::SUCCESS;
}


bool CLogicHomeProcessor::DoUserRunHomeGetLevelInfo(const msgpack::unpacked& stFormatData)
{
    CResponseHomeLevelInfo stRsp;
    stRsp.m_iLevelExp = m_pUserInfo->m_stUserInfoTable.GetHomeExp();
    stRsp.m_stLevelRewardStatus = m_pUserInfo->m_stUserInfoTable.GetHomeExpReward();

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    return SERVER_ERRCODE::SUCCESS;
}


bool CLogicHomeProcessor::DoUserRunHomeGetLevelReward(const msgpack::unpacked& stFormatData)
{
    CRequestHomeGetLevelReward stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    auto homeLevelRewardConfig = CLogicConfig::GetHomeLevelExpConfig();
    auto userHomeExp = m_pUserInfo->m_stUserInfoTable.GetHomeExp();
    auto userRewardStatus = m_pUserInfo->m_stUserInfoTable.GetHomeExpReward();

    for(auto level: stReq.m_stGetLevel)
    {
        auto iterHomeLevelConfig = homeLevelRewardConfig.GetHomeLevelRewardConfig(level);
        if (iterHomeLevelConfig == nullptr)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
        }
        if (userHomeExp < iterHomeLevelConfig->m_iNeedExp)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_LEVEL_NOT_MATCH);
        }
        for (auto HasGetLevel : userRewardStatus)
        {
            if (level == HasGetLevel)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_LEVEL_NOT_MATCH);
            }
        }
    }

    std::vector<CPackGameItem> stReward;
    for(auto level: stReq.m_stGetLevel)
    {
        auto iterHomeLevelConfig = homeLevelRewardConfig.GetHomeLevelRewardConfig(level);
        stReward.insert(stReward.end(),iterHomeLevelConfig->m_levelReward.begin(),iterHomeLevelConfig->m_levelReward.end());

        //添加奖励领取标记
        userRewardStatus.push_back(level);
    }

    m_pUserInfo->m_stUserInfoTable.SetHomeExpReward(userRewardStatus);
    //添加奖励
    static std::string strBonus;
    CLogicCacheCommon::AddGameItem(m_pUserInfo, stReward, strBonus, m_usCmd);

    CResponseHomeGetLevelReward stRsp;
    stRsp.m_stGetReward = m_pUserInfo->m_stCacheBonusData;

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return SERVER_ERRCODE::SUCCESS;
}

bool CLogicHomeProcessor::DoUserRunHomeHotSpringInfo(const msgpack::unpacked& stFormatData)
{
    //添加温泉信息
    CResponseHomeHotSpringInfo stRsp;
    auto& otherHotSpringUsers = m_pUserInfo->m_stOnlyInCacheData.m_stHotSpringOtherInfo;
    for(auto iterHotSpring = otherHotSpringUsers.begin(); iterHotSpring != otherHotSpringUsers.end(); iterHotSpring++)
    {
        //清除已经过期的
        if (iterHotSpring->second.m_iEndTime < CLogicCommon::GetSec())
        {
            iterHotSpring = otherHotSpringUsers.erase(iterHotSpring);
            if (iterHotSpring == otherHotSpringUsers.end()) break;

            continue;
        }
        CPackHotSpringUserInfo userInfo;
        userInfo.m_iEndTime = iterHotSpring->second.m_iEndTime;
        userInfo.m_iCardID = iterHotSpring->second.m_iCardID;
        userInfo.m_stBrief = iterHotSpring->second.m_stUser;
        userInfo.m_iSkinID = iterHotSpring->second.m_iSkinID;
        stRsp.m_stHotSpringInfo.emplace_back(userInfo);
    }

    auto selfHotSpringUser = m_pUserInfo->m_stOnlyInCacheData.m_stSelfHotSpringInfo;
    CPackHotSpringUserInfo userInfo;
    userInfo.m_iEndTime = selfHotSpringUser.m_iEndTime;
    userInfo.m_iCardID = selfHotSpringUser.m_iCardID;
    userInfo.m_stBrief = selfHotSpringUser.m_stUser;
    userInfo.m_iSkinID = selfHotSpringUser.m_iSkinID;
    stRsp.m_stHotSpringInfo.emplace_back(userInfo);

    auto otherHotSpringUser = m_pUserInfo->m_stOnlyInCacheData.m_stFirstHotSpringInfo;
    CPackHotSpringUserInfo otherUserInfo;
    otherUserInfo.m_iEndTime = otherHotSpringUser.m_iEndTime;
    otherUserInfo.m_iCardID = otherHotSpringUser.m_iCardID;
    otherUserInfo.m_stBrief = otherHotSpringUser.m_stUser;
    otherUserInfo.m_iSkinID = otherHotSpringUser.m_iSkinID;
    stRsp.m_stFirstHotSpringInfo = otherUserInfo;

    stRsp.m_bReward = m_pUserInfo->m_stUserInfoTable.GetHotSpringReward();
    stRsp.m_bOtherReward = m_pUserInfo->m_stUserInfoTable.GetHotSpringRewardOther();
    stRsp.m_bJoinOtherReward = m_pUserInfo->m_stUserInfoTable.GetHotSpringRewardEmail();

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return SERVER_ERRCODE::SUCCESS;
}


bool CLogicHomeProcessor::DoUserRunHomeHotSpringCreate(const msgpack::unpacked& stFormatData)
{
    CRequestHomeHotSpringCreate stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    if (m_pUserInfo->m_stOnlyInCacheData.m_stSelfHotSpringInfo.m_iEndTime != 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_HOT_SPRING_HAS_CREATE)
    }

    if (m_pUserInfo->m_stUserInfoTable.GetHotSpringJoin() == 1)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_HOT_SPRING_HAS_CREATE)
    }

    if(!CheckHotSprintTimeIsOpen())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_HOT_SPRING_IS_NOT_OPEN)
    }

    auto hotSpringConfig = CLogicConfig::GetHomeFuncConfig();
    m_pUserInfo->m_stOnlyInCacheData.m_stSelfHotSpringInfo.m_iEndTime = CLogicCommon::GetSec() + hotSpringConfig.m_iHotSpringDuration;
    m_pUserInfo->m_stOnlyInCacheData.m_stSelfHotSpringInfo.m_stUser = CLogicCacheCommon::GetUserBriefInfo(m_pUserInfo);
    m_pUserInfo->m_stOnlyInCacheData.m_stSelfHotSpringInfo.m_iSkinID = stReq.m_iSkinID;
    m_pUserInfo->m_stOnlyInCacheData.m_stSelfHotSpringInfo.m_iCardID = stReq.m_iCardID;
    m_pUserInfo->m_stUserInfoTable.SetHotSpringJoin(1);

    CResponseHomeHotSpringCreate stRsp;
    stRsp.m_iEndTime = m_pUserInfo->m_stOnlyInCacheData.m_stSelfHotSpringInfo.m_iEndTime;
    stRsp.m_iCardID = stReq.m_iCardID;
    stRsp.m_iHomeID = stReq.m_iHomeID;
    stRsp.m_iSkinID = stReq.m_iSkinID;

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return SERVER_ERRCODE::SUCCESS;
}

bool CLogicHomeProcessor::DoUserRunHomeHotSpringInvite(const msgpack::unpacked& stFormatData)
{
    CRequestHomeHotSpringInvite stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    auto hotSpringConfig = CLogicConfig::GetHomeFuncConfig();

    if(!CheckHotSprintTimeIsOpen())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_HOT_SPRING_IS_NOT_OPEN)
    }


    if ((int32_t)m_pUserInfo->m_stOnlyInCacheData.m_stHotSpringOtherInfo.size() >= hotSpringConfig.m_iFriendLimit)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_HOT_SPRING_FRIEND_LIMIT)
    }

    //转发给各个logic就行了
    CCrossHomeHotSpringInvite stCross;
    stCross.m_iEndTime = CLogicCommon::GetSec() + hotSpringConfig.m_iHotSpringDuration;
    stCross.m_stOwnerBrief = CLogicCacheCommon::GetUserBriefInfo(m_pUserInfo);
    stCross.m_iHomeID = stReq.m_iHomeID;
    for(auto inviteUser: stReq.m_stInvitedUser)
    {
        SendToCrossServer(enmCmdType_cross, MSG_LOGIC_HOME_HOT_SPRING_INVITE, stCross, inviteUser.m_iUid, inviteUser.m_iGroupID);
    }

    return SERVER_ERRCODE::SUCCESS;
}


bool CLogicHomeProcessor::DoUserRunHomeHotSpringJoin(const msgpack::unpacked& stFormatData)
{
    CRequestHomeHotSpringJoin stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    CCrossHomeHotSpringJoin stCross;
    stCross.m_stUser = CLogicCacheCommon::GetUserBriefInfo(m_pUserInfo);
    stCross.m_iCardID = stReq.m_iCardID;
    stCross.m_iUid = m_iUin;
    stCross.m_iGroupID = m_iGroupID;
    stCross.m_iSkinID = stReq.m_iSkinID;
    SendToCrossServer(enmCmdType_cross, MSG_CROSS_HOME_HOT_SPRING_OTHER_JOIN, stCross, stReq.m_iUid, stReq.m_iGroupID);

    return SERVER_ERRCODE::SUCCESS;
}

bool CLogicHomeProcessor::DoUserRunHomeHotSpringGetReward(const msgpack::unpacked& stFormatData)
{
    CRequestHomeHotSpringGetReward stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    CPackGameItem stItem;
    stItem.m_iItemType = CLogicConfigDefine::CURRENCY;
    stItem.m_iCardID = CLogicConfigDefine::ECIT_EnergyID;

    //判断是否存在指定的家园buff
    auto userHomeBuff = m_pUserInfo->m_stIBusyData.GetHomeBuffID();
    auto hotSpringIter = userHomeBuff.find(EnumHomeGoodBuffType::HomeGoodBuff_HotSpring);
    auto hasHotSpringBuff = (hotSpringIter != userHomeBuff.end());

    //判断领取的奖励是自己的还是别人的
    if (stReq.m_iUid == m_iUin && stReq.m_iGroupID == m_iGroupID)
    {
        auto iHotSpringEndTime = m_pUserInfo->m_stOnlyInCacheData.m_stSelfHotSpringInfo.m_iEndTime;
        if (iHotSpringEndTime == 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_HOT_SPRING_NOT_EXIST)
        }

        if (iHotSpringEndTime  > CLogicCommon::GetSec())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_HOT_SPRING_NOT_END)
        }

        if (m_pUserInfo->m_stUserInfoTable.GetHotSpringReward() == 1)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_HOT_SPRING_HAS_RECEIVED_SELF)
        }

        auto baseNum = CLogicConfig::GetHomeFuncConfig().m_iHotSpringSelfReward;
        stItem.m_iNum = !hasHotSpringBuff ?  baseNum : (int32_t)(baseNum * (1+hotSpringIter->second.m_fParam1/10000));

        //检查公会buff
        auto guildBuffIter = m_pUserInfo->m_stOnlyInCacheData.m_mapGuildBuff.find(EnumGuildBuffType::GuildBuffHotSpring);
        if (guildBuffIter != m_pUserInfo->m_stOnlyInCacheData.m_mapGuildBuff.end())
        {
            stItem.m_iNum += guildBuffIter->second.m_iBuffParam[0];
        }
        m_pUserInfo->m_stUserInfoTable.SetHotSpringReward(1);

        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_HotSpringCount;
        stEventData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
    } else
    {
        auto invitedUser = m_pUserInfo->m_stOnlyInCacheData.m_stFirstHotSpringInfo;
        if (invitedUser.m_stUser.m_iUid != stReq.m_iUid || invitedUser.m_stUser.m_iGroupID != stReq.m_iGroupID)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_HOT_SPRING_NOT_EXIST)
        }

        auto iHotSpringEndTime = invitedUser.m_iEndTime;
        if (iHotSpringEndTime == 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_HOT_SPRING_NOT_EXIST)
        }

        if (iHotSpringEndTime > CLogicCommon::GetSec())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_HOT_SPRING_NOT_END)
        }

        if (m_pUserInfo->m_stUserInfoTable.GetHotSpringRewardOther() == 1)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_HOT_SPRING_HAS_RECEIVED_OTHER)
        }

        m_pUserInfo->m_stOnlyInCacheData.m_stFirstHotSpringInfo = {};
        m_pUserInfo->m_stUserInfoTable.SetHotSpringRewardOther(1);

        auto baseNum = CLogicConfig::GetHomeFuncConfig().m_iHotSpringFriendReward;
        stItem.m_iNum = !hasHotSpringBuff ?  baseNum : (int32_t)(baseNum * (1+hotSpringIter->second.m_fParam1/10000));
    }
    CLogicCacheCommon::AddGameItem(m_pUserInfo,stItem, m_usCmd);

    CResponseHomeHotSpringGetReward stRsp;
    stRsp.m_iUid = stReq.m_iUid;
    stRsp.m_iGroupID = stReq.m_iGroupID;
    stRsp.m_iCardID = stReq.m_iCardID;
    stRsp.m_stHotSpringReward = m_pUserInfo->m_stCacheBonusData;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return SERVER_ERRCODE::SUCCESS;
}

bool CLogicHomeProcessor::DoUserRunHomeTopicSave(const msgpack::unpacked& stFormatData)
{
    CRequestHomeSaveHomeTopic stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    if (stReq.m_stTopicElem.homeTopicName.size() > HOME_TOPIC_NAME)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_TOPIC_NAME_UP_LIMIT)
    }

    if (stReq.m_stTopicElem.m_stHomeGoods.size() > HOME_GOODS_PER_HOME_LIMIT)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_TOPIC_GOODS_UP_LIMIT)
    }
    auto iterHomeTopicInfo = m_pUserInfo->m_stUserHomeTopicMap.Find(stReq.m_stTopicElem.m_iTopicIndex);
    bool bIsNew = !(iterHomeTopicInfo != m_pUserInfo->m_stUserHomeTopicMap.End());

    if (m_pUserInfo->m_stUserHomeTopicMap.Size() >= HOME_TOPIC_UP_LIMIT)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_TOPIC_HAS_UP_LIMIT)
    }

    if (bIsNew && stReq.m_stTopicElem.m_iTopicIndex != 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if (bIsNew)
    {
        stReq.m_stTopicElem.m_iTopicIndex = CLogicCacheCommon::GetUserPrimaryKeyIndex(m_pUserInfo);

        TUserHomeTopicTableValueType homeTopicTable;
        homeTopicTable.m_iHomeTopicID = stReq.m_stTopicElem.m_iTopicIndex;
        homeTopicTable.m_stHomeTopicInfo =  stReq.m_stTopicElem;
        homeTopicTable.m_iUid = m_iUin;
        homeTopicTable.m_iGroupID = m_iGroupID;
        m_pUserInfo->m_stUserHomeTopicMap.Insert(std::make_pair(homeTopicTable.m_iHomeTopicID,homeTopicTable));
    }

    CResponseHomeSaveHomeTopic stRsp;
    stRsp.m_stTopicElem = stReq.m_stTopicElem;
    SendToClient(stRsp,SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::CheckHotSprintTimeIsOpen()
{
    const int32_t now_time = CLogicCommon::GetSec();
    const int32_t today_begin_time = get_day_begin_time(now_time);

    const int32_t startHour = int32_t(CLogicConfig::GetHomeFuncConfig().m_fStartTime);
    const int32_t endHour = int32_t(CLogicConfig::GetHomeFuncConfig().m_fEndTime);

    const int32_t startTime = today_begin_time + 1_hour * startHour;
    const int32_t endTime = today_begin_time + 1_hour * endHour;

    if (now_time > startTime && now_time < endTime)
    {
        return true;
    } else {
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLogicHomeProcessor::DoServerRunHomeVisitOther(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CRequestHomeVisit stReqCross;
    if(!DecodeMsgPack(stFormatData, stReqCross)) return false;

    CResponseHomeVisit stRsp;
    GetHomeVisitInfo(m_iGroupID, m_iUin, stReqCross.m_iHomeID, stRsp);
    SendToCrossServer(enmCmdType_cross, MSG_LOGIC_HOME_VISIT, stRsp, stReqCross.m_iUid, stReqCross.m_iGroupID);

    //添加到内存记录,同时通知被拜访者
    if (0 == stReqCross.m_iHomeID)
    {
        const auto& visitHistoryIter = m_pUserInfo->m_stOnlyInCacheData.m_stUid2HomeVisit.find(stReqCross.m_iUid);
        if (visitHistoryIter != m_pUserInfo->m_stOnlyInCacheData.m_stUid2HomeVisit.end())
        {
            m_pUserInfo->m_stOnlyInCacheData.m_stUid2HomeVisit.erase(stReqCross.m_iUid);
        }
    }
    else
    {
        m_pUserInfo->m_stOnlyInCacheData.m_stUid2HomeVisit[stReqCross.m_iUid] = stReqCross;
    }

    //只通知在线玩家
    if(CLogicUser::GetInstance().IsUserOnline(m_iUin, m_iGroupID))
    {
        CNotifyHomeVisit notify;
        notify.m_iUid = stReqCross.m_iUid;
        notify.m_iHomeID = stReqCross.m_iHomeID;
        notify.m_iGroupID = stReqCross.m_iGroupID;
        notify.m_iCardID = stReqCross.m_iCardID;
        notify.m_strNickName = stReqCross.m_strNickName;
        notify.m_iSkinID = stReqCross.m_iSkinID;
        NotifyToOtherClient(m_iGroupID,m_iUin,MSG_LOGIC_HOME_VISIT_NOTIFY,notify);
    }

    return true;
}

bool CLogicHomeProcessor::DoServerRunHomeVisit(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseHomeVisit stRsp;
    if(!DecodeMsgPack(stFormatData, stRsp)) return false;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoServerRunHomeHotSpringInvite(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossHomeHotSpringInvite stCross;
    if(!DecodeMsgPack(stFormatData, stCross)) return false;
    SendToClient(stCross, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicHomeProcessor::DoServerRunHomeHotSpringJoin(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseHomeHotSpringJoinOther stRsp;
    if(!DecodeMsgPack(stFormatData, stRsp)) return false;

    if (stRsp.m_iRet != SERVER_ERRCODE::SUCCESS)
    {
        SendToClient(stRsp, SERVER_ERRCODE::HOME_HOT_SPRING_FRIEND_LIMIT);
    }

    //直接发送邮件给客户端
    auto homeFuncConfig = CLogicConfig::GetHomeFuncConfig();
    if (m_pUserInfo->m_stUserInfoTable.GetHotSpringRewardEmail() == 0)
    {
        CPackGameItem gameItem;
        gameItem.m_iItemType = CLogicConfigDefine::CURRENCY;
        gameItem.m_iCardID = CLogicConfigDefine::ECIT_EnergyID;
        gameItem.m_iNum = homeFuncConfig.m_iHotSpringEmailReward;

        std::vector<CPackGameItem> stReward;
        stReward.emplace_back(gameItem);
        CLogicGlobal::AddAsyncUserMail(m_iUin, m_iGroupID, homeFuncConfig.m_strHotSpringEmailSender, homeFuncConfig.m_strHotSpringEmailTitle
                                       , homeFuncConfig.m_strHotSpringEmailContent,stReward, "SEND_HOT_SPRINT",1,
                                       false,homeFuncConfig.m_iExpiredTimeDay);

        m_pUserInfo->m_stUserInfoTable.SetHotSpringRewardEmail(1);
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}


bool CLogicHomeProcessor::DoServerRunHomeHotSpringOtherJoin(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossHomeHotSpringJoin stCross;
    if(!DecodeMsgPack(stFormatData, stCross)) return false;

    auto iDuration = CLogicConfig::GetHomeFuncConfig().m_iHotSpringDuration;
    auto iLimitFriend = CLogicConfig::GetHomeFuncConfig().m_iFriendLimit;
    hot_spring_info info;
    info.m_iCardID = stCross.m_iCardID;
    info.m_stUser = stCross.m_stUser;
    info.m_iEndTime = CLogicCommon::GetSec() + iDuration;
    info.m_iSkinID = stCross.m_iSkinID;

    CResponseHomeHotSpringJoinOther stRsp;
    stRsp.m_iUid = m_iUin;
    stRsp.m_iCardID = stCross.m_iCardID;
    stRsp.m_iSkinID = stCross.m_iSkinID;
    stRsp.m_iRet = SERVER_ERRCODE::SUCCESS;

    if ((int32_t)m_pUserInfo->m_stOnlyInCacheData.m_stHotSpringOtherInfo.size() >= iLimitFriend)
    {
        stRsp.m_iRet = SERVER_ERRCODE::HOME_HOT_SPRING_FRIEND_LIMIT;
    }
    SendToCrossServer(enmCmdType_cross, MSG_LOGIC_HOME_HOT_SPRING_JOIN, stRsp, stCross.m_iUid, stCross.m_iGroupID);

    m_pUserInfo->m_stOnlyInCacheData.m_stHotSpringOtherInfo.insert(std::make_pair(stCross.m_stUser.m_iUid,info));

    if (!m_pUserInfo->m_stUserInfoTable.GetHotSpringRewardOther() && m_pUserInfo->m_stOnlyInCacheData.m_stFirstHotSpringInfo.m_iCardID == 0)
    {
        m_pUserInfo->m_stOnlyInCacheData.m_stFirstHotSpringInfo = info;
    }
    stCross.m_iEndTime = info.m_iEndTime;
    m_pUserInfo->m_stUserInfoTable.SetHotSpringInvite(1);
    NotifyToOtherClient(m_iGroupID,m_iUin,MSG_LOGIC_HOME_HOT_SPRING_JOIN_NOTIFY,stCross);
    return true;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLogicHomeProcessor::HomeEquipGoods(int32_t iHomeID, int32_t iHomeType, CPackHomeGoodsElem& stGoods)
{
    if(!CLogicConfigHomeGoodsParser::IsValidPos(stGoods.m_iPosX, stGoods.m_iPosY) ||
       !CLogicConfigHomeGoodsParser::IsValidStat(stGoods.m_cStat) ||
       !CLogicConfigHomeGoodsParser::IsValidStat(stGoods.m_cQuadrant))
    {
        SET_ERR_INFO(SERVER_ERRCODE::PARAMETER_ERROR);
        return false;
    }

    auto pConfig = CLogicConfig::GetHomeGoodsConfig().GetHomeGoodsConfig(stGoods.m_iGoodsID);
    if(nullptr == pConfig)
    {
        SET_ERR_INFO(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
        return false;
    }

    if(pConfig->m_iLimitHomeType != 0 && pConfig->m_iLimitHomeType != iHomeType)
    {
        SET_ERR_INFO(SERVER_ERRCODE::HOME_TYPE_LIMIT);
        return false;
    }

    auto& stGoodsMap = m_pUserInfo->m_stHomeGoodsMap[iHomeID];
    if(stGoodsMap.Size() >= CLogicConfigHomeGoodsParser::E_MaxHomeGoodsNum)
    {
        SET_ERR_INFO(SERVER_ERRCODE::HOME_GOODS_TOO_MUCH);
        return false;
    }

    TUserHomeGoodsTableValueType stData;
    stData.m_iUid = m_iUin;
    stData.m_iGroupID = m_iGroupID;
    stData.m_iHomeID = iHomeID;
    stData.m_iGoodsIndex = stGoods.m_iGoodsIndex = CLogicCacheCommon::GetUserPrimaryKeyIndex(m_pUserInfo);
    stData.m_iGoodsID = stGoods.m_iGoodsID;
    stData.m_iPosX = stGoods.m_iPosX;
    stData.m_iPosY = stGoods.m_iPosY;
    stData.m_cStat = stGoods.m_cStat;
    stData.m_cQuadrant = stGoods.m_cQuadrant;
    stData.m_iSeatIndex = stGoods.m_iSeatIndex;
    const auto stRet = stGoodsMap.Insert(std::make_pair(stData.m_iGoodsIndex, stData));
    if (!stRet.second)
    {
        PROCESSOR_LOG_ERROR << "INSERT NEW HOME_GOODS ERROR!" << "|INDEX:" << stGoods.m_iGoodsIndex << std::endl;

        SET_ERR_INFO(SERVER_ERRCODE::INTERN_ERROR);
        return false;
    }

    return true;
}

bool CLogicHomeProcessor::CheckExploreTeamLimit(const TLogicExploreQuestElem* pConfig, const CPackCardTeam& stTeam)
{
    int32_t iNum = 0;
    std::map<int, int> stCardTypeNum;
    for(auto card_id : stTeam)
    {
        if(card_id == 0) continue;

        auto iterData = m_pUserInfo->m_stHeroCardTableMap.Find(card_id);
        if(iterData == m_pUserInfo->m_stHeroCardTableMap.End())
            continue;

        if(iterData->second->GetCardLevel() < pConfig->m_iCardLevel ||
            iterData->second->GetGradeLevel() < pConfig->m_iCardGrade ||
            iterData->second->GetCardColor() < pConfig->m_iCardColor)
        {
            return false;
        }

        auto pCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(card_id);
        if(nullptr == pCardConfig) continue;

        ++iNum;
        ++stCardTypeNum[pCardConfig->m_iCardType];
    }

    for(auto& config : pConfig->m_stCardLimitVec)
    {
        switch (config.m_iLimitType)
        {
        case CLogicConfigHomeGardenExploreParser::EEL_CardNum:
            if(iNum < config.m_iLimitPara) return false;
            break;
        case CLogicConfigHomeGardenExploreParser::EEL_CardTypeNum:
            if(stCardTypeNum[config.m_iLimitPara] < config.m_iLimitParaEx) return false;
            break;
        case CLogicConfigHomeGardenExploreParser::EEL_CardID:
            if(std::find(stTeam.begin(), stTeam.end(), config.m_iLimitPara) == stTeam.end()) return false;
            break;
        default:
            return false;
        }
    }

    return true;
}

void CLogicHomeProcessor::GetHomeVisitInfo(int iGroupID, int iUin, int iHomeID, CResponseHomeVisit& stRsp)
{
    auto pUser = CLogicCacheCommon::LoadUserData(iGroupID, iUin);
    if(nullptr == pUser) return;

    auto iterHome = pUser->m_stHomeMap.Find(iHomeID);
    if(iterHome == pUser->m_stHomeMap.End())
    {
        return;
    }

    stRsp.m_iHomeID = iHomeID;
    for(auto iter = pUser->m_stHomeMap.Begin(); iter != pUser->m_stHomeMap.End(); ++iter)
    {
        CPackHomeElem stHome;
        stHome.m_iHomeID = iter->first;
        stHome.m_iHomeType = iter->second->GetHomeType();
        stHome.m_iPos = iter->second->GetPos();
        stHome.m_iPageID = iter->second->GetPageID();
        stHome.m_iFloorPageID = iter->second->GetFloorPageID();
        stHome.m_stHeroVec = iter->second->GetHeroVec();
        stRsp.m_stHomeInfoVec.emplace_back(std::move(stHome));
    }

    auto& stHomeGoodsMap = pUser->m_stHomeGoodsMap[iHomeID];
    for(auto iterGoods = stHomeGoodsMap.Begin(); iterGoods != stHomeGoodsMap.End(); ++iterGoods)
    {
        CPackHomeGoodsElem stGoods;
        stGoods.m_iGoodsIndex = iterGoods->first;
        stGoods.m_iGoodsID = iterGoods->second->GetGoodsID();
        stGoods.m_cQuadrant = iterGoods->second->GetQuadrant();
        stGoods.m_iPosX = iterGoods->second->GetPosX();
        stGoods.m_iPosY = iterGoods->second->GetPosY();
        stGoods.m_cStat = iterGoods->second->GetStat();
        stGoods.m_iSeatIndex = iterGoods->second->GetSeatIndex();
        stRsp.m_stEquipGoodsVec.emplace_back(std::move(stGoods));
    }

    auto& stHomeFootMap = pUser->m_stHomeFootclothMap[iHomeID];
    for(auto iterFoot = stHomeFootMap.Begin(); iterFoot != stHomeFootMap.End(); ++iterFoot)
    {
        CPackHomeFootclothElem stFoot;
        stFoot.m_iFootclothID = iterFoot->first;
        stFoot.m_stPosVec = iterFoot->second->GetPosVec();
        stRsp.m_stFootclothVec.emplace_back(std::move(stFoot));
    }

    stRsp.m_stHomeBgm = pUser->m_stIBusyData.GetHomeBgm();
    stRsp.m_iBanquetTeamID = pUser->m_stUserInfoTable.GetBanquetAllTeamID().m_iCreateId;
    stRsp.m_stUid2HomeVisit = pUser->m_stOnlyInCacheData.m_stUid2HomeVisit;
}



