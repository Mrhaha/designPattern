#include "server_cmd_logic_define.h"
#include "server_errcode.h"
#include "logic_config.h"
#include "logic_game_atlas_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_global.h"
#include "server_atlas_protocol.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_ATLAS_INFO, CLogicAtlasProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_LEVEL_UP_ATLAS, CLogicAtlasProcessor)

CLogicAtlasProcessor::CLogicAtlasProcessor(uint16_t usCmd, const std::string& strCmdName)
	: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicAtlasProcessor::DoUserRun(const msgpack::unpacked & stFormatData)
{
	bool bRet = false;
	switch (m_usCmd)
	{
	case MSG_LOGIC_GET_ATLAS_INFO:
		bRet = DoUserRunGetAtlasInfo(stFormatData);
		break;
    case MSG_LOGIC_LEVEL_UP_ATLAS:
        bRet = DoUserRunLevelUpAtlasBuff(stFormatData);
        break;
	default:
		break;
	}

	return bRet;
}

bool CLogicAtlasProcessor::DoUserRunGetAtlasInfo(const msgpack::unpacked& stFormatData)
{
    CResponseGetAtlasInfo stRsp;
    stRsp.m_mapBuffID2Level = m_pUserInfo->m_stIBusyData.GetAtlasBuffInfo();
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicAtlasProcessor::DoUserRunLevelUpAtlasBuff(const msgpack::unpacked& stFormatData)
{
    CRequestLevelUpAtlas stParam;
    if (!DecodeMsgPack(stFormatData, stParam)) return (false);

    auto atlasConfig = CLogicConfig::GetCompleteAtlasConfig().m_mapBuffID2Config;
    auto iterAreaConfig = atlasConfig.find(stParam.m_iAreaID);
    if (atlasConfig.end() == iterAreaConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }
    //玩家指定地区HeroCard的总星级
    int32_t iTotalHeroStar = 0;
    for(auto& areaHeroID : iterAreaConfig->second.m_vecCardID)
    {
        auto iterUserHeroInfo = m_pUserInfo->m_stHeroCardTableMap.Find(areaHeroID);
        if (m_pUserInfo->m_stHeroCardTableMap.End() == iterUserHeroInfo)
        {
            continue;
        }

        iTotalHeroStar += iterUserHeroInfo->second->GetCardStar();
    }

    //玩家指定地区的消耗总星级
    int32_t iTotalAreaConsume = 0;
    int32_t iCurAreaBuffLevel = 0;
    auto iterUserAtlasBuffInfo = m_pUserInfo->m_stIBusyData.GetAtlasBuffInfo().find(stParam.m_iAreaID);
    if (m_pUserInfo->m_stIBusyData.GetAtlasBuffInfo().end() != iterUserAtlasBuffInfo)
    {
        iTotalAreaConsume = CalcAreaTotalStar(stParam.m_iAreaID,iterUserAtlasBuffInfo->second,iterAreaConfig->second.m_iLevelParam);
        iCurAreaBuffLevel = iterUserAtlasBuffInfo->second;
    }

    //获取下一等级升级buff消耗
    auto iterAreaLevelUp = CLogicConfig::GetCompleteAtlasConfig().m_mapLevelUpInfo.find(iCurAreaBuffLevel+1);
    if (CLogicConfig::GetCompleteAtlasConfig().m_mapLevelUpInfo.end() == iterAreaLevelUp)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    //总星级-当前地区等级需要星级，判断是否可以继续升级地区buff等级
    int32_t iLeftStar = iTotalHeroStar - iTotalAreaConsume;
    if (iLeftStar < iterAreaLevelUp->second.m_iNeedStar * iterAreaConfig->second.m_iLevelParam)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ATLAS_LEVEL_UP_STAR_NOT_ENOUGH);
    }

    PROCESSOR_LOG_DEBUG << "iLeftStar: " << iLeftStar
                        << "|iTotalHeroStar: " << iTotalHeroStar
                        << "|iTotalAreaConsume: " << iTotalAreaConsume
                        << std::endl;

    auto userAtlasBuffInfo = m_pUserInfo->m_stIBusyData.GetAtlasBuffInfo();
    userAtlasBuffInfo[stParam.m_iAreaID] += 1;
    m_pUserInfo->m_stIBusyData.SetAtlasBuffInfo(userAtlasBuffInfo);

    CResponseLevelUpAtlas stRsp;
    stRsp.m_iAreaID = stParam.m_iAreaID;
    stRsp.m_iCurLevel = userAtlasBuffInfo[stParam.m_iAreaID];
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}


int32_t CLogicAtlasProcessor::CalcAreaTotalStar(int32_t iAreaID,int32_t iLevel,int32_t iLevelUpParam)
{
    int32_t iTotalConsumeStar = 0;
    auto atlasConfig = CLogicConfig::GetCompleteAtlasConfig();
    auto iterAreaConfig = atlasConfig.m_mapBuffID2Config.find(iAreaID);
    if (atlasConfig.m_mapBuffID2Config.end() == iterAreaConfig)
    {
        return 0;
    }

    for(auto &levelConsume: atlasConfig.m_mapLevelUpInfo)
    {
        if (iLevel < levelConsume.first)
        {
            break;
        }
        iTotalConsumeStar += levelConsume.second.m_iNeedStar * iLevelUpParam;
    }
    return iTotalConsumeStar;
}









