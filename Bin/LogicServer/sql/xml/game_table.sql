set names utf8;

CREATE TABLE IF NOT EXISTS table_info_t
(
	table_name char(48) NOT NULL,
	table_div int(10) NOT NULL,
	table_info longblob,
	PRIMARY KEY(table_name)
) ENGINE = InnoDB, CHARSET = utf8;

CREATE TABLE IF NOT EXISTS `bill_recharge_t` (
	`stat_time` 	INT ( 10 ) 		NOT NULL 			COMMENT '账单时间戳',
	`uid` 			INT ( 10 ) 		NOT NULL 			COMMENT '玩家uid',
	`group_id` 		INT ( 10 ) 		NOT NULL 			COMMENT '玩家区服',
	`server_id` 	INT ( 10 ) 		NOT NULL 			COMMENT '服务器编号',
	`channel_id` 	INT ( 10 ) 		NOT NULL 			COMMENT '渠道ID',
	`nick` 			VARCHAR ( 32 ) 	NOT NULL 			COMMENT '玩家昵称',
	`order_id` 		VARCHAR ( 64 ) 	NOT NULL 			COMMENT '平台方账单ID',
	`bill_tag` 		VARCHAR ( 50 ) 	NOT NULL 			COMMENT '游戏方账单标识',
	`pay_cash` 		DECIMAL (15,2)  NOT NULL DEFAULT '0.0000' COMMENT '充值的现金',
	`pay_gamecoin` 	INT ( 10 ) 		NOT NULL DEFAULT 0 	COMMENT '充值的游戏币',
	`pay_item`	 	INT ( 10 ) 		NOT NULL DEFAULT 0 	COMMENT '充值的道具ID',
	`pay_type` 		INT ( 10 ) 		NOT NULL DEFAULT 0 	COMMENT '充值的类型',
	`pay_method` 	VARCHAR ( 50 ) 	NOT NULL DEFAULT 0 	COMMENT '支付方式',
	`pay_time` 		INT ( 10 ) 		NOT NULL DEFAULT 0 	COMMENT '支付时间',
	`is_test_order` INT ( 10 ) 		NOT NULL DEFAULT 0 	COMMENT '是否测试订单',
	`additional` 	VARCHAR ( 255 ) NOT NULL DEFAULT '' COMMENT '额外数据',
	
	PRIMARY KEY ( `stat_time`, `uid`, `group_id` ),
	UNIQUE KEY `order_id` ( `order_id` ),
	KEY `uid` ( `uid` ) 
) ENGINE = INNODB DEFAULT CHARSET = utf8;