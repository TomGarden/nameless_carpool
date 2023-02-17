#pragma once

#include <string>
#include <optional>
#include "../../utils/json/include_json.h"

namespace nameless_carpool {
  using namespace std;
  /** 对 http 请求体的 抽象表达 */
  struct ResponseBody {
    optional<int>       status;       /* 状态码 */
    optional<string>    data;         /* 真正的数据, 可能为空 */
    optional<string>    statusDesc;   /* 状态码解读 */
    optional<string>    internalMsg;  /* 不可对外公开的消息体, 软件内部流转的消息体 */
    optional<string>    externalMsg;  /* 可以对外公开的消息体, 比如对用户提示 */

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ResponseBody, status, statusDesc, data, internalMsg, externalMsg)
    // NLOHMANN_DEFINE_OPTIONAL_TYPE_INTRUSIVE(ResponseBody, status, data, internalMsg, externalMsg)

/* 

NLOHMANN_DEFINE_TYPE_INTRUSIVE(ResponseBody, status, statusDesc, data, internalMsg, externalMsg)
  -> friend void to_json(nlohmann::json& nlohmann_json_j, const Type& nlohmann_json_t) { 
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) 
     }
  ->NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, ResponseBody, status, statusDesc, data, internalMsg, externalMsg) 
    NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_GET_MACRO(NLOHMANN_JSON_TO, ResponseBody, status, statusDesc, data, internalMsg, externalMsg, \
        NLOHMANN_JSON_PASTE64, \
        ...
        NLOHMANN_JSON_PASTE7, \
        NLOHMANN_JSON_PASTE6, \
        NLOHMANN_JSON_PASTE5, \
        NLOHMANN_JSON_PASTE4, \
        NLOHMANN_JSON_PASTE3, \
        NLOHMANN_JSON_PASTE2, \
        NLOHMANN_JSON_PASTE1)(NLOHMANN_JSON_TO, ResponseBody, status, statusDesc, data, internalMsg, externalMsg))
  ->NLOHMANN_JSON_PASTE7(NLOHMANN_JSON_TO, ResponseBody, status, statusDesc, data, internalMsg, externalMsg)
  ->NLOHMANN_JSON_TO(ResponseBody)

 */


  };

}
