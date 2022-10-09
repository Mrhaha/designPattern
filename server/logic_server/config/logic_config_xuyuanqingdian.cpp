#include "logic_config_xuyuanqingdian.h"
#include "Markup.h"
#include "logic_link_common.h"
#include "common_datetime.h"

///////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string> CLogicConfigMonthPassParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
    CMarkup stXML;

    std::stringstream strErrorInfo;

    try
    {
        CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty());

        CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent));

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"));

        stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"));
        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("time_type"), m_iTimeType));
        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("start_day"), m_iStartDay));
        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("end_day"), m_iEndDay));
        CONFIG_ASSERT_EXCEPTION(try_time_from_string(stXML.GetAttrib("custom_create_date"), m_iCustomCreateTime));
        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("deposit_id"), m_iDepositID));
        m_strWishMailSender = stXML.GetAttrib("wish_mail_sender");
        m_strWishMailTitle = stXML.GetAttrib("wish_mail_title");
        m_strWishMailContent = stXML.GetAttrib("wish_mail_content");

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("level_exp"))
        stXML.IntoElem();
        while (stXML.FindElem("level"))
        {
            TMonthPassLevelExp data;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("exp"), data.m_iExp));

            for (int32_t i = 1; i <= 10; ++i)
            {
                if (!stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).empty())
                {
                    CPackGameItem item;
                    CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)), item.m_iItemType));
                    CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)), item.m_iCardID));
                    CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)), item.m_iNum));
                    data.m_stBonusVec.push_back(item);
                }

                if (!stXML.GetAttrib("deposit_item_type_" + std::to_string(i)).empty())
                {
                    CPackGameItem item;
                    CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("deposit_item_type_" + std::to_string(i)), item.m_iItemType));
                    CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("deposit_item_id_" + std::to_string(i)), item.m_iCardID));
                    CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("deposit_item_num_" + std::to_string(i)), item.m_iNum));
                    data.m_stDepositVec.push_back(item);
                }
            }

            int32_t level = 0;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("index"), level));
            CONFIG_ASSERT_EXCEPTION(level > 0);
            m_stLevelMap[level] = std::move(data);

        }
        stXML.OutOfElem();


        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("daily_task"))
        stXML.IntoElem();
        while (stXML.FindElem("task"))
        {
            int32_t day = 0;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("index"), day));
            CONFIG_ASSERT_EXCEPTION(day > 0);

            for (int i = 1; i <= 20; ++i)
            {
                if (!stXML.GetAttrib("task_" + std::to_string(i)).empty())
                {
                    int32_t task_id = 0;
                    CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("task_" + std::to_string(i)), task_id));
                    m_stDailyTask.emplace(day, task_id);
                }
            }

        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("weekly_task"))
        stXML.IntoElem();
        while (stXML.FindElem("task"))
        {
            int32_t week = 0;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("index"), week));
            CONFIG_ASSERT_EXCEPTION(week > 0);

            for (int i = 1; i <= 20; ++i)
            {
                if (!stXML.GetAttrib("task_" + std::to_string(i)).empty())
                {
                    int32_t task_id = 0;
                    CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("task_" + std::to_string(i)), task_id));
                    m_stWeeklyTask.emplace(week, task_id);
                }
            }

        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("task_pool"))
        stXML.IntoElem();
        while (stXML.FindElem("task"))
        {
            TMonthPassTask task;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("type"), task.m_iType));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("target"), task.m_iTarget));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("target_amount"), task.m_iTargetAmount));

            for (int32_t i = 1; i <= 10; ++i)
            {
                if (!stXML.GetAttrib("reward_type_" + std::to_string(i)).empty())
                {
                    CPackGameItem item;
                    CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("reward_type_" + std::to_string(i)), item.m_iItemType));
                    CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("reward_id_" + std::to_string(i)), item.m_iCardID));
                    CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("reward_amount_" + std::to_string(i)), item.m_iNum));
                    task.m_stRewardItemVec.push_back(item);
                }
            }

            int32_t task_id = 0;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), task_id));
            CONFIG_ASSERT_EXCEPTION(task_id > 0);
            m_stTaskInfoMap[task_id] = std::move(task);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("exchange"))
        stXML.IntoElem();
        while (stXML.FindElem("task"))
        {
            TMonthPassExchange exchange;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("max_count"), exchange.m_iMaxCount));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("need_pass"), exchange.m_iNeedPass));

            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_type"), exchange.m_stBonusItem.m_iItemType));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_id"), exchange.m_stBonusItem.m_iCardID));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_num"), exchange.m_stBonusItem.m_iNum));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("consume_item_type"), exchange.m_stConsumeItem.m_iItemType));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("consume_item_id"), exchange.m_stConsumeItem.m_iCardID));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("consume_item_num"), exchange.m_stConsumeItem.m_iNum));

            int32_t id = 0;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), id));
            CONFIG_ASSERT_EXCEPTION(id > 0);
            m_stExchangeMap[id] = exchange;
        }
        stXML.OutOfElem();


        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("wish_pool"))
        stXML.IntoElem();
        while (stXML.FindElem("pool"))
        {
            TMonthPassWishPool pool;
            pool.m_iNeedLevel = atoi(stXML.GetAttrib("max_count").c_str());
            pool.m_iNeedCharge = atoi(stXML.GetAttrib("need_charge").c_str());

            int32_t index = 0;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("index"), index));
            CONFIG_ASSERT_EXCEPTION(index > 0);
            m_stWishPoolMap[index] = pool;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("wish_item"))
        stXML.IntoElem();
        while (stXML.FindElem("item"))
        {
            TMonthPassWishItem item;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("need_pass"), item.m_iNeedPass));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_type"), item.m_stWishItem.m_iItemType));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_id"), item.m_stWishItem.m_iCardID));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_num"), item.m_stWishItem.m_iNum));

            int32_t index = 0;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("index"), index));
            CONFIG_ASSERT_EXCEPTION(index > 0);
            m_stWishItemMap[index] = item;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("wish_event"))
        stXML.IntoElem();
        while (stXML.FindElem("event"))
        {
            TMonthPassWishEvent event;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("begin_day"), event.m_iStartDay));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("end_day"), event.m_iEndDay));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("weight"), event.m_iWeight));
            event.m_strEvent = stXML.GetAttrib("event");
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_index"), event.m_iBonusIndex));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_num"), event.m_iBonusNum));

            int32_t index = 0;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("index"), index));
            CONFIG_ASSERT_EXCEPTION(index > 0);
            m_stWishEventMap[index] = std::move(event);
        }
        stXML.OutOfElem();


        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }
    return (std::pair<bool, std::string>(true, "OK"));
}

int32_t CLogicConfigMonthPassParser::GetLevelByExp(int32_t iExp) const
{
    int32_t iLevel = 0;
    for(const auto& level : m_stLevelMap)
    {
        if (iExp >= level.second.m_iExp)
        {
            iLevel = level.first;
        }
    }
    return iLevel;
}

const TMonthPassLevelExp* CLogicConfigMonthPassParser::GetLevelExpConfig(int32_t iLevel) const
{
    const auto iter = m_stLevelMap.find(iLevel);
    return iter == m_stLevelMap.end() ? nullptr : &iter->second;
}

const TMonthPassTask* CLogicConfigMonthPassParser::GetTaskConfig(int32_t iTaskID) const
{
    const auto iter = m_stTaskInfoMap.find(iTaskID);
    return iter == m_stTaskInfoMap.end() ? nullptr : &iter->second;
}

const TMonthPassExchange* CLogicConfigMonthPassParser::GetExchangeConfig(int32_t iExchangeID) const
{
    const auto iter = m_stExchangeMap.find(iExchangeID);
    return iter == m_stExchangeMap.end() ? nullptr : &iter->second;
}

const TMonthPassWishPool* CLogicConfigMonthPassParser::GetWishPoolConfig(int32_t iPoolIndex) const
{
    const auto iter = m_stWishPoolMap.find(iPoolIndex);
    return iter == m_stWishPoolMap.end() ? nullptr : &iter->second;
}

const TMonthPassWishItem* CLogicConfigMonthPassParser::GetWishItemConfig(int32_t iItemIndex) const
{
    const auto iter = m_stWishItemMap.find(iItemIndex);
    return iter == m_stWishItemMap.end() ? nullptr : &iter->second;
}

const TMonthPassWishEvent* CLogicConfigMonthPassParser::GetWishEventConfig(int32_t iEventIndex) const
{
    const auto iter = m_stWishEventMap.find(iEventIndex);
    return iter == m_stWishEventMap.end() ? nullptr : &iter->second;
}

