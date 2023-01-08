#pragma once

#include <time.h>
#include <string>
#include <optional> 

#include "../sql/db_include.h"


namespace nameless_carpool {
  using namespace std;
  //using string = std::string;

  struct BaseTimeNames {
    using const_string = const std::string;

    const_string className        =  "BaseTime"         ;
    const_string create_time      =  "create_time"      ;
    const_string create_time_tz   =  "create_time_tz"   ;
    const_string update_time      =  "update_time"      ;
    const_string update_time_tz   =  "update_time_tz"   ;
    const_string del_time         =  "del_time"         ;
    const_string del_time_tz      =  "del_time_tz"      ;

    /**  @description: 将本类中的所有字段拼装并返回 . 最后一个字段没有逗号
     * @return {*}    \t         `field_1`      ,
     *                \t         `field_2`      ,
     *                \t         `field_last`   
     */
    virtual const std::string queryAllFieldSql() const;
    
    /**  @description: 插入语句使用 , 只是增加 反引号包裹
     * @return {*}
     */
    virtual const std::string insertAllFieldSql() const;
  };
  /** 基本时间信息 */
  struct BaseTime {
    BaseTime();
    BaseTime(const BaseTime& obj);
    BaseTime(const BaseTime&& obj);

    optional<string>    create_time      ;        /*  */
    optional<string>    create_time_tz   ;        /*  */
    optional<string>    update_time      ;        /*  */
    optional<string>    update_time_tz   ;        /*  */
    optional<string>    del_time         ;        /*  */
    optional<string>    del_time_tz      ;        /*  */


    /**
     * @description: 当 name 与 names 中的某个属性匹配的时候 , 从 db_value 获取对应的内容填充到 obj 
     * @param {BaseTime&} obj
     * @param {BaseTimeNames&} names
     * @param {string&} name
     * @param {Value&} value
     * @return {*}  true  填充成功 ; false names 中没有 name
     */
    static bool inflateBaseTime(BaseTime& obj, const BaseTimeNames& names, const std::string& name, const Value& value);
    
    /**
     * @description:      curColumnName 对应到 names 中的某个属性名 , 若匹配 , 将 (index : curColumnName ) 插入 map 
     * @param {map<int, string>&} indexNameMap
     * @param {int} index
     * @param {string&} curColumnName
     * @param {BaseTimeNames&} names
     * @return {*}  true , 有匹配 , 插入成功; false 无匹配 未插入
     */
    static bool getNameMap(map<int, string>& indexNameMap, const int index , const std::string& curColumnName , const BaseTimeNames& names) ;
  
    /** @description: 构造插入 sql 子串
     * @return {*}
     */
    virtual const std::string insertAllFieldSql() const;
  
  };

}