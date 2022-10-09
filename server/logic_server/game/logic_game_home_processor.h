#pragma once

#include "logic_game_processor_base.h"
#include "logic_config_home.h"

class CLogicHomeProcessor : public CLogicProcessorBase
{
public:
	CLogicHomeProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
	bool DoUserRun(const msgpack::unpacked& stFormatData) override;
    bool DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData) override;

private:
    bool DoUserRunGetHomeInfo(const msgpack::unpacked& stFormatData);
    bool DoUserRunEnterHome(const msgpack::unpacked& stFormatData);
    bool DoUserRunUnequipGoods(const msgpack::unpacked& stFormatData);
    bool DoUserRunEquipGoods(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeBuild(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeChangeHero(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomePutBackHero(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeFishingBegin(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeFishingEnd(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeFishingChangeRod(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeCookBook(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeGoodsMove(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeExploreInfo(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeExploreStart(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeExploreReward(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeGardenPlant(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeGardenOp(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeGardenUpdate(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeVisit(const msgpack::unpacked& stFormatData);
	bool DoUserRunGetSimpleGameState(const msgpack::unpacked& stFormatData);
	bool DoUserRunPlaySimpleGame(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeSave(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeGetLevelInfo(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeGetLevelReward(const msgpack::unpacked& stFormatData);

    bool DoUserRunHomeHotSpringInfo(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeHotSpringCreate(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeHotSpringInvite(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeHotSpringJoin(const msgpack::unpacked& stFormatData);
    bool DoUserRunHomeHotSpringGetReward(const msgpack::unpacked& stFormatData);

    //家园主题保存
    bool DoUserRunHomeTopicSave(const msgpack::unpacked& stFormatData);



    bool CheckHotSprintTimeIsOpen();

    int32_t UnEquipGoods(CRequestHomeUnequipGoods& stReq);
    int32_t EquipGoods(CRequestHomeEquipGoods& stReq);
    int32_t MoveGoods(CRequestHomeGoodsMove& stReq);

private:
    bool DoServerRunHomeVisitOther(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunHomeVisit(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

    bool DoServerRunHomeHotSpringInvite(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunHomeHotSpringJoin(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunHomeHotSpringOtherJoin(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

private:
    // 新家园
    bool HomeEquipGoods(int32_t iHomeID, int32_t iHomeType, CPackHomeGoodsElem& stGoods);   // 摆放家具
    bool CheckExploreTeamLimit(const TLogicExploreQuestElem* pConfig, const CPackCardTeam& stTeam);
    static void GetHomeVisitInfo(int iGroupID, int iUin, int iHomeID, CResponseHomeVisit& stRsp);
};
