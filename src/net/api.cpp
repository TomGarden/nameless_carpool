
#include "api.h"
#include "../model/response_body.h"
// #include "../utils/all_include.h"
#include "../utils/http_util.h"

namespace nameless_carpool {

  void Api::optRequest(const HttpRequest& requestInput, HttpResponse& responseOutput) {
    string methodUri = requestInput.methodUri();

    if(methodUri.compare(AuthApi::loginUri) == 0) {
      AuthApi::login(requestInput, responseOutput);
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
    
    responseOutput.body = Json(bodyObj).dump();
  }
}