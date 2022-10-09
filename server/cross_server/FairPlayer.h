//
// Created by hzl on 2019/4/25.
//

#pragma once

#include <unordered_map>
#include "server_cross_protocol.h"

class CFairPlayer
{
private:
    using CFairPlayerVector = std::vector<CPackFairPlayer>;

public:
    const CFairPlayerVector& GetFairPlayerVector() const;

    void UpdateFairPlayer(int32_t uin, int16_t group_id, const std::vector<int32_t>& cards);

    void ReduceFairPlayer();

private:
    CFairPlayerVector m_stPlayerVector;

    std::unordered_map<int32_t, int32_t> m_stRoleMap;

};



