#include "base_time.h"
#include <optional>
#include <utility>
#include "../../utils/common.h"
#include "../sql/db_manager.h"
#include "db_manager.h"

namespace nameless_carpool {
  
  const std::string BaseTimeNames::queryAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
        << " \t  " << DbManager::dateSelectStatements(create_time)           << ", \n"
        << " \t  " <<            DbManager::backticks(create_time_tz)        << ", \n"
        << " \t  " << DbManager::dateSelectStatements(update_time)           << ", \n"
        << " \t  " <<            DbManager::backticks(update_time_tz)        << ", \n"
        << " \t  " << DbManager::dateSelectStatements(del_time)              << ", \n"
        << " \t  " <<            DbManager::backticks(del_time_tz)           << "  \n";
    return sqlTmp.str();
  }

  const std::string BaseTimeNames::insertAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
        << " \t  " << DbManager::backticks(create_time)        << " ,\n"
        << " \t  " << DbManager::backticks(create_time_tz)     << " ,\n"
        << " \t  " << DbManager::backticks(update_time)        << " ,\n"
        << " \t  " << DbManager::backticks(update_time_tz)     << " ,\n"
        << " \t  " << DbManager::backticks(del_time)           << " ,\n"
        << " \t  " << DbManager::backticks(del_time_tz)        ;
    return sqlTmp.str();
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

  /* TODO - 应该搞清楚 , 时间类型的数据获取原始类型后如何通过 C++ 翻译成字符串或者数据类型 */
  bool BaseTime::inflateBaseTime(BaseTime& obj, const BaseTimeNames& names, const std::string& name, const Value& value) {
    if        (  name.compare(names.BaseTimeNames::create_time    ) == 0  ) {
      obj.create_time     = DbManager::getOptionalDate(value);
    } else if (  name.compare(names.BaseTimeNames::create_time_tz ) == 0  ) {
      obj.create_time_tz  = value.get<string>()              ;
    } else if (  name.compare(names.BaseTimeNames::update_time    ) == 0  ) {
      obj.update_time     = DbManager::getOptionalDate(value);
    } else if (  name.compare(names.BaseTimeNames::update_time_tz ) == 0  ) {
      obj.update_time_tz  = value.get<string>()              ;
    } else if (  name.compare(names.BaseTimeNames::del_time       ) == 0  ) {
      obj.del_time        = DbManager::getOptionalDate(value);
    } else if (  name.compare(names.BaseTimeNames::del_time_tz    ) == 0  ) {
      obj.del_time_tz     = DbManager::getOptional<string>(value)     ;
    } else {
      return false;
    }
    
    return true;
  }

  bool BaseTime::getNameMap(map<int, string>& indexNameMap, const int index, const std::string& columnName, const BaseTimeNames& names) {
    if (columnName.compare(names.BaseTimeNames::create_time) == 0) {
      indexNameMap[index] = names.BaseTimeNames::create_time;
    } else if (columnName.compare(names.BaseTimeNames::create_time_tz) == 0) {
      indexNameMap[index] = names.BaseTimeNames::create_time_tz;
    } else if (columnName.compare(names.BaseTimeNames::update_time) == 0) {
      indexNameMap[index] = names.BaseTimeNames::update_time;
    } else if (columnName.compare(names.BaseTimeNames::update_time_tz) == 0) {
      indexNameMap[index] = names.BaseTimeNames::update_time_tz;
    } else if (columnName.compare(names.BaseTimeNames::del_time) == 0) {
      indexNameMap[index] = names.BaseTimeNames::del_time;
    } else if (columnName.compare(names.BaseTimeNames::del_time_tz) == 0) {
      indexNameMap[index] = names.BaseTimeNames::del_time_tz;
    } else {
      return false;
    }

    return true;
  }

  const std::string BaseTime::insertAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
          << " \t " << DbManager::nullOrApostrophe(create_time)        << " , \n"
          << " \t " << DbManager::nullOrApostrophe(create_time_tz)     << " , \n"
          << " \t " << DbManager::nullOrApostrophe(update_time)        << " , \n"
          << " \t " << DbManager::nullOrApostrophe(update_time_tz)     << " , \n"
          << " \t " << DbManager::nullOrApostrophe(del_time)           << " , \n"
          << " \t " << DbManager::nullOrApostrophe(del_time_tz)        << "   \n";
    return sqlTmp.str();
  }
}