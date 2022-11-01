
/* 说明信息 ：
    在数据库中查看一个表的创建信息 ： SHOW CREATE TABLE db_name.table_name \G 
    删除数据库 
*/


-- 创建数据库
CREATE DATABASE IF NOT EXISTS `nameless_carpool` ;

-- 使用数据库 
USE `nameless_carpool` ;

CREATE TABLE IF NOT EXISTS `goods_info` (
  `id`                            INTEGER             4      NOT NULL  AUTO_INCREMENT  COMMENT '主键 id 自增',
  `desc`                          VARCHAR(16000)      16001          NULL               COMMENT '货物描述信息',
  `weight`                        DECIMAL(6,1)        4          NULL                  COMMENT '重量, 单位(千克) , 一共6 位, 小数点后1 位',
  `volume`                        DECIMAL(6,1)        4          NULL                  COMMENT '体积',
  `volume_unity`                  VARCHAR(10)         11          NULL                  COMMENT '体积单位',

  `create_time`                   DATETIME(6)         8      NOT NULL                  COMMENT '创建时间',
  `create_time_tz`                VARCHAR(255)        256      NOT NULL                  COMMENT '时区',
  `update_time`                   DATETIME(6)         8     NOT NULL                  COMMENT '更新时间',
  `update_time_tz`                VARCHAR(255)        256      NOT NULL                  COMMENT '时区',
  `del_time`                      DATETIME(6)         8         NULL                  COMMENT '删除时间',
  `del_time_tz`                   VARCHAR(255)        256          NULL                  COMMENT '时区',

  PRIMARY KEY (`id`)
) COMMENT '货物信息描述';