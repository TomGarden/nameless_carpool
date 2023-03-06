
/* 说明信息 ：
    在数据库中查看一个表的创建信息 ： SHOW CREATE TABLE db_name.table_name \G 
    删除数据库                    mysql> DROP DATABASE IF EXISTS `nameless_carpool`;

    # 删除一直无法完成可以尝试先杀死对 mysql 的使用进程
    # https://dba.stackexchange.com/a/37646
    $ mysqladmin processlist -u root -p
    $ mysqladmin kill 174 -u root -p

   xxx_time       
   xxx_time_tz
   例如在 "Asia/Shanghai" 时区 , 的时间为 2022-12-13 10:25:26 对应的时间戳为 1670927126 (称为上海时间戳)
   同一时刻在 "UTC-0"      时区 , 的时间为 2022-12-13 02:25:26 对应的时间戳为  比上面的小  (称为 UTC 时间戳)
   
   此时 , xxx_time      表示 上海时间戳
         xxx_time_tz   表示  "Asia/Shanghai"

*/


-- 创建数据库
CREATE DATABASE IF NOT EXISTS `nameless_carpool` ;

-- 使用数据库 
USE `nameless_carpool` ;

CREATE TABLE IF NOT EXISTS `user` (
  `id`                  INTEGER           NOT NULL  AUTO_INCREMENT  COMMENT '主键 id 自增',
  `id_card_num`         VARCHAR(255)          NULL                  COMMENT '身份证号',
  `name`                VARCHAR(255)          NULL                  COMMENT '姓名',
  `gender`              TINYINT(1)            NULL                  COMMENT '性别 : 0女 ; 1男',
  `birth_date`          DATETIME(6)           NULL                  COMMENT '生日',
  `birth_date_tz`       VARCHAR(255)          NULL                  COMMENT '时区',
  `create_time`         DATETIME(6)       NOT NULL                  COMMENT '注册时间',
  `create_time_tz`      VARCHAR(255)      NOT NULL                  COMMENT '时区',
  `update_time`         DATETIME(6)       NOT NULL                  COMMENT '更新时间',
  `update_time_tz`      VARCHAR(255)      NOT NULL                  COMMENT '时区',
  `del_time`            DATETIME(6)           NULL                  COMMENT '注销时间',
  `del_time_tz`         VARCHAR(255)          NULL                  COMMENT '时区',

  PRIMARY KEY (`id`)
) COMMENT '用户基础信息';

-- INSERT INTO nameless_carpool.user
--           (name,     gender,    birth_date,             birth_date_tz,    register_time,          register_time_tz,   update_time,           update_time_tz)
-- VALUES ROW('姓名',     1,        '1990-09-01 11:23:00',   8,               '2022-07-01 16:32:00',   8,                 '2022-07-01 16:32:00',  8             );


CREATE TABLE IF NOT EXISTS `wc_user` (
  `id`                  VARCHAR(255)        NOT NULL                COMMENT '主键 id',
  `user_id`             INTEGER             NOT NULL                COMMENT '外键:用户基础信息 id',
  `wc_number`           VARCHAR(255)            NULL                COMMENT '微信号',
  `create_time`         DATETIME(6)         NOT NULL                COMMENT '创建时间',
  `create_time_tz`      VARCHAR(255)        NOT NULL                COMMENT '时区',
  `update_time`         DATETIME(6)         NOT NULL                COMMENT '更新时间',
  `update_time_tz`      VARCHAR(255)        NOT NULL                COMMENT '时区',
  `del_time`            DATETIME(6)             NULL                COMMENT '删除时间',
  `del_time_tz`         VARCHAR(255)            NULL                COMMENT '时区',

  FOREIGN KEY (`user_id`)   REFERENCES `user`(id)      ON UPDATE CASCADE      ON DELETE RESTRICT  ,
  PRIMARY KEY (`id`)
) COMMENT '微信用户表';



/*--------------------------------------------------------------------------------*/




CREATE TABLE IF NOT EXISTS `telephone` (
  `id`                  INTEGER             NOT NULL  AUTO_INCREMENT                COMMENT '主键 id 自增',
  `number`              VARCHAR(99)             NULL                                COMMENT '手机号',
  `vertify_code`        VARCHAR(20)             NULL                                COMMENT '验证码',
  `vc_update_time`      DATETIME(6)             NULL                                COMMENT '验证码更新时间',
  `vc_update_time_tz`   VARCHAR(255)            NULL                                COMMENT '时区',
  
  `create_time`         DATETIME(6)         NOT NULL                                COMMENT '创建时间',
  `create_time_tz`      VARCHAR(255)        NOT NULL                                COMMENT '时区',
  `update_time`         DATETIME(6)         NOT NULL                                COMMENT '更新时间',
  `update_time_tz`      VARCHAR(255)        NOT NULL                                COMMENT '时区',
  `del_time`            DATETIME(6)             NULL                                COMMENT '删除时间',
  `del_time_tz`         VARCHAR(255)            NULL                                COMMENT '时区',

  PRIMARY KEY (`id`)
) COMMENT '手机号';
/* INSERT INTO `nameless_carpool`.`telephone` (
                  `id`                  ,
                  `number`              ,
                  `vertify_code`        ,
                  `vc_update_time`      ,
                  `vc_update_time_tz`   ,
                  `create_time`         ,
                  `create_time_tz`      ,
                  `update_time`         ,
                  `update_time_tz`      ,
                  `del_time`            ,
                  `del_time_tz`         )
            VALUES (
                  NULL            ,
                  '17611261931'   ,
                  'vertify_code'  ,
                  '1663578159'    ,
                  '8'             ,
                  '1663578159'    ,
                  '8'             ,
                  '1663578159'    ,
                  '8'             ,
                  NULL            ,
                  NULL            )
                  ; */


CREATE TABLE IF NOT EXISTS `user_tel` (
  `user_id`             INTEGER             NOT NULL                                COMMENT '外键:用户基础信息 id',
  `telephone_id`        INTEGER             NOT NULL                                COMMENT '外键 手机号 id ',
  `desc`                VARCHAR(255)            NULL                                COMMENT '描述信息',
  `flag`                VARCHAR(8)              NULL                                COMMENT '标识符 , 8 位二进制数 , 不同的用户对同一个手机号的标示可能是不同的
                                                                                              0000 0001 第一位: 表示可用于登录 , 
                                                                                              0000 0010 第二位: 表示在自己的数据中 , 这个号码作为联系人出现过
                                                                                                        其他位: 待用 , 有需要时再填充
                                                                                              ',

  `create_time`         DATETIME(6)         NOT NULL                                COMMENT '创建时间',
  `create_time_tz`      VARCHAR(255)        NOT NULL                                COMMENT '时区',
  `update_time`         DATETIME(6)         NOT NULL                                COMMENT '更新时间',
  `update_time_tz`      VARCHAR(255)        NOT NULL                                COMMENT '时区',
  `del_time`            DATETIME(6)             NULL                                COMMENT '删除时间',
  `del_time_tz`         VARCHAR(255)            NULL                                COMMENT '时区',

  FOREIGN KEY (`user_id`)       REFERENCES `user`(`id`)       ON UPDATE CASCADE  ON DELETE RESTRICT ,
  FOREIGN KEY (`telephone_id`)  REFERENCES `telephone`(`id`)  ON UPDATE CASCADE  ON DELETE RESTRICT ,
  PRIMARY KEY (`user_id` , `telephone_id`)
) COMMENT '关联表 : 用户 & 手机号';



/*--------------------------------------------------------------------------------*/





CREATE TABLE IF NOT EXISTS `car` (
  `id`                        INTEGER             NOT NULL  AUTO_INCREMENT  COMMENT '主键 id 自增',
  `plate`                     VARCHAR(255)            NULL                  COMMENT '车牌号',
  `brand`                     VARCHAR(255)            NULL                  COMMENT '品牌',
  `model`                     VARCHAR(255)            NULL                  COMMENT '型号',
  `age`                       VARCHAR(255)            NULL                  COMMENT '车龄',
  `passenger_capacity`        INTEGER             NOT NULL                  COMMENT '客容量(不含司机)',
  `freight_capacity`          INTEGER             NOT NULL                  COMMENT '货容量',
  `freight_capacity_unity`    VARCHAR(255)        NOT NULL                  COMMENT '货容量描述单位',

  `create_time`               DATETIME(6)         NOT NULL                  COMMENT '创建时间',
  `create_time_tz`            VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `update_time`               DATETIME(6)         NOT NULL                  COMMENT '更新时间',
  `update_time_tz`            VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `del_time`                  DATETIME(6)             NULL                  COMMENT '删除时间',
  `del_time_tz`               VARCHAR(255)            NULL                  COMMENT '时区',

  PRIMARY KEY (`id`)
) COMMENT '汽车';


CREATE TABLE IF NOT EXISTS `user_car` (
  `user_id`             INTEGER             NOT NULL                COMMENT '外键:用户基础信息 id',
  `car_id`              INTEGER             NOT NULL                COMMENT '外键 手机号 id ',

  FOREIGN KEY (`user_id`)       REFERENCES `user`(`id`)       ON UPDATE CASCADE  ON DELETE RESTRICT ,
  FOREIGN KEY (`car_id`)        REFERENCES `car`(`id`)  ON UPDATE CASCADE  ON DELETE RESTRICT ,
  PRIMARY KEY (`user_id` , `car_id`)
) COMMENT '关联表 : 用户 & 汽车';



/*--------------------------------------------------------------------------------*/





CREATE TABLE IF NOT EXISTS `area` (
  `id`                        INTEGER             NOT NULL  AUTO_INCREMENT  COMMENT '主键 id 自增',
  `fullname`                  VARCHAR(255)            NULL                  COMMENT '中文全名',
  `pinyin`                    VARCHAR(255)            NULL                  COMMENT '拼音全名',
  `longitude`                 DECIMAL(18,15)          NULL                  COMMENT '经度(数字最长 18 位, 小数点后最多15 位)',
  `latitude`                  DECIMAL(18,15)          NULL                  COMMENT '纬度(数字最长 18 位, 小数点后最多15 位)',
  `cidx`                      JSON                    NULL                  COMMENT '下级地区 id 数组',
  `pid`                       INTEGER                 NULL                  COMMENT '上级地区 id ',
  `deep`                      INTEGER             NOT NULL                  COMMENT '深度,行政级别0:省,1:市,2:区,3:镇,4:村  需要进一步分析"街道级别',
  
  `create_time`               DATETIME(6)         NOT NULL                  COMMENT '创建时间',
  `create_time_tz`            VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `update_time`               DATETIME(6)         NOT NULL                  COMMENT '更新时间',
  `update_time_tz`            VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `del_time`                  DATETIME(6)             NULL                  COMMENT '删除时间',
  `del_time_tz`               VARCHAR(255)            NULL                  COMMENT '时区',

  PRIMARY KEY (`id`)
) COMMENT '地区';


CREATE TABLE IF NOT EXISTS `user_area` (
  `user_id`                   INTEGER             NOT NULL                  COMMENT '外键, 用户 ID',
  `area_id`                   INTEGER             NOT NULL                  COMMENT '外键, 地区 id',
  `history_areas`             JSON                    NULL                  COMMENT '用户历史地区们, json 数组',
  
  `create_time`               DATETIME(6)         NOT NULL                  COMMENT '创建时间',
  `create_time_tz`            VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `update_time`               DATETIME(6)         NOT NULL                  COMMENT '更新时间',
  `update_time_tz`            VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `del_time`                  DATETIME(6)             NULL                  COMMENT '删除时间',
  `del_time_tz`               VARCHAR(255)            NULL                  COMMENT '时区',

  FOREIGN KEY (`user_id`)       REFERENCES `user`(`id`)  ON UPDATE CASCADE  ON DELETE RESTRICT ,
  FOREIGN KEY (`area_id`)       REFERENCES `area`(`id`)  ON UPDATE CASCADE  ON DELETE RESTRICT ,
  PRIMARY KEY (`user_id` , `area_id`)
) COMMENT '关联表 : 用户 & 地区';


/*--------------------------------------------------------------------------------*/


CREATE TABLE IF NOT EXISTS `goods_info` (
  `id`                            INTEGER             NOT NULL  AUTO_INCREMENT  COMMENT '主键 id 自增',
  `desc`                          VARCHAR(255)            NULL                  COMMENT '货物描述信息',
  `weight`                        DECIMAL(6,1)            NULL                  COMMENT '重量, 单位(千克) , 一共6 位, 小数点后1 位',
  `volume`                        DECIMAL(6,1)            NULL                  COMMENT '体积',
  `volume_unity`                  VARCHAR(10)             NULL                  COMMENT '体积单位',

  `create_time`                   DATETIME(6)         NOT NULL                  COMMENT '创建时间',
  `create_time_tz`                VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `update_time`                   DATETIME(6)         NOT NULL                  COMMENT '更新时间',
  `update_time_tz`                VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `del_time`                      DATETIME(6)             NULL                  COMMENT '删除时间',
  `del_time_tz`                   VARCHAR(255)            NULL                  COMMENT '时区',

  PRIMARY KEY (`id`)
) COMMENT '货物信息描述';


CREATE TABLE IF NOT EXISTS `find_car` (
  `id`                            INTEGER             NOT NULL  AUTO_INCREMENT  COMMENT '主键 id 自增',
  `start_point_longitude`         DECIMAL(18,15)          NULL                  COMMENT '出发点 经度',
  `start_point_latitude`          DECIMAL(18,15)          NULL                  COMMENT '出发点 纬度',
  `start_point_area_id`           INTEGER                 NULL                  COMMENT '出发点地区 id ',
  `end_point_longitude`           DECIMAL(18,15)          NULL                  COMMENT '终点 经度',
  `end_point_latitude`            DECIMAL(18,15)          NULL                  COMMENT '终点 纬度',
  `end_point_area_id`             INTEGER                 NULL                  COMMENT '终点地区 id ',
  `way_of_using_car`              VARCHAR(255)        NOT NULL                  COMMENT '用车形式 拼车(carpool), 单程包车(one_way_charter), 全程包车(full_charter)',
  `departure_time_range_start`    DATETIME(6)         NOT NULL                  COMMENT '发车时间范围起点',
  `departure_time_range_end`      DATETIME(6)         NOT NULL                  COMMENT '发车时间范围终点',
  `pick_up_point`                 TINYINT             NOT NULL                  COMMENT '接送点 附近(nearby), 上门(door to door)',
  `people_number`                 TINYINT             NOT NULL                  COMMENT '跟车人数',
  `goods_info_id`                 INTEGER                 NULL                  COMMENT '外键 : 货物信息 ID',
  `append_info`                   VARCHAR(255)            NULL                  COMMENT '附加信息 , 最多 255 个字 ',

  `create_time`                   DATETIME(6)         NOT NULL                  COMMENT '创建时间',
  `create_time_tz`                VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `update_time`                   DATETIME(6)         NOT NULL                  COMMENT '更新时间',
  `update_time_tz`                VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `del_time`                      DATETIME(6)             NULL                  COMMENT '删除时间',
  `del_time_tz`                   VARCHAR(255)            NULL                  COMMENT '时区',

  FOREIGN KEY (`start_point_area_id`)       REFERENCES `area`(`id`)        ON UPDATE CASCADE  ON DELETE RESTRICT ,
  FOREIGN KEY (`end_point_area_id`)         REFERENCES `area`(`id`)        ON UPDATE CASCADE  ON DELETE RESTRICT ,
  FOREIGN KEY (`goods_info_id`)             REFERENCES `goods_info`(`id`)  ON UPDATE CASCADE  ON DELETE RESTRICT ,
  PRIMARY KEY (`id`)
) COMMENT '人找车表单';


CREATE TABLE IF NOT EXISTS `find_customers` (
  `id`                            INTEGER             NOT NULL  AUTO_INCREMENT  COMMENT '主键 id 自增',
  `start_points_area_list`        JSON                    NULL                  COMMENT '出发点 jons 数组',
  `end_points_area_list`          JSON                    NULL                  COMMENT '到达点 json 数组 ',
  `car_supply_form`               VARCHAR(255)        NOT NULL                  COMMENT '供车形式 拼车(carpool), 单程包车(one_way_charter), 全程包车(full_charter)',
  `departure_time_range_start`    DATETIME(6)         NOT NULL                  COMMENT '发车时间范围起点',
  `departure_time_range_end`      DATETIME(6)         NOT NULL                  COMMENT '发车时间范围终点',
  `pick_up_point`                 TINYINT             NOT NULL                  COMMENT '接送点 附近(nearby), 上门(door to door)',
  `people_number`                 TINYINT             NOT NULL                  COMMENT '客容量',
  `goods_info_id`                 INTEGER                 NULL                  COMMENT '货容量 -> 外键 : 货物信息 ID',
  `car_id`                        INTEGER             NOT NULL                  COMMENT '车辆信息',
  `append_info`                   VARCHAR(255)            NULL                  COMMENT '附加信息 , 最多 255 个字 ',

  `create_time`                   DATETIME(6)         NOT NULL                  COMMENT '创建时间',
  `create_time_tz`                VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `update_time`                   DATETIME(6)         NOT NULL                  COMMENT '更新时间',
  `update_time_tz`                VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `del_time`                      DATETIME(6)             NULL                  COMMENT '删除时间',
  `del_time_tz`                   VARCHAR(255)            NULL                  COMMENT '时区',

  FOREIGN KEY (`goods_info_id`)             REFERENCES `goods_info`(`id`)  ON UPDATE CASCADE  ON DELETE RESTRICT ,
  FOREIGN KEY (`car_id`)                    REFERENCES `car`(`id`)        ON UPDATE CASCADE  ON DELETE RESTRICT ,
  PRIMARY KEY (`id`)
) COMMENT '车找人表单';


CREATE TABLE IF NOT EXISTS `car_bind_customers` (
  `find_car_id`                   INTEGER             NOT NULL                  COMMENT '既是主键,又是外键',
  `find_customers_id`             INTEGER             NOT NULL                  COMMENT '既是主键,又是外键',
  `car_request`                   TINYINT                 NULL                  COMMENT '车主发起绑定',
  `car_response`                  TINYINT                 NULL                  COMMENT '车主相应绑定',
  `customers_request`             TINYINT                 NULL                  COMMENT '乘客发起绑定',
  `customers_response`            TINYINT                 NULL                  COMMENT '乘客相应绑定',

  `create_time`                   DATETIME(6)         NOT NULL                  COMMENT '创建时间',
  `create_time_tz`                VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `update_time`                   DATETIME(6)         NOT NULL                  COMMENT '更新时间',
  `update_time_tz`                VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `del_time`                      DATETIME(6)             NULL                  COMMENT '删除时间',
  `del_time_tz`                   VARCHAR(255)            NULL                  COMMENT '时区',

  FOREIGN KEY (`find_car_id`)             REFERENCES `find_car`(`id`)              ON UPDATE CASCADE  ON DELETE RESTRICT ,
  FOREIGN KEY (`find_customers_id`)       REFERENCES `find_customers`(`id`)        ON UPDATE CASCADE  ON DELETE RESTRICT ,
  PRIMARY KEY (`find_car_id` , `find_customers_id`)
) COMMENT '人车绑定表单';


CREATE TABLE IF NOT EXISTS `session` (
  `id`                            INTEGER             NOT NULL  AUTO_INCREMENT  COMMENT '主键 id 自增',
  `client_more_info`              VARCHAR(255)            NULL                  COMMENT '更多设备信息描述',
  `client_type`                   VARCHAR(255)        NOT NULL                  COMMENT '客户端类型',
  `token`                         VARCHAR(128)        NOT NULL                  COMMENT 'token 登录成功生成',
  `max_age`                       INTEGER             NOT NULL                  COMMENT '单位 秒 ; 失效时间 , 同一时区登录时间 + 失效持续描述 与当前时间做对比',
  `from_where`                    VARCHAR(128)            NULL                  COMMENT '从哪里来到网站登录的 , 不确定能否采集到此信息',
  `auth_method`                   VARCHAR(128)            NULL                  COMMENT '登录获取此 session 的验证方式 , 比如手机验证码 , 或者微信授权 , 或者其他',
  `token_update_time`             DATETIME(6)         NOT NULL                  COMMENT 'token 更新时间',
  `token_update_time_tz`          VARCHAR(255)        NOT NULL                  COMMENT 'token 更新时区',

  `create_time`                   DATETIME(6)         NOT NULL                  COMMENT '创建时间',
  `create_time_tz`                VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `update_time`                   DATETIME(6)         NOT NULL                  COMMENT '更新时间',
  `update_time_tz`                VARCHAR(255)        NOT NULL                  COMMENT '时区',
  `del_time`                      DATETIME(6)             NULL                  COMMENT '删除时间',
  `del_time_tz`                   VARCHAR(255)            NULL                  COMMENT '时区',

  PRIMARY KEY (`id`) ,
  INDEX `token` (`token`)
) COMMENT '登录会话';

CREATE TABLE IF NOT EXISTS `user_session` (
  `user_id`                     INTEGER             NOT NULL                                COMMENT '外键:用户基础信息 id',
  `session_id`                  INTEGER             NOT NULL                                COMMENT '外键 会话 id ',

  `create_time`                 DATETIME(6)         NOT NULL                                COMMENT '创建时间',
  `create_time_tz`              VARCHAR(255)        NOT NULL                                COMMENT '时区',
  `update_time`                 DATETIME(6)         NOT NULL                                COMMENT '更新时间',
  `update_time_tz`              VARCHAR(255)        NOT NULL                                COMMENT '时区',
  `del_time`                    DATETIME(6)             NULL                                COMMENT '删除时间',
  `del_time_tz`                 VARCHAR(255)            NULL                                COMMENT '时区',

  FOREIGN KEY (`user_id`)               REFERENCES `user`(`id`)               ON UPDATE CASCADE  ON DELETE RESTRICT ,
  FOREIGN KEY (`session_id`)            REFERENCES `session`(`id`)            ON UPDATE CASCADE  ON DELETE RESTRICT ,
  PRIMARY KEY (`user_id` , `session_id`)
) COMMENT '关联表 : 用户 & 登录会话';