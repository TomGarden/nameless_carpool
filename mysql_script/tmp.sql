/* 时间转换 */
-- SELECT * , DATE_FORMAT(`vc_update_time`,'%Y-%m-%d %H:%i:%S.%f') as `vc_update_time`
-- FROM   `nameless_carpool`.`telephone` WHERE  `nameless_carpool`.`telephone`.`number` = '15912341234' ;


/* SELECT `vc_update_time` , CAST(vc_update_time `nameless_carpool`.`telephone`.`vc_update_time` TIME ZONE INTERVAL '+08:00' AS DATETIME)
FROM   `nameless_carpool`.`telephone` ; */

/* SELECT * , CAST(`vc_update_time` AT TIME ZONE INTERVAL '+02:00' AS DATETIME) AS VUT
FROM   `nameless_carpool`.`telephone`  WHERE  `telephone`.`number` = '15912341234';  */



/* INSERT LOW_PRIORITY INTO `nameless_carpool`.`telephone` ( 
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
--                   `vertify_code`        ,
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
--                         'vertify_code'  ,
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

UPDATE `nameless_carpool`.`telephone`                                                             
SET                                                                
       `number`              = CASE `id`                                                            
                               WHEN '1' THEN '15111111111'                                                            
                               WHEN '2' THEN '15122222222'                                                            
                               END,                                                            
       `vertify_code`        = CASE `id`                                                            
                               WHEN '1' THEN 'vvv11111111'                                                            
                               WHEN '2' THEN 'vvv22222222'                                                            
                               END                                                            
WHERE  `id`                  IN ('1', '2') ;


