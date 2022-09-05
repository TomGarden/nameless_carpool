#pragma once

#include <string>
// #include "../../utils/all_include.h"
#include "../../utils/http_util.h"

/* 管理用户认证相关 api */
namespace nameless_carpool {

  using namespace std;

  class AuthApi {
    public: 
      static const string uriPrefix;/* uri 统一前缀 */

      /* 登录接口 */
      static const string loginUri;
      static void login(const HttpRequest& requestInput, HttpResponse& responseOutput);
  };

}