
#include <typeinfo>
#include <locale>
#include <codecvt>
#include <iostream>
#include <clocale>
#include <cwchar>
#include <fstream>
#include <any>

#include <boost/version.hpp>


#include "db/model/user_info.h"
#include "utils/input_check.h"
#include "utils/json/include_json.h"
#include "utils/log_utils.h"
#include "utils/tom_string_utils.h"
#include "utils/http_util.h"
#include "utils/linux_os.h"
#include "utils/common.h"
#include "net/api.h"
#include "net/api/authenticate_m.h"

#include "db/sql/db_manager.h"




using namespace std;
using namespace nameless_carpool;
using namespace google;


void optRequest(const HttpRequest& requestInput, HttpResponse& responseOutput) {
  Api::optRequest(requestInput, responseOutput);
}

/* 操作本地数据 , 
   检查是否是在命令行做数据调试 , 如果是 , 做进一步的逻辑处理 return true
   如果不是 return false */
void optLocaleData(int argc, char **argv) {

  HttpRequest httpRequest;
  HttpResponse httpResponse;
  bool operateRequest = accept(argc, argv, httpRequest, httpResponse);

  httpRequest.printSelf();

  if(operateRequest) {
    optRequest(httpRequest, httpResponse.clear());
  } 
  httpResponse.printSelf();
}

/* 操作网络数据 */
void optNetData(int argc, char **argv) {
  FCGX_Request request;
  FCGX_Init();
  FCGX_InitRequest(&request, 0, 0);

  while (FCGX_Accept_r(&request) == 0) {
    try {


      string requestIn;
      { /* 打印从 nginx 读取到的部分内容 */
        logInfo << "requestId : " << request.requestId << endl;
        logInfo << "role : " << request.role << endl;
        char **envp = request.envp;
        for (int index = 0; *envp; index++, envp++) {
          logInfo << "[" << index << "]\t" << *envp << endl;
        }

               requestIn  = fcgxStreamRead(request.in);
        string requestOut = fcgxStreamRead(request.out);
        string requestErr = fcgxStreamRead(request.err);
        logInfo << "request.in : " << requestIn << endl;
        logInfo << "request.out : " << requestOut << endl;
        logInfo << "request.err : " << requestErr << endl;
      }

      HttpResponse httpResponse;
      HttpRequest httpRequest;
      { /* 只获取我们关心的数据 HttpRequest */
        char **envp = request.envp;
        for (int index = 0; *envp; index++, envp++) {
          // string env = utfMbsrtowcs(*envp);
          string env = *envp;
          int equalIndex = env.find('=');
          string key = env.substr(0, equalIndex);
          if(key == "REQUEST_METHOD") {
            httpRequest.method = env.substr(equalIndex + 1);
          } else if(key == "REQUEST_URI") {
            httpRequest.uri = env.substr(equalIndex + 1);
          } else {
            string prefix = env.substr(0, 5);
            if(prefix == "HTTP_") {
              string value = env.substr(equalIndex+1);
              httpRequest.addHeader(key, value);
            }
          }
        }

        /* 排除入参非法的可能性 */
        try {
          httpRequest.setBody(requestIn);
        } catch (const Json::exception& jsonException) {
          logError << jsonException.what() << endl;
          httpResponse.initResponse(HttpStatusEnum::badRequest, HttpResponse::BODY_FORMAT_ERR);
        }

        httpRequest.printSelf();
      }

      if(httpResponse.isEmpty()) {
        optRequest(httpRequest, httpResponse);
      }

      string strResponse = httpResponse.toString();

      FCGX_FPrintF(request.out, strResponse.c_str());

      httpResponse.printSelf();
      logInfo << endl << strResponse << endl;

    } catch (const exception & except) {
      logError << "程序异常:" << except.what() << "\n" << getStackTrace() << endl;
    }

    logFlush();
    FCGX_Finish_r(&request);

  }
}


/**
 kill -9 NULL && \
spawn-fcgi  \
-a 127.0.0.1 \
-p 9000 \
-f /mount_point/data/_document/c_cpp_program/nameless_carpool/build_src/execute.run \
-d /mount_point/data/_document/c_cpp_program/nameless_carpool/tmp_dir/run_path \

 * 
*/
int main(int argc, char ** argv) {

  setlocale(LC_CTYPE, tom_utils::defLocalStr); //必须在 initGlog 之前设置完成
  initGlog(argv[0]);

  for (int i = 0; i < argc; i++) {
    logInfo << argv[i] << endl;
  }

  bool isDebug = contentDebugParam(argc, argv);

  if(isDebug) {
    optLocaleData(argc, argv);
  } else {
    optNetData(argc, argv);
  }

  return 0;
}