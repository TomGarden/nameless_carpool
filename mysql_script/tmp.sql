
SELECT @@GLOBAL.time_zone, @@SESSION.time_zone;
SELECT UNIX_TIMESTAMP('2007-03-11 2:00:00') - UNIX_TIMESTAMP('2007-03-11 3:00:00');
SELECT
  CONVERT_TZ('2007-03-11 2:00:00','US/Eastern','US/Central') AS time1,
  CONVERT_TZ('2007-03-11 3:00:00','US/Eastern','US/Central') AS time2,
  CONVERT_TZ('2023-04-12 00:17:57','America/Los_Angeles','Asia/Shanghai') AS time3;

DROP FUNCTION IF EXISTS RADIAN_TO_ANGLE_DEGREE;
DROP FUNCTION IF EXISTS COORDINATE_DISTANCE_METER;
DROP FUNCTION IF EXISTS COORDINATE_DISTANCE;
DROP FUNCTION IF EXISTS TEST_ADD_FUN; 
DROP FUNCTION IF EXISTS TOM_TEST_TEN_DIVIDED_BY_THREE; 

/** 计算 两个经纬度之间的直线距离 , 测试函数创建 */

DELIMITER //

/** 将角度转换为弧度 */
CREATE FUNCTION IF NOT EXISTS RADIAN_TO_ANGLE_DEGREE(radian DECIMAL(18,15))
    RETURNS DECIMAL(31,30) DETERMINISTIC /* 因为入参是经纬度经纬度取值在 -180 ~ 180 之间 , 结果最多有一个整数位 */
    RETURN radian * PI() / 180;

/** 计算两个坐标点间距 */
CREATE FUNCTION IF NOT EXISTS COORDINATE_DISTANCE_METER(lng_one DECIMAL(18,15), lat_one DECIMAL(18,15),
														lng_two DECIMAL(18,15), lat_two DECIMAL(18,15) )
    RETURNS DECIMAL(38,30) DETERMINISTIC /* 之所以是 38,30 , 因为地球上最远的两个顶点的距离(单位米) ≈ 20037508(八位数)  */
		BEGIN
		    SET @rad_lng_one = RADIAN_TO_ANGLE_DEGREE(lng_one); /* 两个经度 转弧度 */
		    SET @rad_lng_two = RADIAN_TO_ANGLE_DEGREE(lng_two);
		    
		    SET @rad_lat_one = RADIAN_TO_ANGLE_DEGREE(lat_one); /* 两个纬度 转弧度*/
		    SET @rad_lat_two = RADIAN_TO_ANGLE_DEGREE(lat_two);
		    SET @rad_lng_diff = @rad_lng_one - @rad_lng_two;      /* 经 弧度 差值 */
		    SET @rad_lat_diff = @rad_lat_one - @rad_lat_two;      /* 纬 弧度 差值 */
            /* 两个经纬度坐标的直线距离 , 单位 米 */
		    RETURN 2 * 6378137 * ASIN(
                SQRT(
                    POW( SIN(@rad_lat_diff / 2) , 2 ) +
                    COS( @rad_lat_one ) * COS( @rad_lat_two ) * POW( SIN(@rad_lng_diff / 2) , 2 )
                    )
		        );
		END//
		
DELIMITER ;



SELECT COORDINATE_DISTANCE_METER(NULL , 90 , 39.959369 , 116.360280);
SELECT COORDINATE_DISTANCE_METER(123.123456789123456789 , 1234.123456789123456789 , 39.959369 , 116.360280);


SELECT COORDINATE_DISTANCE_METER(39.1,       116.2,     39.3,       116.4);
SELECT 39.1 * PI() / 180;
SELECT 116.2 * PI() / 180;
SELECT 39.3 * PI() / 180;
SELECT 116.4 * PI() / 180;
SELECT 114.0709395212723 * PI() / 180;

SELECT COORDINATE_DISTANCE_METER(39.966259 , 116.36824, 39.959369 , 116.360280);




SELECT COORDINATE_DISTANCE_METER(112.74732 , 37.69508, 112.747148 , 37.697135) AS one ,   /* 228.86	0.22886	0.00014 */
       COORDINATE_DISTANCE_METER(111.215876 , 28.381802,	111.215159 , 28.372567) AS two ,/* 1028.63	1.0286300000000002	0.00064 */
       COORDINATE_DISTANCE_METER(115 , 31 , 116 , 31) AS three ,			/* 95252.49	95.25249000000001	0.05919 */
       COORDINATE_DISTANCE_METER(112.73486 , 37.67425 , 112.735221 , 37.6739) AS four ,			/* 50.21	0.05021	0.00003 */
       COORDINATE_DISTANCE_METER(0 , 90 , 0 , -90) AS five,
       COORDINATE_DISTANCE_METER(180 , 90 , 180 , -90) AS six;


/*  一些测试语句 */
SELECT TIMESTAMPDIFF(SECOND,'2009-05-18 12:30:12','2023-05-18 12:30:11') > 0;



/* 计算两个时间段之间有多少重合的 秒 */
DELIMITER //
CREATE FUNCTION IF NOT EXISTS CALCULATE_TIME_PERIOD_OVERLAP(
        tpStartOne DATETIME(6), tpEndOne DATETIME(6), tzOne VARCHAR(255),
        tpStartTwo DATETIME(6), tpEndTwo DATETIME(6), tzTwo VARCHAR(255) )
    RETURNS BIGINT  /* 返回的以秒为单位的重复时间段 ,
                            - 可能正数表示重合 ,
                            - 可能负数表示距离 ,
                            - 可能 NULL 表示入参非法 */
    BEGIN
        IF (tpEndOne < tpStartOne) THEN RETURN NULL; END IF;
        IF (tpEndTwo < tpStartTwo) THEN RETURN NULL; END IF;

        SET @to_tz = "Asia/Shanghai";
        SET @start_one = UNIX_TIMESTAMP( CONVERT_TZ(tpStartOne, tzOne, @to_tz) );
        SET @end_one   = UNIX_TIMESTAMP( CONVERT_TZ(tpEndOne,   tzOne, @to_tz) );
        SET @start_two = UNIX_TIMESTAMP( CONVERT_TZ(tpStartTwo, tzTwo, @to_tz) );
        SET @end_two   = UNIX_TIMESTAMP( CONVERT_TZ(tpEndTwo,   tzTwo, @to_tz) );

        IF @start_one <= @start_two THEN
            IF @end_one <= @start_two THEN RETURN @end_one - @start_two;  /* 负值 , 表示两个时间段间隔的秒数 */
            ELSE                           RETURN @end_one - @start_two;  /* 正值 , 表示重合的秒数 */
            END IF;
        ELSE
            IF @start_one <= @end_two THEN RETURN @end_two - @start_one;  /* 负值 , 表示两个时间段间隔的秒数 */
            ELSE                           RETURN @end_two - @start_one;  /* 正值 , 表示重合的秒数 */
            END IF;
        END IF;
    END//
DELIMITER ;


/** 根据计算结果 出发点距离,到达点距离,时间重合秒数,服务方式匹配度 几个入参返回数字 ; 返回值越大说明匹配度越高 */
DELIMITER //

CREATE FUNCTION IF NOT EXISTS CALUCLATE_SEARCH_X_FIND_X_SORT_VALUE()


DELIMITER ;


/** 计算 两个经纬度之间的直线距离 , 测试函数创建

DELIMITER //
DROP FUNCTION TEST_ADD_FUN;
CREATE FUNCTION IF NOT EXISTS TEST_ADD_FUN(one DECIMAL(18,15), two DECIMAL(18,15))
	RETURNS DECIMAL(18,15) DETERMINISTIC
		BEGIN
			SET @result = one + two;
			RETURN @result;
		END//


CREATE FUNCTION IF NOT EXISTS COORDINATE_DISTANCE_METER(lng_one DECIMAL(18,15), lat_one DECIMAL(18,15),
														lng_two DECIMAL(18,15), lat_two DECIMAL(18,15) )
    RETURNS DECIMAL(18,15) DETERMINISTIC
			BEGIN
						SET @lon = TEST_ADD_FUN( lng_one , lng_two ) ; /* 巴拉巴拉 */
						SET @LAT = TEST_ADD_FUN( lat_one , lat_two ) ; /* 巴拉巴拉 */
						SET @RESULT = @lon + @LAT;
			RETURN @RESULT;
			END//

DELIMITER ;

SELECT COORDINATE_DISTANCE(1.1, 1.1, 2.2, 2.2);

*/

SELECT

SELECT
	store_id,
	lng,
	lat,
	ROUND(
		6378.138 * 2 * ASIN(
			SQRT(
				POW(
					SIN((
							22.299439 * PI()/ 180-lat * PI()/ 180 
							)/ 2 
					),
					2 
					)+ COS( 22.299439 * PI()/ 180 )* COS( lat * PI()/ 180 )* POW(
					SIN((
							114.173881 * PI()/ 180-lng * PI()/ 180 
							)/ 2 
					),
					2 
				)))* 1000 
	) AS juli 
FROM
	store_info 
ORDER BY
	juli DESC 
	LIMIT 316

SELECT * 
FROM `user` 
WHERE `user`.`id` IN (
	SELECT `user_session`.`user_id` 
	FROM `user_session` 
	WHERE `user_session`.`session_id` = '9'
);




INSERT INTO `nameless_carpool`.`goods_info` ( `id`, `size_length`, `size_width`, `size_height`, `size_unity`, `volume`,
                                             `volume_unit`, `weight`, `weight_unit`, `number`, `tag`, `desc`, `create_time`,
                                             `create_time_tz`, `update_time`, `update_time_tz`, `del_time`, `del_time_tz` )
VALUES ('1', '43', '31', '13', 'CM', '17.4',  'L', NULL, NULL, '1', '["可登机"]', '16吋行李箱', CURRENT_DATE(), 'Asia/Shanghai', CURRENT_DATE(), 'Asia/Shanghai', NULL, NULL),
       ('2', '44', '34', '20', 'CM', '30.0',  'L', NULL, NULL, '1', '["可登机"]', '18吋行李箱', CURRENT_DATE(), 'Asia/Shanghai', CURRENT_DATE(), 'Asia/Shanghai', NULL, NULL),
       ('3', '50', '34', '20', 'CM', '34.0',  'L', NULL, NULL, '1', '["可登机"]', '20吋行李箱', CURRENT_DATE(), 'Asia/Shanghai', CURRENT_DATE(), 'Asia/Shanghai', NULL, NULL),
       ('4', '58', '39', '24', 'CM', '54.3',  'L', NULL, NULL, '1', NULL,        '22吋行李箱', CURRENT_DATE(), 'Asia/Shanghai', CURRENT_DATE(), 'Asia/Shanghai', NULL, NULL),
       ('5', '68', '42', '26', 'CM', '74.3',  'L', NULL, NULL, '1', NULL,        '24吋行李箱', CURRENT_DATE(), 'Asia/Shanghai', CURRENT_DATE(), 'Asia/Shanghai', NULL, NULL),
       ('6', '67', '45', '28', 'CM', '84.5',  'L', NULL, NULL, '1', NULL,        '26吋行李箱', CURRENT_DATE(), 'Asia/Shanghai', CURRENT_DATE(), 'Asia/Shanghai', NULL, NULL),
       ('7', '78', '47', '28', 'CM', '102.7', 'L', NULL, NULL, '1', NULL,        '28吋行李箱', CURRENT_DATE(), 'Asia/Shanghai', CURRENT_DATE(), 'Asia/Shanghai', NULL, NULL),
       ('8', '88', '53', '30', 'CM', '140.0', 'L', NULL, NULL, '1', NULL,        '32吋行李箱', CURRENT_DATE(), 'Asia/Shanghai', CURRENT_DATE(), 'Asia/Shanghai', NULL, NULL)
ON DUPLICATE KEY UPDATE  `size_length`    = VALUES(`size_length`   ),
                         `size_width`     = VALUES(`size_width`    ),
                         `size_height`    = VALUES(`size_height`   ),
                         `size_unity`     = VALUES(`size_unity`    ),
                         `volume`         = VALUES(`volume`        ),
                         `volume_unit`    = VALUES(`volume_unit`   ),
                         `weight`         = VALUES(`weight`        ),
                         `weight_unit`    = VALUES(`weight_unit`   ),
                         `number`         = VALUES(`number`        ),
                         `tag`            = VALUES(`tag`           ),
                         `desc`           = VALUES(`desc`          ),
                         `create_time`    = VALUES(`create_time`   ),
                         `create_time_tz` = VALUES(`create_time_tz`),
                         `update_time`    = VALUES(`update_time`   ),
                         `update_time_tz` = VALUES(`update_time_tz`),
                         `del_time`       = VALUES(`del_time`      ),
                         `del_time_tz`    = VALUES(`del_time_tz`   );



SELECT `id`,`intent` FROM `nameless_carpool`.`find_customers`
ORDER BY CASE WHEN `id` >= 6 THEN `id` END ASC ,
         CASE WHEN `id` < 6  THEN `id` END DESC;



SELECT `id`,`intent` FROM `nameless_carpool`.`find_customers` ORDER BY  `intent`;

/* 预期按照给定的这个顺序排列 */
SELECT `intent` FROM `nameless_carpool`.`find_customers`
ORDER BY FIELD(`intent`, 'people', 'goods', 'one', 'two', 'three', 'four');

SELECT `intent` FROM `nameless_carpool`.`find_customers`
ORDER BY FIELD(`intent`, 'people');

SELECT `intent` FROM `nameless_carpool`.`find_customers`
ORDER BY FIELD(`intent`, 'people') DESC;

-- SELECT `intent` FROM `nameless_carpool`.`find_customers`
-- ORDER BY FIELD(`intent`, 'people') DESC, PRIORITY;

SELECT `intent` FROM `nameless_carpool`.`find_customers`
ORDER BY FIELD(`intent`, 'people') ASC;



INSERT INTO `nameless_carpool`.`find_customers` (
    `id` ,
    `start_point_longitude` ,
    `start_point_latitude` ,
    `start_point_json` ,
    `end_point_longitude` ,
    `end_point_latitude` ,
    `end_point_json` ,
    `service_form` ,
    `departure_time_range_start` ,
    `departure_time_range_end` ,
    `pick_up_point` ,
    `intent` ,
    `people_number` ,
    `goods_info_id` ,
    `append_info` ,
    `car_id` ,
    `create_time` ,
    `create_time_tz` ,
    `update_time` ,
    `update_time_tz` ,
    `del_time` ,
    `del_time_tz`
)
VALUES
    ( NULL ,NULL ,NULL ,'{"adcode":"130128","district":"1河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,NULL ,NULL ,'{"adcode":"130128","district":"河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,'full_charter' ,'2023-03-28 08:40:01' ,'2023-03-30 08:40:01' ,'nearby' ,'goods' ,'3' ,'1' ,'我是附加信息蹦巴拉' ,'2' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,NULL ,NULL ) ,
    ( NULL ,NULL ,NULL ,'{"adcode":"130128","district":"2河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,NULL ,NULL ,'{"adcode":"130128","district":"河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,'full_charter' ,'2023-03-28 08:40:01' ,'2023-03-30 08:40:01' ,'nearby' ,'goods' ,'3' ,'1' ,'我是附加信息蹦巴拉' ,'2' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,NULL ,NULL ) ,
    ( NULL ,NULL ,NULL ,'{"adcode":"130128","district":"3河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,NULL ,NULL ,'{"adcode":"130128","district":"河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,'full_charter' ,'2023-03-28 08:40:01' ,'2023-03-30 08:40:01' ,'nearby' ,'one' ,'3' ,'1' ,'我是附加信息蹦巴拉' ,'2' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,NULL ,NULL ) ,
    ( NULL ,NULL ,NULL ,'{"adcode":"130128","district":"4河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,NULL ,NULL ,'{"adcode":"130128","district":"河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,'full_charter' ,'2023-03-28 08:40:01' ,'2023-03-30 08:40:01' ,'nearby' ,'one' ,'3' ,'1' ,'我是附加信息蹦巴拉' ,'2' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,NULL ,NULL ) ,
    ( NULL ,NULL ,NULL ,'{"adcode":"130128","district":"5河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,NULL ,NULL ,'{"adcode":"130128","district":"河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,'full_charter' ,'2023-03-28 08:40:01' ,'2023-03-30 08:40:01' ,'nearby' ,'two' ,'3' ,'1' ,'我是附加信息蹦巴拉' ,'2' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,NULL ,NULL ) ,
    ( NULL ,NULL ,NULL ,'{"adcode":"130128","district":"6河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,NULL ,NULL ,'{"adcode":"130128","district":"河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,'full_charter' ,'2023-03-28 08:40:01' ,'2023-03-30 08:40:01' ,'nearby' ,'two' ,'3' ,'1' ,'我是附加信息蹦巴拉' ,'2' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,NULL ,NULL ) ,
    ( NULL ,NULL ,NULL ,'{"adcode":"130128","district":"7河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,NULL ,NULL ,'{"adcode":"130128","district":"河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,'full_charter' ,'2023-03-28 08:40:01' ,'2023-03-30 08:40:01' ,'nearby' ,'three' ,'3' ,'1' ,'我是附加信息蹦巴拉' ,'2' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,NULL ,NULL ) ,
    ( NULL ,NULL ,NULL ,'{"adcode":"130128","district":"8河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,NULL ,NULL ,'{"adcode":"130128","district":"河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,'full_charter' ,'2023-03-28 08:40:01' ,'2023-03-30 08:40:01' ,'nearby' ,'three' ,'3' ,'1' ,'我是附加信息蹦巴拉' ,'2' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,NULL ,NULL ) ,
    ( NULL ,NULL ,NULL ,'{"adcode":"130128","district":"9河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,NULL ,NULL ,'{"adcode":"130128","district":"河北省石家庄市深泽县","location":"115.259604,38.216147","name":"小直要村"}' ,'full_charter' ,'2023-03-28 08:40:01' ,'2023-03-30 08:40:01' ,'nearby' ,'four' ,'3' ,'1' ,'我是附加信息蹦巴拉' ,'2' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,'2023-04-09 14:33:23.546241' ,'Asia/Shanghai' ,NULL ,NULL ) ;


-- 更新表的某一列列名
-- ALTER TABLE nameless_carpool.telephone RENAME COLUMN vertify_code TO verify_code;

-- ALTER TABLE nameless_carpool.user_tel MODIFY COLUMN flag VARCHAR(20) NULL COMMENT '枚举 login,标记此手机号作为登录账号 ; connect,标记此手机号作为联系人号码 ';



-- START TRANSACTION ; 
--  INSERT INTO `nameless_carpool`.`user` ( 
--           `id` ,
--           `id_card_num` ,
--           `name` ,
--           `gender` ,
--           `birth_date` ,
--           `birth_date_tz` ,
--           `create_time` ,
--           `create_time_tz` ,
--           `update_time` ,
--           `update_time_tz` ,
--           `del_time` ,
--           `del_time_tz` 
--  )
--  VALUES 
--         (
--                  NULL ,
--                  NULL ,
--                  NULL ,
--                  NULL ,
--                  NULL ,
--                  NULL ,
--                  '2023-03-06 12:16:45.774661' ,
--                  'Asia/Shanghai' ,
--                  '2023-03-06 12:16:45.774661' ,
--                  'Asia/Shanghai' ,
--                  NULL ,
--                  NULL 
--         ) ;
--  SET @last_insert_user_id = (SELECT LAST_INSERT_ID());
--  INSERT INTO `nameless_carpool`.`user_tel` ( 
--           `user_id` ,
--           `telephone_id` ,
--           `desc` ,
--           `flag` ,
--           `create_time` ,
--           `create_time_tz` ,
--           `update_time` ,
--           `update_time_tz` ,
--           `del_time` ,
--           `del_time_tz` 
--  )
--  VALUES 
--         (
--                  @last_insert_user_id ,
--                  '1' ,
--                  NULL ,
--                  '00000001' ,
--                  '2023-03-06 12:16:45.774661' ,
--                  'Asia/Shanghai' ,
--                  '2023-03-06 12:16:45.774661' ,
--                  'Asia/Shanghai' ,
--                  NULL ,
--                  NULL 
--         ) ;
--  SELECT @last_insert_user_id ;
-- COMMIT ; 


/* 时间转换 */
-- SELECT * , DATE_FORMAT(`vc_update_time`,'%Y-%m-%d %H:%i:%S.%f') as `vc_update_time_123`
-- FROM   `nameless_carpool`.`telephone` WHERE  `nameless_carpool`.`telephone`.`number` = '15912341234' ;


/* SELECT `vc_update_time` , CAST(vc_update_time `nameless_carpool`.`telephone`.`vc_update_time` TIME ZONE INTERVAL '+08:00' AS DATETIME)
FROM   `nameless_carpool`.`telephone` ; */

/* SELECT * , CAST(`vc_update_time` AT TIME ZONE INTERVAL '+02:00' AS DATETIME) AS VUT
FROM   `nameless_carpool`.`telephone`  WHERE  `telephone`.`number` = '15912341234';  */



/* INSERT LOW_PRIORITY INTO `nameless_carpool`.`telephone` ( 
         `id`                  ,
         `number`              ,
         `verify_code`        ,
         `vc_update_time`      ,
         `vc_update_time_tz`   ,
         `create_time`         ,
         `create_time_tz`      ,
         `update_time`         ,
         `update_time_tz`      ,
         `del_time`            ,
         `del_time_tz`         )
 VALUES                         
 (  NULL , 
  '15912341234' , 
  '747775' , 
  '2022-09-25 17:57:22.978573' , 
  'Asia/Shanghai' , 
  '2022-09-25 17:57:22.978573' , 
  'Asia/Shanghai' , 
  '2022-09-25 17:57:22.978573' , 
  'Asia/Shanghai' , 
  NULL , 
  NULL  
 ) ; */

/* 用于存放单独的 sql 语句, 在 mysql 交互模式下 通过 source filePath 的形式进行交互 */


/* USE nameless_carpool

SELECT  *
FROM    `telephone`, `user_tel`
WHERE   `telephone`.`id` = `user_tel`.`telephone_id` AND  
        `telephone`.`number` = ``
        ; */


/* 仅查找手机号是否入库了 */
/* SELECT COUNT(*) 
FROM   `nameless_carpool`.`telephone`
WHERE  `telephone`.`number` = '--123123' ; */


-- INSERT INTO `nameless_carpool`.`telephone` (
--                   `id`                  ,
--                   `number`              ,
--                   `verify_code`        ,
--                   `vc_update_time`      ,
--                   `vc_update_time_tz`   ,
--                   `create_time`         ,
--                   `create_time_tz`      ,
--                   `update_time`         ,
--                   `update_time_tz`      ,
--                   `del_time`            ,
--                   `del_time_tz`         )
--             VALUES (
--                         NULL            ,
--                         '17611111111'   ,
--                         'verify_code'  ,
--                         '2022-09-22 19:28:00.123456'    ,
--                         'Asia/Shanghai'             ,
--                         '2022-09-22 19:28:00.123456'    ,
--                         'Asia/Shanghai'             ,
--                         '2022-09-22 19:28:00.123456'    ,
--                         'Asia/Shanghai'             ,
--                         NULL            ,
--                         NULL            ),
--                   (
--                         NULL            ,
--                         '17622222222'   ,
--                         '======'  ,
--                         '2022-09-22 19:59:35.123456'    ,
--                         'Asia/Shanghai'             ,
--                         '2022-09-22 19:59:35.123456'    ,
--                         'Asia/Shanghai'             ,
--                         '2022-09-22 19:59:35.123456'    ,
--                         'Asia/Shanghai'             ,
--                         NULL            ,
--                         NULL            )
--                         ;



/* SELECT `id` FROM `nameless_carpool`.`telephone` WHERE id = '1'; */

-- UPDATE `nameless_carpool`.`telephone`                                                             
-- SET                                                                
--        `number`              = CASE `id`                                                            
--                                WHEN '1' THEN '15111111111'                                                            
--                                WHEN '2' THEN '15122222222'                                                            
--                                END,                                                            
--        `verify_code`        = CASE `id`
--                                WHEN '1' THEN 'vvv11111111'                                                            
--                                WHEN '2' THEN 'vvv22222222'                                                            
--                                END                                                            
-- WHERE  `id`                  IN ('1', '2') ;

-- UPDATE `nameless_carpool`.`user_tel`                                                             
-- SET                                                                
--        `desc`              = CASE `user_id`                                           
--                                WHEN '1' THEN '15111111111'                                                            
--                                WHEN '2' THEN '15122222222'                                                            
--                                END,                                                            
--        `verify_code`        = CASE `id`
--                                WHEN '1' THEN 'vvv11111111'                                                            
--                                WHEN '2' THEN 'vvv22222222'                                                            
--                                END                                                            
-- WHERE  `id`                  IN ('1', '2') ;

-- UPDATE `nameless_carpool`.`user_tel`                                                             
-- SET                                                                
--        `desc`   = CASE                                    
--                     WHEN (`user_id`='1' AND `telephone_id`='1') THEN '15111111111'                                                               
--                     END,                                                          
-- WHERE  (user_tel.user_id, user_tel.telephone_id) IN ( (1, 1), (2, 2) );


-- -------------------------------------------------------------
-- 复合主键表的更新动作 , 成功
-- -------------------------------------------------------------
-- UPDATE `nameless_carpool`.`user_tel`                                                             
-- SET                                                                
--        `desc`   = CASE                                    
--                     WHEN (`user_id`='1' AND `telephone_id`='1') THEN '22222'                                                               
--                     END                                                          
-- WHERE  (`user_id`, `telephone_id`) IN ( ('1', '1') );

-- -------------------------------------------------------------
-- 复合主键表的更新动作 , 成功 ②
-- -------------------------------------------------------------
-- UPDATE `nameless_carpool`.`user_tel`                                                             
-- SET                                                                
--        `desc`   = CASE    CONCAT(`user_id` , `telephone_id`='1')                                
--                     WHEN  CONCAT('1' , '1') THEN '15111111111'                                                               
--                     END                                                     
-- WHERE  CONCAT(`user_id` , `telephone_id`='1') IN ( CONCAT('1' , '1') );

--  DELETE FROM `nameless_carpool`.`user_tel`
--  WHERE (`user_id`, `telephone_id`) IN ( ('2', '2') );

-- -------------------------------------------------------------
-- 复合主键表的更新删除动作
-- -------------------------------------------------------------

-- INSERT INTO `nameless_carpool`.`user_tel` ( `user_id`, 
--                                             `telephone_id`,
--                                             `desc`,
--                                             `flag`,
--                                             `create_time`,
--                                             `create_time_tz`,
--                                             `update_time`,
--                                             `update_time_tz`,
--                                             `del_time`,
--                                             `del_time_tz`
--                                           )  
-- VALUES (    '1',
--             '1',
--             'desc111111111',
--             'flag',
--             '2023-02-22 17:19:53.000000',
--             'shangHai',
--             '2023-02-22 17:19:53.000000',
--             'shangHai',
--             NULL,
--             NULL
--         )
-- ON DUPLICATE KEY UPDATE         `desc`           =VALUES(`desc`          ),
--                                 `flag`           =VALUES(`flag`          ),
--                                 `create_time`    =VALUES(`create_time`   ),
--                                 `create_time_tz` =VALUES(`create_time_tz`),
--                                 `update_time`    =VALUES(`update_time`   ),
--                                 `update_time_tz` =VALUES(`update_time_tz`),
--                                 `del_time`       =VALUES(`del_time`      ),
--                                 `del_time_tz`    =VALUES(`del_time_tz`   )
--                         ;


-- 这条语句报错 , 暂时舍弃它
-- UPDATE `nameless_carpool`.`user_tel`                                                        
-- SET                                                                
--        `desc`              = CASE (`user_id`, `telephone_id`)
--                                WHEN ('1', '1') THEN '这回是中文了'
--                                END                                               
-- WHERE  (`user_id`, `telephone_id`) IN ( ('1', '1') ) ;




-- 
-- DELETE FROM `nameless_carpool`.`user_tel`
-- WHERE (`user_id`, `telephone_id`) IN ( ('1', '1') );
-- 



-- START TRANSACTION;
-- 
-- INSERT INTO `nameless_carpool`.`user` ( id, id_card_num, `name`, gender, birth_date, birth_date_tz, create_time, create_time_tz, update_time, update_time_tz)
-- VALUE(NULL, 'id_card_name', 'name', (SELECT LAST_INSERT_ID()), NULL, Null , '2023-02-27 06:52:24', 'create_time_tz', '2023-02-27 06:52:24', 'update_time_tz');
-- 
-- SET @last_insert_user_id = (SELECT LAST_INSERT_ID());
-- 
-- INSERT INTO `nameless_carpool`.`user` ( id, id_card_num, `name`, gender, birth_date, birth_date_tz, create_time, create_time_tz, update_time, update_time_tz)
-- VALUE(NULL, 'id_card_name', 'name', @last_insert_user_id, NULL, Null , '2023-02-27 06:52:24', 'create_time_tz', '2023-02-27 06:52:24', 'update_time_tz');
-- 
-- SELECT	 @last_insert_user_id;
-- 
-- COMMIT;

-- SELECT *  From telephone WHERE id in (1 , 2 , 3) ;
-- SELECT *  From telephone WHERE id in (SELECT telephone.id FROM telephone WHERE id < 3) AND del_time IS NULL;



-- SELECT     `id` ,
--           `number` ,
--           `verify_code` ,
--           `vc_update_time` ,
--           `vc_update_time_tz` ,
--           `create_time` ,
--           `create_time_tz` ,
--           `update_time` ,
--           `update_time_tz` ,
--           `del_time` ,
--           `del_time_tz` 
--  FROM   `nameless_carpool`.`telephone` 
--  WHERE  `id`='15912341111'  AND  ( `del_time` IS NULL AND `del_time_tz` IS NULL ) ; 

-- SELECT   *
--  FROM   `nameless_carpool`.`telephone` 
--  WHERE  `number`='15912341111' AND  ( `del_time` IS NOT NULL AND `del_time_tz` IS NOT NULL )  ; 
-- 
-- 


-- START TRANSACTION ; 
--  INSERT INTO `nameless_carpool`.`user` ( 
--           `id` ,
--           `id_card_num` ,
--           `name` ,
--           `gender` ,
--           `birth_date` ,
--           `birth_date_tz` ,
--           `create_time` ,
--           `create_time_tz` ,
--           `update_time` ,
--           `update_time_tz` ,
--           `del_time` ,
--           `del_time_tz` 
--  )
--  VALUES 
--         (
--                  NULL ,
--                  NULL ,
--                  NULL ,
--                  NULL ,
--                  NULL ,
--                  NULL ,
--                  '2023-03-06 11:59:52.903733' ,
--                  'Asia/Shanghai' ,
--                  '2023-03-06 11:59:52.903733' ,
--                  'Asia/Shanghai' ,
--                  NULL ,
--                  NULL 
--         ) ;
-- SET @last_insert_user_id = (SELECT LAST_INSERT_ID());
--  INSERT INTO `nameless_carpool`.`user_tel` ( 
--           `user_id` ,
--           `telephone_id` ,
--           `desc` ,
--           `flag` ,
--           `create_time` ,
--           `create_time_tz` ,
--           `update_time` ,
--           `update_time_tz` ,
--           `del_time` ,
--           `del_time_tz` 
--  )
--  VALUES 
--         (
--                  @last_insert_user_id ,
--                  '1' ,
--                  NULL ,
--                  '00000001' ,
--                  '2023-03-06 11:59:52.903733' ,
--                  'Asia/Shanghai' ,
--                  '2023-03-06 11:59:52.903733' ,
--                  'Asia/Shanghai' ,
--                  NULL ,
--                  NULL 
--         ) ;
-- SELECT @last_insert_user_id ;
-- COMMIT ; 












