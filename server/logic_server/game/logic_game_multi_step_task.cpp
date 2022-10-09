#include "logic_link_common.h"
#include "logic_game_user.h"
#include "logic_config.h"
#include "logic_game_multi_step_task.h"

CLogicMultiStepTaskBase::changed_muti_step_task_info_map_type CLogicMultiStepTaskBase::m_stChangedMutiStepTaskList;

TMutiStepTaskTableValueType
CLogicMultiStepTaskBase::InitMutiStepTaskTableValue(int iGroupID, int iUin, int iTaskID, int iTargetNum)
{
    TMutiStepTaskTableValueType stMutiStepTaskTableValue;
    stMutiStepTaskTableValue.m_iGroupID = iGroupID;
    stMutiStepTaskTableValue.m_iUid = iUin;
    stMutiStepTaskTableValue.m_iTaskID = iTaskID;
    stMutiStepTaskTableValue.m_iTaskStep = 1;
    stMutiStepTaskTableValue.m_iTargetNum = iTargetNum;
    
    return (stMutiStepTaskTableValue);
}

CLogicMultiStepTaskBase::changed_muti_step_task_info_map_type& 
CLogicMultiStepTaskBase::GetChangedMutiStepTask()
{
    return (m_stChangedMutiStepTaskList);
}
    
CLogicMultiStepTaskBase::changed_muti_step_task_info_map_type 
CLogicMultiStepTaskBase::GetChangedMutiStepTaskAndClear()
{
    auto stCopy = m_stChangedMutiStepTaskList;
    m_stChangedMutiStepTaskList.clear();
    
    return (stCopy);
}

CLogicMultiStepTaskBase::CLogicMultiStepTaskBase(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
: m_stTaskConfig(stConfig), m_stUserData(stUserData) {  }

bool 
CLogicMultiStepTaskBase::IsInReceiveBonusStatus(const TMutiStepTaskTable& stUserTaskInfo) const
{
    return ((stUserTaskInfo.GetTaskStep() & 0x1) == 0);
}

const TLogicMutiStepTaskConfigStepDetail*
CLogicMultiStepTaskBase::GetTaskStepConfig(int iTaskStep) const
{
    int iTaskStepIndex = (int)((iTaskStep - 1) / 2);
    if((iTaskStepIndex >= 0) && (iTaskStepIndex < (int)m_stTaskConfig.m_stStepDetail.size()))
    {
        return (&(m_stTaskConfig.m_stStepDetail[iTaskStepIndex]));
    }
    
    return (nullptr);
}

int
CLogicMultiStepTaskBase::GetCompletedMutiTaskAmount(user_data_table_ptr_type pUserData)
{
    int iCompletedTaskAmount = 0;
    auto& stMutiQuestConfig = CLogicConfig::GetMutiStepTaskConfig();
    for (auto stIter = pUserData->m_stMutiStepTaskTableMap.Begin(); stIter != pUserData->m_stMutiStepTaskTableMap.End(); stIter++)
    {
        auto stMutiQuestConfigIT = stMutiQuestConfig.find(stIter->second->m_stData.m_iTaskID);
        if (stMutiQuestConfigIT == stMutiQuestConfig.end())
            continue;

        int iTaskStep = stIter->second->GetTaskStep();
        int iTaskStepIndex = (int)((iTaskStep - 1) / 2);
        if((iTaskStepIndex < 0) || (iTaskStepIndex >= (int)stMutiQuestConfigIT->second.m_stStepDetail.size()))
            continue;

        if (!(iTaskStep & 0x1))
            ++iCompletedTaskAmount;
    }
    return iCompletedTaskAmount;
}

void 
CLogicMultiStepTaskBase::ChangeBonusState(TMutiStepTaskTable& stTaskInfo)
{
    stTaskInfo.AddTaskStep(1);
}

bool 
CLogicMultiStepTaskBase::IsTaskCanOpen() const
{
    return (m_stUserData.m_stUserInfoTable.GetLevel() >= m_stTaskConfig.m_iOpenTaskUserLevelLimit);
}

void 
CLogicMultiStepTaskBase::NotifyClientTaskChanged(const TMutiStepTaskTable& stTaskInfo)
{
    multi_step_task_info_t stChangedTaskInfo;
    stChangedTaskInfo.m_iTaskStep = stTaskInfo.GetTaskStep();
    stChangedTaskInfo.m_iTargetNum = stTaskInfo.GetTargetNum();
    stChangedTaskInfo.m_iGroupID = m_stUserData.m_stUserInfoTable.m_stData.m_iGroupID;
    stChangedTaskInfo.m_iUin = m_stUserData.m_stUserInfoTable.m_stData.m_iUid;
    
    m_stChangedMutiStepTaskList[stTaskInfo.m_stData.m_iTaskID] = stChangedTaskInfo;
}

TMutiStepTaskTable*
CLogicMultiStepTaskBase::GetTaskInfo()
{
    auto stUserMutiStepTaskIT = m_stUserData.m_stMutiStepTaskTableMap.Find(m_stTaskConfig.m_iTaskID);
    if(stUserMutiStepTaskIT != m_stUserData.m_stMutiStepTaskTableMap.End())
    {
        return (stUserMutiStepTaskIT->second);
    }
    
    return (nullptr);
}

bool 
CLogicMultiStepTaskBase::IsInReceiveBonusStatus(const TMutiStepTaskTable* pTaskInfo) const
{
    if(pTaskInfo) return (IsInReceiveBonusStatus(*pTaskInfo));
    
    return (false);
}

void 
CLogicMultiStepTaskBase::CheckTask(int iNum/* = 1*/)
{
    if (!IsTaskCanOpen()) return;

    const TLogicMutiStepTaskConfigStepDetail* pMutiTaskStepConfig = nullptr;
    
    auto stTaskInfoIT = m_stUserData.m_stMutiStepTaskTableMap.Find(m_stTaskConfig.m_iTaskID);
    if (stTaskInfoIT == m_stUserData.m_stMutiStepTaskTableMap.End())
    {
        pMutiTaskStepConfig = &(m_stTaskConfig.m_stStepDetail[0]);
        stTaskInfoIT = ReceiveTask(*pMutiTaskStepConfig, iNum);
    }
    else
    {
        if (IsInReceiveBonusStatus(*(stTaskInfoIT->second))) return;
        
        //是否任务已经领取最后一阶的奖励了
        pMutiTaskStepConfig = GetTaskStepConfig(stTaskInfoIT->second->GetTaskStep());
        if (nullptr == pMutiTaskStepConfig) return;

        int iTargetNum = CalcTaskTargetNumFromUserData(pMutiTaskStepConfig->m_iTargetID, pMutiTaskStepConfig->m_iTargetAmount, iNum);

        if (iTargetNum == stTaskInfoIT->second->GetTargetNum())
        {
            // 可能因为配置变化导致任务无法更新
            if (stTaskInfoIT->second->GetTargetNum() >= pMutiTaskStepConfig->m_iTargetAmount)
            {
                ChangeBonusState(*(stTaskInfoIT->second));

                NotifyClientTaskChanged(*(stTaskInfoIT->second));
            }
            return;
        }

        stTaskInfoIT->second->SetTargetNum(iTargetNum);
    }
    
    if (stTaskInfoIT->second->GetTargetNum() >= pMutiTaskStepConfig->m_iTargetAmount)
    {
        ChangeBonusState(*(stTaskInfoIT->second));
    }
    
    NotifyClientTaskChanged(*(stTaskInfoIT->second));
}

user_data_table_t::muti_step_task_container_type::iterator
CLogicMultiStepTaskBase::ReceiveTask(const TLogicMutiStepTaskConfigStepDetail& stMutiTaskStepConfig, int iNum)
{
    assert((m_stTaskConfig.m_stStepDetail.empty() == false) && "MUTI TASK STEP CONFIG CAN NOT BE EMPTY!");
    TMutiStepTaskTableValueType stMutiStepTaskTableValue = InitMutiStepTaskTableValue(m_stUserData.m_stUserInfoTable.m_stData.m_iGroupID,
                                                                                             m_stUserData.m_stUserInfoTable.m_stData.m_iUid, m_stTaskConfig.m_iTaskID,
                                                                                             CalcTaskTargetNumFromUserData(stMutiTaskStepConfig.m_iTargetID, stMutiTaskStepConfig.m_iTargetAmount, iNum));
    
    auto stRet = m_stUserData.m_stMutiStepTaskTableMap.Insert(decltype(m_stUserData.m_stMutiStepTaskTableMap)::table_value_type(m_stTaskConfig.m_iTaskID, stMutiStepTaskTableValue));
    
    return (stRet.first);
}

std::pair<CLogicMultiStepTaskBase::multi_step_task_info_t, std::vector<CPackGameItem>>
CLogicMultiStepTaskBase::GoToNextStepAndGetBonusConfig(TMutiStepTaskTable& stTaskInfo)
{
    auto pBounsTaskStepConfig = GetTaskStepConfig(stTaskInfo.GetTaskStep());
    
    std::pair<CLogicMultiStepTaskBase::multi_step_task_info_t, std::vector<CPackGameItem>> stRet;
    if(nullptr != pBounsTaskStepConfig) stRet.second = pBounsTaskStepConfig->m_stBonusItemList;
    
    stTaskInfo.AddTaskStep(1);
    auto pTaskStepConfig = GetTaskStepConfig(stTaskInfo.GetTaskStep());
    if(pTaskStepConfig)
    {
//        auto pPreTaskStepConfig = GetTaskStepConfig(stTaskInfo.GetTaskStep() - 2);
//        if(pPreTaskStepConfig->m_iTargetID != pTaskStepConfig->m_iTargetID)
//        {
        stTaskInfo.SetTargetNum(CalcTaskTargetNumFromUserData(pTaskStepConfig->m_iTargetID, pTaskStepConfig->m_iTargetAmount, 0));
//        }
        
        if(stTaskInfo.GetTargetNum() >= pTaskStepConfig->m_iTargetAmount) stTaskInfo.AddTaskStep(1);
    }
    
    stRet.first.m_iGroupID = m_stUserData.m_stUserInfoTable.m_stData.m_iGroupID;
    stRet.first.m_iUin = m_stUserData.m_stUserInfoTable.m_stData.m_iUid;
    stRet.first.m_iTaskStep = stTaskInfo.GetTaskStep();
    stRet.first.m_iTargetNum = stTaskInfo.GetTargetNum();
    NotifyClientTaskChanged(stTaskInfo);
    
    return (stRet);
}

void
CLogicMultiStepTaskBase::SubscribeGameLevelInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_level_channel_data_t& stData)
{
    int iChapterType = CLogicConfig::GetChapterTypeByLevelID(stData.m_iLevelID);

    CRkLogicMutiStepTaskFactory::MUTI_STEP_TASK_TYPE eumTaskType;

    if(iChapterType == (int)CLogicConfigDefine::NORMAL_CHAPTER)
    {
        eumTaskType = CRkLogicMutiStepTaskFactory::PASS_NORMAL_CHAPTER_TASK_TYPE;
        auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, eumTaskType);
        if(nullptr != pTask.get()) pTask->CheckTask();

        eumTaskType = CRkLogicMutiStepTaskFactory::PASS_NORAML_CHAPTER_TIMES_TASK_TYPE;
        pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, eumTaskType);
        if(nullptr != pTask.get()) pTask->CheckTask();
    }
    else if(iChapterType == (int)CLogicConfigDefine::ELITE_CHAPTER)
    {
        eumTaskType = CRkLogicMutiStepTaskFactory::PASS_ELITE_CHAPTER_TASK_TYPE;
        auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, eumTaskType);
        if(nullptr != pTask.get()) pTask->CheckTask();
    }
    else if(iChapterType == (int)CLogicConfigDefine::DEVIL_CHAPTER)
    {
        eumTaskType = CRkLogicMutiStepTaskFactory::PASS_DEVIL_CHAPTER_TASK_TYPE;
        auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, eumTaskType);
        if(nullptr != pTask.get()) pTask->CheckTask();
    }
}

void
CLogicMultiStepTaskBase::SubscribeGameCardInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_card_channel_data_t& stData)
{
    if (stData.m_iCardOption == CLogicGameChannelEnum::UPGRADE_CARD_COLOR_OPTION)
    {
        auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::HAVE_MANY_SPECIFIED_COLOR_HERO_CARDS_TASK_TYPE);
        if(nullptr != pTask.get()) pTask->CheckTask();

        const auto pCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stData.m_iPara);
        if(nullptr != pCardConfig)
        {
            CRkLogicMutiStepTaskFactory::MUTI_STEP_TASK_TYPE eumTaskType = CRkLogicMutiStepTaskFactory::MUTI_STEP_TASK_TYPE_START;
            if(pCardConfig->m_iCardType == CLogicConfigDefine::ECT_Attack)
                eumTaskType = CRkLogicMutiStepTaskFactory::UPGRADE_ATK_HERO_COLOR_TASK_TYPE;
            else if(pCardConfig->m_iCardType == CLogicConfigDefine::ECT_Defense)
                eumTaskType = CRkLogicMutiStepTaskFactory::UPGRADE_DEF_HERO_COLOR_TASK_TYPE;
            else if(pCardConfig->m_iCardType == CLogicConfigDefine::ECT_Support)
                eumTaskType = CRkLogicMutiStepTaskFactory::UPGRADE_SUP_HERO_COLOR_TASK_TYPE;

            if(eumTaskType != CRkLogicMutiStepTaskFactory::MUTI_STEP_TASK_TYPE_START)
            {
                auto pTypeCardTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, eumTaskType);
                if(nullptr != pTypeCardTask.get()) pTypeCardTask->CheckTask();
            }
        }
    }
    else if (stData.m_iCardOption == CLogicGameChannelEnum::UPGRADE_CARD_STAR_OPTION)
    {
        auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::HAVE_MANY_SPECIFIED_STAR_HERO_CARDS_TASK_TYPE);
        if(nullptr != pTask.get()) pTask->CheckTask();
    }
}

void
CLogicMultiStepTaskBase::SubscribeGameItemAddInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_item_add_channel_data_t& stData)
{
    if (stData.m_iItemType == CLogicConfigDefine::HERO_CARD)
    {
        auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::HAVE_MANY_HERO_CARDS_TASK_TYPE);
        if(nullptr != pTask.get()) pTask->CheckTask();

        pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::HAVE_MANY_SPECIFIED_COLOR_HERO_CARDS_TASK_TYPE);
        if(nullptr != pTask.get()) pTask->CheckTask();

        pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::HAVE_MANY_SPECIFIED_STAR_HERO_CARDS_TASK_TYPE);
        if(nullptr != pTask.get()) pTask->CheckTask();
    } 
}

void
CLogicMultiStepTaskBase::SubscribeGameUserLevelUpInfoCallBack(user_data_table_ptr_type pUserData)
{
    auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::UPGRADE_ROLE_LEVEL_TASK_TYPE);
    if(nullptr != pTask.get()) pTask->CheckTask();
}

void
CLogicMultiStepTaskBase::SubscribeGameCombatInfoCallBack(user_data_table_ptr_type pUserData)
{
    auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::ROLE_COMBAT_TASK_TYPE);
    if(nullptr != pTask.get()) pTask->CheckTask();
}

void CLogicMultiStepTaskBase::SubscribeGameUserActionInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_user_action_channel_data_t& stData)
{
	if (CLogicGameChannelEnum::ACTION_UNLOCK_TACTICS == stData.m_iAction)
	{
		auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::TACTICS_COUNT_TASK_TYPE);
		if (nullptr != pTask.get()) pTask->CheckTask();
	}
	else if (CLogicGameChannelEnum::ACTION_ONLINE_TIME == stData.m_iAction)
	{
		auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::TOTAL_ONLINE_TASK_TYPE);
		if (nullptr != pTask.get()) pTask->CheckTask();
	}
	else if(CLogicGameChannelEnum::ACTION_SIGN_COUNT == stData.m_iAction)
    {
        auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::SIGN_TOTAL_TIMES_TASK_TYPE);
        if (nullptr != pTask.get()) pTask->CheckTask(stData.m_iTimes);
    }
	else if(CLogicGameChannelEnum::ACTION_GUILD_TASK_5_STAR_COUNT == stData.m_iAction)
    {
        auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::GUILD_TASK_STAR_TASK_TYPE);
        if (nullptr != pTask.get()) pTask->CheckTask(stData.m_iTimes);
    }
	else if(CLogicGameChannelEnum::ACTION_HORSE_LEVEL_COUNT == stData.m_iAction)
    {
        auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::PASS_GOLD_LEVEL_TASK_TYPE);
        if (nullptr != pTask.get()) pTask->CheckTask(stData.m_iTimes);
    }
	else if(CLogicGameChannelEnum::ACTION_SEND_ENERGY == stData.m_iAction)
    {
        auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::FRIEND_MEME_TASK_TYPE);
        if (nullptr != pTask.get()) pTask->CheckTask(stData.m_iTimes);
    }
	else if(CLogicGameChannelEnum::ACTION_PVP_WIN == stData.m_iAction)
    {
        auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::PVP_SUCCESS_TIMES_TASK_TYPE);
        if (nullptr != pTask.get()) pTask->CheckTask(stData.m_iTimes);
    }
}

void CLogicMultiStepTaskBase::SubscribeClimbTowerOrderCallBack(user_data_table_ptr_type pUserData, const CLogicClimbTowerOrderChannel::data_type& stData)
{
	auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::CLIMB_TOWER_ORDER_TASK_TYPE);
	if (nullptr != pTask.get()) pTask->CheckTask();
}

void CLogicMultiStepTaskBase::SubscribePvpTerritoryOrderCallBack(user_data_table_ptr_type pUserData, const CLogicPvpTerritoryOrderChannel::data_type& stData)
{
	auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::PVP_TERRITORY_ORDER_TASK_TYPE);
	if (nullptr != pTask.get()) pTask->CheckTask();
}

void CLogicMultiStepTaskBase::SubscribeGameMallInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_mall_channel_data_t& stData)
{
    auto pTask = CRkLogicMutiStepTaskFactory::CreateTask(*pUserData, CRkLogicMutiStepTaskFactory::YUANBAO_COMMODITY_TASK_TYPE);
    if (nullptr != pTask.get()) pTask->CheckTask(stData.m_iCommodityNum);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::map<enum CRkLogicMutiStepTaskFactory::MUTI_STEP_TASK_TYPE, CRkLogicMutiStepTaskFactory::CREATE_MUTI_STEP_TASK_FUNC_TYPE> CRkLogicMutiStepTaskFactory::MUTI_STEP_TASK_CREATOR_MAP = 
{
    { PASS_NORMAL_CHAPTER_TASK_TYPE,                        CreateTaskIMP<CRkLogicPassNormalChapterMutiStepTask>                    },
    { PASS_ELITE_CHAPTER_TASK_TYPE,                         CreateTaskIMP<CRkLogicPassEliteChapterMutiStepTask>                     },
    { PASS_DEVIL_CHAPTER_TASK_TYPE,                         CreateTaskIMP<CRkLogicPassDevilChapterMutiStepTask>                     },
    { HAVE_MANY_HERO_CARDS_TASK_TYPE,                       CreateTaskIMP<CRkLogicHaveManyHeroCardsMutiStepTask>                    },
    { HAVE_MANY_SPECIFIED_COLOR_HERO_CARDS_TASK_TYPE,       CreateTaskIMP<CRkLogicHaveManySpecifiedColorHeroCardsMutiStepTask>      },
    { HAVE_MANY_SPECIFIED_STAR_HERO_CARDS_TASK_TYPE,        CreateTaskIMP<CRkLogicHaveManySpecifiedStarHeroCardsMutiStepTask>       },
    { UPGRADE_ROLE_LEVEL_TASK_TYPE,                         CreateTaskIMP<CRkLogicUpgradeRoleLevelMutiStepTask>                     },
	{ CLIMB_TOWER_ORDER_TASK_TYPE,                          CreateTaskIMP<CRkLogicClimbTowerOrderMutiStepTask>                      },
	{ PVP_TERRITORY_ORDER_TASK_TYPE,                        CreateTaskIMP<CRkLogicPvpTerritoryOrderMutiStepTask>                    },
	{ TACTICS_COUNT_TASK_TYPE,                              CreateTaskIMP<CRkLogicTacticsCountMutiStepTask>                         },
	{ TOTAL_ONLINE_TASK_TYPE,                               CreateTaskIMP<CRkLogicTotalOnlineMutiStepTask>                          },
	{ ROLE_COMBAT_TASK_TYPE,                                CreateTaskIMP<CRkLogicRoleCombatMutiStepTask>                           },
    { PASS_NORAML_CHAPTER_TIMES_TASK_TYPE,                  CreateTaskIMP<CRkLogicPassNormalChapterMutiStepTask>                    },
    { SIGN_TOTAL_TIMES_TASK_TYPE,                           CreateTaskIMP<CRkLogicAddUpTypeMutiStepTask>                            },
    { GUILD_TASK_STAR_TASK_TYPE,                            CreateTaskIMP<CRkLogicAddUpTypeMutiStepTask>                            },
    { PASS_GOLD_LEVEL_TASK_TYPE,                            CreateTaskIMP<CRkLogicAddUpTypeMutiStepTask>                            },
    { FRIEND_MEME_TASK_TYPE,                                CreateTaskIMP<CRkLogicAddUpTypeMutiStepTask>                            },
    { UPGRADE_ATK_HERO_COLOR_TASK_TYPE,                     CreateTaskIMP<CRkLogicUpgradeSpecialHeroColorMutiStepTask>              },
    { UPGRADE_DEF_HERO_COLOR_TASK_TYPE,                     CreateTaskIMP<CRkLogicUpgradeSpecialHeroColorMutiStepTask>              },
    { UPGRADE_SUP_HERO_COLOR_TASK_TYPE,                     CreateTaskIMP<CRkLogicUpgradeSpecialHeroColorMutiStepTask>              },
    { PVP_SUCCESS_TIMES_TASK_TYPE,                          CreateTaskIMP<CRkLogicPvpSuccessTimesMutiStepTask>                      },
    { YUANBAO_COMMODITY_TASK_TYPE,                          CreateTaskIMP<CRkLogicAddUpTypeMutiStepTask>                            },
};

std::shared_ptr<CLogicMultiStepTaskBase> 
CRkLogicMutiStepTaskFactory::CreateTask(user_data_table_t& stUserData, enum CRkLogicMutiStepTaskFactory::MUTI_STEP_TASK_TYPE iTaskID)
{
    auto stCreatorIMPIT = MUTI_STEP_TASK_CREATOR_MAP.find(iTaskID);
    if(stCreatorIMPIT != MUTI_STEP_TASK_CREATOR_MAP.end())
    {
        auto stTaskConfigIT = CLogicConfig::GetMutiStepTaskConfig().find(iTaskID);
        if(stTaskConfigIT != CLogicConfig::GetMutiStepTaskConfig().end()) return (stCreatorIMPIT->second(stUserData, stTaskConfigIT->second));
    }
    
    return (std::shared_ptr<CLogicMultiStepTaskBase>(nullptr));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRkLogicPassNormalChapterMutiStepTask::CRkLogicPassNormalChapterMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
: CLogicMultiStepTaskBase(stConfig, stUserData) {  }

int 
CRkLogicPassNormalChapterMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
    //in this task target_id is level id
    int iVictoryTimes = 0;
    auto stLevelInfoIT = m_stUserData.m_stLevelInfoTableMap.Find(iTargetID);
    if(stLevelInfoIT != m_stUserData.m_stLevelInfoTableMap.End()) iVictoryTimes = (int)stLevelInfoIT->second->GetTotalVictoryTimes();
    
    return (iVictoryTimes >= iMaxTargetNum ? iMaxTargetNum : iVictoryTimes);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRkLogicPassEliteChapterMutiStepTask::CRkLogicPassEliteChapterMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
: CLogicMultiStepTaskBase(stConfig, stUserData) {  }

int 
CRkLogicPassEliteChapterMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
    //in this task target_id is level id
    int iVictoryTimes = 0;
    auto stLevelInfoIT = m_stUserData.m_stLevelInfoTableMap.Find(iTargetID);
    if(stLevelInfoIT != m_stUserData.m_stLevelInfoTableMap.End()) iVictoryTimes = (int)stLevelInfoIT->second->GetTotalVictoryTimes();
    
    return (iVictoryTimes >= iMaxTargetNum ? iMaxTargetNum : iVictoryTimes);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRkLogicPassDevilChapterMutiStepTask::CRkLogicPassDevilChapterMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
: CLogicMultiStepTaskBase(stConfig, stUserData) {  }

int 
CRkLogicPassDevilChapterMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
    //in this task target_id is level id
    int iVictoryTimes = 0;
    auto stLevelInfoIT = m_stUserData.m_stLevelInfoTableMap.Find(iTargetID);
    if(stLevelInfoIT != m_stUserData.m_stLevelInfoTableMap.End()) iVictoryTimes = (int)stLevelInfoIT->second->GetTotalVictoryTimes();
    
    return (iVictoryTimes >= iMaxTargetNum ? iMaxTargetNum : iVictoryTimes);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRkLogicHaveManyHeroCardsMutiStepTask::CRkLogicHaveManyHeroCardsMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
: CLogicMultiStepTaskBase(stConfig, stUserData) {  }

int 
CRkLogicHaveManyHeroCardsMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
    //in this task target_id is wildcard id
    (void)iTargetID;
    int iTotalCardNum = 0;
    for(auto stHeroCardIT = m_stUserData.m_stHeroCardTableMap.Begin(); 
        stHeroCardIT != m_stUserData.m_stHeroCardTableMap.End(); stHeroCardIT ++)
    {
        if(stHeroCardIT->second->GetCardLevel() > 0) 
        {
            if(++ iTotalCardNum >= iMaxTargetNum) break;
        }
    }
    
    return (iTotalCardNum >= iMaxTargetNum ? iMaxTargetNum : iTotalCardNum);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRkLogicHaveManySpecifiedColorHeroCardsMutiStepTask::CRkLogicHaveManySpecifiedColorHeroCardsMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
: CLogicMultiStepTaskBase(stConfig, stUserData) {  }

int 
CRkLogicHaveManySpecifiedColorHeroCardsMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
    //in this task target_id is hero card color
    int iTotalCardNum = 0;
    for(auto stHeroCardIT = m_stUserData.m_stHeroCardTableMap.Begin(); 
        stHeroCardIT != m_stUserData.m_stHeroCardTableMap.End(); stHeroCardIT ++)
    {
        if((stHeroCardIT->second->GetCardLevel() > 0) && (stHeroCardIT->second->GetCardColor() >= iTargetID)) 
        {
            if(++ iTotalCardNum >= iMaxTargetNum) break;
        }
    }
    
    return (iTotalCardNum >= iMaxTargetNum ? iMaxTargetNum : iTotalCardNum);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRkLogicHaveManySpecifiedStarHeroCardsMutiStepTask::CRkLogicHaveManySpecifiedStarHeroCardsMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
: CLogicMultiStepTaskBase(stConfig, stUserData) {  }

int 
CRkLogicHaveManySpecifiedStarHeroCardsMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
    //in this task target_id is hero card star
    int iTotalCardNum = 0;
    for(auto stHeroCardIT = m_stUserData.m_stHeroCardTableMap.Begin(); 
        stHeroCardIT != m_stUserData.m_stHeroCardTableMap.End(); stHeroCardIT ++)
    {
        if((stHeroCardIT->second->GetCardLevel() > 0) && (stHeroCardIT->second->GetCardStar() >= iTargetID))
        {
            if(++ iTotalCardNum >= iMaxTargetNum) break;
        }
    }
    
    return (iTotalCardNum >= iMaxTargetNum ? iMaxTargetNum : iTotalCardNum);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRkLogicUpgradeRoleLevelMutiStepTask::CRkLogicUpgradeRoleLevelMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
: CLogicMultiStepTaskBase(stConfig, stUserData) {  }

int 
CRkLogicUpgradeRoleLevelMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
    (void)iTargetID;(void)iMaxTargetNum;
    return (m_stUserData.m_stUserInfoTable.GetLevel());
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRkLogicRoleCombatMutiStepTask::CRkLogicRoleCombatMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
: CLogicMultiStepTaskBase(stConfig, stUserData) {  }

int 
CRkLogicRoleCombatMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
    (void)iTargetID;(void)iMaxTargetNum;
    return (m_stUserData.m_stUserInfoTable.GetRoleCombat());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRkLogicClimbTowerOrderMutiStepTask::CRkLogicClimbTowerOrderMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
	: CLogicMultiStepTaskBase(stConfig, stUserData) {  }

int
CRkLogicClimbTowerOrderMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
	(void)iTargetID;(void)iMaxTargetNum;
	return (m_stUserData.m_stUserInfoTable.GetClimbTowerMaxOrder());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRkLogicPvpTerritoryOrderMutiStepTask::CRkLogicPvpTerritoryOrderMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
	: CLogicMultiStepTaskBase(stConfig, stUserData) {  }

int
CRkLogicPvpTerritoryOrderMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
	(void)iTargetID;(void)iMaxTargetNum;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRkLogicTacticsCountMutiStepTask::CRkLogicTacticsCountMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
	: CLogicMultiStepTaskBase(stConfig, stUserData) {  }

int
CRkLogicTacticsCountMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
	(void)iTargetID;(void)iMaxTargetNum;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRkLogicTotalOnlineMutiStepTask::CRkLogicTotalOnlineMutiStepTask(const TLogicMutiStepTaskConfigElem& stConfig, user_data_table_t& stUserData)
	: CLogicMultiStepTaskBase(stConfig, stUserData) {  }

int
CRkLogicTotalOnlineMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
	(void)iTargetID;(void)iMaxTargetNum;
	int32_t iAddOnlineTime = 0;
	auto iter = CLogicUser::GetInstance().GetUser(m_stUserData.m_iUin);
	if (iter.second && iter.first->second.m_iGroupID == m_stUserData.m_iGroupID)
	{
		iAddOnlineTime = CLogicCommon::GetSec() - iter.first->second.m_iLoginTime;
	}
	return (m_stUserData.m_stUserInfoTable.GetTotalOnlineTime() + iAddOnlineTime) / 3600;
}

int CRkLogicAddUpTypeMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
    (void)iTargetID;(void)iMaxTargetNum;

    auto stTaskInfoIT = m_stUserData.m_stMutiStepTaskTableMap.Find(m_stTaskConfig.m_iTaskID);
    if (stTaskInfoIT == m_stUserData.m_stMutiStepTaskTableMap.End())
    {
        return iNum;
    }
    else
    {
        return stTaskInfoIT->second->GetTargetNum() + iNum;
    }
}

int CRkLogicUpgradeSpecialHeroColorMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
    //in this task target_id is hero card color
    int iTotalCardNum = 0;
    for(auto stHeroCardIT = m_stUserData.m_stHeroCardTableMap.Begin(); stHeroCardIT != m_stUserData.m_stHeroCardTableMap.End(); ++stHeroCardIT)
    {
        if((stHeroCardIT->second->GetCardLevel() <= 0) || (stHeroCardIT->second->GetCardColor() < iTargetID))
            continue;

        const auto *pstCardConfig =
            CLogicConfig::GetHeroCardConfig().GetConfig(stHeroCardIT->second->m_stData.m_iCardID);
        if (nullptr == pstCardConfig) continue;

        if( (m_stTaskConfig.m_iTaskID == CRkLogicMutiStepTaskFactory::UPGRADE_ATK_HERO_COLOR_TASK_TYPE && pstCardConfig->m_iCardType == CLogicConfigDefine::ECT_Attack) ||
            (m_stTaskConfig.m_iTaskID == CRkLogicMutiStepTaskFactory::UPGRADE_DEF_HERO_COLOR_TASK_TYPE && pstCardConfig->m_iCardType == CLogicConfigDefine::ECT_Defense) ||
            (m_stTaskConfig.m_iTaskID == CRkLogicMutiStepTaskFactory::UPGRADE_SUP_HERO_COLOR_TASK_TYPE && pstCardConfig->m_iCardType == CLogicConfigDefine::ECT_Support) )
        {
            ++iTotalCardNum;
        }

        if(iTotalCardNum >= iMaxTargetNum) break;
    }

    return (iTotalCardNum >= iMaxTargetNum ? iMaxTargetNum : iTotalCardNum);
}

int CRkLogicPvpSuccessTimesMutiStepTask::CalcTaskTargetNumFromUserData(int iTargetID, int iMaxTargetNum, int iNum/* = 1*/)
{
    (void)iTargetID;(void)iMaxTargetNum;
    return m_stUserData.m_stUserInfoTable.GetPvpVictoryTimes();
}
