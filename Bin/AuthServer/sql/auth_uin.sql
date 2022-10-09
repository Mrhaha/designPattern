SET NAMES utf8;

create table uin (
  `id` int(11) not null default 1,
  `uin` int(11) not null default 1,
  primary key  (`id`)
) engine=innodb default charset=utf8;

insert into uin (id, uin) values(1,100000) on duplicate key update uin = 100000;