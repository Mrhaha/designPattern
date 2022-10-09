//
// Created by hzl on 2019/4/22.
//

#pragma once

enum SERVER_CMD_CROSS_DEFINE
{//特别注意:CROSS协议的ID范围: [12000 -- 12999]
    MSG_CROSS_UPDATE_MATCH_USER = 12001,				//更新匹配玩家数据[logic->cross]
    MSG_CROSS_NOTIFY_MATCH_PVP_INFO = 12002,			//通知巅峰对决信息[logic->client]
    MSG_CROSS_FIGHT_MATCH_PVP = 12003,				    //开始巅峰对决[client->logic->cross|logic->cross->logic->client]
    MSG_CROSS_FIGHT_MATCH_PVP_RESULT = 12004,			//结束巅峰对决[client->logic]
    MSG_CROSS_NOTIFY_BE_MATCHED = 12005,				//通知被匹配上[cross->logic]
    MSG_CROSS_BUY_MATCH_STORE_ITEM = 12006,			    //购买巅峰对决商店物品[client->logic]
    MSG_CROSS_GET_MATCH_MAX_AWARD = 12007,			    //获取巅峰对决最高段位奖励[client->logic]
    MSG_CROSS_NOTIFY_CROSS_BROADCAST = 12008,			//通知跨服公告[cross->logic]

    MSG_CROSS_NOTIFY_FAIR_PVP_INFO = 12010,			    //通知公平竞技信息[logic->client]
    MSG_CROSS_REFRESH_FAIR_PVP_CARD = 12011,			//刷新公平竞技武将[client->logic]
    MSG_CROSS_SELECT_FAIR_PVP_TEAM = 12012,			    //选择公平竞技队伍[client->logic]
    MSG_CROSS_FIGHT_FAIR_PVP = 12013,					//开始公平竞技[client->logic->cross|logic->cross->logic->client]
    MSG_CROSS_FIGHT_FAIR_PVP_RESULT = 12014,			//结束公平竞技[client->logic]
    MSG_CROSS_NOTIFY_FAIR_MATCHED = 12015,			    //通知公平竞技被匹配上[cross->logic]

    MSG_CROSS_GET_ATTACK_DEFENSE_INFO = 12020,		    //拉取据点攻防信息[client->logic->cross->logic->client]
    MSG_CROSS_FIGHT_ATTACK_DEFENSE = 12021,			    //开始攻防战斗[client->logic->cross->logic->client]
    MSG_CROSS_FIGHT_ATTACK_DEFENSE_RESULT = 12022,	    //攻防战斗结束[client->logic->cross->logic->client]
    MSG_CROSS_REPAIR_ATTACK_DEFENSE = 12023,			//修复攻防据点[client->logic->cross->logic->client]
    MSG_CROSS_REMOVE_FIGHT_FAIL_CD = 12024,			    //移除挑战失败CD[client->logic->client]
    MSG_CROSS_ATTACK_DEFENSE_SYNC = 12025,		        //跨服攻防数据准备[logic->cross]
    MSG_CROSS_ATTACK_DEFENSE_SYNC_DONE = 12026,	        //跨服攻防数据准备完成[logic->cross]
    MSG_CROSS_ATTACK_DEFENSE_HEART = 12027,		        //跨服攻防心跳触发[logic->cross]
    MSG_CROSS_ATTACK_DEFENSE_AWARD = 12028,		        //跨服攻防发送奖励[cross->logic]
    MSG_CROSS_ATTACK_DEFENSE_START = 12029,		        //跨服攻防开始通知[cross->logic]
    MSG_CROSS_GUILD_WAR_REPORT_NOTIFY = 12030,	        //跨服公会战战报[{logic}->cross->logic]
    MSG_CROSS_ATTACK_DEFENSE_PREPARE = 12031,           //跨服攻防战准备(请求数据)[cross->logic]

    MSG_CROSS_LUCKY_HAMMER_LOTTERY = 12040,			    //幸运魔锤抽奖[client->logic]
    MSG_CROSS_GET_LUCKY_HAMMER_RANK = 12041,			//拉取幸运魔锤排行[client->logic->cross->logic->client]
    MSG_CROSS_GET_LUCKY_HAMMER_LOG = 12042,			    //拉取幸运魔锤日志[client->logic->cross->logic->client]
    MSG_CROSS_LUCKY_HAMMER_LOG_SCORE = 12043,			//蹭幸运魔锤日志好运[client->logic]
    MSG_CROSS_SYNC_LUCKY_HAMMER_SCORE = 12044,	        //同步幸运魔锤积分[logic->cross]
    MSG_CROSS_NOTIFY_LUCKY_HAMMER_RANK_AWARD = 12045,	//通知幸运魔锤排行奖励[cross->logic]
    MSG_CROSS_SYNC_LUCKY_HAMMER_LOG = 12046,	        //同步幸运魔锤日志[logic->cross]

    MSG_CROSS_SERVICE_ALERT = 12200,                    //游戏服务告警[logic->cross]

    MSG_CROSS_CREATE_BANQUET_TEAM = 12201,              //创建宴会队伍
    MSG_CROSS_JOIN_BANQUET_TEAM = 12202,                //加入宴会队伍
    MSG_CROSS_GET_BANQUET_INFO = 12203,                 //获取宴会信息
    MSG_CROSS_BANQUET_FINISH_NOTIFY = 12204,            //宴会结束通知
    MSG_CROSS_BANQUET_INFO_NOTIFY = 12205,              //宴会信息通知
    MSG_CROSS_BANQUET_UPDATE_LOGIN = 12206,             //上线更新宴会信息
    MSG_CROSS_BANQUET_INVITE_NOTIFY = 12207,            //宴会邀请通知
    MSG_CROSS_BANQUET_QUIT_TEAM = 12208,                //退出当前宴会队伍

    MSG_CROSS_NEW_LIMIT_BOSS_NOTIFY = 12209,            //限时Boss出现通知
    MSG_CROSS_NEW_LIMIT_BOSS_HISTORY = 12210,           //限时Boss参与历史
    MSG_CROSS_NEW_LIMIT_BOSS_BEGIN = 12211,             //限时Boss挑战开始
    MSG_CROSS_NEW_LIMIT_BOSS_END   = 12212,             //限时Boss挑战结束
    MSG_CROSS_NEW_LIMIT_BOSS_DEAD_NOTIFY  = 12213,      //限时Boss死亡通知
    MSG_CROSS_NEW_LIMIT_BOSS_INVITE = 12214,            //限时Boss邀请好友
    MSG_CROSS_NEW_LIMIT_BOSS_INVITE_NOTIFY = 12215,     //限时Boss邀请通知
    MSG_CROSS_NEW_LIMIT_BOSS_GET_REWARD = 12216,        //限时Boss获取奖励
    MSG_CROSS_NEW_LIMIT_BOSS_RUN_NOTIFY = 12217,        //限时Boss逃逸通知
    MSG_CROSS_NEW_LIMIT_BOSS_GET_INFO = 12218,          //上线获取限时Boss信息

    MSG_CROSS_BANQUET_GET_FRIEND_OPEN = 12237,          //获取好友的公开宴会信息


    MSG_CROSS_HEART_BEAT = 12233,			            //心跳

    // 跨服
    MSG_CROSS_USER_BRIEF_UPDATE = 12400,                // 玩家简要信息更新
    MSG_CROSS_ADMIN_MODIFY_GUILD = 12401,               // GM修改公会
    MSG_CROSS_ADMIN_DISMISS_GUILD = 12402,              // GM解散公会
    MSG_CROSS_ADMIN_SET_GUILD_CHIEF = 12403,            // GM设置公会会长
    MSG_CROSS_ADMIN_CHANGE_GUILD_NAME = 12404,          // GM修改公会名
    MSG_CROSS_GET_USER_DETAIL = 12405,                  // 跨服获取其他服务器玩家详情[logic->->logic]
    MSG_CROSS_GUILD_BOSS_HURT_NOTIFY = 12406,           // 公会BOSS伤害量通知
    MSG_CROSS_GUILD_BOSS_REWARD_NOTIFY = 12407,         // 公会BOSS奖励通知
    MSG_CROSS_GUILD_CHANGE_NOTIFY = 12408,              // 公会变更通知
    MSG_CROSS_USER_TALK_NOTIFY = 12409,                 // 聊天通知
    MSG_CROSS_GUILD_WISH_NOTIFY = 12410,                // 公会祈愿同步跨服
    MSG_CROSS_GUILD_WISH_GET_LIST = 12411,              // 公会祈愿获取列表
    MSG_CROSS_GUILD_WISH_GET_SWAP_INFO = 12412,         // 公会祈愿获取交换信息[logic->->logic]

    MSG_CROSS_GUILD_WISH_CROSS_DST_GET = 12414,         // 公会祈愿跨服目标玩家获得[logic->->logic]
    MSG_CROSS_GUILD_WISH_CROSS_CHECK = 12415,           // 公会祈愿跨服检查[logic->->logic]
    MSG_CROSS_GUILD_WISH_CROSS_CHECK_RESULT = 12416,    // 公会祈愿跨服检查结果[logic->->logic]

    MSG_CROSS_FRIEND_DEAL_APPLY_CROSS_CHECK = 12420,    // 好友申请处理跨服检查[logic->->logic]
    MSG_CROSS_FRIEND_CROSS_DIRECT_ADD = 12421,          // 好友直接添加跨服[logic->->logic]
    MSG_CROSS_FRIEND_CROSS_DIRECT_DEL = 12422,          // 好友直接删除跨服[logic->->logic]

    MSG_CROSS_HOME_VISIT_OTHER = 12430,                 // 拜访其他服务器玩家家园[logic->->logic]
    MSG_CROSS_HOME_HOT_SPRING_OTHER_JOIN = 12431,       // 通知参加他人温泉[logic->->logic]

    MSG_CROSS_GUILD_WAR_GET_INFO = 12432,               // 获取公会战信息
    MSG_CROSS_GUILD_WAR_BEGIN   = 12433,                // 参与公会战
    MSG_CROSS_GUILD_WAR_END     = 12434,                // 结算公会战
    MSG_CROSS_GUILD_WAR_RANK    = 12435,                // 公会战排名
    MSG_CROSS_GUILD_WAR_GET_SELF_RANK = 12436,          // 每次上线更新公会战排名

    /// 排队服务器
    MSG_QUEUE_USER_LOGIN_NOTIFY = 12900,                //排队服务器通知玩家登录登出
    MSG_QUEUE_USER_LIST_ONLINE_NOTIFY = 12901,          //排队服务器通知在线玩家
    MSG_QUEUE_USER_QUEUE_HEARTBEAT = 12902,             //排队服务器玩家排队心跳信息
    MSG_QUEUE_USER_RECONNECT_CHECK = 12903,             //排队服务器玩家重连检查

};//特别注意:CROSS协议的ID范围: [12000 -- 12999]
