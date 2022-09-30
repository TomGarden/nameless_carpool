
#include "user_info.h"
#include "../../utils/log_utils.h"
#include "../../utils/common.h"
#include "../sql/db_manager.h"

namespace nameless_carpool {

  const UserNames userNames;
  const WcUserNames wcUserNames;
  const TelephoneNames telephoneNames;
  const UserTelNames userTelNames;

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
      obj.vc_update_time_tz = value.get<int64_t>();
    } else {
      return false;
    }

    return true;
  }

  map<int, string> Telephone::getTelNameMap(const Columns& columns) {

    TelephoneNames names;
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
      } else if (columnName.compare(names.BaseTimeNames::create_time    ) == 0) {
        indexNameMap[index] = names.BaseTimeNames::create_time    ;
      } else if (columnName.compare(names.BaseTimeNames::create_time_tz ) == 0) {
        indexNameMap[index] = names.BaseTimeNames::create_time_tz ;
      } else if (columnName.compare(names.BaseTimeNames::update_time    ) == 0) {
        indexNameMap[index] = names.BaseTimeNames::update_time    ;
      } else if (columnName.compare(names.BaseTimeNames::update_time_tz ) == 0) {
        indexNameMap[index] = names.BaseTimeNames::update_time_tz ;
      } else if (columnName.compare(names.BaseTimeNames::del_time       ) == 0) {
        indexNameMap[index] = names.BaseTimeNames::del_time       ;
      } else if (columnName.compare(names.BaseTimeNames::del_time_tz    ) == 0) {
        indexNameMap[index] = names.BaseTimeNames::del_time_tz    ;
      } else {
        logDebug << "未知属性:" << columnName << endl;
      }
      
    }

    return indexNameMap;
  }

  vector<Telephone> Telephone::getTelVector(SqlResult& sqlResult) {
    map<int, string> indexNameMap = getTelNameMap(sqlResult.getColumns());

    TelephoneNames names;
    auto columnCount = indexNameMap.size();
    vector<Telephone> telVector;
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
      
      telVector.push_back(telObj);
    }/* endfor , 知道一个 Vector<telObj> */

    return telVector;
  }
}