#include "base_time.h"
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

  bool BaseTime::inflateBaseTime(BaseTime& obj, const BaseTimeNames& names, const std::string& name, const Value& value) {
    if        (  name.compare(names.BaseTimeNames::create_time    ) == 0  ) {
      obj.create_time     = value.get<string>()               ;
    } else if (  name.compare(names.BaseTimeNames::create_time_tz ) == 0  ) {
      obj.create_time_tz  = value.get<string>()              ;
    } else if (  name.compare(names.BaseTimeNames::update_time    ) == 0  ) {
      obj.update_time     = value.get<string>()               ;
    } else if (  name.compare(names.BaseTimeNames::update_time_tz ) == 0  ) {
      obj.update_time_tz  = value.get<string>()              ;
    } else if (  name.compare(names.BaseTimeNames::del_time       ) == 0  ) {
      obj.del_time        = DbManager::getOptional<string>(value)      ;
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