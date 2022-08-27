


#include <iostream>
#include <cstring>
#include <algorithm>

#include "input_check.h"
#include "tom_string_utils.h"
#include "../model/response_body.h"
#include "http_util.h"
#include "fcgi_util.h"

namespace nameless_carpool {

  using namespace tom_utils;
  using namespace std;

  bool contentDebugParam(int argc, char **argv) {
    string debugParamStr = paramPrefix + inputParam.getName(InputParamEnum::debug);
    for (int index = 1; index < argc; index++) {
      string arg = argv[index];
      if(arg.compare(debugParamStr) == 0) {
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
      
      string arg = argv[index];

      if(arg.size() < 0) {
        forLoopStatus = HttpStatusEnum::parsingFailed;
        stopLoopAppendMsg = "存在空属性";
        break;
      }

      int rightStartFlag = arg.compare(0,2, paramPrefix);
      if(rightStartFlag != 0) { /* 字符串非 -- 打头 , 被认为是格式异常 */
        forLoopStatus = HttpStatusEnum::wrongFormat;
        stopLoopAppendMsg = "属性只接受 '--' 开始的字符串";
        break;
      }

      auto equalFlagIndex = arg.find_first_of('=');
      if(equalFlagIndex == string::npos) {

        string param = arg.substr(2); /* 纯参数名 */
        
        shared_ptr<InputParamEnum> inputParamPtr = inputParam.getEnum(param);
        if(inputParamPtr == nullptr) {
          forLoopStatus = HttpStatusEnum::wrongFormat;
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
            stopLoopAppendMsg = "未知入参2:" + param;
          }
        }

        if(forLoopStatus != HttpStatusEnum::success) {
          break;
        }

      } 

      string param = arg.substr(2, equalFlagIndex - 2);
      shared_ptr<InputParamEnum> inputParamPtr = inputParam.getEnum(param);
      if(inputParamPtr == nullptr) {
        forLoopStatus = HttpStatusEnum::wrongFormat;
        stopLoopAppendMsg = "未知入参3:" + param;
      } else switch(*inputParamPtr) {
        case InputParamEnum::header     : {
          LOG(FATAL) << "json to map , 然后做进一步判断" << endl;
          break;
        }
        case InputParamEnum::method     : {
          if(requestInflate.method.empty()) {
            requestInflate.method = arg.substr(equalFlagIndex+1);
          } else {
            forLoopStatus = HttpStatusEnum::argDefineMultiple;
            stopLoopAppendMsg = "method";
          }
          break;
        }
        case InputParamEnum::function   : {
          if(requestInflate.function.empty()) {
            requestInflate.function = arg.substr(equalFlagIndex+1);
          } else {
            forLoopStatus = HttpStatusEnum::argDefineMultiple;
            stopLoopAppendMsg = "function";
          }
          break;
        }
        case InputParamEnum::body       : {
          if(requestInflate.body.empty()) {
            requestInflate.body = arg.substr(equalFlagIndex+1);
          } else {
            forLoopStatus = HttpStatusEnum::argDefineMultiple;
            stopLoopAppendMsg = "body";
          }
          break;
        }

        default                         : {
          forLoopStatus = HttpStatusEnum::wrongFormat;
          stopLoopAppendMsg = "未知异常, 参数字段:" + param;
          break;
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

