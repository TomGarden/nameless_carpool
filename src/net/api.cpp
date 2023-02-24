
#include "api.h"
#include "response_body.h"
#include "http_util.h"
#include "constant.h"

namespace nameless_carpool {

  void Api::optRequest(const HttpRequest& requestInput, HttpResponse& responseOutput) {
    const string& methodUri = requestInput.methodUri();

    { /* 请求合法性校验 */
      if (!requestInput.headers.contains(httpHeaderNames.timeZone)) { /* 请求头非法 */
        string internalMsg = constantStr.headerMissErr + httpHeaderNames.timeZone;
        responseOutput.inflateResponse(HttpStatusEnum::badRequest, internalMsg);
        return;
      }

      if (!requestInput.headers.contains(httpHeaderNames.token)) {
        if(AuthApi::loginUri().compare(methodUri) == 0) { /* empty */ }
        else if(AuthApi::requestVertifyCodeUri().compare(methodUri) == 0) { /* empty */ }
        else {
        std::string internalMsg = constantStr.headerMissErr + httpHeaderNames.token;
        responseOutput.inflateResponse(HttpStatusEnum::badRequest, internalMsg);
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
    string statusName = httpStatus.getName(HttpStatusEnum::requestUndefined);
    string statusDesc = httpStatus.getDesc(HttpStatusEnum::requestUndefined);
    responseOutput.status = static_cast<int>(HttpStatusEnum::requestUndefined);

    ResponseBody bodyObj;
    bodyObj.status = responseOutput.status;
    bodyObj.internalMsg = statusName + " : " + statusDesc;
    
    responseOutput.setBody(bodyObj);
  }
}