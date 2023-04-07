
#pragma once

#include "src/net/api/authenticate.h"
#include "src/net/http_util.h"

namespace nameless_carpool {
  class Api;
}

class nameless_carpool::Api {
 public:

  static void optRequestTry(const HttpRequest& requestInput, HttpResponse& responseOutput);

  /**
   * 查找已定义的 url 处理能力进行处理
   *
   * 关于 入参 的 json 对象应该有一个简明的要求
   * 1. 客户端想要干预的参数值 , 必须传过来
   * 2. 不传过来的参数 服务端按 Null 处理数据
   *
   * @param requestInput
   * @param responseOutput
   */
  static void optRequest(const HttpRequest& requestInput, HttpResponse& responseOutput);

  /* 未知请求处理 */
  static void unknownRequest([[maybe_unused]] const HttpRequest& requestInput, HttpResponse& responseOutput);
};
