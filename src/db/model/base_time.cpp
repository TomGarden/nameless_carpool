#include "base_time.h"
#include <optional>
#include <utility>
#include "common.h"

namespace nameless_carpool /* BaseTimeNames */ {


}

namespace nameless_carpool/* ::BaseTime */ {

  // BaseTime::BaseTime() = default;
  // BaseTime::BaseTime(const BaseTime& obj) {
  //   this->create_time    = obj.create_time;
  //   this->create_time_tz = obj.create_time_tz;
  //   this->update_time    = obj.update_time;
  //   this->update_time_tz = obj.update_time_tz;
  //   this->del_time       = obj.del_time;
  //   this->del_time_tz    = obj.del_time_tz;
  // }
  // BaseTime::BaseTime(const BaseTime&& obj) {
  //   this->create_time    = std::move(obj.create_time);
  //   this->create_time_tz = std::move(obj.create_time_tz);
  //   this->update_time    = std::move(obj.update_time);
  //   this->update_time_tz = std::move(obj.update_time_tz);
  //   this->del_time       = std::move(obj.del_time);
  //   this->del_time_tz    = std::move(obj.del_time_tz);
  // }

  const std::string BaseTime::insertAllFieldSql(const std::vector<std::optional<std::string>> inStrVector) const {
    std::stringstream sqlTmp;
    for (const std::optional<std::string>& optionalStr : inStrVector) {
      sqlTmp << " \t \t " << SqlUtil::nullOrApostrophe(optionalStr) << " ,\n";
    }
    std::string result = sqlTmp.str();
    return result.erase(result.size() - 2, 1);
  }

  const std::string BaseTime::insertAllFieldSqlWithoutApostrophe(const std::vector<std::string> inStrVector) const {
    std::stringstream sqlTmp;
    for (const std::string& optionalStr : inStrVector) sqlTmp << " \t \t " << optionalStr << " ,\n";
    std::string result = sqlTmp.str();
    return result.erase(result.size() - 2, 1);
  }
}