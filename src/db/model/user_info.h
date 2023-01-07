#pragma once

#include <string>
#include <iostream>
#include <bitset>
#include <map>

#include "../sql/db_include.h"
#include "base_time.h"
#include "../../utils/json/include_json.h"

namespace nameless_carpool {
  using namespace std;

  /** 用户基本信息 */
  struct UserNames : virtual BaseTimeNames {
    const_string className      =  "User"           ;
    const_string tableName      =  "user"           ;
    const_string id             =  "id"             ;
    const_string id_card_num    =  "id_card_num"    ;
    const_string name           =  "name"           ;
    const_string gender         =  "gender"         ;
    const_string birth_date     =  "birth_date"     ;
    const_string birth_date_tz  =  "birth_date_tz"  ;

    virtual const std::string queryAllFieldSql() const override ;
    virtual const std::string insertAllFieldSql() const override;
  };
  extern const UserNames userNames;
  struct User      : virtual BaseTime {
    optional<uint64_t>  id             ;       /*  */
    optional<string>    id_card_num    ;       /* 身份证 */
    optional<string>    name           ;       /*  */
    optional<string>    gender         ;       /* 性别 */
    optional<string>    birth_date     ;       /*  */
    optional<string>    birth_date_tz  ;       /*  */

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(User, 
    //NLOHMANN_DEFINE_OPTIONAL_TYPE_INTRUSIVE(User, 

                                   id,
                                   name,
                                   gender,
                                   birth_date,
                                   birth_date_tz,

                                   create_time        ,
                                   create_time_tz     ,
                                   update_time        ,
                                   update_time_tz     ,
                                   del_time           ,
                                   del_time_tz
                                   )
  
    virtual const std::string insertAllFieldSql() const override;
  };


  /** 微信用户信息 */
  struct WcUserNames : virtual BaseTimeNames {
    const_string className  = "WcUser"        ;
    const_string tableName  = "wc_user"        ;
    const_string user_id    = "user_id"       ;
    const_string wc_uid     = "wc_uid"        ;
    const_string wc_number  = "wc_number"     ;

    virtual const std::string queryAllFieldSql() const override ;
    virtual const std::string insertAllFieldSql() const override;
  };
  extern const WcUserNames wcUserNames;
  struct WcUser      : virtual BaseTime {
    optional<string>  user_id       ;
    optional<string>  wc_uid        ;
    optional<string>  wc_number     ;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(WcUser, 
    // NLOHMANN_DEFINE_OPTIONAL_TYPE_INTRUSIVE(WcUser, 

                                   user_id,
                                   wc_uid,
                                   wc_number,

                                   create_time        ,
                                   create_time_tz     ,
                                   update_time        ,
                                   update_time_tz     ,
                                   del_time           ,
                                   del_time_tz
                                   )
  
    virtual const std::string insertAllFieldSql() const override;
  };


  /* 用户手机号 */
  struct TelephoneNames : virtual BaseTimeNames {
    const_string className          = "Telephone"          ;
    const_string tableName          = "telephone"          ;
    const_string id                 = "id"                 ;
    const_string number             = "number"             ;
    const_string vertify_code       = "vertify_code"       ;
    const_string vc_update_time     = "vc_update_time"     ;
    const_string vc_update_time_tz  = "vc_update_time_tz"  ;

    virtual const std::string queryAllFieldSql() const override ;
    virtual const std::string insertAllFieldSql() const override;
  };
  extern const TelephoneNames telephoneNames;
  struct Telephone      : virtual BaseTime {
    optional<uint64_t>    id                 ;
    optional<string>      number             ;
    optional<string>      vertify_code       ;
    optional<string>      vc_update_time     ;
    optional<string>      vc_update_time_tz  ;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Telephone          ,
                                   id                 ,
                                   number             ,
                                   vertify_code       ,
                                   vc_update_time     ,
                                   vc_update_time_tz  ,

                                   create_time        ,
                                   create_time_tz     ,
                                   update_time        ,
                                   update_time_tz     ,
                                   del_time           ,
                                   del_time_tz
                                   )


    /* 按照 数据库对象 Columns 中各个列名出现的顺序 , 填充 map<index , columnName> */
    static map<int, string> getTelNameMap(const Columns& columns);
    /* 从数据库对象 SqlResult 中读取素有 Telephone 对象并保存到 vector 中 */
    static vector<Telephone> getTelVector(SqlResult& sqlResult);
    /* 将 数据库 对象 Value 中的值转换为相应的类型填充到 Telephone 对象中  */
    static bool inflateTelephone(Telephone& obj, const TelephoneNames& names, const string& name, const Value& value);

    /** @description: 验证码是否过期了 
     * @return {*} true, 验证码过期了
     */
    bool vcIsExpired() const ;
  
    virtual const std::string insertAllFieldSql() const override;
  };



  /* 关联表 , User & Telephone */
  struct UserTelNames : UserNames, TelephoneNames {
    const_string claseName    = "UserTel"           ;
    const_string tableName    = "user_tel"          ;
    const_string user_id      = "user_id"           ;
    const_string telephone_id = "telephone_id"      ;
    const_string desc         = "desc"              ;
    const_string flag         = "flag"              ;

    virtual const std::string queryAllFieldSql() const override ;
    virtual const std::string insertAllFieldSql() const override;
  };
  extern const UserTelNames userTelNames;
  struct UserTel      : User,      Telephone {  
    //User::id
    //Telephone::id

    optional<string>      desc               ;  /* 有可能同一个手机号不同用户有不同的描述信息 */
    optional<string>      flag               ;  /* 标明手机号用途的标示符 */

    struct FlagConstant {
      const string forLogIn   = "00000001"; /* 标记此手机号作为登录账号 */
      const string forConnect = "00000010"; /* 标记此手机号作为联系人号码 */
    };

    /* 看此手机号是否作为 *登录* 手机号的
       查看指定位的值 , 其他位不关心  
       0000 0000 
               ^  false
       0000 0001 
               ^  true
               */
    bool flagIsLogin() {
      bitset<8> flagBitset(flag.value_or(""));
      return flagBitset[0];
    }

    /* 看此手机号是否作为 *联系人* 手机号的
       查看指定位的值 , 其他位不关心  
       0000 0000 
              ^  false
       0000 0010 
              ^  true
               */
    bool flagIsContact() {
      bitset<8> flagBitset(flag.value_or(""));
      return flagBitset[1];
    }

    /* 按照 数据库对象 Columns 中各个列名出现的顺序 , 填充 map<index , columnName> */
    static map<int, string> getUserTelNameMap(const Columns& columns);
    /* 从数据库对象 SqlResult 中读取素有 Telephone 对象并保存到 vector 中 */
    static vector<UserTel> getUserTelVector(SqlResult& sqlResult);
    /* 将 数据库 对象 Value 中的值转换为相应的类型填充到 Telephone 对象中  */
    static bool inflateUserTel(UserTel& obj, const UserTelNames& names, const string& name, const Value& value);

    virtual const std::string insertAllFieldSql() const override;
  };
  extern const UserTel::FlagConstant userTelFlagConstant;
}