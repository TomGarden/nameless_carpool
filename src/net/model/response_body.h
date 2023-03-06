#pragma once

#include <string>
#include <optional>
#include "include_json.h"

namespace nameless_carpool {
  /** 对 http 请求体的 抽象表达 */
  struct ResponseBody {

    struct Names {
      static inline std::string status() { return "status"; }
      static inline std::string data() { return "data"; }
      static inline std::string statusDesc() { return "statusDesc"; }
      static inline std::string internalMsg() { return "internalMsg"; }
      static inline std::string externalMsg() { return "externalMsg"; }
    };


    optional<int>         status;      /* 状态码 */
    optional<std::string> data;        /* 真正的数据, 可能为空 */
    optional<std::string> statusDesc;  /* 状态码解读 */
    optional<std::string> internalMsg; /* 不可对外公开的消息体, 软件内部流转的消息体 */
    optional<std::string> externalMsg; /* 可以对外公开的消息体, 比如对用户提示 */

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ResponseBody, status, statusDesc, data, internalMsg, externalMsg)



  };

}
