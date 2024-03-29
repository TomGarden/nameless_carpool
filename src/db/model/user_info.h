#pragma once

#include <array>
#include <bitset>
#include <iostream>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "base_time.h"
#include "src/db/sql_util.h"
#include "src/utils/json/include_json.h"
#include "src/utils/macro/enum_util.h"

namespace nameless_carpool {

  struct User;        /** 用户基本信息 */
  struct WcUser;      /** 微信用户信息 */
  struct Telephone;   /* 用户手机号 */
  struct UserTel;     /* 关联表 , User & Telephone */
  struct Session;     /* 登录会话 */
  struct UserSession; /* 关联表 , User & Session */

}  // namespace nameless_carpool

/** 用户基本信息 */
struct nameless_carpool::User : public BaseTime {
  struct Names : virtual BaseTimeNames {
    const std::string tableName = "user";

    const std::string id            = "id";
    const std::string id_card_num   = "id_card_num";
    const std::string name          = "name";
    const std::string gender        = "gender";
    const std::string birth_date    = "birth_date";
    const std::string birth_date_tz = "birth_date_tz";

    /* ANCHOR -> 返回表名称字段 */
    virtual const std::string getTableName() const override { return tableName; }
    /* ANCHOR -> 获取主键列名称组成的列表 , 主键可能不止一个 */
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {id}; }
    /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTimeNames::getUnPrimaryKeyNameVector 完成 */
    virtual std::vector<std::string> getSubUnPrimaryKeyNameVector() const override {
      return {id_card_num,
              name,
              gender,
              birth_date,
              birth_date_tz};
    }
  };
  GET_NAMES()

  std::optional<uint64_t>    id            = std::nullopt; /*  */
  std::optional<std::string> id_card_num   = std::nullopt; /* 身份证 */
  std::optional<std::string> name          = std::nullopt; /*  */
  std::optional<std::string> gender        = std::nullopt; /* 性别 */
  std::optional<std::string> birth_date    = std::nullopt; /*  */
  std::optional<std::string> birth_date_tz = std::nullopt; /*  */

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(User,

                                 id,
                                 id_card_num,
                                 name,
                                 gender,
                                 birth_date,
                                 birth_date_tz,

                                 create_time,
                                 create_time_tz,
                                 update_time,
                                 update_time_tz,
                                 del_time,
                                 del_time_tz)


  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);

  virtual const std::vector<std::optional<std::string>> 
  getPrimaryKeyValVector() const override { return {numOptionToStrOption(id)}; };
  
  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  virtual std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override {
    return {id_card_num,
            name,
            gender,
            birth_date,
            birth_date_tz};
  }
};

/** 微信用户信息 */
struct nameless_carpool::WcUser : public BaseTime {

  struct Names : virtual BaseTimeNames {
    const std::string tableName = "wc_user";
    const std::string user_id   = "user_id";
    const std::string wc_uid    = "wc_uid";
    const std::string wc_number = "wc_number";

    virtual const std::string              getTableName() const override { return tableName; }
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {user_id}; }
    virtual std::vector<std::string>       getSubUnPrimaryKeyNameVector() const override {
      return {user_id,
              wc_uid,
              wc_number};
    }
  };
  GET_NAMES()

  std::optional<uint64_t>    user_id   = std::nullopt;
  std::optional<std::string> wc_uid    = std::nullopt;
  std::optional<std::string> wc_number = std::nullopt;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(WcUser,

                                 user_id,
                                 wc_uid,
                                 wc_number,

                                 create_time,
                                 create_time_tz,
                                 update_time,
                                 update_time_tz,
                                 del_time,
                                 del_time_tz)


  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);

  /* ANCHOR -> 获取主键值组成的列表 */
  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return {numOptionToStrOption(user_id)}; };
  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  virtual std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override {
    return {wc_uid,
            wc_number};
  }
};

struct nameless_carpool::Telephone : public BaseTime {
  
  struct Names : virtual BaseTimeNames {
    const std::string tableName          = "telephone"          ;

    const std::string id                 = "id"                 ;
    const std::string number             = "number"             ;
    const std::string verify_code       = "verify_code"       ;
    const std::string vc_update_time     = "vc_update_time"     ;
    const std::string vc_update_time_tz  = "vc_update_time_tz"  ;

    virtual const std::string              getTableName() const override { return tableName; }
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {id}; }
    virtual std::vector<std::string>       getSubUnPrimaryKeyNameVector() const override {
      return {number,
              verify_code,
              vc_update_time,
              vc_update_time_tz};
    }
  };
  GET_NAMES()

  std::optional<uint64_t>      id                = std::nullopt;
  std::optional<std::string>   number            = std::nullopt;
  std::optional<std::string>   verify_code      = std::nullopt;
  std::optional<std::string>   vc_update_time    = std::nullopt;
  std::optional<std::string>   vc_update_time_tz = std::nullopt;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Telephone          ,
                                 id                 ,
                                 number             ,
                                 verify_code       ,
                                 vc_update_time     ,
                                 vc_update_time_tz  ,

                                 create_time        ,
                                 create_time_tz     ,
                                 update_time        ,
                                 update_time_tz     ,
                                 del_time           ,
                                 del_time_tz
                                 )



  /** @description: 验证码是否过期了 
   * @return {*} true, 验证码过期了
   */
  bool vcIsExpired() const ;

  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);

  /* ANCHOR -> 获取主键值组成的列表 */
  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return {numOptionToStrOption(id)}; };
  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  virtual std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override {
    return {number,
            verify_code,
            vc_update_time,
            vc_update_time_tz};
  }
};

struct nameless_carpool::UserTel : public BaseTime {

  struct Names : public BaseTimeNames {
    const std::string tableName    = "user_tel";
    const std::string user_id      = "user_id";
    const std::string telephone_id = "telephone_id";
    const std::string desc         = "desc";
    const std::string flag         = "flag";

    virtual const std::string              getTableName() const override { return tableName; }
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {user_id, telephone_id}; }
    virtual std::vector<std::string>       getSubUnPrimaryKeyNameVector() const override {
      return {desc,
              flag};
    }
  };
  GET_NAMES()

  ENUM_UTIL_DEFINE_ENUM(Flag,   /* 成员字符长度不能超过 20 , 因为要转换成字符串存到数据库中 */
              login,  /* 标记此手机号作为登录账号 */
              connect /* 标记此手机号作为联系人号码 */
  )

  std::optional<uint64_t>    user_id      = std::nullopt;
  std::optional<uint64_t>    telephone_id = std::nullopt;
  std::optional<std::string> desc         = std::nullopt; /* 有可能同一个手机号不同用户有不同的描述信息 */
  std::optional<std::string> flag         = std::nullopt; /* 标明手机号用途的标示符 */



  /* 看此手机号是否作为 *登录* 手机号的 */
  inline bool flagIsLogin() { return flag == Flag::toString(Flag::login); }

  /* 看此手机号是否作为 *联系人* 手机号的 */
  inline bool flagIsContact() { return flag == Flag::toString(Flag::connect); }

  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);

  /* ANCHOR -> 获取主键值组成的列表 */
  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return numOptionToStrOption({user_id, telephone_id}); };
  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  virtual std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override {
    return {desc,
            flag};
  };
};

struct nameless_carpool::Session :public BaseTime {
  struct Names : virtual BaseTimeNames {
    const std::string tableName = "session";

    const std::string id                   = "id";
    const std::string client_more_info          = "client_more_info";
    const std::string client_type          = "client_type";
    const std::string token                = "token";
    const std::string max_age              = "max_age";
    const std::string from_where           = "from_where";
    const std::string auth_method          = "auth_method";
    const std::string token_update_time    = "token_update_time";
    const std::string token_update_time_tz = "token_update_time_tz";

    /* ANCHOR -> 返回表名称字段 */
    virtual const std::string getTableName() const override { return tableName; }
    /* ANCHOR -> 获取主键列名称组成的列表 , 主键可能不止一个 */
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {id}; }
    /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTimeNames::getUnPrimaryKeyNameVector 完成 */
    virtual std::vector<std::string> getSubUnPrimaryKeyNameVector() const override {
      return {client_more_info,
              client_type,
              token,
              max_age,
              from_where,
              auth_method,
              token_update_time,
              token_update_time_tz};
    }
  };
  GET_NAMES()

  /* 客户端类型枚举 */
  struct ClientType {
    const std::string unknow       = "Unknow";
    const std::string android      = "Android";
    const std::string ios          = "iOS";
    const std::string web          = "web";
    const std::string microProgram = "microProgram"; /* 小程序 */

    inline const std::string getClientType(const std::string& inputStr) {
      if (android.compare(inputStr) == 0) return android;
      if (ios.compare(inputStr) == 0) return ios;
      if (web.compare(inputStr) == 0) return web;
      if (microProgram.compare(inputStr) == 0) return microProgram;
      return unknow;
    }

    inline const std::string getClientType(const std::optional<std::string>& inputOpt) {
      if (!inputOpt.has_value()) return unknow;
      return getClientType(inputOpt.value());
    }
  };

  struct AuthMethod {
    const std::string unknow            = "unknow";
    const std::string telMsgVerifyCode  = "tel_msg_verify_code";
  };

  std::optional<uint64_t>    id                   = std::nullopt; /*  */
  std::optional<std::string> client_more_info     = std::nullopt; /* 更多设备信息描述 */
  std::optional<std::string> client_type          = std::nullopt; /* 客户端类型 */
  std::optional<std::string> token                = std::nullopt; /*  */
  std::optional<int64_t>     max_age              = std::nullopt; /* session 有效期 , 从登录时间点开始计时 */
  std::optional<std::string> from_where           = std::nullopt; /*  */
  std::optional<std::string> auth_method          = std::nullopt; /* 登录动作的身份验证方式 */
  std::optional<std::string> token_update_time    = std::nullopt; 
  std::optional<std::string> token_update_time_tz = std::nullopt;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Session,

                                 id,
                                 client_more_info,
                                 client_type,
                                 token,
                                 max_age,
                                 from_where,
                                 auth_method,
                                 token_update_time,
                                 token_update_time_tz,

                                 create_time,
                                 create_time_tz,
                                 update_time,
                                 update_time_tz,
                                 del_time,
                                 del_time_tz)



  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);

  virtual const std::vector<std::optional<std::string>> 
  getPrimaryKeyValVector() const override { return {numOptionToStrOption(id)}; };
  
  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  virtual std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override {
    return {client_more_info,
            client_type,
            token,
            numOptionToStrOption(max_age),
            from_where,
            auth_method,
            token_update_time,
            token_update_time_tz};
  }

  /*┌─────────────────────────────────────────────────────────────────────────────────────┐
  * │                业务功能函数
  * └─────────────────────────────────────────────────────────────────────────────────────┘ */

  /** @return true, token 过期了
   * @note 如果我们期望一个 token 延长有效期 , 可以更新 token_update_time , 到一个比较靠后的时间点 , 可以必当前时间更靠后
   * */
  bool tokenIsExpired() const;

  /* 重置 token , 含相关时间信息 */
  Session* resetToken(const std::string& timeZoneStr);
  
  /* 返回默认的 token 超时时间(15天) , 单位 (秒)         天   时    分   秒 */
  inline uint64_t getDefaultMaxAge() {return 15 * 24 * 60 * 60;}


  /*┌─────────────────────────────────────────────────────────────────────────────────────┐
  * │               快速创建新实例
  * └─────────────────────────────────────────────────────────────────────────────────────┘ */
  static Session newObj(const std::string& timeZoneStr);
};

struct nameless_carpool::UserSession :public BaseTime {
  struct Names : virtual BaseTimeNames {
    const std::string tableName = "user_session";

    const std::string user_id    = "user_id";
    const std::string session_id = "session_id";

    /* ANCHOR -> 返回表名称字段 */
    virtual const std::string getTableName() const override { return tableName; }
    /* ANCHOR -> 获取主键列名称组成的列表 , 主键可能不止一个 */
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {user_id, session_id}; }
    /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTimeNames::getUnPrimaryKeyNameVector 完成 */
    virtual std::vector<std::string> getSubUnPrimaryKeyNameVector() const override { return {}; }
  };
  GET_NAMES()

  std::optional<uint64_t> user_id    = std::nullopt; 
  std::optional<uint64_t> session_id = std::nullopt; 

  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);

  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return {numOptionToStrOption({user_id, session_id})}; };

  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  virtual std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override { return {}; }
};



