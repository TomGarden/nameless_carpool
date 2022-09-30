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
  };
  /** 基本时间信息 */
  struct BaseTime {
    optional<string>    create_time      ;        /*  */
    optional<int8_t>    create_time_tz   ;        /*  */
    optional<string>    update_time      ;        /*  */
    optional<int8_t>    update_time_tz   ;        /*  */
    optional<string>    del_time         ;        /*  */
    optional<int8_t>    del_time_tz      ;        /*  */

    /* 当 name 与 names 中的某个属性匹配的时候 , 从 db_value 获取对应的内容填充到 obj 
       return true  填充成功
              false names 中没有 name
    */
    static bool inflateBaseTime(BaseTime& obj, const BaseTimeNames& names, const std::string& name, const Value& value);
  };



  /* 多重继承的时候用于限定符号作用域 : https://stackoverflow.com/a/4118705/7707781 */
  struct MidBaseTime : BaseTime {};
  struct MidBaseTimeNames : BaseTimeNames {};
}