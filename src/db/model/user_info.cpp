
#include "user_info.h"
#include <chrono>
#include <cstdint>
#include <sstream>
#include "../../utils/log_utils.h"
#include "../../utils/common.h"
#include "../sql/db_manager.h"
#include "libs/date_3.0.1/include/date/tz.h"

namespace nameless_carpool {

  const UserNames userNames;
  const WcUserNames wcUserNames;
  const TelephoneNames telephoneNames;
  const UserTelNames userTelNames;
  const UserTel::FlagConstant userTelFlagConstant;

  const std::string UserNames::queryAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
        << " \t  " << DbManager::backticks(id)                           << ", \n"
        << " \t  " << DbManager::backticks(id_card_num)                  << ", \n"
        << " \t  " << DbManager::backticks(name)                         << ", \n"
        << " \t  " << DbManager::backticks(gender)                       << ", \n"
        << " \t  " << DbManager::dateSelectStatements(birth_date)        << ", \n"
        << " \t  " << DbManager::backticks(birth_date_tz)                << ", \n"
        <<            this->BaseTimeNames::queryAllFieldSql();
    return sqlTmp.str();
  }
  const std::string UserNames::insertAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
        << " \t  " << DbManager::backticks(id)                           << ", \n"
        << " \t  " << DbManager::backticks(id_card_num)                  << ", \n"
        << " \t  " << DbManager::backticks(name)                         << ", \n"
        << " \t  " << DbManager::backticks(gender)                       << ", \n"
        << " \t  " << DbManager::backticks(birth_date)                   << ", \n"
        << " \t  " << DbManager::backticks(birth_date_tz)                << ", \n"
        <<            this->BaseTimeNames::insertAllFieldSql();
    return sqlTmp.str();
  }

  const std::string WcUserNames::queryAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
        << " \t  " << DbManager::backticks(user_id)                      << ", \n"
        << " \t  " << DbManager::backticks(wc_uid)                       << ", \n"
        << " \t  " << DbManager::backticks(wc_number)                    << ", \n"
        <<            this->BaseTimeNames::queryAllFieldSql();
    return sqlTmp.str();
  }
  const std::string WcUserNames::insertAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
        << " \t  " << DbManager::backticks(user_id)                      << ", \n"
        << " \t  " << DbManager::backticks(wc_uid)                       << ", \n"
        << " \t  " << DbManager::backticks(wc_number)                    << ", \n"
        <<            this->BaseTimeNames::insertAllFieldSql();
    return sqlTmp.str();
  }

  const std::string TelephoneNames::queryAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
        << " \t  " << DbManager::backticks(id)                           << ", \n"
        << " \t  " << DbManager::backticks(number)                       << ", \n"
        << " \t  " << DbManager::backticks(vertify_code)                 << ", \n"
        << " \t  " << DbManager::dateSelectStatements(vc_update_time)    << ", \n"
        << " \t  " << DbManager::backticks(vc_update_time_tz)            << ", \n"
        <<            this->BaseTimeNames::queryAllFieldSql();
    return sqlTmp.str();
  }
  const std::string TelephoneNames::insertAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
        << " \t  " << DbManager::backticks(id)                           << ", \n"
        << " \t  " << DbManager::backticks(number)                       << ", \n"
        << " \t  " << DbManager::backticks(vertify_code)                 << ", \n"
        << " \t  " << DbManager::backticks(vc_update_time)               << ", \n"
        << " \t  " << DbManager::backticks(vc_update_time_tz)            << ", \n"
        <<            this->BaseTimeNames::insertAllFieldSql();
    return sqlTmp.str();
  }

  const std::string UserTelNames::queryAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
        << " \t  " << DbManager::backticks(user_id)                      << ", \n" 
        << " \t  " << DbManager::backticks(telephone_id)                 << ", \n" 
        << " \t  " << DbManager::backticks(desc)                         << ", \n" 
        << " \t  " << DbManager::backticks(flag)                         << ", \n" 
        <<            this->BaseTimeNames::queryAllFieldSql();
    return sqlTmp.str();
  }
  const std::string UserTelNames::insertAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
        << " \t  " << DbManager::backticks(user_id)                      << ", \n" 
        << " \t  " << DbManager::backticks(telephone_id)                 << ", \n" 
        << " \t  " << DbManager::backticks(desc)                         << ", \n" 
        << " \t  " << DbManager::backticks(flag)                         << ", \n" 
        <<            this->BaseTimeNames::insertAllFieldSql();
    return sqlTmp.str();
  }



  const std::string User::insertAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
          << " \t " << DbManager::nullOrApostrophe(id)                 << " , \n"
          << " \t " << DbManager::nullOrApostrophe(id_card_num)        << " , \n"
          << " \t " << DbManager::nullOrApostrophe(name)               << " , \n"
          << " \t " << DbManager::nullOrApostrophe(gender)             << " , \n"
          << " \t " << DbManager::nullOrApostrophe(birth_date)         << " , \n"
          << " \t " << DbManager::nullOrApostrophe(birth_date_tz)      << " , \n"
          <<           this->BaseTime::insertAllFieldSql()                      ;
    return sqlTmp.str();
  }
  const std::string WcUser::insertAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
          << " \t " << DbManager::nullOrApostrophe(user_id)           << " , \n"      
          << " \t " << DbManager::nullOrApostrophe(wc_uid)            << " , \n"     
          << " \t " << DbManager::nullOrApostrophe(wc_number)         << " , \n"    
          <<           this->BaseTime::insertAllFieldSql()                      ;    
    return sqlTmp.str();
  }
  const std::string Telephone::insertAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
          << " \t " << DbManager::nullOrApostrophe(id)                 << " , \n"
          << " \t " << DbManager::nullOrApostrophe(number)             << " , \n"
          << " \t " << DbManager::nullOrApostrophe(vertify_code)       << " , \n"
          << " \t " << DbManager::nullOrApostrophe(vc_update_time)     << " , \n"
          << " \t " << DbManager::nullOrApostrophe(vc_update_time_tz)  << " , \n"
          <<           this->BaseTime::insertAllFieldSql()                      ;
    return sqlTmp.str();
  }
  const std::string UserTel::insertAllFieldSql() const {
    std::stringstream sqlTmp;
    sqlTmp
          << " \t " << DbManager::nullOrApostrophe(this->User::id)     << " , \n" 
          << " \t " << DbManager::nullOrApostrophe(this->Telephone::id)<< " , \n" 
          << " \t " << DbManager::nullOrApostrophe(desc)               << " , \n" 
          << " \t " << DbManager::nullOrApostrophe(flag)               << " , \n" 
          <<           this->BaseTime::insertAllFieldSql()                      ;
    return sqlTmp.str();
  }


  bool Telephone::inflateTelephone(Telephone& obj, const TelephoneNames& names, const string& name, const Value& value) {

    if      (name.compare(names.id                ) == 0) {
      obj.id = value.get<uint64_t>();
    } else if (name.compare(names.number            ) == 0) {
      obj.number = value.get<string>();
    } else if (name.compare(names.vertify_code      ) == 0) {
      obj.vertify_code = value.get<string>();
    } else if (name.compare(names.vc_update_time    ) == 0) {
      obj.vc_update_time = value.get<string>() ;
    } else if (name.compare(names.vc_update_time_tz ) == 0) {
      obj.vc_update_time_tz = value.get<string>();
    } else {
      return false;
    }

    return true;
  }

  map<int, string> Telephone::getTelNameMap(const Columns& columns) {

    const TelephoneNames& names = telephoneNames;
    std::map<int,string> indexNameMap; 

    int index = 0;
    for(auto item = columns.begin(); item != columns.end(); item++ , index++) {
      auto& column = *item;
      string columnName = column.getColumnName();
      if(columnName.empty()) {
        columnName = column.getColumnLabel();
      }
        
      if      (columnName.compare(names.id                ) == 0) {
        indexNameMap[index] =  names.id                ;
      } else if (columnName.compare(names.number            ) == 0) {
        indexNameMap[index] =  names.number            ;
      } else if (columnName.compare(names.vertify_code      ) == 0) {
        indexNameMap[index] =  names.vertify_code      ;
      } else if (columnName.compare(names.vc_update_time    ) == 0) {
        indexNameMap[index] =  names.vc_update_time    ;
      } else if (columnName.compare(names.vc_update_time_tz ) == 0) {
        indexNameMap[index] =  names.vc_update_time_tz ;
      } else if( BaseTime::getNameMap(indexNameMap, index, columnName, names) ) {
        /* empty */
      } else {
        logDebug << "未知属性:" << columnName << endl;
      }
      
    }

    return indexNameMap;
  }

  vector<Telephone> Telephone::getTelVector(SqlResult& sqlResult) {
    map<int, string> indexNameMap = getTelNameMap(sqlResult.getColumns());

    const TelephoneNames& names = telephoneNames;
    auto columnCount = indexNameMap.size();
    vector<Telephone> result;
    for (mysqlx::Row row : sqlResult) {
      Telephone telObj;

      for(int columnIndex = 0; columnIndex < columnCount; columnIndex++) {
        Value& value = row.get(columnIndex);

        const string& columnName = indexNameMap[columnIndex];
        if(Telephone::inflateTelephone(telObj, names, columnName, value)) {
          /* empty */
        } else if(BaseTime::inflateBaseTime(telObj, names, columnName, value)) {
          /* empty */ 
        } else {
          logDebug << "未知属性:" << columnName << endl;
        }
      }/* endfor , 制造一个 telObj */
      
      result.push_back(telObj);
    }/* endfor , 知道一个 Vector<telObj> */

    return result;
  }

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

  bool UserTel::inflateUserTel(UserTel& obj, const UserTelNames& names, const string& name, const Value& value) {
    if (name.compare(names.user_id             ) == 0) {
      obj.User::id = value.get<uint64_t>();
    } else if (name.compare(names.telephone_id ) == 0) {
      obj.Telephone::id = value.get<uint64_t>();
    } else if (name.compare(names.desc         ) == 0) {
      obj.desc = value.get<string>();
    } else if (name.compare(names.flag         ) == 0) {
      obj.flag = value.get<string>();
    } else {
      return false;
    }

    return true;
  }

  map<int, string> UserTel::getUserTelNameMap(const Columns& columns) {
    const UserTelNames& names = userTelNames;
    std::map<int,string> indexNameMap; 

    int index = 0;
    for(auto item = columns.begin(); item != columns.end(); item++ , index++) {
      auto& column = *item;
      string columnName = column.getColumnName();
      if(columnName.empty()) {
        columnName = column.getColumnLabel();
      }
        
      if      (columnName.compare(names.user_id      ) == 0) {
        indexNameMap[index] =  names.user_id       ;
      } else if (columnName.compare(names.telephone_id ) == 0) {
        indexNameMap[index] =  names.telephone_id  ;
      } else if (columnName.compare(names.desc         ) == 0) {
        indexNameMap[index] =  names.desc          ;
      } else if (columnName.compare(names.flag         ) == 0) {
        indexNameMap[index] =  names.flag          ;
      } else if( BaseTime::getNameMap(indexNameMap, index, columnName, names) ) {
        /* empty */
      } else {
        logDebug << "未知属性:" << columnName << endl;
      }
      
    }

    return indexNameMap;
  }

  vector<UserTel> UserTel::getUserTelVector(SqlResult& sqlResult) {
    map<int, string> indexNameMap = getUserTelNameMap(sqlResult.getColumns());

    const UserTelNames& names = userTelNames;
    auto columnCount = indexNameMap.size();
    vector<UserTel> result;
    for (mysqlx::Row row : sqlResult) {
      UserTel obj;

      for(int columnIndex = 0; columnIndex < columnCount; columnIndex++) {
        Value& value = row.get(columnIndex);

        const string& columnName = indexNameMap[columnIndex];
        if(UserTel::inflateUserTel(obj, names, columnName, value)) {
          /* empty */
        } else if(BaseTime::inflateBaseTime(obj, names, columnName, value)) {
          /* empty */ 
        } else {
          logDebug << "未知属性:" << columnName << endl;
        }
      }/* endfor , 制造一个 Obj */
      
      result.push_back(obj);
    }/* endfor , 制造一个 Vector<UserTel> */

    return result;
  }
}