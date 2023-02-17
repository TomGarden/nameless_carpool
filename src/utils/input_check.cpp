#include <iostream>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <ios>

#include "include_json.h"
#include "input_check.h"
#include "../net/model/response_body.h"
#include "http_util.h"
#include "fcgi_util.h"
#include "common.h"
#include "log_utils.h"
#include "linux_os.h"

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

    logDebug << "进入 accept =============\n";
    HttpStatusEnum forLoopStatus = HttpStatusEnum::success; /* 跳出 for 循环的识别标识 */
    string stopLoopAppendMsg;   /* 停止循环并附加信息 */

    /** 标记 InputFile 读取失败 */
    auto debugInputFileReadFailed = [&](const std::string& str) {
      forLoopStatus = HttpStatusEnum::wrongFormat;
      stopLoopAppendMsg = inputParam.getName(InputParamEnum::inputFile) + "  " + str;
    };

    /** 读取并解析 inputFilePath  
     *  @param inputFilePath 表示 绝对路径
     */
    auto debugInputFileParse = [&](const std::string& inputFilePath) {
      logInfo << "配置文件路径:" << inputFilePath << std ::endl;

      try {
        OrderedJson json = OrderedJson::parse(std::ifstream(inputFilePath));
        json             = json[1];
        json.get_to<HttpRequest>(requestInflate);
        forLoopStatus = HttpStatusEnum::success;
      } catch (const Json::exception& jsonException) {
        logDebug << '[' << jsonException.id << ']' << jsonException.what() << endl;
        forLoopStatus     = HttpStatusEnum::badRequest;
        stopLoopAppendMsg = "请求配置文件内 , json 格式解析失败";
      }

    };

    for (int index = 1; index < argc; index++) {
      
      string kvStr = argv[index];

      /* 确保 入参 键值对 必然是有值的字符串 */
      if(kvStr.size() < 0) {
        forLoopStatus = HttpStatusEnum::parsingFailed;
        stopLoopAppendMsg = "存在空属性";
        break;
      }

      /* 确保入参是 "--" 开始的字符串 */
      int rightStartFlag = kvStr.compare(0,2, paramPrefix);
      if(rightStartFlag != 0) { /* 字符串非 -- 打头 , 被认为是格式异常 */
        forLoopStatus = HttpStatusEnum::wrongFormat;
        stopLoopAppendMsg = "属性只接受 '--' 开始的字符串";
        break;
      }

      /* 只有 key 没有 value */
      auto equalFlagIndex = kvStr.find_first_of('=');
      if(equalFlagIndex == string::npos) {

        string key = kvStr.substr(2); /* 纯参数名 */
        
        shared_ptr<InputParamEnum> inputParamPtr = inputParam.getEnum(key);
        if(inputParamPtr == nullptr) {
          forLoopStatus = HttpStatusEnum::wrongFormat;
          stopLoopAppendMsg = "未知入参1:" + key;
        } else switch(*inputParamPtr) {
          case InputParamEnum::help       : {
            forLoopStatus = HttpStatusEnum::requestHelp;
            string helpInfo;
            Common::getContent(helpInfo, "nameless_carpool.software.usage.txt", "意外: 读取帮助信息失败");
            responseInflate.inflateResponse(HttpStatusEnum::success, helpInfo);
            break;
          }
          case InputParamEnum::inputFile  : {
            /* 没有给定参数的时候查看执行文件所在目录是否有相应文件 */
            std::string exeFileFd;
            if( getCurExeFd(exeFileFd) ) {
              exeFileFd
                  .append(&std::filesystem::path::preferred_separator)
                  .append("tom_doc_file_dir/debugInput.json");
              debugInputFileParse(exeFileFd);
            } else {
              debugInputFileReadFailed("没有 入参 值 , 获取默认文件路径失败");
            }            
            break;
          }
          default :                     {
            forLoopStatus = HttpStatusEnum::wrongFormat;
            stopLoopAppendMsg = "未知入参2:" + key;
            break;
          }
        }
      }

      if(forLoopStatus != HttpStatusEnum::success) { 
        break;
      }
    }

    if(forLoopStatus != HttpStatusEnum::success) {
      responseInflate.setStatus(static_cast<int>(forLoopStatus));
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

