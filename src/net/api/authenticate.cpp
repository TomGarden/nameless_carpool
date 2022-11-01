
#include <string>

#include "authenticate.h"
#include "authenticate_m.h"
#include "../../utils/log_utils.h"
#include "../../utils/http_util.h"
#include "../../control/db_proxy.h"

namespace nameless_carpool {

  using namespace std;

  const string AuthApi::uriPrefix = "/fcgi_test/nameless_carpool";  

  extern HttpMethodUtil httpMethodUtil;
  extern HttpHeaderNames httpHeaderNames;

  string AuthApi::getUri(const HttpMethodEnum &method, const string &path) {
    return httpMethodUtil.getName(method) + AuthApi::uriPrefix + path;
  }

  const string AuthApi::loginUri = getUri(HttpMethodEnum::POST, "/login");
  void AuthApi::login(const HttpRequest& requestInput, HttpResponse& responseOutput){


    string inlegalDesc; /* 非法内容描述 */
    bool bodyLegal;
    Login::RequestBody loginBody;

    try {
      requestInput.body.get_to<Login::RequestBody>(loginBody);
      bodyLegal = loginBody.legalityCheck(inlegalDesc);
    } catch(const Json::exception& e) {
      bodyLegal = false;
      inlegalDesc = e.what();
      logInfo << inlegalDesc << endl;
    }
    
    if(!bodyLegal) {
      responseOutput.initResponse(HttpStatusEnum::badRequest, inlegalDesc);
      return;
    }
    
    

    logInfo << "进入登录接口处理块" << endl;
  }

  const string AuthApi::requestVertifyCodeUri = getUri(HttpMethodEnum::POST, "/request_vertify_code");
  void AuthApi::requestVC(const HttpRequest& requestInput, HttpResponse& responseOutput) {

    string inlegalDesc; /* 非法内容描述 */
    bool bodyLegal;
    RequestVertifyCode::RequestBody body;

    try {
      requestInput.body.get_to<RequestVertifyCode::RequestBody>(body);
      bodyLegal = body.legalityCheck(inlegalDesc);
    } catch(const Json::exception& e) {
      bodyLegal = false;
      inlegalDesc = e.what();
      logInfo << inlegalDesc << endl;
    }
    
    if(!bodyLegal) {
      responseOutput.initResponse(HttpStatusEnum::badRequest, inlegalDesc);
      return;
    }
    if(!requestInput.headers.contains(httpHeaderNames.timeZone)) {
      string internalMsg = HttpResponse::HEADER_MISS_ERR + httpHeaderNames.timeZone ;
      responseOutput.initResponse(HttpStatusEnum::badRequest, internalMsg);
      return;
    }

    string internalMsg;
    string externalMsg;
    string timeZone = requestInput.headers[httpHeaderNames.timeZone].get<string>();
    DbProxy::getInstance().requestVertifyCode(body.phone.value(), timeZone, internalMsg, externalMsg);

    responseOutput.setStatus(HttpStatusEnum::success);

  }
}