#pragma once

#include <array>
#include <optional>
#include <stdexcept>
#include <string>
#include <iostream>
#include <bitset>
#include <map>
#include <type_traits>
#include <vector>

#include "db_include.h"
#include "base_time.h"
#include "include_json.h"
#include "sql_util.h"

namespace nameless_carpool {
  using namespace std;

  struct User;      /** 用户基本信息 */
  struct WcUser;    /** 微信用户信息 */
  struct Telephone; /* 用户手机号 */
  struct UserTel;   /* 关联表 , User & Telephone */

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

  std::optional<uint64_t>    id            = std::nullopt; /*  */
  std::optional<std::string> id_card_num   = std::nullopt; /* 身份证 */
  std::optional<std::string> name          = std::nullopt; /*  */
  std::optional<std::string> gender        = std::nullopt; /* 性别 */
  std::optional<std::string> birth_date    = std::nullopt; /*  */
  std::optional<std::string> birth_date_tz = std::nullopt; /*  */

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(User,
                                 id,
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
  bool inflate(const User::Names& names, const string& name, const Value& value);

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

  std::optional<uint64_t>    user_id   = std::nullopt;
  std::optional<std::string> wc_uid    = std::nullopt;
  std::optional<std::string> wc_number = std::nullopt;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(WcUser,

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
  bool inflate(const WcUser::Names& names, const string& name, const Value& value);

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
    const std::string vertify_code       = "vertify_code"       ;
    const std::string vc_update_time     = "vc_update_time"     ;
    const std::string vc_update_time_tz  = "vc_update_time_tz"  ;

    virtual const std::string              getTableName() const override { return tableName; }
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {id}; }
    virtual std::vector<std::string>       getSubUnPrimaryKeyNameVector() const override {
      return {number,
              vertify_code,
              vc_update_time,
              vc_update_time_tz};
    }
  };

  std::optional<uint64_t>      id                = std::nullopt;
  std::optional<std::string>   number            = std::nullopt;
  std::optional<std::string>   vertify_code      = std::nullopt;
  std::optional<std::string>   vc_update_time    = std::nullopt;
  std::optional<std::string>   vc_update_time_tz = std::nullopt;

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



  /** @description: 验证码是否过期了 
   * @return {*} true, 验证码过期了
   */
  bool vcIsExpired() const ;

  bool inflate(const Telephone::Names& names, const string& name, const Value& value);

  /* ANCHOR -> 获取主键值组成的列表 */
  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return {numOptionToStrOption(id)}; };
  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  virtual std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override {
    return {number,
            vertify_code,
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

  struct FlagConstant {
    const string forLogIn   = "00000001"; /* 标记此手机号作为登录账号 */
    const string forConnect = "00000010"; /* 标记此手机号作为联系人号码 */
  };

  std::optional<uint64_t>    user_id      = std::nullopt;
  std::optional<uint64_t>    telephone_id = std::nullopt;
  std::optional<std::string> desc         = std::nullopt; /* 有可能同一个手机号不同用户有不同的描述信息 */
  std::optional<std::string> flag         = std::nullopt; /* 标明手机号用途的标示符 */



  /* 看此手机号是否作为 *登录* 手机号的
     查看指定位的值 , 其他位不关心  
     0000 0000 
             ^  false
     0000 0001 
             ^  true
             */
  inline bool flagIsLogin() {
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
  inline bool flagIsContact() {
    bitset<8> flagBitset(flag.value_or(""));
    return flagBitset[1];
  }

  bool inflate(const UserTel::Names& names, const string& name, const Value& value);

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




namespace nameless_carpool {

  extern const User::Names      userNames;
  extern const WcUser::Names    wcUserNames;
  extern const Telephone::Names telephoneNames;
  extern const UserTel::Names   userTelNames;

  extern const UserTel::FlagConstant userTelFlagConstant;

  template <typename Model, typename ModelNames = typename Model::Names>
  inline ModelNames getModelNames() {
    const void* ptr = nullptr;
    if (std::is_same<User, Model>::value) ptr = &userNames;
    else if (std::is_same<WcUser, Model>::value) ptr = &wcUserNames;
    else if (std::is_same<Telephone, Model>::value) ptr = &telephoneNames;
    else if (std::is_same<UserTel, Model>::value) ptr = &userTelNames;

    else throw std::runtime_error("not implement");

    const ModelNames* result = reinterpret_cast<const ModelNames*>(ptr);

    return *result;
  }
};