#include "logic_config_qiyu.h"
#include "Markup.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigQiyuParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("qiyu_config"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		/////////////////////////////////////////////////////////////////////////////////
		if (false == stXML.FindElem("day_qiyu"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}
		else
		{
			m_iOpenLevel = atoi(stXML.GetAttrib("openLevel").c_str());
			for (int i = 1; i < 100; ++i)
			{
				std::stringstream stStream;
				stStream << "_" << i;
				if (stXML.GetAttrib("day_0" + stStream.str()).empty()) break;

				m_mpDayQiyu[0].push_back(atoi(stXML.GetAttrib("day_0" + stStream.str()).c_str()));
			}

			for (int i = 1; i < 100; ++i)
			{
				std::stringstream stStream;
				stStream << "_" << i;
				if (stXML.GetAttrib("day_1" + stStream.str()).empty()) break;

				m_mpDayQiyu[1].push_back(atoi(stXML.GetAttrib("day_1" + stStream.str()).c_str()));
			}

			for (int i = 1; i < 100; ++i)
			{
				std::stringstream stStream;
				stStream << "_" << i;
				if (stXML.GetAttrib("day_2" + stStream.str()).empty()) break;

				m_mpDayQiyu[2].push_back(atoi(stXML.GetAttrib("day_2" + stStream.str()).c_str()));
			}

			for (int i = 1; i < 100; ++i)
			{
				std::stringstream stStream;
				stStream << "_" << i;
				if (stXML.GetAttrib("day_3" + stStream.str()).empty()) break;

				m_mpDayQiyu[3].push_back(atoi(stXML.GetAttrib("day_3" + stStream.str()).c_str()));
			}

			for (int i = 1; i < 100; ++i)
			{
				std::stringstream stStream;
				stStream << "_" << i;
				if (stXML.GetAttrib("day_4" + stStream.str()).empty()) break;

				m_mpDayQiyu[4].push_back(atoi(stXML.GetAttrib("day_4" + stStream.str()).c_str()));
			}

			for (int i = 1; i < 100; ++i)
			{
				std::stringstream stStream;
				stStream << "_" << i;
				if (stXML.GetAttrib("day_5" + stStream.str()).empty()) break;

				m_mpDayQiyu[5].push_back(atoi(stXML.GetAttrib("day_5" + stStream.str()).c_str()));
			}

			for (int i = 1; i < 100; ++i)
			{
				std::stringstream stStream;
				stStream << "_" << i;
				if (stXML.GetAttrib("day_6" + stStream.str()).empty()) break;

				m_mpDayQiyu[6].push_back(atoi(stXML.GetAttrib("day_6" + stStream.str()).c_str()));
			}		
		}

		/////////////////////////////////////////////////////////////////////////////////
		while (stXML.FindElem("qiyu"))
		{
			stQiyuMapData qiyuMap;
			qiyuMap.m_iID = atoi(stXML.GetAttrib("mapId").c_str());

			stXML.IntoElem();			

			if (false == stXML.FindElem("info"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}
			
			qiyuMap.m_iLifeTime = atoi(stXML.GetAttrib("lifetime").c_str());
			qiyuMap.m_iJingjieHeroNum = atoi(stXML.GetAttrib("jingjie_hero_num").c_str());
			qiyuMap.m_iCangbaotuID = atoi(stXML.GetAttrib("cangbaotu_id").c_str());

			if (false == stXML.FindElem("node"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}
			else
			{
				stXML.IntoElem();
				while (stXML.FindElem("node_item"))
				{
					stQiyuMapNode mapNode;
					mapNode.m_iID = atoi(stXML.GetAttrib("id").c_str());
					mapNode.m_iPreID = atoi(stXML.GetAttrib("pre_node").c_str());

					qiyuMap.m_mpMapNodes[mapNode.m_iID] = mapNode;
				}
				stXML.OutOfElem();
			}

			if (false == stXML.FindElem("qiyu_pool"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}
			else
			{
				stXML.IntoElem();
				while (stXML.FindElem("qiyu_level_pool"))
				{
					int level = atoi(stXML.GetAttrib("level").c_str());
					stXML.IntoElem();

					while (stXML.FindElem("qiyu_item"))
					{
						stQiyuEventNode eventNode;
						eventNode.m_strID = stXML.GetAttrib("id").c_str();
						eventNode.m_cType = (char)atoi(stXML.GetAttrib("type").c_str());
						eventNode.m_iEventID = atoi(stXML.GetAttrib("type_id").c_str());
						eventNode.m_iCount = atoi(stXML.GetAttrib("count").c_str());

						for (int i = 1; i < 100; ++i)
						{
							std::stringstream stStream;
							stStream << "_" << i;
							if (stXML.GetAttrib("node" + stStream.str()).empty()) break;

							eventNode.m_aMapNodes.push_back(atoi(stXML.GetAttrib("node" + stStream.str()).c_str()));
						}
						qiyuMap.m_mpEventNodes[level].emplace_back(eventNode);
					}
					stXML.OutOfElem();
				}
				stXML.OutOfElem();
			}


			stXML.OutOfElem();

			m_mpQiyuMap[qiyuMap.m_iID] = qiyuMap;
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (false == stXML.FindElem("reward_group"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}
		else
		{
			stXML.IntoElem();
			while (stXML.FindElem("reward"))
			{
				stRewardGroup rewardGroup;
				std::string rewardId = stXML.GetAttrib("id").c_str();

				for (int i = 1; i < 10; ++i)
				{
					CPackGameItem gameItem;

					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

					gameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
					gameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
					gameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

					rewardGroup.m_aRewards.emplace_back(gameItem);
				}
				rewardGroup.m_iExParam = atoi(stXML.GetAttrib("exParam").c_str());
				rewardGroup.m_strBadRewardID = stXML.GetAttrib("badreward_id").c_str();

				m_mpRewardGroup[rewardId] = rewardGroup;
			}

			stXML.OutOfElem();
		}
		/////////////////////////////////////////////////////////////////////////////////
		if (false == stXML.FindElem("reward_pool"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}
		else
		{
			stXML.IntoElem();
			while (stXML.FindElem("pool"))
			{
				stRewardPool rewardPool;
				std::string poolId = stXML.GetAttrib("id").c_str();

				for (int i = 1; i < 100; ++i)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("group_id" + stStream.str()).empty()) break;

					int					coef;
					std::string			groupID;

					coef = atoi(stXML.GetAttrib("coef" + stStream.str()).c_str());
					groupID = stXML.GetAttrib("group_id" + stStream.str()).c_str();

					rewardPool.m_aRewardGroup.push_back(std::make_pair(groupID, coef));
				}

				m_mpRewardPool[poolId] = rewardPool;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (false == stXML.FindElem("level_reward_pool"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}
		else
		{
			stXML.IntoElem();
			while (stXML.FindElem("pool"))
			{
				stLevelRewardPool rewardPool;
				std::string poolId = stXML.GetAttrib("id").c_str();

				for (int i = 1; i < 100; ++i)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("begin_level" + stStream.str()).empty()) break;

					stLevelRewardItem rewardItem;

					rewardItem.m_iBeginLevel = atoi(stXML.GetAttrib("begin_level" + stStream.str()).c_str());
					rewardItem.m_iEndLevel = atoi(stXML.GetAttrib("end_level" + stStream.str()).c_str());
					rewardItem.m_strRewardPoolName = stXML.GetAttrib("pool_id" + stStream.str()).c_str();

					rewardPool.m_aLevelReward.emplace_back(rewardItem);
				}

				m_mpLevelRewardPool[poolId] = rewardPool;
			}

			stXML.OutOfElem();
		}
		/////////////////////////////////////////////////////////////////////////////////
		if (false == stXML.FindElem("jiaoyi_items"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}
		else
		{
			stXML.IntoElem();
			while (stXML.FindElem("jiaoyi"))
			{
				stJiaoyiItem jiaoyiItem;
				std::string jiaoyiId = stXML.GetAttrib("id").c_str();

				for (int i = 1; i < 10; ++i)
				{
					CPackGameItem desGameItem;

					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("des_type" + stStream.str()).empty()) break;

					desGameItem.m_iItemType = atoi(stXML.GetAttrib("des_type" + stStream.str()).c_str());
					desGameItem.m_iCardID = atoi(stXML.GetAttrib("des_id" + stStream.str()).c_str());
					desGameItem.m_iNum = atoi(stXML.GetAttrib("des_amount" + stStream.str()).c_str());

					jiaoyiItem.m_aDesItemVec.emplace_back(desGameItem);
				}
				for (int i = 1; i < 10; ++i)
				{
					CPackGameItem sourceGameItem;

					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("source_type" + stStream.str()).empty()) break;

					sourceGameItem.m_iItemType = atoi(stXML.GetAttrib("source_type" + stStream.str()).c_str());
					sourceGameItem.m_iCardID = atoi(stXML.GetAttrib("source_id" + stStream.str()).c_str());
					sourceGameItem.m_iNum = atoi(stXML.GetAttrib("source_amount" + stStream.str()).c_str());

					jiaoyiItem.m_aSourceItemVec.emplace_back(sourceGameItem);
				}

				m_mpJiaoyiItem[jiaoyiId] = jiaoyiItem;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (false == stXML.FindElem("jiaoyi_pool"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}
		else
		{
			stXML.IntoElem();
			while (stXML.FindElem("pool"))
			{
				stJiaoyiPool jiaoyiPool;
				std::string poolId = stXML.GetAttrib("id").c_str();

				for (int i = 1; i < 100; ++i)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("jiaoyi_id" + stStream.str()).empty()) break;

					int					coef;
					std::string			jiaoyiID;

					coef = atoi(stXML.GetAttrib("coef" + stStream.str()).c_str());
					jiaoyiID = stXML.GetAttrib("jiaoyi_id" + stStream.str()).c_str();

					jiaoyiPool.m_aJiaoyiGroup.push_back(std::make_pair(jiaoyiID, coef));
				}

				m_mpJiaoyiPool[poolId] = jiaoyiPool;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (false == stXML.FindElem("jiaoyi_level_pool"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}
		else
		{
			stXML.IntoElem();
			while (stXML.FindElem("pool"))
			{
				stJiaoyiLevelPool jiaoyiPool;
				std::string poolId = stXML.GetAttrib("id").c_str();

				for (int i = 1; i < 100; ++i)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("begin_level" + stStream.str()).empty()) break;

					stLevelJiaoyiItem jiaoyiItem;

					jiaoyiItem.m_iBeginLevel = atoi(stXML.GetAttrib("begin_level" + stStream.str()).c_str());
					jiaoyiItem.m_iEndLevel = atoi(stXML.GetAttrib("end_level" + stStream.str()).c_str());
					jiaoyiItem.m_strJiaoyiPoolName = stXML.GetAttrib("pool_id" + stStream.str()).c_str();

					jiaoyiPool.m_aLevelJiaoyi.emplace_back(jiaoyiItem);
				}

				m_mpJiaoyiLevelPool[poolId] = jiaoyiPool;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("nomal_reward"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("nomal_reward_item"))
			{
				int	id = atoi(stXML.GetAttrib("id").c_str());
				std::string level_reward_pool = stXML.GetAttrib("level_reward_pool").c_str();

				m_mpNormalReward[id] = level_reward_pool;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("hongyun"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("hongyun_item"))
			{
				int	hongyunId = atoi(stXML.GetAttrib("id").c_str());
				std::string level_reward_pool = stXML.GetAttrib("level_reward_pool").c_str();

				m_mpHongyun[hongyunId] = level_reward_pool;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("hongyun_content"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("content"))
			{
				int id = atoi(stXML.GetAttrib("id").c_str());				

				m_aHongyunContent.push_back(id);
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("hide_hongyun"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("hide_hongyun_item"))
			{
				int	hongyunId = atoi(stXML.GetAttrib("id").c_str());
				std::string level_reward_pool = stXML.GetAttrib("level_reward_pool").c_str();

				m_mpHideHongyun[hongyunId] = level_reward_pool;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("hide_hongyun_content"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("hide_content"))
			{
				int id = atoi(stXML.GetAttrib("id").c_str());
				m_aHideHongyunContent.push_back(id);
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("mingyun"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("mingyun_item"))
			{
				int	id = atoi(stXML.GetAttrib("id").c_str());
				stMingyun	mingyunItem;
				mingyunItem.m_strLevelRewardPool1 = stXML.GetAttrib("level_reward_pool_1").c_str();
				//mingyunItem.m_strLevelRewardPool2 = stXML.GetAttrib("level_reward_pool_2").c_str();

				m_mpMingyun[id] = mingyunItem;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("mingyun_content"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("content"))
			{
				int id = atoi(stXML.GetAttrib("id").c_str());				

				m_aMingyunContent.insert(id);
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("miti"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("miti_item"))
			{
				int id = atoi(stXML.GetAttrib("id").c_str());
				stMiti mitiItem;

				mitiItem.m_strLevelRewardPool = stXML.GetAttrib("level_reward_pool").c_str();
				mitiItem.m_iBuffType = atoi(stXML.GetAttrib("buff_type").c_str());
				mitiItem.m_iBuffPercent = atoi(stXML.GetAttrib("buff_percent").c_str());
				
				m_mpMiti[id] = mitiItem;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("miti_content"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("content"))
			{
				int id = atoi(stXML.GetAttrib("id").c_str());
				int right_answer = atoi(stXML.GetAttrib("right_answer").c_str());

				m_mpMitiContent[id] = right_answer;
			}

			stXML.OutOfElem();
		}
		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("zhiyu"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("content"))
			{
				int id = atoi(stXML.GetAttrib("id").c_str());
				stZhiyu zhiyuItem;
				zhiyuItem.m_iBuffType = atoi(stXML.GetAttrib("buff_type").c_str());
				zhiyuItem.m_iBuffPercent = atoi(stXML.GetAttrib("buff_percent").c_str());

				m_mpZhiyu[id] = zhiyuItem;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("zhanzheng"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("content"))
			{
				int id = atoi(stXML.GetAttrib("id").c_str());
				stZhanzheng zhanzhengItem;
				zhanzhengItem.m_iBuffType = atoi(stXML.GetAttrib("buff_type").c_str());
				zhanzhengItem.m_iBuffPercent = atoi(stXML.GetAttrib("buff_percent").c_str());

				m_mpZhanzheng[id] = zhanzhengItem;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("baozang"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("baozang_item"))
			{
				int	id = atoi(stXML.GetAttrib("id").c_str());
				std::string level_reward_pool = stXML.GetAttrib("level_reward_pool").c_str();

				m_mpBaozang[id] = level_reward_pool;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("jiaoyisuo"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("jiaoyisuo_item"))
			{
				int	id = atoi(stXML.GetAttrib("id").c_str());
				stJiaoyisuo jiaoyisuoItem;
				for (int i = 1; i < 10; ++i)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("jiaoyi_level_pool" + stStream.str()).empty()) break;

					jiaoyisuoItem.m_aJiaoyiLevelPool.push_back(stXML.GetAttrib("jiaoyi_level_pool" + stStream.str()).c_str());
				}

				m_mpJiaoyisuo[id] = jiaoyisuoItem;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("ganwu_content"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("ganwu_content_item"))
			{
				int id = atoi(stXML.GetAttrib("id").c_str());
				int sexual = atoi(stXML.GetAttrib("sexual").c_str());
				
				if (sexual == 1)
					m_aMaleGanwuContent.push_back(id);
				else
					m_aFemaleGanwuContent.push_back(id);
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("ganwu_hero"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("ganwu_hero_item"))
			{
				int	id = atoi(stXML.GetAttrib("card_id").c_str());
				stGanwuHero heroItem;
				heroItem.m_stConsumeItem1.m_iCardID = atoi(stXML.GetAttrib("item_id_1").c_str());
				heroItem.m_stConsumeItem1.m_iItemType = atoi(stXML.GetAttrib("item_type_1").c_str());
				heroItem.m_stConsumeItem1.m_iNum = atoi(stXML.GetAttrib("item_num_1").c_str());
				
				heroItem.m_stReward1.m_iCardID = atoi(stXML.GetAttrib("reward_item_id_1").c_str());
				heroItem.m_stReward1.m_iItemType = atoi(stXML.GetAttrib("reward_item_type_1").c_str());
				heroItem.m_stReward1.m_iNum = atoi(stXML.GetAttrib("reward_item_num_1").c_str());

				heroItem.m_stConsumeItem2.m_iCardID = atoi(stXML.GetAttrib("item_id_2").c_str());
				heroItem.m_stConsumeItem2.m_iItemType = atoi(stXML.GetAttrib("item_type_2").c_str());
				heroItem.m_stConsumeItem2.m_iNum = atoi(stXML.GetAttrib("item_num_2").c_str());
				
				heroItem.m_stReward2.m_iCardID = atoi(stXML.GetAttrib("reward_item_id_2").c_str());
				heroItem.m_stReward2.m_iItemType = atoi(stXML.GetAttrib("reward_item_type_2").c_str());
				heroItem.m_stReward2.m_iNum = atoi(stXML.GetAttrib("reward_item_num_2").c_str());

				m_mpGanwuHero[id] = heroItem;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("ronglian_item"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("item"))
			{
				int	item_type = atoi(stXML.GetAttrib("item_type").c_str());
				int	item_id = atoi(stXML.GetAttrib("item_id").c_str());
				int	ronglian_value = atoi(stXML.GetAttrib("ronglian_value").c_str());

				m_mpItemRonglianValue[std::make_tuple(item_type, item_id)] = ronglian_value;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("ronglian"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("ronglian_item"))
			{
				int	id = atoi(stXML.GetAttrib("id").c_str());
				stRonglian ronglianItem;

				ronglianItem.m_iRonglianValue = atoi(stXML.GetAttrib("max_ronglian_value").c_str());
				ronglianItem.m_strLevelRewardPool = stXML.GetAttrib("level_reward_pool").c_str();

				m_mpRonglian[id] = ronglianItem;
			}

			stXML.OutOfElem();
		}
		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("mibaoku"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("mibaoku_item"))
			{
				int	id = atoi(stXML.GetAttrib("id").c_str());
				stMibaoku mibaokuItem;

				mibaokuItem.m_iBox2Cash = atoi(stXML.GetAttrib("box_2_cash").c_str());
				mibaokuItem.m_iBox3Cash = atoi(stXML.GetAttrib("box_3_cash").c_str());
				mibaokuItem.m_strLevelRewardPool1 = stXML.GetAttrib("level_reward_pool_1").c_str();
				mibaokuItem.m_strLevelRewardPool2 = stXML.GetAttrib("level_reward_pool_2").c_str();
				mibaokuItem.m_strLevelRewardPool3 = stXML.GetAttrib("level_reward_pool_3").c_str();
				mibaokuItem.m_iCoef1 = atoi(stXML.GetAttrib("coef_1").c_str());
				mibaokuItem.m_iCoef2 = atoi(stXML.GetAttrib("coef_2").c_str());
				mibaokuItem.m_iCoef3 = atoi(stXML.GetAttrib("coef_3").c_str());

				m_mpMibaoku[id] = mibaokuItem;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("guanqia_pool"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("guanqia_item"))
			{
				int	id = atoi(stXML.GetAttrib("id").c_str());
				stGuanqiaPool guanqiaPool;

				guanqiaPool.m_strType = stXML.GetAttrib("type").c_str();
				for (int i = 1; i < 100; ++i)
				{
					stGuanqiaItem guanqiaItem;

					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("guanqia_id" + stStream.str()).empty()) break;

					int guanqiaId = atoi(stXML.GetAttrib("guanqia_id" + stStream.str()).c_str());
					guanqiaItem.m_iCoef = atoi(stXML.GetAttrib("guanqia_coef" + stStream.str()).c_str());
					guanqiaItem.m_iSex = atoi(stXML.GetAttrib("sexual" + stStream.str()).c_str());
					guanqiaItem.m_bPVEOrPVP = true;
					if (strcmp(stXML.GetAttrib("type").c_str(), "pvp") == 0)
						guanqiaItem.m_bPVEOrPVP = false;

					guanqiaPool.m_mpGuanqia[guanqiaId] = guanqiaItem;
				}

				m_mpGuanqiaPool[id] = guanqiaPool;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("level_guanqia_pool"))
		{
			stXML.IntoElem();
			while (stXML.FindElem("pool"))
			{
				stLevelGuanqiaPool levelGuanqiaPool;
				int poolId = atoi(stXML.GetAttrib("id").c_str());

				for (int i = 1; i < 100; ++i)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("begin_level" + stStream.str()).empty()) break;

					stLevelGuanqiaItem guanqiaItem;

					guanqiaItem.m_iBeginLevel = atoi(stXML.GetAttrib("begin_level" + stStream.str()).c_str());
					guanqiaItem.m_iEndLevel = atoi(stXML.GetAttrib("end_level" + stStream.str()).c_str());
					guanqiaItem.m_iGuanqiaPoolId = atoi(stXML.GetAttrib("pool_id" + stStream.str()).c_str());
					guanqiaItem.m_fRatio = atof(stXML.GetAttrib("ratio" + stStream.str()).c_str());

					levelGuanqiaPool.m_aLevelGuanqia.emplace_back(guanqiaItem);
				}

				m_mpGuanqiaLevelPool[poolId] = levelGuanqiaPool;
			}

			stXML.OutOfElem();
		}
		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("qiangzhetiaozhan"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("qiangzhetiaozhan_item"))
			{
				int	id = atoi(stXML.GetAttrib("id").c_str());
				stQiangzhetiaozhan qiangzhetiaozhanItem;

				qiangzhetiaozhanItem.m_iLevelGuanqiaPool = atoi(stXML.GetAttrib("level_guanqia_pool").c_str());
				//qiangzhetiaozhanItem.m_strLevelRewardPool = stXML.GetAttrib("level_reward_pool").c_str();

				m_mpQiangzhetiaozhan[id] = qiangzhetiaozhanItem;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("qiangzhetiaozhan_content"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("qiangzhetiaozhan_content_item"))
			{
				int id = atoi(stXML.GetAttrib("id").c_str());
				int sexual = atoi(stXML.GetAttrib("sexual").c_str());

				if (sexual == 1)
					m_aMaleQiangzheContent.push_back(id);
				else
					m_aFemaleQiangzheContent.push_back(id);
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("baozangshouwei"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("baozangshouwei_item"))
			{
				int	id = atoi(stXML.GetAttrib("id").c_str());
				stBaozangshouwei baozangshouweiItem;

				baozangshouweiItem.m_iLevelGuanqiaPool = atoi(stXML.GetAttrib("level_guanqia_pool").c_str());
				baozangshouweiItem.m_strLevelRewardPool = stXML.GetAttrib("level_reward_pool").c_str();

				for (int i = 1; i < 10; ++i)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("consume_item_type" + stStream.str()).empty()) break;

					CPackGameItem gameItem;

					gameItem.m_iItemType = atoi(stXML.GetAttrib("consume_item_type" + stStream.str()).c_str());
					gameItem.m_iCardID = atoi(stXML.GetAttrib("consume_item_id" + stStream.str()).c_str());
					gameItem.m_iNum = atoi(stXML.GetAttrib("consume_item_num" + stStream.str()).c_str());

					baozangshouweiItem.m_aConsumeItem.emplace_back(gameItem);
				}

				m_mpBaozangshouwei[id] = baozangshouweiItem;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("consume_group"))
		{
			stXML.IntoElem();
			while (stXML.FindElem("consume"))
			{
				int id = atoi(stXML.GetAttrib("id").c_str());

				for (int i = 1; i < 10; ++i)
				{
					CPackGameItem gameItem;

					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("consume_item_type" + stStream.str()).empty()) break;

					gameItem.m_iItemType = atoi(stXML.GetAttrib("consume_item_type" + stStream.str()).c_str());
					gameItem.m_iCardID = atoi(stXML.GetAttrib("consume_item_id" + stStream.str()).c_str());
					gameItem.m_iNum = atoi(stXML.GetAttrib("consume_item_num" + stStream.str()).c_str());

					m_mpConsumeGroup[id].m_aConsumes.emplace_back(gameItem);
				}
			}

			stXML.OutOfElem();
		}
		/////////////////////////////////////////////////////////////////////////////////
		if (stXML.FindElem("level_consume_pool"))
		{
			stXML.IntoElem();
			while (stXML.FindElem("pool"))
			{
				stLevelConsumePool consumePool;
				consumePool.m_iID = atoi(stXML.GetAttrib("id").c_str());

				for (int i = 1; i < 100; ++i)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("consume_id" + stStream.str()).empty()) break;

					stLevelConsumeItem consumeItem;

					consumeItem.m_iBeginLevel = atoi(stXML.GetAttrib("begin_level" + stStream.str()).c_str());
					consumeItem.m_iEndLevel = atoi(stXML.GetAttrib("end_level" + stStream.str()).c_str());
					consumeItem.m_iConsumeGroupID = atoi(stXML.GetAttrib("consume_id" + stStream.str()).c_str());

					consumePool.m_aConsume.emplace_back(consumeItem);
				}

				m_mpLevelConsumePool[consumePool.m_iID] = consumePool;
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////

		if (stXML.FindElem("maozei"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("maozei_item"))
			{
				stMaozei maozeiItem;

				maozeiItem.m_iID = atoi(stXML.GetAttrib("id").c_str());
				maozeiItem.m_iLevelGuanqiaPool = atoi(stXML.GetAttrib("level_guanqia_pool").c_str());
				maozeiItem.m_strLevelRewardPool1 = stXML.GetAttrib("level_reward_pool_1").c_str();
				maozeiItem.m_strLevelRewardPool2 = stXML.GetAttrib("level_reward_pool_2").c_str();
				maozeiItem.m_iLevelConsumePool = atoi(stXML.GetAttrib("consume_pool").c_str());

				m_mpMaozei[maozeiItem.m_iID] = maozeiItem;
			}

			stXML.OutOfElem();
		}
		/////////////////////////////////////////////////////////////////////////////////

		if (stXML.FindElem("maozei_content"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("maozei_content_item"))
			{
				int id = atoi(stXML.GetAttrib("id").c_str());
				m_aMaozeiContent.push_back(id);
			}

			stXML.OutOfElem();
		}
		/////////////////////////////////////////////////////////////////////////////////

		if (stXML.FindElem("mubei_content"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("mubei_content_item"))
			{
				int id = atoi(stXML.GetAttrib("id").c_str());
				m_aMubeiContent.push_back(id);
			}

			stXML.OutOfElem();
		}
		/////////////////////////////////////////////////////////////////////////////////
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

std::pair<bool, std::string>
CLogicConfigQiyuBotParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("qiyu_config"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();
		/////////////////////////////////////////////////////////////////////////////////

		if (stXML.FindElem("pvpbotlist"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("bots"))
			{
				int	id = atoi(stXML.GetAttrib("id").c_str());
				stPvpBot pvpBot;
				pvpBot.m_strRewardGroup = stXML.GetAttrib("reward_group").c_str();
				pvpBot.m_strCardName = stXML.GetAttrib("card_name").c_str();

				m_mpPVPBot[id] = std::move(pvpBot);
			}

			stXML.OutOfElem();
		}

		/////////////////////////////////////////////////////////////////////////////////
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}
