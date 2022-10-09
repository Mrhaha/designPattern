//
// Created by hzl on 2019/4/30.
//

#pragma once


enum SERVER_CMD_ADMIN_DEFINE
{//特别注意:ADMIN协议的ID范围: [9000 -- 9999]
    MSG_ADMIN_START = 9000,
    MSG_ADMIN_RECHARGE = 9000,              //GM充值
    MSG_ADMIN_MODIFY_ITEM = 9001,           //GM修改玩家物品
    MSG_ADMIN_ADD_SYSTEM_MAIL = 9002,		//GM发系统邮件
    MSG_ADMIN_DELETE_SYSTEM_MAIL = 9003,	//GM删除系统邮件
    MSG_ADMIN_GIFT_CARD_AWARD = 9004,		//GM通知礼包卡
    MSG_ADMIN_SEND_ACTIVE_MAIL = 9005,		//GM发送活跃邮件
    MSG_ADMIN_SET_USER_BAN = 9006,		    //GM封禁玩家

    MSG_ADMIN_MODIFY_CHAPTER = 9009,		//GM修改玩家的章节进度
    MSG_ADMIN_MODIFY_HERO_CARD = 9010,		//GM修改玩家的武将卡牌
    MSG_ADMIN_MODIFY_CREATE_TIME = 9011,	//GM修改玩家创角时间
    MSG_ADMIN_MODIFY_GUIDE = 9012,		    //GM修改玩家引导
    MSG_ADMIN_MODIFY_GUILD = 9013,		    //GM修改公会数据
    MSG_ADMIN_DELETE_HERO = 9014,		    //GM删除伙伴
    MSG_ADMIN_DELETE_RANK = 9015,		    //GM删除排行
    MSG_ADMIN_CLEAR_CACHE = 9016,		    //清除缓存
    MSG_ADMIN_SET_GUILD_BULLETIN = 9017,	//GM设置公会公告
    MSG_ADMIN_JUBAO_AWARD = 9018,		    //GM举报奖励
    MSG_ADMIN_USER_RANK = 9019,		        //GM玩家排行
    MSG_ADMIN_CROSS_DAY = 9020,		        //GM玩家跨天
    MSG_ADMIN_ADD_FISH = 9021,              //GM添加鱼

    MSG_ADMIN_ADD_EQUIP = 9023,             //GM添加特定装备
    MSG_ADMIN_DISMISS_GUILD = 9024,		    //GM解散公会
    MSG_ADMIN_SET_GUILD_CHIEF = 9025,       //GM修改公会会长
    MSG_ADMIN_GET_CAN_DEPOSIT = 9026,       //GM请求可以充值的ID
    MSG_ADMIN_SET_ENTIRE_WAR_PASS = 9027,   //GM开启全名战役关卡
    MSG_ADMIN_CHANGE_GUILD_NAME = 9028,     //GM修改公会名字

    MSG_ADMIN_COPY_USER = 9031,             //GM复制玩家数据
    MSG_ADMIN_ADD_CONSTEL = 9032,           //GM添加特定星纹
    MSG_ADMIN_DEL_CONSTEL = 9033,           //GM删除星纹
    MSG_ADMIN_CHANGE_AGE_LIMIT = 9034,      //GM修改年龄选择

    MSG_ADMIN_END,

};//特别注意:ADMIN协议的ID范围: [9000 -- 9999]

