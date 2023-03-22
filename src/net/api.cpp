
#include "api.h"
#include "src/net/model/response_body.h"
#include "src/net/http_util.h"
#include "src/utils/constant.h"

namespace nameless_carpool {

  void Api::optRequest(const HttpRequest& requestInput, HttpResponse& responseOutput) {
    const std::string& methodUri = requestInput.methodUri();

    { /* 请求合法性校验 */
      if (!requestInput.headers.contains(httpHeaderNames.timeZone)) { /* 请求头非法 */
        std::string internalMsg = constantStr.headerMissErr + httpHeaderNames.timeZone;
        responseOutput.inflateResponse(HttpStatus::Enum::badRequest, internalMsg);
        return;
      }

      if (!requestInput.headers.contains(httpHeaderNames.token)) {
        if(AuthApi::loginUri().compare(methodUri) == 0) { /* empty */ }
        else if(AuthApi::requestVertifyCodeUri().compare(methodUri) == 0) { /* empty */ }
        else {
        std::string internalMsg = constantStr.headerMissErr + httpHeaderNames.token;
        responseOutput.inflateResponse(HttpStatus::Enum::badRequest, internalMsg);
        return;
        }
      }
    }

    if(AuthApi::loginUri().compare(methodUri) == 0) {
      AuthApi::login(requestInput, responseOutput);
    } else if(AuthApi::requestVertifyCodeUri().compare(methodUri) == 0) {
      AuthApi::requestVC(requestInput, responseOutput);
    }
    else {
      Api::unknownRequest(requestInput, responseOutput);
    }
  }

  void Api::unknownRequest(const HttpRequest& requestInput, HttpResponse& responseOutput) {
    std::string statusName = httpStatus.getName(HttpStatus::Enum::requestUndefined);
    std::string statusDesc = httpStatus.getDesc(HttpStatus::Enum::requestUndefined);
    responseOutput.status = HttpStatus::Enum::requestUndefined;

    ResponseBody bodyObj;
    bodyObj.status = responseOutput.getIntStatus();
    bodyObj.internalMsg = statusName + " : " + statusDesc;
    
    responseOutput.setBody(bodyObj);
  }
}