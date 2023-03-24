#include <iostream>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <ios>

#include "src/utils/json/include_json.h"
#include "input_check.h"
#include "src/net/http_util.h"
#include "src/net/http_util.h"
#include "src/net/http_util.h"
#include "src/utils/common.h"
#include "src/utils/log_utils.h"
#include "linux_os.h"
#include "application.h"

namespace nameless_carpool {

  using namespace tom_utils;
  using namespace std;

  InputParam inputParam;

  bool contentDebugParam(int argc, char **argv) {
    string helpParamStr  = paramPrefix + inputParam.getName(InputParamEnum::help);
    string fileParamStr  = paramPrefix + inputParam.getName(InputParamEnum::inputFile);
    for (int index = 1; index < argc; index++) {
      string arg = argv[index];
      if(arg.compare(helpParamStr) == 0  ||
         arg.compare(fileParamStr) == 0     ) {
        return true;
      } 
    }

    return false;
  }

  bool accept(int argc, char **argv, 
              HttpRequest& requestInflate, HttpResponse& responseInflate) {

    logError << "进入 accept =============" << logEndl;
    HttpStatus::Enum forLoopStatus = HttpStatus::Enum::success; /* 跳出 for 循环的识别标识 */
    string stopLoopAppendMsg;   /* 停止循环并附加信息 */

    /** 标记 InputFile 读取失败 */
    auto debugInputFileReadFailed = [&](const std::string& str) {
      forLoopStatus = HttpStatus::Enum::wrongFormat;
      stopLoopAppendMsg = inputParam.getName(InputParamEnum::inputFile) + "  " + str;
    };

    /** 读取并解析 inputFilePath  
     *  @param inputFilePath 表示 绝对路径
     */
    auto debugInputFileParse = [&](const std::string& inputFilePath) {
      logInfo << "配置文件路径:" << inputFilePath << std ::endl ;

      try {
        nlohmann::ordered_json json = nlohmann::ordered_json::parse(std::ifstream(inputFilePath));
        json             = json[1];
        json.get_to<HttpRequest>(requestInflate);
        forLoopStatus = HttpStatus::Enum::success;
      } catch (const nlohmann::json::exception& jsonException) {
        logError << '[' << jsonException.id << ']' << jsonException.what() << std::endl;
        forLoopStatus     = HttpStatus::Enum::badRequest;
        stopLoopAppendMsg = "请求配置文件内 , json 格式解析失败";
      }

    };

    for (int index = 1; index < argc; index++) {
      
      string kvStr = argv[index];

      /* 确保 入参 键值对 必然是有值的字符串 */
      if(kvStr.size() < 0) {
        forLoopStatus = HttpStatus::Enum::parsingFailed;
        stopLoopAppendMsg = "存在空属性";
        break;
      }

      /* 确保入参是 "--" 开始的字符串 */
      int rightStartFlag = kvStr.compare(0,2, paramPrefix);
      if(rightStartFlag != 0) { /* 字符串非 -- 打头 , 被认为是格式异常 */
        forLoopStatus = HttpStatus::Enum::wrongFormat;
        stopLoopAppendMsg = "属性只接受 '--' 开始的字符串";
        break;
      }

      /* 只有 key 没有 value */
      auto equalFlagIndex = kvStr.find_first_of('=');
      if(equalFlagIndex == string::npos) {

        string key = kvStr.substr(2); /* 纯参数名 */
        
        shared_ptr<InputParamEnum> inputParamPtr = inputParam.getEnum(key);
        if(inputParamPtr == nullptr) {
          forLoopStatus = HttpStatus::Enum::wrongFormat;
          stopLoopAppendMsg = "未知入参1:" + key;
        } else switch(*inputParamPtr) {
          case InputParamEnum::help       : {
            forLoopStatus = HttpStatus::Enum::requestHelp;
            string helpInfo;
            Common::getContent(helpInfo, "nameless_carpool.software.usage.txt", "意外: 读取帮助信息失败");
            responseInflate.inflateResponse(HttpStatus::Enum::success, helpInfo);
            break;
          }
          case InputParamEnum::inputFile  : {
            /* 没有给定参数的时候查看执行文件所在目录是否有相应文件 */
            debugInputFileParse(app().appDebugInputPath);
            break;
          }
          default :                     {
            forLoopStatus = HttpStatus::Enum::wrongFormat;
            stopLoopAppendMsg = "未知入参2:" + key;
            break;
          }
        }
      }

      if(forLoopStatus != HttpStatus::Enum::success) {
        break;
      }
    }

    if(forLoopStatus != HttpStatus::Enum::success) {
      responseInflate.setStatus(forLoopStatus);
      if(!stopLoopAppendMsg.empty()) {
        responseInflate.inflateResponse(forLoopStatus, stopLoopAppendMsg);
      }
                     
      return false;
    } else {
      return true;
    }

  }


  bool readStrFromFile(const string filePath, string& result) {
    ifstream helpTxtFs(filePath) ;
    string line;

    bool good = true;

    while(helpTxtFs.good()) {
      getline(helpTxtFs, line);
      if(helpTxtFs.good() || helpTxtFs.eof()) {
        result.append(line).append("\n");
      } else {
        good = false;
      }
    }

    helpTxtFs.close();
    return good;
  }

}

