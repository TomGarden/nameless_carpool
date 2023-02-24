#include "base_time.h"
#include <optional>
#include <utility>
#include "common.h"
#include "db_manager.h"
#include "db_manager.h"

namespace nameless_carpool /* BaseTimeNames */ {

  const std::string BaseTimeNames::allFieldSql(const std::vector<std::string> inStrVector) const {
    if (inStrVector.empty()) return "";

    std::stringstream sqlTmp;
    for(const std::string& tempStr: inStrVector) {
      sqlTmp << " \t  " << SqlUtil::backticks(tempStr)        << " ,\n";
    }
    std::string result = sqlTmp.str();
    return result.erase(result.size() - 2, 1);
  }

}

namespace nameless_carpool/* ::BaseTime */ {

  BaseTime::BaseTime() = default;
  BaseTime::BaseTime(const BaseTime& obj) {
    this->create_time    = obj.create_time;
    this->create_time_tz = obj.create_time_tz;
    this->update_time    = obj.update_time;
    this->update_time_tz = obj.update_time_tz;
    this->del_time       = obj.del_time;
    this->del_time_tz    = obj.del_time_tz;
  }
  BaseTime::BaseTime(const BaseTime&& obj) {
    this->create_time    = std::move(obj.create_time);
    this->create_time_tz = std::move(obj.create_time_tz);
    this->update_time    = std::move(obj.update_time);
    this->update_time_tz = std::move(obj.update_time_tz);
    this->del_time       = std::move(obj.del_time);
    this->del_time_tz    = std::move(obj.del_time_tz);
  }

  const std::string BaseTime::insertAllFieldSql(const std::vector<optional<std::string>> inStrVector) const {
    std::stringstream sqlTmp;
    for (optional<string> optionalStr : inStrVector) {
      sqlTmp << " \t \t " << SqlUtil::nullOrApostrophe(optionalStr) << " ,\n";
    }
    std::string result = sqlTmp.str();
    return result.erase(result.size() - 2, 1);
  }
}