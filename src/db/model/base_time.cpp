#include "base_time.h"
#include "../../utils/common.h"
#include "../sql/db_manager.h"

namespace nameless_carpool {
  

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
}