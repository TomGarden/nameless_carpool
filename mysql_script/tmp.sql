
-- 更新表的某一列列名
-- ALTER TABLE nameless_carpool.telephone RENAME COLUMN vertify_code TO verify_code;

ALTER TABLE nameless_carpool.user_tel MODIFY COLUMN vertify_code TO verify_code;



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












