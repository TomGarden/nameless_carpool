
#include "src/net/api/authenticate.h"

#include <string>

#include "db_proxy.h"
#include "src/net/http_util.h"
#include "src/utils/log_utils.h"
// #include "authenticate_m.h"
#include "a_map.h"
#include "application.h"
#include "input_tip.h"
#include "net/model/authenticate.h"
#include "find_xxx.h"

namespace nameless_carpool {

  extern HttpHeaderNames httpHeaderNames;

  void AuthApi::requestVC(const HttpRequest& requestInput, HttpResponse& outResponse) {
    std::string                          inlegalDesc; /* 非法内容描述 */
    bool                            bodyLegal;   /* true : body 合法 */
    RequestVerifyCode::RequestBody body;

    try {
      requestInput.body.get_to<RequestVerifyCode::RequestBody>(body);
      bodyLegal = body.legalityCheck(inlegalDesc);
    } catch (const nlohmann::json::exception& objException) {
      bodyLegal   = false;
      inlegalDesc = objException.what();
      logInfo << inlegalDesc << endl;
    }

    if (!bodyLegal) { /* 请求体 非法 */
      outResponse.inflateResponse(HttpStatus::Enum::badRequest, inlegalDesc);
      return;
    }
    /* HttpHeaderNames::instance().timeZone 请求头 在 Api::optRequest 统一校验了 */

    std::string internalMsg;
    std::string externalMsg;
    std::string timeZone = requestInput.headers[HttpHeaderNames::instance().timeZone].get<std::string>();

    const HttpStatus::Enum& result = DbProxy::getInstance().requestVerifyCode(
        body.phone.value(), timeZone, internalMsg, externalMsg);

    outResponse.inflateResponse(result, internalMsg, externalMsg);
  }

  void AuthApi::login(const HttpRequest& requestInput, HttpResponse& outResponse) {
    std::string        illegalDesc; /* 非法内容描述 */
    bool               bodyLegal;
    Login::RequestBody loginBody;

    try {
      requestInput.body.get_to<Login::RequestBody>(loginBody);
      bodyLegal = loginBody.legalityCheck(illegalDesc);
    } catch (const nlohmann::json::exception& e) {
      bodyLegal   = false;
      illegalDesc = e.what();
      logInfo << illegalDesc << endl;
    }

    if (!bodyLegal) {
      outResponse.inflateResponse(HttpStatus::Enum::badRequest, illegalDesc);
      return;
    }

    logInfo << "进入登录接口处理块" << endl;

    bool loginSuccess = DbProxy::getInstance().login(requestInput, loginBody, outResponse);

    if(loginSuccess) {

    } else {
      if (outResponse.isEmpty()) outResponse.inflateResponse(HttpStatus::Enum::unknowErr);
    }




    /* 先通过 token 判断是否已经登陆了 */
    // if (requestInput.headers.contains(HttpHeaderNames::instance().token)) {
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

  void AuthApi::inputPositionTip(const HttpRequest& requestInput, HttpResponse& outResponse) {
    InputTipRequest inputTip  = requestInput.body.get<InputTipRequest>();
    inputTip.key = app().localConfig.amap_key.value_or("");
    if (!inputTip.isLegal()) {
      outResponse.inflateResponse(HttpStatus::Enum::badRequest, WITH_LINE_INFO("必填值无效"));
      return;
    }

    nlohmann::json jObj = inputTip;
    aMap().doPositionInputTip(clearNull(jObj), outResponse);
  }

  bool AuthApi::tokenIsLegal(const std::string& inToken, std::string& outErrMsg) { return dbProxy().tokenIsLegal(inToken, outErrMsg); }

  bool AuthApi::tokenIsLegal(const std::string& inToken, std::string& outErrMsg, RequestBasicInfo& outRequestBasicPtr) {
    return dbProxy().tokenIsLegal(inToken, outErrMsg, outRequestBasicPtr);
  }

  void AuthApi::postXxFindCar(const RequestBasicInfo& requestBasic,
                            const HttpRequest&      requestInput,
                            HttpResponse&           outResponse) {
    body::FindCarBody findCarBody = requestInput.body.get<body::FindCarBody>();
    /* body 合法性校验 */ {
      std::string errMsg;
      if (!findCarBody.isLegal(errMsg)) {
        outResponse.inflateResponse(HttpStatus::Enum::badRequest, errMsg);
        return;
      }
    }

    /* 确保数据合法后才做进一步逻辑处理 */
    dbProxy().postFindCar(requestInput, requestBasic, findCarBody, outResponse.clear());
  }

  void AuthApi::postCarFindXx(const RequestBasicInfo& requestBasic,
                                  const HttpRequest&      requestInput,
                                  HttpResponse&           outResponse) {
    body::FindCustomersBody findBody = requestInput.body.get<body::FindCustomersBody>();
    /* body 合法性校验 */ {
      std::string errMsg;
      if (!findBody.isLegal(errMsg)) {
        outResponse.inflateResponse(HttpStatus::Enum::badRequest, errMsg);
        return;
      }
    }

    dbProxy().postFindCustomers(requestInput, requestBasic, findBody, outResponse);
  }

  /** X找车 页面发起搜索 */
  void AuthApi::postSearchXxFindCar(const RequestBasicInfo& requestBasic,
                                    const HttpRequest& requestInput, HttpResponse& outResponse) {
    body::FindCarBody findCarBody = requestInput.body.get<body::FindCarBody>();
    dbProxy().postSearchXxFindCar(requestInput, requestBasic, findCarBody, outResponse);
  }

  /** 车找X 页面发起搜索 */
  void AuthApi::postSearchCarFindXx(const RequestBasicInfo& requestBasic,
                                    const HttpRequest& requestInput, HttpResponse& outResponse) {
  }


}  // namespace nameless_carpool