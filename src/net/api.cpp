
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
        if (authApi().loginUri() != methodUri &&
            authApi().requestVerifyCodeUri() != methodUri) {
          std::string internalMsg = httpStatus.getDesc(HttpStatus::Enum::requestUndefined) + " 请求未定义 / 请求链接含错别字 / 未携带 token ";
          responseOutput.inflateResponse(HttpStatus::Enum::requestUndefined, WITH_LINE_INFO(internalMsg));
          return;
        }
      } else { /* 其余都是有 token 的 , 需要校验 token 合法性 */
        const std::string& strToken = requestInput.headers[httpHeaderNames.token];
        std::string internalMsg;
        bool legal = authApi().tokenIsLegal(strToken, internalMsg);
        if(!legal) {
          responseOutput.inflateResponse(HttpStatus::Enum::forbidden, WITH_LINE_INFO(internalMsg));
          return;
        }
      }
    }

    if (/**/ methodUri == authApi().loginUri()) authApi().login(requestInput, responseOutput);
    else if (methodUri == authApi().requestVerifyCodeUri()) authApi().requestVC(requestInput, responseOutput);
    else if (methodUri == authApi().inputPositionTip()) authApi().inputPositionTip(requestInput, responseOutput);
    else Api::unknownRequest(requestInput, responseOutput);

    responseOutput.appendDefaultHeaders();
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