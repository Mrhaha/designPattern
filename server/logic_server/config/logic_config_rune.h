//
// Created by czy on 2022/4/25.
//

#pragma once
#include "logic_config_base.h"
#include "server_base_protocol.h"

struct TLogicRuneElem
{
    TLogicRuneElem():m_iRuneID(0),m_iLockItemID(0),m_iUpgradeID(0) {}

    int32_t m_iRuneID;
    int32_t m_iLockItemID;
    int32_t m_iUpgradeID;
};

struct TLogicUnLockElem
{
    TLogicUnLockElem():m_iUnlockType(0),m_iUnlockPara(),m_iUnlockParaEx() {}

    int32_t m_iUnlockType;
    std::vector<int32_t> m_iUnlockPara;
    std::vector<int32_t> m_iUnlockParaEx;
};

struct TLogicRuneUpGradeElem
{
    TLogicRuneUpGradeElem():m_iUpGradeID(0),m_stUnlock() {}

    int32_t m_iUpGradeID;
    std::map<int32_t ,TLogicUnLockElem> m_stUnlock;
};


class CLogicConfigRuneParser: public CLogicConfigParserBase
{
public:
    CLogicConfigRuneParser()
        : CLogicConfigParserBase(){}

    std::pair<bool,std::string> Load(const std::string &strFileName,const std::string &strXMLContent);

    std::unordered_map<int32_t,TLogicRuneElem> m_stRuneMap;
    std::unordered_map<int32_t,TLogicRuneUpGradeElem> m_stRuneUpgradeMap;

    const TLogicRuneElem* GetRuneElemByID(int32_t runeID) const;
    const TLogicRuneUpGradeElem* GetRuneUpgradeByID(int32_t runeID) const;

};