//
// Created by luoxiaohu on 2020/12/31.
//

#pragma once

#include "logic_config_base.h"
#include "server_base_protocol.h"

struct TLogicConstelElem
{
    TLogicConstelElem() : m_iItemID(0), m_iGrade(0), m_iEquipmentSelectType(0), m_stEquipmentValue(), m_iSuitSelectType(0), m_stSuitValue() {}

    int32_t     m_iItemID;
    int32_t     m_iGrade;
    int32_t     m_iEquipmentSelectType;
    std::vector<int32_t> m_stEquipmentValue;
    int32_t     m_iSuitSelectType;
    std::vector<int32_t> m_stSuitValue;
};

class CLogicConfigConstelParser : public CLogicConfigParserBase
{
  public:
    CLogicConfigConstelParser() : CLogicConfigParserBase() {}

    std::pair<bool, std::string> Load(const std::string& strFileName, const std::string& strXMLContent) override;

    const TLogicConstelElem* GetConfig(int32_t iItemID) const;

    std::map<int32_t, TLogicConstelElem> m_stConfig;
};

// 装备位主属性随机权重
struct TLogicEquipMainAttr
{
    TLogicEquipMainAttr() : m_stAttrWeight(), m_iTotalWeight(0) {}

    std::vector<std::pair<int32_t, int32_t>> m_stAttrWeight;      // [attr_type, weight]
    int32_t m_iTotalWeight;
};

struct TLogicConstelGradeAttr
{
    struct main_attr
    {
        int32_t m_iBaseValue = 0;
        int32_t m_iParaValue = 0;
        int32_t m_iEquationType = 0;
    };

    struct ex_attr
    {
        int32_t m_iWeight = 0;
        int32_t m_iValueMin = 0;
        int32_t m_iValueMax = 0;
    };

    TLogicConstelGradeAttr() : m_iBaseExp(0), m_stMainAttr(), m_stAttrNumWeight(), m_stExAttr(), m_stLevelExp() {}

    void ReCalcLevelExp(int32_t& iLevel, int32_t& iExp) const
    {
        while (iLevel < int(m_stLevelExp.size() - 1) && m_stLevelExp[iLevel] <= iExp)
        {
            iExp -= m_stLevelExp[iLevel++];
        }

        if (iLevel == (int)(m_stLevelExp.size() - 1) && (iExp > m_stLevelExp[iLevel]))
        {
            iExp = m_stLevelExp[iLevel];
        }
    }

    int32_t                         m_iBaseExp;
    std::map<int32_t, main_attr>    m_stMainAttr;
    TLogicEquipMainAttr             m_stAttrNumWeight;
    std::map<int32_t, ex_attr>      m_stExAttr;
    std::vector<int32_t>            m_stLevelExp;
};

// 套装属性
struct TLogicConstelSuitEffectElem
{
    TLogicConstelSuitEffectElem() : m_iSuitNum(0), m_iEffectType(0), m_iAttrType(0), m_iAttrValue(0), m_iFightPower(0) {}

    int32_t m_iSuitNum;
    int32_t m_iEffectType;
    int32_t m_iAttrType;
    int32_t m_iAttrValue;
    int32_t m_iFightPower;
};

struct TLogicConstelResolveReward
{
    TLogicConstelResolveReward() : m_iGold(0), m_iExp(0), m_iTotalWeight(0), m_stNumWeight() {}

    int32_t     m_iGold;
    int32_t     m_iExp;
    int32_t     m_iTotalWeight;
    std::vector<std::pair<int, int>> m_stNumWeight;
};

class CLogicConfigConstelAttrParser : public CLogicConfigParserBase
{
public:
    enum
    {
        E_XAdd = 1,
        E_AddX = 2,
        E_ExAttrMaxNum = 4,             // 副属性条目最大数量
        E_MaxBagSize = 5000,            // 背包最大数量
    };

    CLogicConfigConstelAttrParser() : CLogicConfigParserBase(), m_iBagMaxSize(0), m_iOnceNeedBagSpace(0), m_dExpInheritPct(0.0), m_dConsumeGoldPct(0.0)
        , m_stLevelAddAttr(), m_stMainAttrRand(), m_stGradeAttrRand(), m_stGradeResolveReward(), m_stSuitMap() {}

    std::pair<bool, std::string> Load(const std::string& strFileName, const std::string& strXMLContent) override;

    bool IsValidEquationType(int32_t iType) const { return (E_XAdd <= iType && iType <= E_AddX); }

    const TLogicConstelGradeAttr* GetGradeConfig(int32_t iGrade) const;
    const std::pair<int, std::string>* GetGradeResolveConfig(int32_t iGrade) const;


    int32_t                 m_iBagMaxSize;
    int32_t                 m_iOnceNeedBagSpace;
    double                  m_dExpInheritPct;           // 经验继承百分比
    double                  m_dConsumeGoldPct;          // 升级消耗金币系数
    std::vector<int32_t>    m_stLevelAddAttr;           // 会触发新加副属性的等级
    std::map<int32_t, TLogicEquipMainAttr>      m_stMainAttrRand;               // 装备位对应主属性随机表
    std::map<int32_t, TLogicConstelGradeAttr>   m_stGradeAttrRand;              // 品质对应属性随机表
    std::map<int32_t, std::pair<int, std::string>> m_stGradeResolveReward;      // 品质对应分解奖励
    std::map<int32_t, std::vector<TLogicConstelSuitEffectElem>> m_stSuitMap;    // 套装属性
};

struct TLogicEquipAttrElem
{
    TLogicEquipAttrElem() : m_stAttrList(),m_iMainSkill(0),m_iExtSkill(0),m_iRefineIndex(0) {}

    std::vector<std::pair<int, int>> m_stAttrList;
    int32_t m_iMainSkill;
    int32_t m_iExtSkill;
    int32_t m_iRefineIndex;
};

struct TLogicRefineRandAttrElem
{
    TLogicRefineRandAttrElem():m_iRandom(),m_iWeight(){}

    std::vector<int32_t> m_iRandom;
    std::vector<int32_t> m_iWeight;
};

struct TLogicEquipRank
{
    TLogicEquipRank():m_iNeedLevel(0),m_stCostItem(),m_iCostGold(0) {}

    int32_t m_iNeedLevel;
    std::vector<CPackGameItem> m_stCostItem;
    int32_t m_iCostGold;
};



class CLogicConfigEquipAttrParser : public CLogicConfigParserBase
{
public:
    CLogicConfigEquipAttrParser() : CLogicConfigParserBase(), m_iEquipBagMax(0), m_iOnceNeedBagSpace(0),
          m_iTokenItemType(0), m_iTokenItemID(0), m_stGradeResolveReward(), m_stAttrMap(), m_stAffixVec(),m_iRefineItem(),m_stRandRefine() {}

    std::pair<bool, std::string> Load(const std::string& strFileName, const std::string& strXMLContent) override;

    const TLogicEquipAttrElem* GetAttrConfig(int32_t iIndex) const;
    const TLogicConstelResolveReward* GetGradeResolveConfig(int32_t iGrade) const;
    std::vector<int32_t> GetRandAffix() const;

    int32_t     m_iEquipBagMax;
    int32_t     m_iOnceNeedBagSpace;
    int32_t     m_iTokenItemType;           // 代币物品类型
    int32_t     m_iTokenItemID;             // 代币物品ID
    std::map<int32_t, TLogicConstelResolveReward> m_stGradeResolveReward;       // 品质对应分解奖励
    std::map<int32_t, TLogicEquipAttrElem> m_stAttrMap;     // 属性组
    std::vector<std::vector<int32_t>> m_stAffixVec;
    CPackGameItem m_iRefineItem;               //洗练消耗道具
    std::map<int32_t,TLogicRefineRandAttrElem> m_stRandRefine;
    std::unordered_map<int32_t,TLogicEquipRank> m_stEquipUpGrade;   // 装备进阶<rank,config>

};

struct TLogicEquipAttrRandBag
{
    TLogicEquipAttrRandBag() : m_iTotalWeight(0), m_stIndexRandMap() {}

    int32_t     m_iTotalWeight;
    std::map<int32_t, int32_t> m_stIndexRandMap;
};

class CLogicConfigEquipAttrRandParser : public CLogicConfigParserBase
{
public:
    CLogicConfigEquipAttrRandParser() : CLogicConfigParserBase(), m_stConfig() {}

    std::pair<bool, std::string> Load(const std::string& strFileName, const std::string& strXMLContent) override;

    const TLogicEquipAttrRandBag* GetConfig(std::string strName) const;

    std::map<std::string, TLogicEquipAttrRandBag> m_stConfig;
};

struct TLogicEquipElem
{
    TLogicEquipElem() : m_iItemID(0), m_iGrade(0), m_iEquipType(0), m_strAttrBag() {}

    int32_t     m_iItemID;
    int32_t     m_iGrade;
    int32_t     m_iEquipType;
    std::string m_strAttrBag;
};

class CLogicConfigEquipItemParser : public CLogicConfigParserBase
{
public:
    CLogicConfigEquipItemParser() : CLogicConfigParserBase() {}

    std::pair<bool, std::string> Load(const std::string& strFileName, const std::string& strXMLContent) override;

    const TLogicEquipElem* GetConfig(int32_t iItemID) const;

    std::map<int32_t, TLogicEquipElem> m_stConfig;
};
