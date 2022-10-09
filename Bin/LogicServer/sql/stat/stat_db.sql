SET NAMES utf8;

USE STAT_DATABASE;

CREATE TABLE IF NOT EXISTS `platform_account` (
    `link_date`           DATE        NOT NULL                             COMMENT '链接日期',
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `account`             VARCHAR(64) NOT NULL                             COMMENT '账号',
    `sdk`                 VARCHAR(64) NOT NULL                             COMMENT 'sdk',
    `channel_id`          INT         NOT NULL DEFAULT 1                   COMMENT '初始渠道ID',
    `analytics_id`        VARCHAR(64) NOT NULL DEFAULT ''                  COMMENT '初始分析ID',
    `last_login_group`    INT         NOT NULL DEFAULT 0                   COMMENT '最后登录区服',
    `link_time`           timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '链接时间',
    `created_at`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '创建时间',
    `updated_at`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',

    INDEX(`link_date`),
    INDEX(`link_time`),
    INDEX(`updated_at`),
    PRIMARY KEY(`platform`, `account`, `sdk`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='平台账号';

CREATE TABLE IF NOT EXISTS `platform_login` (
    `login_date`          DATE        NOT NULL                             COMMENT '登录日期',
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `account`             VARCHAR(64) NOT NULL                             COMMENT '账号',
    `sdk`                 VARCHAR(64) NOT NULL                             COMMENT 'sdk',
    `login_time`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '登录时间',
    `created_at`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '创建时间',
    `updated_at`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',

    INDEX(`updated_at`),
    PRIMARY KEY(`login_date`, `platform`, `account`, `sdk`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='平台登录';

CREATE TABLE IF NOT EXISTS `platform_group_role_num` (
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `group_id`     	  	  INT         NOT NULL                             COMMENT '区服',
	`role_num`     	  	  INT         NOT NULL                             COMMENT '玩家数量',

    PRIMARY KEY(`platform`, `group_id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='区服玩家数';

CREATE TABLE IF NOT EXISTS `platform_pay` (
    `pay_date`            DATE        NOT NULL                             COMMENT '充值日期',
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `account`             VARCHAR(64) NOT NULL                             COMMENT '账号',
    `sdk`                 VARCHAR(64) NOT NULL                             COMMENT 'sdk',
    `pay_cash`            DECIMAL (15,2)  NOT NULL                         COMMENT '充值现金',
    `created_at`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '创建时间',
    `updated_at`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',

    INDEX(`updated_at`),
    PRIMARY KEY(`pay_date`, `platform`, `account`, `sdk`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='平台充值';

CREATE TABLE IF NOT EXISTS `platform_device_64bit` (
    `device_id`           VARCHAR(128) NOT NULL                            COMMENT '设备ID',
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `channel_id`          INT         NOT NULL                             COMMENT '渠道ID',
    `os`                  VARCHAR(64) NOT NULL                             COMMENT '系统',
    `is_64bit`            INT         NOT NULL                             COMMENT '是否64位',
    `created_at`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '创建时间',
    `updated_at`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
 
    INDEX(`updated_at`),
    PRIMARY KEY (`device_id`),
	KEY (`platform`, `channel_id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='平台设备64bit';

CREATE TABLE IF NOT EXISTS `platform_account_channel` (
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `account`             VARCHAR(64) NOT NULL                             COMMENT '账号',
    `sdk`                 VARCHAR(64) NOT NULL                             COMMENT 'sdk',
    `channel_id`          INT         NOT NULL                             COMMENT '渠道ID',
    `link_time`           timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '链接时间',
    `created_at`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '创建时间',
    `updated_at`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',

    INDEX(`link_time`),
    INDEX(`updated_at`),
    PRIMARY KEY(`platform`, `account`, `sdk`, `channel_id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='平台账号渠道';

CREATE TABLE IF NOT EXISTS `stat_online` (
  `stat_time`     TIMESTAMP   NOT NULL                                   COMMENT '统计时间',
  `platform`      VARCHAR(32) NOT NULL                                   COMMENT '平台',
  `server_id`     INT         NOT NULL                                   COMMENT '服务器编号',
  `channel_id`    INT         NOT NULL     DEFAULT '0'                   COMMENT '渠道ID',
  `online_num`    INT         NOT NULL     DEFAULT '0'                   COMMENT '在线人数',
  `create_num`    INT         NOT NULL     DEFAULT '0'                   COMMENT '创角人数',
  `created_at`    timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '创建时间',
  `updated_at`    timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
  
  INDEX(`updated_at`),
  PRIMARY KEY (`stat_time`, `platform`, `server_id`, `channel_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='服务器人数统计';

CREATE TABLE IF NOT EXISTS `stat_player` (
    `stat_date`        DATE        NOT NULL COMMENT '统计日期',
    `uin`              INT         NOT NULL COMMENT '玩家UIN',
    `platform`         VARCHAR(32) NOT NULL COMMENT '平台',
    `channel_id`       INT         NOT NULL COMMENT '渠道ID',    
    `group_id`         INT         NOT NULL COMMENT '玩家服号',
    `server_id`        INT         NOT NULL DEFAULT 0     COMMENT '服务器编号',
    `create_time`      INT         NOT NULL DEFAULT 0     COMMENT '创角时间',
    `nick`             VARCHAR(32) NOT NULL DEFAULT ''    COMMENT '角色名',    
    `level`            INT         NOT NULL DEFAULT 0     COMMENT '角色等级',
    `login_count`      INT         NOT NULL DEFAULT 0     COMMENT '登陆次数',
    `login_time`       INT         NOT NULL DEFAULT 0     COMMENT '最后登录时间',
    `online_time`      INT         NOT NULL DEFAULT 0     COMMENT '在线时长',
    `daily_pay_cash`   INT         NOT NULL DEFAULT 0     COMMENT '每日充值现金',
    `pay_cash`         INT         NOT NULL DEFAULT 0     COMMENT '充值现金',
    `yuanbao`          INT         NOT NULL DEFAULT 0     COMMENT '元宝数',
    `fair_win_count`   INT         NOT NULL DEFAULT 0     COMMENT '公平竞技胜利次数',
    `fair_team_card`   VARCHAR(32) NOT NULL DEFAULT ''    COMMENT '公平竞技阵容',
	`role_combat`      INT         NOT NULL DEFAULT 0     COMMENT 'role_combat',
    `created_at`       timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '创建时间',
    `updated_at`       timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',

    INDEX(`updated_at`),
    PRIMARY KEY(`stat_date`, `uin`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='每日玩家信息统计';

CREATE TABLE IF NOT EXISTS `stat_guild_wish` (
	`platform`         VARCHAR(32) NOT NULL COMMENT '平台',
    
	`stat_time`        INT         NOT NULL COMMENT '统计时间',
	`uin`              INT         NOT NULL COMMENT '玩家UIN',
	`group_id`         INT         NOT NULL COMMENT '玩家服号',
	`dst_uin`          INT         NOT NULL COMMENT '目标玩家UIN',
	`dst_group_id`     INT         NOT NULL COMMENT '目标玩家服号',
	`card_id`          INT         NOT NULL COMMENT '目标伙伴',
    `swap_card_id`     INT         NOT NULL COMMENT '交换的伙伴',
    
	`consume`          INT         NOT NULL DEFAULT 0     COMMENT '是否已消耗',
	`dst_accept`       INT         NOT NULL DEFAULT 0     COMMENT '目标玩家是否已接收',
    
	`created_at`       timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '创建时间',
    `updated_at`       timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',

    INDEX(`updated_at`),
    PRIMARY KEY(`stat_time`, `uin`, `dst_uin`, `card_id`, `swap_card_id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='公会祈愿记录';

CREATE TABLE IF NOT EXISTS `game_questionnaire` (
	`stat_time`        TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '生成时间',
	`uin`              INT         NOT NULL COMMENT '玩家UIN',
	`group_id`         INT         NOT NULL COMMENT '玩家服号',
    `platform`         VARCHAR(32) NOT NULL COMMENT '平台',
    `channel_id`       INT         NOT NULL COMMENT '渠道ID',
	`server_id`        INT         NOT NULL COMMENT '服务器编号',
	`questionnaire_id` INT         NOT NULL COMMENT '问卷ID',
	
	`nick`             VARCHAR(32) NOT NULL DEFAULT ''    COMMENT '角色名',
    `level`            INT         NOT NULL DEFAULT 0     COMMENT '角色等级',
	`vip_level`        INT         NOT NULL DEFAULT 0     COMMENT 'VIP等级',
	`combat`           INT         NOT NULL DEFAULT 0     COMMENT '战斗力',
    
	`select1`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案1',
	`select2`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案2',
	`select3`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案3',
	`select4`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案4',
	`select5`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案5',
	`select6`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案6',
	`select7`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案7',
	`select8`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案8',
	`select9`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案9',
	`select10`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案10',
	
	`select11`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案11',
	`select12`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案12',
	`select13`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案13',
	`select14`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案14',
	`select15`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案15',
	`select16`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案16',
	`select17`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案17',
	`select18`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案18',
	`select19`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案19',
	`select20`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案20',
	
	`select21`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案21',
	`select22`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案22',
	`select23`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案23',
	`select24`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案24',
	`select25`   	   VARCHAR(20) NOT NULL DEFAULT ''    COMMENT '选择答案25',
	
	`problem1`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目1',
	`problem2`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目2',
	`problem3`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目3',
	`problem4`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目4',
	`problem5`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目5',
	`problem6`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目6',
	`problem7`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目7',
	`problem8`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目8',
	`problem9`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目9',
	`problem10`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目10',
	
	`problem11`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目11',
	`problem12`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目12',
	`problem13`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目13',
	`problem14`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目14',
	`problem15`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目15',
	`problem16`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目16',
	`problem17`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目17',
	`problem18`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目18',
	`problem19`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目19',
	`problem20`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目20',
	
	`problem21`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目21',
	`problem22`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目22',
	`problem23`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目23',
	`problem24`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目24',
	`problem25`   	   VARCHAR(400) NOT NULL DEFAULT ''    COMMENT '题目25',

    PRIMARY KEY(`questionnaire_id`, `uin`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='玩家问卷调查';

CREATE TABLE IF NOT EXISTS `stat_event` (
    `stat_date`        DATE        NOT NULL COMMENT '统计日期',
    `platform`         VARCHAR(32) NOT NULL COMMENT '平台',
    `event`            VARCHAR(32) NOT NULL COMMENT '事件',
    `event_id`         VARCHAR(32) NOT NULL COMMENT '事件ID',
    `event_num`        INT         NOT NULL DEFAULT 0     COMMENT '事件数量',
    `extra_num`        INT         NOT NULL DEFAULT 0     COMMENT '额外数量',
    `created_at`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '创建时间',
    `updated_at`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',

    INDEX(`updated_at`),
    PRIMARY KEY(`stat_date`, `platform`, `event`, `event_id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='每日事件统计';

CREATE TABLE IF NOT EXISTS `stat_item` (
    `stat_date`           DATE   	  NOT NULL 							   COMMENT '账单日期',
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `server_id`           INT         NOT NULL                             COMMENT '服务器编号',
    `cmd`                 INT         NOT NULL DEFAULT 0                   COMMENT '消息事件',
    `item_type`           INT         NOT NULL DEFAULT 0                   COMMENT '道具类型',
    `item_id`             INT         NOT NULL DEFAULT 0                   COMMENT '道具ID',
    `sum_output_value`    BIGINT      NOT NULL DEFAULT 0                   COMMENT '产出的总量',
    `sum_consume_value`   BIGINT      NOT NULL DEFAULT 0                   COMMENT '消耗的总量',
    `created_at`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '创建时间',
    `updated_at`          timestamp   NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',

    PRIMARY KEY(`stat_date`, `platform`, `server_id`, `cmd`, `item_type`, `item_id`),
    INDEX `stat_date`(`stat_date`),
    INDEX `uid`(`platform`, `server_id`),
    INDEX (`item_type`, `item_id`),
    INDEX(`updated_at`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='道具账单中间表';

CREATE TABLE IF NOT EXISTS `game_key_gift` (
    `key_type`            INT           NOT NULL  AUTO_INCREMENT  COMMENT '类型',
    `title`               VARCHAR(60)   NOT NULL  DEFAULT ''      COMMENT '礼品标题',
    `platform`            VARCHAR(32)   NOT NULL  DEFAULT ''      COMMENT '适用平台',
    `channel`             INT           NOT NULL  DEFAULT 0       COMMENT '适用渠道',
    `share_key`           INT           NOT NULL  DEFAULT 0       COMMENT '是否共享',
	`begin_time`          INT           NOT NULL  DEFAULT 0       COMMENT '开始时间',
    `end_time`            INT           NOT NULL  DEFAULT 0       COMMENT '结束时间',
    `max_award_count`     INT           NOT NULL  DEFAULT 0       COMMENT '最大领取次数',
    `gift_item_type`      INT           NOT NULL  DEFAULT 0       COMMENT '礼品类型',
    `gift_item_id`        INT           NOT NULL  DEFAULT 0       COMMENT '礼品ID',
    `gift_item_num`       INT           NOT NULL  DEFAULT 0       COMMENT '礼品数量',
    `gift_item_type_1`    INT           NOT NULL  DEFAULT 0       COMMENT '礼品类型1',
    `gift_item_id_1`      INT           NOT NULL  DEFAULT 0       COMMENT '礼品ID1',
    `gift_item_num_1`     INT           NOT NULL  DEFAULT 0       COMMENT '礼品数量1',
    `gift_item_type_2`    INT           NOT NULL  DEFAULT 0       COMMENT '礼品类型2',
    `gift_item_id_2`      INT           NOT NULL  DEFAULT 0       COMMENT '礼品ID2',
    `gift_item_num_2`     INT           NOT NULL  DEFAULT 0       COMMENT '礼品数量2',
    `gift_item_type_3`    INT           NOT NULL  DEFAULT 0       COMMENT '礼品类型3',
    `gift_item_id_3`      INT           NOT NULL  DEFAULT 0       COMMENT '礼品ID3',
    `gift_item_num_3`     INT           NOT NULL  DEFAULT 0       COMMENT '礼品数量3',
    `gift_item_type_4`    INT           NOT NULL  DEFAULT 0       COMMENT '礼品类型4',
    `gift_item_id_4`      INT           NOT NULL  DEFAULT 0       COMMENT '礼品ID4',
    `gift_item_num_4`     INT           NOT NULL  DEFAULT 0       COMMENT '礼品数量4',

    PRIMARY KEY (`key_type`)

) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='游戏内平台兑换码礼品配置';

CREATE TABLE IF NOT EXISTS `game_key_code` (
    `gift_key`            VARCHAR(32)   NOT NULL  DEFAULT ''  COMMENT '礼包激活码',
    `key_type`            INT           NOT NULL  DEFAULT 0   COMMENT '类型',
    `flag`                INT           NOT NULL  DEFAULT 0   COMMENT '领取标志，0未领取，1已领取',
    `role_uin`            INT           NOT NULL  DEFAULT 0   COMMENT '领取uin',
    `group_id`            INT           NOT NULL  DEFAULT 0   COMMENT '服ID',
    `account`             VARCHAR(32)   NOT NULL  DEFAULT ''  COMMENT '平台账号',
    `platform`            VARCHAR(32)   NOT NULL  DEFAULT ''  COMMENT '平台',
    `channel`             INT           NOT NULL  DEFAULT 0   COMMENT '渠道',
    `request_time`        INT           NOT NULL  DEFAULT 0   COMMENT '领取时间',
    `create_time`         TIMESTAMP     NOT NULL  DEFAULT CURRENT_TIMESTAMP COMMENT '生成时间',

    PRIMARY KEY (`gift_key`),
    KEY `key_uin` (`key_type`, `role_uin`)

) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='游戏内平台兑换码领取';

CREATE TABLE IF NOT EXISTS `game_chat` (
    `id`                  INT         NOT NULL AUTO_INCREMENT              COMMENT '编号',
    `stat_time`           TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '账单时间戳',
    `platform`            VARCHAR(32) NOT NULL DEFAULT ''                  COMMENT '平台',
    `channel`             INT         NOT NULL                             COMMENT '渠道',
    `uin`                 INT         NOT NULL DEFAULT 0                   COMMENT '玩家UIN',
    `group_id`            INT         NOT NULL DEFAULT 0                   COMMENT '玩家服号',
    `server_id`           INT         NOT NULL                             COMMENT '服务器编号',    
    `address`             VARCHAR(32) NOT NULL DEFAULT ''                  COMMENT 'ip地址',
    `level`               INT         NOT NULL DEFAULT 0                   COMMENT '等级',
    `vip_level`           INT         NOT NULL DEFAULT 0                   COMMENT 'vip等级',
    `nick`                VARCHAR(64) NOT NULL DEFAULT ''                  COMMENT '昵称',
    `talk_channel`        INT         NOT NULL DEFAULT 0                   COMMENT '聊天渠道',
    `talk_uin`            INT         NOT NULL DEFAULT 0                   COMMENT '聊天目标uin',
    `talk_content`        VARCHAR(512) NOT NULL DEFAULT ''                 COMMENT '消息内容',

    PRIMARY KEY (`id`),
    INDEX(`platform`, `stat_time`, `group_id`, `uin`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='游戏内聊天消息';

CREATE TABLE IF NOT EXISTS `game_jubao` (
    `id`                  INT         NOT NULL AUTO_INCREMENT              COMMENT '编号',
    `stat_time`           TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '账单时间戳',
    `platform`            VARCHAR(32) NOT NULL DEFAULT ''                  COMMENT '平台',
    `channel`             INT         NOT NULL                             COMMENT '渠道',
    `uin`                 INT         NOT NULL DEFAULT 0                   COMMENT '玩家UIN',
    `group_id`            INT         NOT NULL DEFAULT 0                   COMMENT '玩家服号',
    `server_id`           INT         NOT NULL                             COMMENT '服务器编号',    
    `nick`                VARCHAR(64) NOT NULL DEFAULT ''                  COMMENT '昵称',
    `dst_uin`             INT         NOT NULL DEFAULT 0                   COMMENT '目标玩家UIN',
    `dst_group_id`        INT         NOT NULL DEFAULT 0                   COMMENT '目标玩家服号',
    `dst_nick`            VARCHAR(64) NOT NULL DEFAULT ''                  COMMENT '目标昵称',
    `dst_level`           INT         NOT NULL DEFAULT 0                   COMMENT '目标等级',
    `dst_vip_level`       INT         NOT NULL DEFAULT 0                   COMMENT '目标vip等级',
    `dst_fight_power`     INT         NOT NULL DEFAULT 0                   COMMENT '目标战斗力',
    `type`                INT         NOT NULL DEFAULT 0                   COMMENT '举报类型',
    `from_ui`             VARCHAR(64) NOT NULL DEFAULT ''                  COMMENT 'UI页签',
    `content`             VARCHAR(512) NOT NULL DEFAULT ''                 COMMENT '举报内容',
    `is_award`            INT         NOT NULL DEFAULT 0                   COMMENT '是否奖励',
    `user`                VARCHAR(64) NOT NULL DEFAULT ''                  COMMENT '处理人',

    PRIMARY KEY (`id`),
    INDEX(`platform`, `stat_time`, `group_id`, `uin`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='游戏内举报信息';

CREATE TABLE IF NOT EXISTS `bill_login` (
    `stat_time`           TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '账单时间戳',
    `bill_ration`         VARCHAR(64) NOT NULL                             COMMENT '账单号',
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `channel`             INT         NOT NULL                             COMMENT '渠道',
    `uin`                 INT         NOT NULL                             COMMENT '玩家UIN',
    `group_id`            INT         NOT NULL                             COMMENT '玩家服号',
    `server_id`           INT         NOT NULL                             COMMENT '服务器编号',    
    `cmd`                 INT         NOT NULL DEFAULT 0                   COMMENT '消息事件',
    `account`             VARCHAR(50) NOT NULL DEFAULT ''                  COMMENT '平台账号',
    `client_addr`         VARCHAR(60) NOT NULL DEFAULT ''                  COMMENT '客户端地址',
    `device_id`           VARCHAR(60) NOT NULL DEFAULT ''                  COMMENT '设备ID',
    `device_model`        VARCHAR(60) NOT NULL DEFAULT ''                  COMMENT '设备model',
    `device_os`           VARCHAR(60) NOT NULL DEFAULT ''                  COMMENT '设备系统',
    `device_ratio`        VARCHAR(60) NOT NULL DEFAULT ''                  COMMENT '设备ratio',
    `device_carrier`      VARCHAR(60) NOT NULL DEFAULT ''                  COMMENT '设备carrier',
    `device_net`          VARCHAR(60) NOT NULL DEFAULT ''                  COMMENT '设备网络',
    `online_time`         INT         NOT NULL DEFAULT 0                   COMMENT '在线时长',

    INDEX `stat_time`(`stat_time`),
    INDEX `uid`(`platform`, `uin`, `group_id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='玩家登录登出账单';

CREATE TABLE IF NOT EXISTS `bill_item` (
    `stat_time`           TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '账单时间戳',
    `bill_ration`         VARCHAR(64) NOT NULL                             COMMENT '账单号',
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `channel`             INT         NOT NULL                             COMMENT '渠道',    
    `uin`                 INT         NOT NULL                             COMMENT '玩家UIN',
    `group_id`            INT         NOT NULL                             COMMENT '玩家服号',
    `server_id`           INT         NOT NULL                             COMMENT '服务器编号',    
    `cmd`                 INT         NOT NULL DEFAULT 0                   COMMENT '消息事件',
    `item_type`           INT         NOT NULL DEFAULT 0                   COMMENT '道具类型',
    `item_id`             INT         NOT NULL DEFAULT 0                   COMMENT '道具ID',
    `current_value`       INT         NOT NULL DEFAULT 0                   COMMENT '剩余的数量',
    `delta_value`         INT         NOT NULL DEFAULT 0                   COMMENT '变化的数量',
    `description`         VARCHAR(256) NOT NULL DEFAULT ''                 COMMENT '详细信息',

    INDEX `stat_time`(`stat_time`),
    INDEX `uid`(`platform`, `uin`, `group_id`),
    INDEX `item`(`item_type`, `item_id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='道具账单';

CREATE TABLE IF NOT EXISTS `bill_equip` (
    `stat_time`           TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '账单时间戳',
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `channel`             INT         NOT NULL                             COMMENT '渠道',
    `uin`                 INT         NOT NULL                             COMMENT '玩家UIN',
    `group_id`            INT         NOT NULL                             COMMENT '玩家服号',
    `server_id`           INT         NOT NULL                             COMMENT '服务器编号',    
    `cmd`                 INT         NOT NULL DEFAULT 0                   COMMENT '消息事件',
    `unique_id`           INT         NOT NULL DEFAULT 0                   COMMENT '唯一ID',
    `grade`           	  INT         NOT NULL DEFAULT 0                   COMMENT '品质',
    `equip_tag`           INT         NOT NULL DEFAULT 0                   COMMENT '装备位',
    `attr_index`          INT         NOT NULL DEFAULT 0                   COMMENT '属性组索引',
    `delta_value`         INT         NOT NULL DEFAULT 0                   COMMENT '变化的数量',

    INDEX `stat_time`(`stat_time`),
    INDEX `uid`(`platform`, `uin`, `group_id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='装备账单';

CREATE TABLE IF NOT EXISTS `bill_constel` (
    `stat_time`           TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '账单时间戳',
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `channel`             INT         NOT NULL                             COMMENT '渠道',
    `uin`                 INT         NOT NULL                             COMMENT '玩家UIN',
    `group_id`            INT         NOT NULL                             COMMENT '玩家服号',
    `server_id`           INT         NOT NULL                             COMMENT '服务器编号',    
    `cmd`                 INT         NOT NULL DEFAULT 0                   COMMENT '消息事件',
    `unique_id`           INT         NOT NULL DEFAULT 0                   COMMENT '唯一ID',
    `grade`           	  INT         NOT NULL DEFAULT 0                   COMMENT '品质',
    `equip_tag`           INT         NOT NULL DEFAULT 0                   COMMENT '装备位',
    `suit`                INT         NOT NULL DEFAULT 0                   COMMENT '套装',
	`level`               INT         NOT NULL DEFAULT 0                   COMMENT '等级',
    `attr_str`        	  VARCHAR(256) NOT NULL DEFAULT ''                 COMMENT '属性字符串',
    `delta_value`         INT         NOT NULL DEFAULT 0                   COMMENT '变化的数量',

    INDEX `stat_time`(`stat_time`),
    INDEX `uid`(`platform`, `uin`, `group_id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='星纹账单';

CREATE TABLE IF NOT EXISTS `bill_first_fight` (
    `stat_time`           TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '账单时间戳',
    `bill_ration`         VARCHAR(64) NOT NULL                             COMMENT '账单号',
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `channel`             INT         NOT NULL                             COMMENT '渠道',
    `uin`                 INT         NOT NULL                             COMMENT '玩家UIN',
    `group_id`            INT         NOT NULL                             COMMENT '玩家服号',
    `server_id`           INT         NOT NULL                             COMMENT '服务器编号',    
    `cmd`                 INT         NOT NULL DEFAULT 0                   COMMENT '消息事件',
    `level_id`            INT         NOT NULL DEFAULT 0                   COMMENT '地图ID',
    `level_step`          INT         NOT NULL DEFAULT 0                   COMMENT '步骤,0开始战斗，1进行胜利结算',
    `fight_power`         INT         NOT NULL DEFAULT 0                   COMMENT '战斗力',

    INDEX `stat_time`(`stat_time`),
    INDEX `uid`(`platform`, `uin`, `group_id`),
    INDEX `level`(`level_id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='新手首次挑战账单';

CREATE TABLE IF NOT EXISTS `bill_fight_pass` (
    `stat_time`           TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '账单时间戳',
    `bill_ration`         VARCHAR(64) NOT NULL                             COMMENT '账单号',
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `channel`             INT         NOT NULL                             COMMENT '渠道',
    `uin`                 INT         NOT NULL                             COMMENT '玩家UIN',
    `group_id`            INT         NOT NULL                             COMMENT '玩家服号',
    `server_id`           INT         NOT NULL                             COMMENT '服务器编号',    
    `cmd`                 INT         NOT NULL DEFAULT 0                   COMMENT '消息事件',
    `level_id`            INT         NOT NULL DEFAULT 0                   COMMENT '地图ID',
    `fight_power`         INT         NOT NULL DEFAULT 0                   COMMENT '战斗力',
    `fight_team`          VARCHAR(50) NOT NULL DEFAULT ''                  COMMENT '战斗队伍',
    `addition`            VARCHAR(255) NOT NULL DEFAULT ''                 COMMENT '附加信息',

    INDEX `stat_time`(`stat_time`),
    INDEX `uid`(`platform`, `uin`, `group_id`),
    INDEX `level`(`level_id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='玩家通关账单';

CREATE TABLE IF NOT EXISTS `bill_anti_cheating` (
    `stat_time`           TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '账单时间戳',
    `bill_ration`         VARCHAR(64) NOT NULL                             COMMENT '账单号',
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `channel`             INT         NOT NULL                             COMMENT '渠道',
    `uin`                 INT         NOT NULL                             COMMENT '玩家UIN',
    `group_id`            INT         NOT NULL                             COMMENT '玩家服号',
    `server_id`           INT         NOT NULL                             COMMENT '服务器编号',    
    `cmd`                 INT         NOT NULL DEFAULT 0                   COMMENT '消息事件',
    `level_id`            INT         NOT NULL DEFAULT 0                   COMMENT '地图ID',
    `fight_power`         INT         NOT NULL DEFAULT 0                   COMMENT '通关战斗力',
    `fight_team`          VARCHAR(50) NOT NULL DEFAULT ''                  COMMENT '战斗队伍',
    `anti_fight_power`    INT         NOT NULL DEFAULT 0                   COMMENT '反外挂战斗力',
    `addition`            VARCHAR(255) NOT NULL DEFAULT ''                 COMMENT '附加信息',

    INDEX `stat_time`(`stat_time`),
    INDEX `uid`(`platform`, `uin`, `group_id`),
    INDEX `level`(`level_id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='玩家通关反作弊账单';

CREATE TABLE IF NOT EXISTS `bill_anti_score` (
    `stat_time`           TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '账单时间戳',
    `bill_ration`         VARCHAR(64) NOT NULL                             COMMENT '账单号',
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `channel`             INT         NOT NULL                             COMMENT '渠道',
    `uin`                 INT         NOT NULL                             COMMENT '玩家UIN',
    `group_id`            INT         NOT NULL                             COMMENT '玩家服号',
    `server_id`           INT         NOT NULL                             COMMENT '服务器编号',    
    `cmd`                 INT         NOT NULL DEFAULT 0                   COMMENT '消息事件',
    `level_id`            INT         NOT NULL DEFAULT 0                   COMMENT '通过天数',
    `pass_score`          INT         NOT NULL DEFAULT 0                   COMMENT '通过分数',
    `score_limit`         INT         NOT NULL DEFAULT 0                   COMMENT '分数限制',
    `fight_power`         INT         NOT NULL DEFAULT 0                   COMMENT '战斗力',
    `fight_team`          VARCHAR(50) NOT NULL DEFAULT ''                  COMMENT '战斗队伍',
    `addition`            VARCHAR(255) NOT NULL DEFAULT ''                 COMMENT '附加信息',

    INDEX `stat_time`(`stat_time`),
    INDEX `uid`(`platform`, `uin`, `group_id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='玩家通关积分反作弊账单';

CREATE TABLE IF NOT EXISTS `bill_client_step` (
	`id` INT ( 11 ) NOT NULL AUTO_INCREMENT COMMENT 'id',
	`stat_time` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '账单时间戳',
	`device_id` VARCHAR ( 128 ) NOT NULL DEFAULT '' COMMENT '设备ID',
	`step` VARCHAR ( 64 ) NOT NULL DEFAULT '' COMMENT '步骤',
	`address` VARCHAR ( 32 ) NOT NULL DEFAULT '' COMMENT 'ip地址',
	`os` VARCHAR ( 32 ) NOT NULL DEFAULT '' COMMENT '系统',
	`platform` VARCHAR ( 32 ) NOT NULL DEFAULT '' COMMENT '平台',
	`channel` INT ( 11 ) NOT NULL DEFAULT '0' COMMENT '渠道',
	`pkg_ver` INT ( 11 ) NOT NULL DEFAULT '0' COMMENT '包版本',
	`res_ver` INT ( 11 ) NOT NULL DEFAULT '0' COMMENT '资源版本',
	`analytics_id` VARCHAR ( 64 ) NOT NULL DEFAULT '' COMMENT '分析ID',
	PRIMARY KEY ( `id` ),
	KEY `stat_time` (`stat_time`),
	UNIQUE KEY `step_device` (`platform`,`step`,`device_id`)
) ENGINE = INNODB DEFAULT CHARSET = utf8 COMMENT = '客户端步骤账单';

CREATE TABLE IF NOT EXISTS `bill_client_error` (
    `stat_time`           TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '账单时间戳',
    `error_type`          INT         NOT NULL DEFAULT 0                   COMMENT '错误类型',
    `address`             VARCHAR(32) NOT NULL DEFAULT ''                  COMMENT 'ip地址',
    `os`                  VARCHAR(32) NOT NULL DEFAULT ''                  COMMENT '系统',
    `platform`            VARCHAR(32) NOT NULL DEFAULT ''                  COMMENT '平台',
    `channel`             INT         NOT NULL DEFAULT 0                   COMMENT '渠道',
    `uin`                 INT         NOT NULL DEFAULT 0                   COMMENT '玩家UIN',
    `group_id`            INT         NOT NULL DEFAULT 0                   COMMENT '玩家服号',
    `pkg_ver`             INT         NOT NULL DEFAULT 0                   COMMENT '包版本',
    `res_ver`             INT         NOT NULL DEFAULT 0                   COMMENT '资源版本',
    `msg`                 VARCHAR(2048) NOT NULL DEFAULT ''                COMMENT '报错信息',
    `brief`               VARCHAR(128)  NOT NULL DEFAULT ''                COMMENT '报错精简信息',
    `device_id`           VARCHAR(128) NOT NULL DEFAULT ''                 COMMENT '设备ID',

    INDEX(`stat_time`, `error_type`, `platform`),
    INDEX(`stat_time`, `brief`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='客户端错误账单';

CREATE TABLE IF NOT EXISTS `bill_mail` (
    `stat_time`           TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP   COMMENT '账单时间戳',
    `bill_ration`         VARCHAR(64) NOT NULL                             COMMENT '账单号',
    `platform`            VARCHAR(32) NOT NULL                             COMMENT '平台',
    `channel`             INT         NOT NULL                             COMMENT '渠道',
    `uin`                 INT         NOT NULL                             COMMENT '玩家UIN',
    `group_id`            INT         NOT NULL                             COMMENT '玩家服号',
    `server_id`           INT         NOT NULL                             COMMENT '服务器编号',
	`mail_handle_type`	  INT		  NOT NULL							   COMMENT '邮件操作类型(1,新邮件 2,提取附件后删除 3,超时删除)',
    `cmd_name`            VARCHAR(99) NOT NULL DEFAULT ''                  COMMENT '消息事件',
    `mail_id`             INT         NOT NULL DEFAULT 0                   COMMENT '邮件ID',
    `read_time`           INT         NOT NULL DEFAULT 0                   COMMENT '阅读时间',
    `award_multiple` 	  INT 		  NOT NULL DEFAULT 1                   COMMENT '奖励倍数',
	`content`             VARCHAR(1024) NOT NULL DEFAULT ''  			   COMMENT '邮件正文',
	`create_time` 		  INT 		  NOT NULL DEFAULT 0 				   COMMENT '创建时间',
	`sender` 			  VARCHAR(64) NOT NULL DEFAULT '' 				   COMMENT '邮件发送者',
	`title` 			  VARCHAR(64) NOT NULL DEFAULT '' 				   COMMENT '邮件标题',
	`attachment` 		  VARCHAR(256) NOT NULL DEFAULT '' 			   	   COMMENT '邮件附件',

    INDEX `stat_time`(`stat_time`),
    INDEX `uid`(`stat_time`, `platform`, `uin`, `group_id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='带附件的历史邮件';

