# sa
stone age

两个编译好了，搞半天gmsv还是跑不了。

贴一些网上找的

1.sql
```
CREATE TABLE IF NOT EXISTS `logindata` (
  `name` varchar(16) NOT NULL,
  `password` varchar(16) NOT NULL,
  `email` varchar(32) NOT NULL,
  `sex` varchar(32) NOT NULL,
  `account_id` varchar(32) NOT NULL,
  `IP` varchar(16) NOT NULL default '-',
  `RegTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `LoginTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `OnlineName` varchar(30) default '',
  `Online` int(11) default '0',
  `Path` varchar(10) default '',
  `VipPoint` int(11) default '0',
  `level` varchar(32) NOT NULL,
  PRIMARY KEY  (`Name`),
  KEY `Name` (`Name`)
)

CREATE TABLE IF NOT EXISTS `LOCK` (
  `name` varchar(16) NOT NULL,
  PRIMARY KEY  (`name`),
  FULLTEXT KEY `name` (`name`)
);
输入测试帐号：
INSERT INTO `logindata` (`name`, `password`, `email`, `sex`, `account_id`, `IP`, `RegTime`, `LoginTime`, `OnlineName`, `Online`, `Path`, `VipPoint`, `level`) VALUES ('test', '123456', 'root@localhost'
, '', '', '-', '0000-00-00 00:00:00', '0000-00-00 00:00:00', '', 0, '', 0, 1);

```

2. gmsv下的makefile，把第一行的export去掉了，不然make不了。