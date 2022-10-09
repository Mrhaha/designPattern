//
// Created by czy on 2022/04/23.
//

#pragma once

#include "logic_game_processor_base.h"

class CLogicNewPassProcessor : public CLogicProcessorBase
{
public:
    CLogicNewPassProcessor(uint16_t usCmd, const std::string& strCmdName);

    static int32_t BuyWhichMonthPass(user_data_table_ptr_type m_pUserInfo);


protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;

    bool DoUserRunGetNewPassInfo(const msgpack::unpacked& stFormatData);

    bool DoUserRunGetNewPassAward(const msgpack::unpacked& stFormatData);

private:
    int32_t GetNewPassCurrentLevel();


};
