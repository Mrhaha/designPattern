//
// Created by hzl on 2019/2/20.
//

#include "logic_game_item_base.h"

int CLogicGameItemI::Add()
{
    if (m_iNum <= 0) return 0;

    return RealAdd();
}

int CLogicGameItemI::Consume()
{
    if (m_iNum <= 0) return 0;

    return RealConsume();
}

void CLogicGameItemI::SetItemInfo(int iItemType, int iItemID, int iNum)
{
    m_iItemType = iItemType;
    m_iItemID = iItemID;
    m_iNum = iNum;
}

void CLogicGameItemI::SetUserPtr(user_data_table_ptr_type pBaseUser)
{
    m_pBaseUser = pBaseUser;
}

void CLogicGameItemI::SetCmd(int32_t iCmd)
{
    m_iCmd = iCmd;
}

int32_t CLogicGameItemI::GetCmd() const
{
    return m_iCmd;
}

int32_t CLogicGameItemI::GetItemType() const
{
    return m_iItemType;
}

void CLogicGameItemI::SetInfo(const std::map<std::string, std::string> &info)
{
    if (!info.empty())
    {
        m_strInfo = "[";
        for (const auto& one : info)
        {
            m_strInfo += one.first + ":" + one.second + ",";
        }
        m_strInfo.back() = ']';
    }
}

const std::string &CLogicGameItemI::GetInfo() const
{
    return m_strInfo;
}
