
#include "src/db/model/user_info.h"

#include <boost/uuid/uuid.hpp>             // uuid class
#include <boost/uuid/uuid_generators.hpp>  // generators
#include <boost/uuid/uuid_io.hpp>          // streaming operators etc.
#include <chrono>
#include <cstdint>
#include <optional>
#include <sstream>
#include <string>

#include "base_time.h"
#include "src/utils/common.h"
#include "libs/date_3.0.1/include/date/date.h"
#include "libs/date_3.0.1/include/date/tz.h"
#include "src/db/sql_util.h"


namespace nameless_carpool {

  #define BASE_TIME_INFLATE \
    else if (this->BaseTime::inflate(names, name, value)){  \
      /* empty */                                           \
    } else {                                                \
      return false;                                         \
    }                                                       \

  /* 定义导出的常量们 */
  const User::Names        userNames          ;
  const WcUser::Names      wcUserNames        ;
  const Telephone::Names   telephoneNames     ;
  const UserTel::Names     userTelNames       ;
  const Session::Names     sessionNames       ;
  const UserSession::Names userSessionNames   ;

  const UserTel::FlagConstant userTelFlagConstant;


  /*┌─────────────────────────────────────────────────────────────────────────────────────┐
  * │                Telephone
  * └─────────────────────────────────────────────────────────────────────────────────────┘ */


  /** @description: 验证码是否过期了
   * @return {*} true, 验证码过期了
   */
  bool Telephone::vcIsExpired() const {
    if ((!vc_update_time.has_value() || vc_update_time->empty()) &&
        (!vc_update_time_tz.has_value() || vc_update_time_tz->empty())) return true;
    std::optional<int64_t> offset = Common::Date::passedNanoseconds(vc_update_time.value(), vc_update_time_tz.value());
    if (offset == std::nullopt) return true;
    if (offset < 0) return false; /* 小于零的情况认为是未过期 */
    if (offset <= Common::Config::VERIFY_CODE_LIFE_TIME_NANOSECONDS) return false;
    return true;
  }



  /*┌─────────────────────────────────────────────────────────────────────────────────────┐
  * │                Model inflate
  * └─────────────────────────────────────────────────────────────────────────────────────┘ */

  bool User::inflate(const Names& names, const std::string& name, const mysqlx::Value& value) {
    if        (name.compare(names.id                ) == 0) {
      this->id = SqlUtil::getOptional<uint64_t>(value);
    } else if (name.compare(names.id_card_num            ) == 0) {
      this->id_card_num = SqlUtil::getOptional<std::string>(value);
    } else if (name.compare(names.name      ) == 0) {
      this->name = SqlUtil::getOptional<std::string>(value);
    } else if (name.compare(names.gender    ) == 0) {
      this->gender = SqlUtil::getOptional<std::string>(value);
    } else if (name.compare(names.birth_date ) == 0) {
      this->birth_date = SqlUtil::getOptionalDate(value);
    } else if (name.compare(names.birth_date_tz ) == 0) {
      this->birth_date_tz = SqlUtil::getOptional<std::string>(value);
    } BASE_TIME_INFLATE

    return true;
  }
  
  bool WcUser::inflate(const Names& names, const std::string& name, const mysqlx::Value& value) {
    if        (name.compare(names.user_id                ) == 0) {
      this->user_id = SqlUtil::getOptional<uint64_t>(value);
    } else if (name.compare(names.wc_uid            ) == 0) {
      this->wc_uid = SqlUtil::getOptional<std::string>(value);
    } else if (name.compare(names.wc_number      ) == 0) {
      this->wc_number = SqlUtil::getOptional<std::string>(value);
    } BASE_TIME_INFLATE

    return true;
  }

  bool Telephone::inflate(const Names& names, const std::string& name, const mysqlx::Value& value)  {
    if        (name.compare(names.id                ) == 0) {
      this->id = SqlUtil::getOptional<uint64_t>(value);
    } else if (name.compare(names.number            ) == 0) {
      this->number = SqlUtil::getOptional<std::string>(value);
    } else if (name.compare(names.verify_code      ) == 0) {
      this->verify_code = SqlUtil::getOptional<std::string>(value);
    } else if (name.compare(names.vc_update_time    ) == 0) {
      this->vc_update_time = SqlUtil::getOptionalDate(value);
    } else if (name.compare(names.vc_update_time_tz ) == 0) {
      this->vc_update_time_tz = SqlUtil::getOptional<std::string>(value);
    } BASE_TIME_INFLATE

    return true;
  }

  bool UserTel::inflate(const Names& names, const std::string& name, const mysqlx::Value& value) {
    if (name.compare(names.user_id             ) == 0) {
      this->user_id = SqlUtil::getOptional<uint64_t>(value);
    } else if (name.compare(names.telephone_id ) == 0) {
      this->telephone_id = SqlUtil::getOptional<uint64_t>(value);
    } else if (name.compare(names.desc         ) == 0) {
      this->desc = SqlUtil::getOptional<std::string>(value);
    } else if (name.compare(names.flag         ) == 0) {
      this->flag = SqlUtil::getOptional<std::string>(value);
    } BASE_TIME_INFLATE

    return true;
  }

  bool Session::inflate(const Names &names, const std::string &name, const mysqlx::Value &value) {
    if (name.compare(names.id) == 0)/*                       */this->id = SqlUtil::getOptional<uint64_t>(value);
    else if (name.compare(names.client_more_info) == 0)/*    */this->client_more_info = SqlUtil::getOptional<std::string>(value);
    else if (name.compare(names.client_type) == 0)/*         */this->client_type = SqlUtil::getOptional<std::string>(value);
    else if (name.compare(names.token) == 0)/*               */this->token = SqlUtil::getOptional<std::string>(value);
    else if (name.compare(names.max_age) == 0)/*             */this->max_age = SqlUtil::getOptional<uint64_t>(value);
    else if (name.compare(names.from_where) == 0)/*          */this->from_where = SqlUtil::getOptionalDate(value);
    else if (name.compare(names.auth_method) == 0)/*         */this->auth_method = SqlUtil::getOptional<std::string>(value);
    else if (name.compare(names.token_update_time) == 0)/*   */this->token_update_time = SqlUtil::getOptionalDate(value);
    else if (name.compare(names.token_update_time_tz) == 0)/**/this->token_update_time_tz = SqlUtil::getOptional<std::string>(value);
    BASE_TIME_INFLATE

    return true;
  }

  bool UserSession::inflate(const Names& names, const std::string& name, const mysqlx::Value& value) {
    if        (name.compare(names.user_id                ) == 0) {
      this->user_id = SqlUtil::getOptional<uint64_t>(value);
    } else if (name.compare(names.session_id            ) == 0) {
      this->session_id = SqlUtil::getOptional<uint64_t>(value);
    } BASE_TIME_INFLATE

    return true;
  }
}

namespace nameless_carpool /* Session */{
  bool Session::tokenIsExpired() const {
    if (max_age == std::nullopt) return false;
    std::optional<int64_t> offset = Common::Date::passedNanoseconds(token_update_time, token_update_time_tz);
    if (offset == std::nullopt) return false;
    
    /* offset 表示 tocken update time 到 当前时刻 经过了多少纳秒 */
    /* maxAgeNs 表示 , 经过的纳秒数最大值 */

    using namespace std::chrono;
    int64_t maxAgeNs = duration_cast<nanoseconds>(seconds(max_age.value())).count(); /* 秒 → 纳秒 */

    return offset.value() > maxAgeNs;
  }

  Session* Session::resetToken(const std::string& timeZoneStr) {
    
    token = boost::uuids::to_string(boost::uuids::random_generator()());
    token_update_time    = date::format(Common::Date::defFormatStr, date::make_zoned(timeZoneStr, std::chrono::system_clock::now()).get_local_time());
    token_update_time_tz = timeZoneStr;
    update_time = token_update_time;
    update_time_tz = token_update_time_tz;

    return this;
  }


  /*┌─────────────────────────────────────────────────────────────────────────────────────┐
  * │               快速创建新实例
  * └─────────────────────────────────────────────────────────────────────────────────────┘ */
  Session Session::newObj(const std::string& timeZoneStr) {
    Session session ;

    // id
    // client_more_info
    // client_type
    session.token        = boost::uuids::to_string(boost::uuids::random_generator()());
    session.max_age      = session.getDefaultMaxAge();
    // from_where
    // auth_method
    session.token_update_time    = date::format(Common::Date::defFormatStr, date::make_zoned(timeZoneStr, std::chrono::system_clock::now()).get_local_time());
    session.token_update_time_tz = timeZoneStr;

    session.BaseTime::create_time    = session.token_update_time;
    session.BaseTime::create_time_tz = session.token_update_time_tz;
    session.BaseTime::update_time    = session.token_update_time;
    session.BaseTime::update_time_tz = session.token_update_time_tz;

    return session;
  }
}