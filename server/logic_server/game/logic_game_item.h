#pragma once

#include "common_datetime.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "logic_config_game_item.h"
#include "logic_config_card.h"
#include "logic_config_home.h"
#include "logic_game_item_base.h"
#include "logic_game_cache_table.h"
#include "logic_config_constellation.h"

struct currency_item_tag {};
struct fashion_item_tag {};
struct hero_card_item_tag {};
struct avatar_item_tag {};
struct item_tag {};
struct equip_item_tag {};
struct constellation_tag {};

class CLogicCurrencyGameItem : public CLogicGameItemI
{
public:
    typedef currency_item_tag type;

    CLogicCurrencyGameItem(int iAmount, const TLogicCurrencyItem* pConfig, user_data_table_ptr_type pUser)
        : m_iAmount(iAmount), m_pConfig(pConfig), m_pUser(pUser) { assert(nullptr != m_pConfig); }

    int RealAdd() override;
    int RealConsume() override;
    bool CanConsume() const override;
	int Sell() override { using namespace SERVER_ERRCODE; SET_ERR_INFO(GAME_ITEM_CAN_NOT_SELL); return (-1); };

private:
    void LevelUpHandle(int iPreLevel, int iCurLevel);
    
protected:
    int                                 m_iAmount;
    const TLogicCurrencyItem*           m_pConfig;
    user_data_table_ptr_type            m_pUser;
};

class CLogicFashionGameItem : public CLogicGameItemI
{
public:
    typedef fashion_item_tag type;

    CLogicFashionGameItem(int iAmount, const TLogicFashionConfigElem* pConfig, user_data_table_ptr_type pUser)
        : m_iAmount(iAmount), m_pConfig(pConfig), m_pUser(pUser) { assert(nullptr != m_pConfig); }

    int RealAdd() override;
    bool CanConsume() const override { return (false); }

protected:
    int RealConsume() override { return (1); }
    int Sell() override { using namespace SERVER_ERRCODE; SET_ERR_INFO(GAME_ITEM_CAN_NOT_SELL); return (-1); }

protected:
    int                                 m_iAmount;
    const TLogicFashionConfigElem*      m_pConfig;
    user_data_table_ptr_type            m_pUser;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CLogicItemGameItem : public CLogicGameItemI
{
public:
    typedef item_tag type;

    CLogicItemGameItem(int iAmount, const TLogicItemConfig* pConfig, user_data_table_ptr_type pUser)
        : m_iAmount(iAmount), m_pConfig(pConfig), m_pUser(pUser) { assert(nullptr != m_pConfig); }

	int RealAdd() override;
    
    int RealConsume() override;
    
    bool CanConsume() const override;

    int Sell() override;
    
protected:
    int                             m_iAmount;
    const TLogicItemConfig*         m_pConfig;
    user_data_table_ptr_type        m_pUser;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CLogicHeroCardGameItem : public CLogicGameItemI
{
public:
    // type traist for object factory 
    typedef hero_card_item_tag type;
    
public:
    CLogicHeroCardGameItem(int iAmount, const TLogicHeroCardConfigElem* pConfig, user_data_table_ptr_type pUser);
    
    int RealAdd() override;
    bool CanConsume() const override { return (false); }
    
protected:
    int RealConsume() override { return (1); }
    int Sell() override { using namespace SERVER_ERRCODE; SET_ERR_INFO(GAME_ITEM_CAN_NOT_SELL); return (-1); }
    
protected:
    int                                             m_iAmount;
    const TLogicHeroCardConfigElem*         m_pConfig;
    user_data_table_ptr_type                     m_pUser;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CLogicAvatarGameItem : public CLogicGameItemI
{
public:
    // type traist for object factory 
    typedef avatar_item_tag type;
    
public:
    CLogicAvatarGameItem(int iAmount, const TLogicAvatarElem* pConfig, user_data_table_ptr_type pUser);
    
    int RealAdd() override;
    bool CanConsume() const override { return (false); }
    
protected:
    int RealConsume() override { return (1); }
    int Sell() override { using namespace SERVER_ERRCODE; SET_ERR_INFO(GAME_ITEM_CAN_NOT_SELL); return (-1); }

    
protected:
    int                                             m_iAmount;
    const TLogicAvatarElem*                   m_pConfig;
    user_data_table_ptr_type                     m_pUser;
};

// 伙伴装备
class CLogicEquipGameItem : public CLogicGameItemI
{
public:
    typedef equip_item_tag type;

public:
    CLogicEquipGameItem(int iAmount, const TLogicEquipElem* pConfig, user_data_table_ptr_type pUser);

    int RealAdd() override;
    int RealConsume() override { return (1); };
    bool CanConsume() const override { return (false); };

protected:
    int Sell() override { using namespace SERVER_ERRCODE; SET_ERR_INFO(GAME_ITEM_CAN_NOT_SELL); return (-1); }

protected:
    int                                         m_iAmount;
    const TLogicEquipElem*                      m_pConfig;
    user_data_table_ptr_type                    m_pUser;
};

// 星纹
class CLogicConstellationGameItem : public CLogicGameItemI
{
  public:
    typedef constellation_tag type;

  public:
    CLogicConstellationGameItem(int iAmount, const TLogicConstelElem* pConfig, user_data_table_ptr_type pUser);

    int RealAdd() override;
    int RealConsume() override { return (1); };
    bool CanConsume() const override { return (false); };

  protected:
    int Sell() override { using namespace SERVER_ERRCODE; SET_ERR_INFO(GAME_ITEM_CAN_NOT_SELL); return (-1); }

    // [iRandomMin, iRandomMax) 半闭区间随机
    int32_t RandomConstel(int32_t iSelectType, const std::vector<int32_t>& stValues, const int32_t iRandomMin, const int32_t iRandomMax);

  protected:
    int                                         m_iAmount;
    const TLogicConstelElem*                    m_pConfig;
    user_data_table_ptr_type                    m_pUser;
};
