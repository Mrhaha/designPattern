SET NAMES utf8;

create table auth_info_TABLENO (
  `uin` int(11) not null default 0,
  `channel_id` int(11) not null default 1,
  `group_id` int(11) not null default 1,
  `platform` char(64) not null default '',
  `account` char(64) not null default '',
  `sdk` char(64) not null default '',
  `login_time` int(11) not null default 0,
  `create_time` int(11) not null default 0,
  primary key (`uin`),
  index idx_account (`account`),
  index idx_create (`create_time`)
) engine=innodb default charset=utf8;
