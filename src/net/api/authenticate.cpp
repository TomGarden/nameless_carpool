
#include "authenticate.h"

#include <string>

#include "../../control/db_proxy.h"
#include "../../utils/http_util.h"
#include "../../utils/log_utils.h"
#include "authenticate_m.h"

namespace nameless_carpool {

  using namespace std;

  const string AuthApi::uriPrefix = "/fcgi_test/nameless_carpool";

  extern HttpMethodUtil  httpMethodUtil;
  extern HttpHeaderNames httpHeaderNames;

  string AuthApi::getUri(const HttpMethodEnum& method, const string& path) {
    return httpMethodUtil.getName(method) + AuthApi::uriPrefix + path;
  }

  const string AuthApi::requestVertifyCodeUri() {
    return getUri(HttpMethodEnum::POST, "/request_vertify_code");
  }
  void AuthApi::requestVC(const HttpRequest& requestInput, HttpResponse& responseOutput) {
    string                          inlegalDesc; /* 非法内容描述 */
    bool                            bodyLegal;   /* true : body 合法 */
    RequestVertifyCode::RequestBody body;

    try {
      requestInput.body.get_to<RequestVertifyCode::RequestBody>(body);
      bodyLegal = body.legalityCheck(inlegalDesc);
    } catch (const Json::exception& e) {
      bodyLegal   = false;
      inlegalDesc = e.what();
      logInfo << inlegalDesc << endl;
    }

    if (!bodyLegal) { /* 请求体 非法 */
      responseOutput.inflateResponse(HttpStatusEnum::badRequest, inlegalDesc);
      return;
    }
    /* httpHeaderNames.timeZone 请求头 在 Api::optRequest 统一校验了 */

    string internalMsg;
    string externalMsg;
    string timeZone = requestInput.headers[httpHeaderNames.timeZone].get<string>();

    const HttpStatusEnum& result = DbProxy::getInstance().requestVertifyCode(
        body.phone.value(), timeZone, internalMsg, externalMsg);

    responseOutput.inflateResponse(result, internalMsg, externalMsg);
  }

  const string AuthApi::loginUri() {
    return getUri(HttpMethodEnum::POST, "/login");
  }
  void AuthApi::login(const HttpRequest& requestInput, HttpResponse& responseOutput) {
    string             inlegalDesc; /* 非法内容描述 */
    bool               bodyLegal;
    Login::RequestBody loginBody;

    try {
      requestInput.body.get_to<Login::RequestBody>(loginBody);
      bodyLegal = loginBody.legalityCheck(inlegalDesc);
    } catch (const Json::exception& e) {
      bodyLegal   = false;
      inlegalDesc = e.what();
      logInfo << inlegalDesc << endl;
    }

    if (!bodyLegal) {
      responseOutput.inflateResponse(HttpStatusEnum::badRequest, inlegalDesc);
      return;
    }

    logInfo << "进入登录接口处理块" << endl;

    bool loginCodeDone = DbProxy::getInstance().login(requestInput, requestInput.body, responseOutput);

    if(loginCodeDone) {

    } else {
      responseOutput.clear();
      responseOutput.inflateResponse(HttpStatusEnum::unknowErr);
    }




    /* 先通过 token 判断是否已经登陆了 */
    // if (requestInput.headers.contains(httpHeaderNames.token)) {
    // }

    /* 后通过 手机号 查询用户名判断是否已经登陆了 
    
      查询对应手机号的验证码是否有效 , 验证码有效 , 
    
    */

    // DbProxy::getInstance().

    /* 
    是否登录先通过 token 判断

    1. 查询是否已经登陆 , 
        已经登录应该更新 token 失效时间 , 并重新返回 token 
    2. 没有登录的情况下 , 需要创建登录信息 , 并返回 token
     */
  }

}  // namespace nameless_carpool