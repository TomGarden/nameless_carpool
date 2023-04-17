
#include "api.h"
#include "src/net/model/response_body.h"
#include "src/net/http_util.h"
#include "src/utils/constant.h"
#include "include_db_model.h"

namespace nameless_carpool {

  void Api::optRequestTry(const HttpRequest& requestInput, HttpResponse& responseOutput) {
    try {
      optRequest(requestInput, responseOutput);
    } catch (const std::exception& objException) {
      logError << objException.what() << logEndl;
    }
  }

  void Api::optRequest(const HttpRequest& requestInput, HttpResponse& responseOutput) {
    const std::string& methodUri = requestInput.methodUri();
    RequestBasicInfo requestBasicPtr;

    /* 请求合法性校验 , 通过条件 : 时区信息存在 */ {
      if (!requestInput.headers.contains(HttpHeaderNames::instance().timeZone)) { /* 请求头非法 */
        std::string internalMsg = constantStr.headerMissErr + HttpHeaderNames::instance().timeZone;
        responseOutput.inflateResponse(HttpStatus::Enum::badRequest, internalMsg);
        return;
      }

      /* 请求合法性校验 , 通过条件 : token信息存在 */
      if (!requestInput.headers.contains(HttpHeaderNames::instance().token)) {
        if (authApi().login() != methodUri &&
            authApi().requestVC() != methodUri) {
          std::string internalMsg = HttpStatus::enum_to_desc(HttpStatus::Enum::requestUndefined) + " 请求未定义 / 请求链接含错别字 / 未携带 token ";
          responseOutput.inflateResponse(HttpStatus::Enum::requestUndefined, WITH_LINE_INFO(internalMsg));
          return;
        }
      } else { /* 其余都是有 token 的 , 需要校验 token 合法性 */
        const std::string& strToken = requestInput.getToken();
        std::string internalMsg;
        bool legal = authApi().tokenIsLegal(strToken, internalMsg, requestBasicPtr);
        if(!legal) {
          responseOutput.inflateResponse(HttpStatus::Enum::forbidden, WITH_LINE_INFO(internalMsg));
          return;
        }
      }
    }

    #define IF_CHECK_URI(call)                       if (methodUri == call()) call(requestInput, responseOutput);
    #define ELSE_IF_CHECK_URI(call)                  else if (methodUri == call()) call(requestInput, responseOutput);
    #define ELSE_IF_CHECK_URI_WITH_BASIC_INFO(call)  else if (methodUri == call()) call(requestBasicPtr, requestInput, responseOutput);

    IF_CHECK_URI(authApi().login)
    ELSE_IF_CHECK_URI(authApi().requestVC)
    ELSE_IF_CHECK_URI(authApi().inputPositionTip)
    ELSE_IF_CHECK_URI_WITH_BASIC_INFO(authApi().postXxFindCar)
    ELSE_IF_CHECK_URI_WITH_BASIC_INFO(authApi().postCarFindXx)
    ELSE_IF_CHECK_URI_WITH_BASIC_INFO(authApi().postSearchXxFindCar)
    ELSE_IF_CHECK_URI_WITH_BASIC_INFO(authApi().postSearchCarFindXx)
    else Api::unknownRequest(requestInput, responseOutput);

    clearNull(responseOutput.body);
    responseOutput.appendDefaultHeaders();
  }

  void Api::unknownRequest(const HttpRequest& requestInput, HttpResponse& responseOutput) {
    std::string statusName = HttpStatus::enum_to_str(HttpStatus::Enum::requestUndefined);
    std::string statusDesc = HttpStatus::enum_to_desc(HttpStatus::Enum::requestUndefined);
    responseOutput.status = HttpStatus::Enum::requestUndefined;

    ResponseBody bodyObj;
    bodyObj.status = responseOutput.getIntStatus();
    bodyObj.internalMsg = statusName + " : " + statusDesc;
    
    responseOutput.setBody(bodyObj);
  }
}