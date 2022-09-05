#include <iostream>
#include <cstring>
#include <algorithm>

#include "input_check.h"
#include "../model/response_body.h"
#include "http_util.h"
#include "fcgi_util.h"

namespace nameless_carpool {

  using namespace tom_utils;
  using namespace std;

  InputParam inputParam;

  bool contentDebugParam(int argc, char **argv) {
    string debugParamStr = paramPrefix + inputParam.getName(InputParamEnum::debug);
    string helpParamStr = paramPrefix + inputParam.getName(InputParamEnum::help);
    for (int index = 1; index < argc; index++) {
      string arg = argv[index];
      if(arg.compare(debugParamStr) == 0 || 
         arg.compare(helpParamStr) == 0     ) {
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
          case InputParamEnum::debug  : {
            /* empty */
            continue;
          }
          case InputParamEnum::help   : {
            forLoopStatus = HttpStatusEnum::requestHelp;
            responseInflate.setBody(helpInfo);
            break;
          }
          default :                     {
            forLoopStatus = HttpStatusEnum::wrongFormat;
            stopLoopAppendMsg = "未知入参2:" + key;
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
          case InputParamEnum::function   : {
            if(requestInflate.function.empty()) {
              requestInflate.function = kvStr.substr(equalFlagIndex+1);
            } else {
              forLoopStatus = HttpStatusEnum::argDefineMultiple;
              stopLoopAppendMsg = "function";
            }
            break;
          }
          case InputParamEnum::body       : {
            if(requestInflate.body.empty()) {
              requestInflate.body = kvStr.substr(equalFlagIndex+1);
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
        responseInflate.setBody("(ResponseBody) 应该转换为 Json 还没来得及\n" 
                              + stopLoopAppendMsg);
      }
                     
      return false;
    } else {
      return true;
    }

  }
}

