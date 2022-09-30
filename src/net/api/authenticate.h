#pragma once

#include <string>
#include "../../utils/http_util.h"

/* 管理用户认证相关 api */
namespace nameless_carpool {

  using namespace std;

  class AuthApi {
    public: 
      static const string uriPrefix;/* uri 统一前缀 */

      static string getUri(const HttpMethodEnum& method, const string& path);

      /* 登录接口 */
      static const string loginUri;
      static void login(const HttpRequest& requestInput, HttpResponse& responseOutput);

      static const string requestVertifyCodeUri;
      static void requestVC(const HttpRequest& requestInput, HttpResponse& responseOutput);
  };

}