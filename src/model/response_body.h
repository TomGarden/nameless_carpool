#pragma once
#include <string>

namespace nameless_carpool {

  using namespace std;

  /** 对 http 请求体的 抽象表达 */
  struct ResponseBody {
    int status;           /* 状态码 */
    wstring data;         /* 真正的数据, 可能为空 */
    wstring internalMsg;  /* 不可对外公开的消息体, 软件内部流转的消息体 */
    wstring externalMsg;  /* 可以对外公开的消息体, 比如对用户提示 */

    wstring toJsonW();
  };

}
