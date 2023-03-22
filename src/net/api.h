
#pragma once

#include "src/net/api/authenticate.h"
#include "src/net/http_util.h"

namespace nameless_carpool {
  class Api;
}

class nameless_carpool::Api {
 public:
  /* 查找已定义的 url 处理能力进行处理 */
  static void optRequest(const HttpRequest& requestInput, HttpResponse& responseOutput);

  /* 未知请求处理 */
  static void unknownRequest(const HttpRequest& requestInput, HttpResponse& responseOutput);
};
