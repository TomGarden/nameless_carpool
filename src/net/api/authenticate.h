#pragma once

#include <string>

#include "src/net/http_util.h"

/* 管理用户认证相关 api */
namespace nameless_carpool {

  class AuthApi ;
  struct UriPath;

}  // namespace nameless_carpool

struct nameless_carpool::UriPath {
  const std::string pathPrefix            = "/fcgi_test/nameless_carpool";       /* uri 统一前缀 */
  const std::string requestVerifyCode     = pathPrefix + "/request_verify_code"; /*请求验证码*/
  const std::string login                 = pathPrefix + "/login";               /*登录*/
};

class nameless_carpool::AuthApi {
 public:
  static UriPath uriPath() {
    static const UriPath uriPath;
    return uriPath;
  }
  static std::string getUri(const HttpMethodEnum& method, const std::string& path) { return httpMethodUtil.getName(method) + path; }
  static std::string getPostUri(const std::string& path) { return getUri(HttpMethodEnum::POST, path); }

  /* 请求验证码 */
  static std::string requestVerifyCodeUri() { return getPostUri(uriPath().requestVerifyCode); }
  static void        requestVC(const HttpRequest& requestInput, HttpResponse& outResponse);

  /* 登录接口 */
  static std::string loginUri() { return getPostUri(uriPath().login); }
  static void        login(const HttpRequest& requestInput, HttpResponse& outResponse);
};