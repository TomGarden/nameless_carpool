
#include <typeinfo>
#include "model/user.h"
#include "../libs/nlohmann_json_3.10.5/json.hpp"
#include "utils/all_include.h"

#include <locale>
#include <codecvt>
#include <iostream>
#include <clocale>
#include <cwchar>



using namespace std;
using namespace nameless_carpool;
using namespace google;


void optData(const HttpRequest& requestInput, HttpResponse& responseOutput) {
  
}

/* 操作本地数据 , 
   检查是否是在命令行做数据调试 , 如果是 , 做进一步的逻辑处理 return true
   如果不是 return false */
void optLocaleData(int argc, char **argv) {

  HttpRequest httpRequest;
  HttpResponse httpResponse;
  bool optRequest = accept(argc, argv, httpRequest, httpResponse);

  if(optRequest) {
    optData(httpRequest, httpResponse.clear());
    LOG(FATAL) << "暂未实现此逻辑" << endl;
  } else {
    LOG(ERROR) << endl 
               << httpResponse.toString() 
               << endl << endl << endl;
  }

}

/* 操作网络数据 */
void optNetData(int argc, char **argv) {
  FCGX_Request request;
  FCGX_Init();
  FCGX_InitRequest(&request, 0, 0);

  while (FCGX_Accept_r(&request) == 0) {
    try {

      { /* 打印从 nginx 读取大的所有内容 */
        LOG(INFO) << "requestId : " << request.requestId << endl;
        LOG(INFO) << "role : " << request.role << endl;
        char **envp = request.envp;
        for (int index = 0; *envp; index++, envp++) {
          LOG(INFO) << "[" << index << "]\t" << *envp << endl;
        }
        LOG(INFO) << "request.in : " << fcgxStreamRead(request.in) << endl;
        LOG(INFO) << "request.out : " << fcgxStreamRead(request.out) << endl;
        LOG(INFO) << "request.err : " << fcgxStreamRead(request.err) << endl;
      }


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
            httpRequest.function = env.substr(equalIndex + 1);
          } else {
            string prefix = env.substr(0, 5);
            if(prefix == "HTTP_") {
              string value = env.substr(equalIndex+1);
              httpRequest.addHeader(key, value);
            }
          }
        }
        httpRequest.body = fcgxStreamRead(request.in);
      }

      HttpResponse httpResponse;
      optData(httpRequest, httpResponse);

      // httpResponse.addHeader(L"testKey",L"testValue")
      //             .setBody(L"{ 123, wc_uid, wc_number}");

      string strResponse = httpResponse.toString();
      const char *response = strResponse.c_str();

      LOG(INFO) << "返回值输出2 : \n" << response << endl;

      FCGX_FPrintF(request.out, response);

    } catch (const exception & except) {
      LOG(ERROR) << "程序异常:" << except.what() << "\n" << getStackTrace() << endl;
    }

    gLogFlushAllLogFiles();
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

  setlocale(LC_CTYPE, "C.UTF-8"); //必须在 initGlog 之前设置完成
  initGlog(argv[0]);

  for (int i = 0; i < argc; i++) {
    LOG(INFO) << argv[i] << endl;
  }

  bool isDebug = contentDebugParam(argc, argv);

  if(isDebug) {
    optLocaleData(argc, argv);
  } else {
    optNetData(argc, argv);
  }

  return 0;
}