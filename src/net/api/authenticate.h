#pragma once

#include <string>

#include "src/net/http_util.h"
#include "include_db_model.h"

/* 管理用户认证相关 api */
namespace nameless_carpool {

  class AuthApi ;
  struct UriPath;

}  // namespace nameless_carpool

struct nameless_carpool::UriPath {
  const std::string pathPrefix        = "/fcgi_test/nameless_carpool";       /* uri 统一前缀 */
  const std::string requestVerifyCode = pathPrefix + "/request_verify_code"; /* 请求验证码 */
  const std::string login             = pathPrefix + "/login";               /* 登录 */
  const std::string inputPositionTip  = pathPrefix + "/input_position_tip";  /* 地点输入提示 */
  const std::string xxFindCar         = pathPrefix + "/find_car";            /* X找车 */
  const std::string carFindXx         = pathPrefix + "/find_customers";      /* 车找X */
  const std::string searchXxFindCar   = pathPrefix + "/find_car";            /* 搜索 X找车 */
  const std::string searchCarFindXx   = pathPrefix + "/find_customers";      /* 搜索 车找X */
};

class nameless_carpool::AuthApi {
 private:
  AuthApi() = default;
  static const UriPath& uriPath() {
    static const UriPath uriPath;
    return uriPath;
  }

 public:
  static AuthApi& getInstance() {
    static AuthApi authApi;
    return authApi;
  }


  inline std::string getUri(const HttpMethod::Enum& method, const std::string& path) { return HttpMethod::enum_to_name(method) + path; }
  inline std::string getPostUri(const std::string& path) { return getUri(HttpMethod::POST, path); }

  /* 请求验证码 */
  inline std::string requestVC() { return getPostUri(uriPath().requestVerifyCode); }
  void        requestVC(const HttpRequest& requestInput, HttpResponse& outResponse);

  /* 登录接口 */
  inline std::string login() { return getPostUri(uriPath().login); }
  void        login(const HttpRequest& requestInput, HttpResponse& outResponse);

  /**
   * 判断 token 是否合法
   * @param inToken
   * @param outErrMsg       非法时 , 有意义
   * @return                true, 合法 ; false , 非法
   */
  [[deprecated("暂时标记弃用 , 用同名函数取代本函数")]]
  bool tokenIsLegal(const std::string& inToken, std::string& outErrMsg);

  /**
   * * 判断 token 是否合法
   * @param inToken
   * @param outErrMsg       return false , 有意义
   * @param outRequestBasic return true , 有意义
   * @return                true, 合法 ; false , 非法
   */
  bool tokenIsLegal(const std::string& inToken, std::string& outErrMsg, RequestBasicInfo& outRequestBasicPtr);

  /* 输入提示 */
  inline std::string inputPositionTip() { return getPostUri(uriPath().inputPositionTip); }
  void               inputPositionTip(const HttpRequest& requestInput, HttpResponse& outResponse);

  /* X找车 */
  inline std::string postXxFindCar() { return getPostUri(uriPath().xxFindCar); }
  void               postXxFindCar(const RequestBasicInfo& requestBasic,
                                 const HttpRequest& requestInput, HttpResponse& outResponse);

  /* 车找X */
  inline std::string postCarFindXx() { return getPostUri(uriPath().carFindXx); }
  void               postCarFindXx(const RequestBasicInfo& requestBasic,
                                   const HttpRequest& requestInput, HttpResponse& outResponse);

  /* 搜索 X找车 */
  inline std::string postSearchXxFindCar() { return getPostUri(uriPath().searchXxFindCar); }
  void               postSearchXxFindCar(const RequestBasicInfo& requestBasic,
                                         const HttpRequest& requestInput, HttpResponse& outResponse);

  /* 搜索 车找X */
  inline std::string postSearchCarFindXx() { return getPostUri(uriPath().searchCarFindXx); }
  void               postSearchCarFindXx(const RequestBasicInfo& requestBasic,
                                         const HttpRequest& requestInput, HttpResponse& outResponse);
};

namespace nameless_carpool{
  inline AuthApi& authApi() { return AuthApi::getInstance(); }
}