#include <iostream>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <ios>

#include "input_check.h"
#include "../net/model/response_body.h"
#include "http_util.h"
#include "fcgi_util.h"
#include "common.h"
#include "log_utils.h"

namespace nameless_carpool {

  using namespace tom_utils;
  using namespace std;

  InputParam inputParam;

  bool contentDebugParam(int argc, char **argv) {
    string debugParamStr = paramPrefix + inputParam.getName(InputParamEnum::debug);
    string helpParamStr = paramPrefix + inputParam.getName(InputParamEnum::help);
    string fileParamStr = paramPrefix + inputParam.getName(InputParamEnum::inputFile);
    for (int index = 1; index < argc; index++) {
      string arg = argv[index];
      if(arg.compare(debugParamStr) == 0 || 
         arg.compare(helpParamStr) == 0  ||
         arg.compare(fileParamStr) == 0     ) {
        return true;
      } 
    }

    return false;
  }

  bool accept(int argc, char **argv, 
              HttpRequest& requestInflate, HttpResponse& responseInflate) {

    HttpStatusEnum forLoopStatus = HttpStatusEnum::success; /* 跳出 for 循环的识别标识 */
    string stopLoopAppendMsg;   /* 停止循环并附加信息 */

    for (int index = 1; index < argc; index++) {
      
      string kvStr = argv[index];

      if(kvStr.size() < 0) {
        forLoopStatus = HttpStatusEnum::parsingFailed;
        stopLoopAppendMsg = "存在空属性";
        break;
      }

      int rightStartFlag = kvStr.compare(0,2, paramPrefix);
      if(rightStartFlag != 0) { /* 字符串非 -- 打头 , 被认为是格式异常 */
        forLoopStatus = HttpStatusEnum::wrongFormat;
        stopLoopAppendMsg = "属性只接受 '--' 开始的字符串";
        break;
      }

      auto equalFlagIndex = kvStr.find_first_of('=');
      if(equalFlagIndex == string::npos) {

        string key = kvStr.substr(2); /* 纯参数名 */
        
        shared_ptr<InputParamEnum> inputParamPtr = inputParam.getEnum(key);
        if(inputParamPtr == nullptr) {
          forLoopStatus = HttpStatusEnum::wrongFormat;
          stopLoopAppendMsg = "未知入参1:" + key;
        } else switch(*inputParamPtr) {
          case InputParamEnum::debug      : {
            /* empty */
            continue;
          }
          case InputParamEnum::help       : {
            forLoopStatus = HttpStatusEnum::requestHelp;
            string helpInfo;
            Common::getContent(helpInfo, "nameless_carpool.software.usage.txt", "意外: 读取帮助信息失败");
            responseInflate.initResponse(HttpStatusEnum::success, helpInfo);
            break;
          }
          case InputParamEnum::inputFile  : {
            string debugInfo;
            bool success = Common::getContent(debugInfo, "debugInput.json", "意外: 读取入参文件失败");
            
            if(success) {
              if(requestInflate.isEmpty()) {
                try{
                  Json json = Json::parse(debugInfo);
                  json.get_to<HttpRequest>(requestInflate);
                } catch (const Json::exception& jsonException) {
                  logDebug << '[' << jsonException.id << ']' << jsonException.what() << endl; 
                  forLoopStatus = HttpStatusEnum::badRequest;
                  stopLoopAppendMsg = "请求配置文件内 , json 格式解析失败";
                }
                
              } else {
                forLoopStatus = HttpStatusEnum::wrongFormat;
                stopLoopAppendMsg = inputParam.getName(InputParamEnum::inputFile) + "应该单独使用";
              }

            } else {
              forLoopStatus = HttpStatusEnum::unknowErr;
              stopLoopAppendMsg = "需要近一半探查 inputFile 入参解析异常";
            }
            
            break;
          }
          default :                     {
            forLoopStatus = HttpStatusEnum::wrongFormat;
            stopLoopAppendMsg = "未知入参2:" + key;
            break;
          }
        }

        if(forLoopStatus != HttpStatusEnum::success) {
          break;
        }

      }

      string key = kvStr.substr(2, equalFlagIndex - 2);
      shared_ptr<InputParamEnum> keyEnumPtr = inputParam.getEnum(key);
      if(keyEnumPtr == nullptr) {
        forLoopStatus = HttpStatusEnum::wrongFormat;
        stopLoopAppendMsg = "未知入参3:" + key;
      } else {
        string value = kvStr.substr(equalFlagIndex + 1);

        switch(*keyEnumPtr) {
          case InputParamEnum::inputFile  : {
            
            break;
          }
          case InputParamEnum::header     : {
            if(requestInflate.headers.empty()) {
              Json  jsonHeader = Json::parse(value);
              for(auto& iterator : jsonHeader.items()) {
                string iteKey = iterator.key();
                auto iteVal = iterator.value();
                string valueTypeStr = iteVal.type_name();
                string iteStrVal;
                if(valueTypeStr.compare("string") == 0) {
                  iteStrVal = iteVal.get<string>();
                } else {
                  iteStrVal = to_string(iteVal);
                }
                requestInflate.addHeader(iteKey, iteStrVal);
              }
            } else {
              forLoopStatus = HttpStatusEnum::argDefineMultiple;
              stopLoopAppendMsg = "header";
            }
            break;
          }
          case InputParamEnum::method     : {
            if(requestInflate.method.empty()) {
              requestInflate.method = kvStr.substr(equalFlagIndex+1);
            } else {
              forLoopStatus = HttpStatusEnum::argDefineMultiple;
              stopLoopAppendMsg = "method";
            }
            break;
          }
          case InputParamEnum::uri   : {
            if(requestInflate.uri.empty()) {
              requestInflate.uri = kvStr.substr(equalFlagIndex+1);
            } else {
              forLoopStatus = HttpStatusEnum::argDefineMultiple;
              stopLoopAppendMsg = "uri";
            }
            break;
          }
          case InputParamEnum::body       : {
            if(requestInflate.body.empty()) {
              try{
                Json jsonBody = kvStr.substr(equalFlagIndex+1);
                requestInflate.setBody(jsonBody);
              } catch (const Json::exception& jsonException) {
                logError << jsonException.what() << endl;
                forLoopStatus = HttpStatusEnum::badRequest;
                stopLoopAppendMsg.append("请求体解析失败")
                                 .append("\n")
                                 .append(jsonException.what());
              }
            } else {
              forLoopStatus = HttpStatusEnum::argDefineMultiple;
              stopLoopAppendMsg = "body";
            }
            break;
          }

          default                         : {
            forLoopStatus = HttpStatusEnum::wrongFormat;
            stopLoopAppendMsg = "未知异常, 参数字段:" + key;
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
        responseInflate.initResponse(forLoopStatus, stopLoopAppendMsg);
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

