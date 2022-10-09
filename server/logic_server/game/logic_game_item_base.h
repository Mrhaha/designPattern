//
// Created by hzl on 2019/2/20.
//

#pragma once

#include "logic_game_cache_table.h"

class CLogicGameItemI
{
public:
    virtual ~CLogicGameItemI() = default;

    int Add();
    int Consume();

    virtual int RealAdd() = 0;
    virtual int RealConsume() = 0;
    virtual int Sell() = 0;
    virtual bool CanConsume() const = 0;

    void SetItemInfo(int iItemType, int iItemID, int iNum);
    void SetUserPtr(user_data_table_ptr_type pBaseUser);
    void SetCmd(int32_t iCmd);
    int32_t GetCmd() const;
    int32_t GetItemType() const;
    void SetInfo(const std::map<std::string, std::string>& info);
    const std::string& GetInfo() const;

private:
    int32_t m_iItemType;
    int32_t m_iItemID;
    int32_t m_iNum;
    int32_t m_iCmd;
    std::string m_strInfo;
    user_data_table_ptr_type             m_pBaseUser;
};
