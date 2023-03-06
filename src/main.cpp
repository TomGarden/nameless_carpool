
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
#include "utils/constant.h"
#include "utils/input_check.h"
#include "utils/json/include_json.h"
#include "utils/log_utils.h"
#include "utils/tom_string_utils.h"
#include "http_util.h"
#include "utils/linux_os.h"
#include "utils/common.h"
#include "net/api.h"
// #include "net/api/authenticate_m.h"

#include "db/sql/db_manager.h"




using namespace std;
using namespace nameless_carpool;
using namespace google;

/** 真正处理 request 请求 */
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
        logInfo << "requestId : " << request.requestId << std::endl;
        logInfo << "role : " << request.role << std::endl;
        char **envp = request.envp;
        for (int index = 0; *envp; index++, envp++) {
          logInfo << "[" << index << "]\t" << *envp << std::endl;
        }

               requestIn  = fcgxStreamRead(request.in);
        string requestOut = fcgxStreamRead(request.out);
        string requestErr = fcgxStreamRead(request.err);
        logInfo << "request.in : " << requestIn << std::endl;
        logInfo << "request.out : " << requestOut << std::endl;
        logInfo << "request.err : " << requestErr << std::endl;
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
        }/* for end */

        /* 排除入参非法的可能性 */
        try {
          httpRequest.setBody(requestIn);
        } catch (const Json::exception& jsonException) {
          logError << jsonException.what() << std::endl;
          httpResponse.inflateResponse(HttpStatusEnum::badRequest, constantStr.bodyFormatErr);
        }

        httpRequest.printSelf();
      }

      if(httpResponse.isEmpty()) {
        optRequest(httpRequest, httpResponse);
      }

      string strResponse = httpResponse.toString();

      FCGX_FPrintF(request.out, strResponse.c_str());

      httpResponse.printSelf();
      logInfo << std::endl << strResponse << std::endl;

    } catch (const exception & except) {
      logError << "程序异常:" << except.what() << "\n" << getStackTrace() << std::endl;
    }

    logFlush();
    FCGX_Finish_r(&request);

  }
}


void testSql() {
  std::string sqlTmp1 = 
      "START TRANSACTION ;                                    \n"
      " INSERT INTO `nameless_carpool`.`user` (               \n"
      "          `id` ,                                       \n"
      "          `id_card_num` ,                              \n"
      "          `name` ,                                     \n"
      "          `gender` ,                                   \n"
      "          `birth_date` ,                               \n"
      "          `birth_date_tz` ,                            \n"
      "          `create_time` ,                              \n"
      "          `create_time_tz` ,                           \n"
      "          `update_time` ,                              \n"
      "          `update_time_tz` ,                           \n"
      "          `del_time` ,                                 \n"
      "          `del_time_tz`                                \n"
      " )                                                     \n"
      " VALUES                                                \n"
      "        (                                              \n"
      "                 NULL ,                                \n"
      "                 NULL ,                                \n"
      "                 NULL ,                                \n"
      "                 NULL ,                                \n"
      "                 NULL ,                                \n"
      "                 NULL ,                                \n"
      "                 '2023-03-06 12:16:45.774661' ,        \n"
      "                 'Asia/Shanghai' ,                     \n"
      "                 '2023-03-06 12:16:45.774661' ,        \n"
      "                 'Asia/Shanghai' ,                     \n"
      "                 NULL ,                                \n"
      "                 NULL                                  \n"
      "        ) ;                                            \n"
      " SET @last_insert_user_id = (SELECT LAST_INSERT_ID()); \n"
      " INSERT INTO `nameless_carpool`.`user_tel` (           \n"
      "          `user_id` ,                                  \n"
      "          `telephone_id` ,                             \n"
      "          `desc` ,                                     \n"
      "          `flag` ,                                     \n"
      "          `create_time` ,                              \n"
      "          `create_time_tz` ,                           \n"
      "          `update_time` ,                              \n"
      "          `update_time_tz` ,                           \n"
      "          `del_time` ,                                 \n"
      "          `del_time_tz`                                \n"
      " )                                                     \n"
      " VALUES                                                \n"
      "        (                                              \n"
      "                 @last_insert_user_id ,                \n"
      "                 '1' ,                                 \n"
      "                 NULL ,                                \n"
      "                 '00000001' ,                          \n"
      "                 '2023-03-06 12:16:45.774661' ,        \n"
      "                 'Asia/Shanghai' ,                     \n"
      "                 '2023-03-06 12:16:45.774661' ,        \n"
      "                 'Asia/Shanghai' ,                     \n"
      "                 NULL ,                                \n"
      "                 NULL                                  \n"
      "        ) ;                                            \n"
      " SELECT @last_insert_user_id ;                         \n"
      "COMMIT ;                                               \n";

  std::string sqlTmp2 =
      "START TRANSACTION ;                                    \n"
      "  SELECT * FROM `nameless_carpool`.`user`;             \n"
      "  SELECT * FROM `nameless_carpool`.`telephone`;        \n"
      "  SELECT * FROM `nameless_carpool`.`user_tel`;         \n"
      "COMMIT ;                                               \n";

  std::vector<std::string> sqlTmp3 = {
      "  SELECT * FROM `nameless_carpool`.`user`;        ",
      "  SELECT * FROM `nameless_carpool`.`telephone`;   ",
      "  SELECT * FROM `nameless_carpool`.`user_tel`;    "};

  DbManager::getInstance().executeTransactionSql(sqlTmp3);
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

  logInfo << "Main 开始执行" ;

  for (int i = 0; i < argc; i++) {
    logInfo << argv[i] << std::endl;
  }

  bool isDebug = contentDebugParam(argc, argv);

  if(isDebug) {
    optLocaleData(argc, argv);
  } else {
    optNetData(argc, argv);
  }

  return 0;
}