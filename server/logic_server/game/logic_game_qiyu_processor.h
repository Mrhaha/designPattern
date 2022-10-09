#pragma once

#include "server_qiyu_protocol.h"
#include "logic_config_qiyu.h"
#include "logic_game_processor_base.h"

enum EQiyuType
{
	QIYU_HONGYUN = 1,
	QIYU_MINGYUN = 2,
	QIYU_MITI = 3,
	QIYU_ZHIYU = 4,
	QIYU_ZHANZHENG = 5,
	QIYU_BAOZANG = 6,
	QIYU_JIAOYI = 7,
	QIYU_JINGJIE = 8,
	QIYU_RONGLU = 9,
	QIYU_MIBAOKU = 10,
	QIYU_QIANGZHETIAOZHAN = 11,
	QIYU_SHOUWEI = 12,
	QIYU_MAOZEI = 13,
	QIYU_MUBEI = 14,
	QIYU_NORMALREWARD = 15,
	QIYU_HIDEHONGYUN = 16,
};

class CLogicQiyuProcessor : public CLogicProcessorBase
{
public:
	CLogicQiyuProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
	bool DoUserRun(const msgpack::unpacked& stFormatData) override;

private:
	bool DoQIYU_GET_INFO_REQ(const msgpack::unpacked& stFormatData);
	bool DoQIYU_REFRESH_REQ(const msgpack::unpacked& stFormatData);
	bool DoQIYU_MOVE_REQ(const msgpack::unpacked& stFormatData);
	bool DoQIYU_FINISH_REQ(const msgpack::unpacked& stFormatData);
	bool DoQIYU_RONGLIAN_REQ(const msgpack::unpacked& stFormatData);
	bool DoQIYU_FIGHT_START(const msgpack::unpacked& stFormatData);
	bool DoQIYU_FIGHT_RESULT(const msgpack::unpacked& stFormatData);
	bool DoQIYU_CANGBAOTU_DUIHUAN_REQ(const msgpack::unpacked& stFormatData);
	bool DoQIYU_CANGBAOTU_USE_REQ(const msgpack::unpacked& stFormatData);

private:
	std::string RandomRewardGroup(int userLevel, const std::string &levelRewardPoolName);
	std::string RandomJiaoyiGroup(int userLevel, const std::string &jiaoyiLevelPoolName);
	std::pair<int, stGuanqiaItem>	RandomGuanqia(int userLevel, int levelGuanqiaPoolId);
	int	RandomConsumeGroup(int userLevel, int levelConsumePoolId);
	void GenQiyuBuff(bool dayQiyuOrCangbaotu, const CPackEntireWarBuff &newBuff);

	bool GenEventParam(const stQiyuMapData &mapData, int eventType, int eventID, const std::vector<int> &inputParams, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenHongyunParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenMingyunParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenMitiParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenZhiyuParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenZhanzhengParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenBaozangParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenJiaoyiParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenJingjieParam(const stQiyuMapData &mapData, int eventType, int eventID, const std::vector<int> &inputParams, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenRonglianParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenMibaokuParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenNormalRewardParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenQiangzheParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenShouweiParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenMaozeiParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams);
	bool GenMubeiParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams);

	bool FinishHongyunEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice);
	bool FinishMingyunEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, const std::vector<int> &params, CNotifyQiyuReward &notice);
	bool FinishMitiEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, const std::vector<int> &params, CNotifyQiyuReward &notice);
	bool FinishZhiyuEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice);
	bool FinishZhanzhengEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice);
	bool FinishBaozangEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice);
	bool FinishJiaoyiEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, const std::vector<int> &params, CNotifyQiyuReward &notice);
	bool FinishJingjieEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, const std::vector<int> &params, CNotifyQiyuReward &notice);
	bool FinishRonglianEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice);
	bool FinishMibaokuEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice);
	bool FinishNormalRewardEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice);
	bool FinishQiangzheEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice);
	bool FinishShouweiEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice);
	bool FinishMaozeiEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, const std::vector<int> &params, CNotifyQiyuReward &notice);
	bool FinishMubeiEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, int mapID, CNotifyQiyuReward &notice);
};
