
#include "include_json.h"

namespace nameless_carpool {
  nlohmann::json& clearNull(nlohmann::json& jObj) {
//    方法 ①
//    jObj.erase(std::remove(jObj.begin(), jObj.end(), nullptr), jObj.end());
//    等同于 方法 ②
//    nlohmann::json::iterator jBegin = std::remove_if(
//        jObj.begin(), jObj.end(), [](const nlohmann::json& item) { return item.is_null(); });
//    jObj.erase(jBegin, jObj.end());
//    猜测是库 BUG

    jObj = jObj.flatten();
    nlohmann::json jClone = std::move(jObj);
    /* jObj.clear(); */

    for (auto it = jClone.begin(); it != jClone.end(); it++) {
      if (it.value().is_null()) continue;
      jObj[it.key()] = std::move(it.value());
    }

    jObj = jObj.unflatten();


    //    nlohmann::json jClone = jObj;
//    jObj.clear();
//
//    for (const auto& item : jClone.items()) {
//      if (!item.value().is_null()) {
//        jObj[item.key()] = item.value();
//      } else if (item.value().is_object()) {
//        nlohmann::json jSubObj = clearNull(item.value());
//        if (!jSubObj.empty()) jObj[item.key()] = jSubObj;
//      } else if(item.value().is_array() && !item.value().empty()) {
//        jObj[item.key()] = item.value();
//      }
//    }

    logInfo << jObj.dump() << logEndl;

    return jObj;
  }
}