/* 我们把一些常见操作备份于此 , 方便后续使用 */

/* 根据手机号查询用户信息 */

SELECT  `telephone.id`, `telephone.number`, `telephone.desc`
FROM    `telephone`
WHERE   `telephone.number` = {}                   AND
        `telephone.for_login` = true

SELECT  `user.*`, 
FROM    `user`, `telephone`, `user_tel`,
WHERE   `user.id` = `user_tel.user_id`            AND 
        `telephone.id` = `user_tel.telephone_id`  AND 
        `telephone.number` = {}                   AND
        `telephone.for_login` = true


SELECT  `user.*`, 
FROM    `user`, `telephone`, `user_tel`,
WHERE   `user.id` = `user_tel.user_id`            AND 
        `telephone.id` = `user_tel.telephone_id`  AND 
        `telephone.number` = {}                   AND
        `telephone.for_login` = true

/* 根据手机号查找 user_tel 关联表 */
SELECT  *
FROM    `telephone`, `user_tel`
WHERE   `telephone`.`id` = `user_tel`.`telephone_id`

/* 仅查找手机号是否入库了 */
SELECT COUNT(*) 
FROM   `telephone`
WHERE  `telephone`.`number` = '123123'

/* 仅查找手机号信息 */
SELECT * 
FROM   `telephone`
WHERE  `telephone`.`number` = '123123'

/* 插入手机号 */
INSERT LOW_PRIORITY INTO `telephone` (
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
        NULL                  ),
        (
                
        )



/* 
INSERT LOW_PRIORITY INTO `telephone`(
        `number`            ,
        `for_login`         ,
        `vertify_code`      ,
        `desc`              ,
        `vc_update_time`    ,
        `vc_update_time_tz` ,
        `create_time`       ,
        `create_time_tz`    ,
        `update_time`       ,
        `update_time_tz`    ,
        `del_time`          ,
        `del_time_tz`       )
        VALUES (
                {}                        //  number
                true                      //  for_login
                NULL                        //  vertify_code
                NULL                    //  desc
                                        //  vc_update_time
                                        //  vc_update_time_tz
                                        //  create_time
                                        //  create_time_tz
                                        //  update_time
                                        //  update_time_tz
                                        //  del_time
                                        //  del_time_tz
        ) 
        
*/
