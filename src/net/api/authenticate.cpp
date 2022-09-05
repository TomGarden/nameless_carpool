
#include "authenticate.h"
#include "authenticate_m.h"

namespace nameless_carpool {

  using namespace std;

  const string AuthApi::uriPrefix = "/fcgi_test/nameless_carpool";
  
  extern HttpMethodUtil httpMethodUtil;
  const string AuthApi::loginUri = httpMethodUtil.getName(HttpMethodEnum::POST) + 
                                   AuthApi::uriPrefix + "/login";
  void AuthApi::login(const HttpRequest& requestInput, HttpResponse& responseOutput){

    Json jsonBody = requestInput.body;
    Body::Login loginBody = jsonBody.get<Body::Login>();

    string inlegalDesc; /* 非法内容描述 */
    bool bodyLegal = loginBody.legalityCheck(inlegalDesc);

    if(!bodyLegal) {
      responseOutput.initByInlegalRequest(inlegalDesc);
    } else {
      LOG(FATAL) << "请求合法 , 应该完成请求" << endl;
    }

    LOG(INFO) << "进入登录接口处理块" << endl;
  }

}