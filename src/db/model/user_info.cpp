
#include "user_info.h"
#include <chrono>
#include <cstdint>
#include <sstream>
#include "log_utils.h"
#include "common.h"
#include "db_manager.h"
#include "base_time.h"
#include "libs/date_3.0.1/include/date/tz.h"
#include "libs/mysql_connector_arm_static/include/mysqlx/devapi/common.h"


namespace nameless_carpool {

  // const UserNames userNames;
  // const WcUserNames wcUserNames;
  // const TelephoneNames telephoneNames;
  // const UserTelNames userTelNames;

  const User::Names      userNames      ;
  const WcUser::Names    wcUserNames    ;
  const Telephone::Names telephoneNames ;
  const UserTel::Names   userTelNames   ;

  const UserTel::FlagConstant userTelFlagConstant;


  /*┌─────────────────────────────────────────────────────────────────────────────────────┐
  * │                Telephone                                                            │
  * └─────────────────────────────────────────────────────────────────────────────────────┘ */


  /** @description: 验证码是否过期了
   * @return {*} true, 验证码过期了
   */
  bool Telephone::vcIsExpired() const {
    if (!this->vc_update_time_tz.has_value() || !this->vc_update_time.has_value()) return false;

    const date::local_time<std::chrono::nanoseconds>& localTimeNano = Common::Date::toLocalTime(vc_update_time.value());
    const date::zoned_time                            zonedTime     = date::zoned_time(vc_update_time_tz.value(), localTimeNano);
    const uint64_t                                    recordTime    = zonedTime.get_sys_time().time_since_epoch().count();
    const uint64_t                                    nowTime       = std::chrono::system_clock::now().time_since_epoch().count();
    uint64_t                                          offset        = nowTime - recordTime;

    logDebug << "offset:" << offset << std::endl;

    if (nowTime < recordTime) return true;
    if (offset <= Common::Config::VERTIFY_CODE_LIFE_TIME_NANOSECONDS) return false;
    return true;
  }






  bool User::inflate(const User::Names& names, const string& name, const Value& value) {
    if        (name.compare(names.id                ) == 0) {
      this->id = value.get<uint64_t>();
    } else if (name.compare(names.id_card_num            ) == 0) {
      this->id_card_num = value.get<string>();
    } else if (name.compare(names.name      ) == 0) {
      this->name = value.get<string>();
    } else if (name.compare(names.gender    ) == 0) {
      this->gender = value.get<string>();
    } else if (name.compare(names.birth_date ) == 0) {
      this->birth_date = SqlUtil::getOptionalDate(value);
    } else if (name.compare(names.birth_date_tz ) == 0) {
      this->birth_date_tz = value.get<string>();
    } else if (this->BaseTime::inflate(names, name, value)){
      /* empty */
    } else {
      return false;
    }

    return true;
  }
  
  bool WcUser::inflate(const WcUser::Names& names, const string& name, const Value& value) {
    if        (name.compare(names.user_id                ) == 0) {
      this->user_id = value.get<uint64_t>();
    } else if (name.compare(names.wc_uid            ) == 0) {
      this->wc_uid = value.get<string>();
    } else if (name.compare(names.wc_number      ) == 0) {
      this->wc_number = value.get<string>();
    } else if (this->BaseTime::inflate(names, name, value)){
      /* empty */
    } else {
      return false;
    }

    return true;
  }

  bool Telephone::inflate(const Telephone::Names& names, const string& name, const Value& value)  {
    if        (name.compare(names.id                ) == 0) {
      this->id = value.get<uint64_t>();
    } else if (name.compare(names.number            ) == 0) {
      this->number = value.get<string>();
    } else if (name.compare(names.vertify_code      ) == 0) {
      this->vertify_code = value.get<string>();
    } else if (name.compare(names.vc_update_time    ) == 0) {
      this->vc_update_time = SqlUtil::getOptionalDate(value);
    } else if (name.compare(names.vc_update_time_tz ) == 0) {
      this->vc_update_time_tz = value.get<string>();
    } else if (this->BaseTime::inflate(names, name, value)){
      /* empty */
    } else {
      return false;
    }

    return true;
  }

  bool UserTel::inflate(const UserTel::Names& names, const string& name, const Value& value) {
    if (name.compare(names.user_id             ) == 0) {
      this->user_id = value.get<uint64_t>();
    } else if (name.compare(names.telephone_id ) == 0) {
      this->telephone_id = value.get<uint64_t>();
    } else if (name.compare(names.desc         ) == 0) {
      this->desc = value.get<string>();
    } else if (name.compare(names.flag         ) == 0) {
      this->flag = value.get<string>();
    } else if (this->BaseTime::inflate(names, name, value)){
      /* empty */
    } else {
      return false;
    }

    return true;
  }
}