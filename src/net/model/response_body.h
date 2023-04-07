#pragma once

#include <string>
#include <optional>
#include "src/utils/json/include_json.h"
#include "for_model.h"

namespace nameless_carpool {
  /** 对 http 请求体的 抽象表达 */
  struct ResponseBody {
    struct Names {
      const std::string status      = "status";
      const std::string data        = "data";
      const std::string statusDesc  = "statusDesc";
      const std::string internalMsg = "internalMsg";
      const std::string externalMsg = "externalMsg";
    };
    GET_NAMES()


    std::optional<int>         status;      /* 状态码 */
    std::optional<std::string> data;        /* 真正的数据, 可能为空 */
    std::optional<std::string> statusDesc;  /* 状态码解读 */
    std::optional<std::string> internalMsg; /* 不可对外公开的消息体, 软件内部流转的消息体 */
    std::optional<std::string> externalMsg; /* 可以对外公开的消息体, 比如对用户提示 */

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ResponseBody, status, statusDesc, data, internalMsg, externalMsg)
  };

}
