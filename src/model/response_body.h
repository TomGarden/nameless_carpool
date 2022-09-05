#pragma once

#include <string>

namespace nameless_carpool {

  /** 对 http 请求体的 抽象表达 */
  struct ResponseBody {
    int status;          /* 状态码 */
    string data;         /* 真正的数据, 可能为空 */
    string statusDesc;   /* 状态码解读 */
    string internalMsg;  /* 不可对外公开的消息体, 软件内部流转的消息体 */
    string externalMsg;  /* 可以对外公开的消息体, 比如对用户提示 */

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ResponseBody, status, data, internalMsg, externalMsg)
  };

}
