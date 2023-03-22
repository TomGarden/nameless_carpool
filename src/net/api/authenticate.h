#pragma once

#include <string>

#include "src/net/http_util.h"

/* 管理用户认证相关 api */
namespace nameless_carpool {

  class AuthApi ;
  struct LogicUri;

}  // namespace nameless_carpool

struct nameless_carpool::LogicUri {
  const std::string uriPrefix             = "/fcgi_test/nameless_carpool"; /* uri 统一前缀 */
  const std::string uriRequestVerifyCode = uriPrefix + "/request_verify_code";
  const std::string uriLogin              = uriPrefix + "/login";
};

class nameless_carpool::AuthApi {
 public:
  static const std::string uriPrefix; /* uri 统一前缀 */

  static std::string getUri(const HttpMethodEnum& method, const std::string& path);

  /* 请求验证码 */
  static std::string requestVerifyCodeUri();
  static void        requestVC(const HttpRequest& requestInput, HttpResponse& outResponse);

  /* 登录接口 */
  static std::string loginUri();
  static void        login(const HttpRequest& requestInput, HttpResponse& outResponse);
};