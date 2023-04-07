
#include <boost/algorithm/string.hpp>
#include <clocale>
#include <fstream>
#include <iostream>
#include <typeinfo>

#include "drogon/drogon.h"
#include "src/db/sql/db_manager.h"
#include "src/net/api.h"
#include "src/net/drogon_proxy/web_client.h"
#include "src/net/drogon_proxy/web_server.h"
#include "src/net/http_util.h"
#include "src/utils/constant.h"
#include "src/utils/input_check.h"
#include "src/utils/json/include_json.h"
#include "src/utils/linux_os.h"
#include "src/utils/log_utils.h"
#include "src/utils/tom_string_utils.h"
#include "test_drogon.h"
#include "tz.h"



using namespace nameless_carpool;

/** 真正处理 request 请求 */
void optRequest(const HttpRequest& requestInput, HttpResponse& responseOutput) {
  Api::optRequestTry(requestInput, responseOutput);
}

/* 操作本地数据 ,
   检查是否是在命令行做数据调试 , 如果是 , 做进一步的逻辑处理 return true
   如果不是 return false */
void optLocaleData(int argc, char** argv) {
  HttpRequest  httpRequest;
  HttpResponse httpResponse;
  bool         operateRequest = accept(argc, argv, httpRequest, httpResponse);

  httpRequest.printSelf();

  if (operateRequest) {
    optRequest(httpRequest, httpResponse.clear());
  }
  httpResponse.printSelf();
}

/* 操作网络数据 , 这里使用的 是 fast_cgi 来接收网络请求 , 但是接收到之后的事件派发逻辑我们没有实现所以转而寻求 web 框架的帮助 */
void optNetData(int argc, char** argv) {
  FCGX_Request request;
  FCGX_Init();
  FCGX_InitRequest(&request, 0, 0);

  while (FCGX_Accept_r(&request) == 0) {
    try {
      std::string requestIn;
      { /* 打印从 nginx 读取到的部分内容 */
        logInfo << "requestId : " << request.requestId << std::endl;
        logInfo << "role : " << request.role << std::endl;
        char** pString = request.envp;
        for (int index = 0; *pString; index++, pString++) {
          logInfo << "[" << index << "]\t" << *pString << std::endl;
        }

        requestIn                     = fcgxStreamRead(request.in);
        const std::string& requestOut = fcgxStreamRead(request.out);
        const std::string& requestErr = fcgxStreamRead(request.err);
        logInfo << "request.in : " << requestIn << std::endl;
        logInfo << "request.out : " << requestOut << std::endl;
        logInfo << "request.err : " << requestErr << std::endl;
      }

      HttpResponse httpResponse;
      HttpRequest  httpRequest;
      { /* 只获取我们关心的数据 HttpRequest */
        char** envp = request.envp;
        for (int index = 0; *envp; index++, envp++) {
          const std::string& env        = *envp;
          int                equalIndex = env.find('=');
          const std::string& key        = env.substr(0, equalIndex);
          if (key == "REQUEST_METHOD") {
            httpRequest.method = env.substr(equalIndex + 1);
          } else if (key == "REQUEST_URI") {
            httpRequest.uri = env.substr(equalIndex + 1);
          } else {
            std::string prefix = "HTTP_";
            if (boost::starts_with(env, prefix)) {
              std::string convertKey = key.substr(prefix.size());
              boost::to_lower(convertKey); /*原地转换为小写*/
              const std::string& value = env.substr(equalIndex + 1);
              httpRequest.addHeader(convertKey, value);
            }
          }
        } /* for end */

        /* 排除入参非法的可能性 */
        try {
          httpRequest.setBody(requestIn);
        } catch (const nlohmann::json::exception& jsonException) {
          logError << jsonException.what() << std::endl;
          httpResponse.inflateResponse(HttpStatus::Enum::badRequest, constantStr.bodyFormatErr);
        }

        httpRequest.printSelf();
      }

      if (httpResponse.isEmpty()) {
        optRequest(httpRequest, httpResponse);
      }

      const std::string& strResponse = httpResponse.toString();
//      const std::string& strResponse = "Status: 404\r\n"
//          "Content-type: text/html\r\n"
//          "\r\n"
//          "<html><body>Not Found</body></html>\n";
//      const std::string& strResponse =
//          "Status: 200\r\n"
//          "Content-type: application/json; charset=utf-8\r\n"
//          "PID: 437\r\n"
//          "\r\n"
//          "{\r\n"
//          "  data: ,\r\n"
//          "  externalMsg: ,\r\n"
//          "  internalMsg: ,\r\n"
//          "  status: 200,\r\n"
//          "  statusDesc: 解析完成\r\n"
//          "}";


      int result = FCGX_FPrintF(request.out, strResponse.c_str());

//      FCGX_PutS(strResponse.c_str(), request.out);
      //      FCGX_PutStr(strResponse.c_str(), strResponse.size(), request.out);

      logInfo << boost::str(boost::format("FCGX_FPrintF result = %d") % result) << std::endl;
      logInfo << std::endl << strResponse << std::endl;


      //      httpResponse.printSelf();
//      logInfo << std::endl << strResponse << std::endl;

    } catch (const std::exception& except) {
      logError << "程序异常:" << except.what() << "\n"
               << getStackTrace() << std::endl;
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
int main(int argc, char** argv) {
//  date::set_install("~/Downloads/tzdata");
  setlocale(LC_CTYPE, tom_utils::defLocalStr);  // 必须在 initGlog 之前设置完成
  initGlog(argv[0]);
  logInfo << "可执行文件路径: " << std::filesystem::current_path() << endl;

//  WebClient webClient;
//  webClient.test();
//  testDrogonServer();
//  return 0 ;

  for (int i = 0; i < argc; i++) {
    logInfo << argv[i] << std::endl;
  }

  bool isDebug = contentDebugParam(argc, argv);

  if (isDebug) {
    logInfo << "进入本地入参调试逻辑" << logEndl;
    optLocaleData(argc, argv);
  } else {
    //optNetData(argc, argv);

    logInfo << "进入 drogon 调试逻辑" << logEndl;
    WebServer webServer;
    webServer.run();
  }

  return 0;
}