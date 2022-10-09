#pragma once

#include "server_base_protocol.h"
#include "logic_game_processor_base.h"

class CLogicPVPProcessor : public CLogicProcessorBase
{
public:
	CLogicPVPProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;
    
private:
    int SendHighestPVPRankMail(int iHighestRank);

    //从内存数据找匹配的玩家(积分，等级),或者直接匹配对应机器人
    int FindChallenger(std::vector<CPackPvpUserInfo>* stVecUserInfo);

    //随机挑战奖励
    void RandomPVPReward(const std::vector<CPackRandomGameItem>&rewardConfig ,std::vector<CPackGameItem>* pvpReward);
};
