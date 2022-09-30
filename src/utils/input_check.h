#pragma once

#include <string>
#include <map>
#include <memory>

#include "http_util.h"



/** 
    nameless_carpool 软件本地调用的格式说明
    
    nameless_carpool \
        --debug                               \   关键字, 用于区分数据当做网络数据还是当做本地数据
        --help                                \   关键字, 包含此参数时候 , 其他参数被忽略
        --method=get,post,put,delete ···      \   http method
        --header={key:value, key1:value1 ···} \   http header
        --uri=httpUri                    \   http uri
        --body={anyjsondata}                  \   http body

    只接受 utf-8 编码的数据 , 几个参数顺序不分先后


 */
namespace nameless_carpool {
  
  using namespace std;

  /** 参数前缀 */
  inline const string paramPrefix = "--";

  struct InputData {
    string charset = {nullptr};
    string header = {nullptr};
    string method = {nullptr};
    string function = {nullptr};
    string body = {nullptr};
  };

  /* 入参变量枚举 , 与 inputParamMap 结合定义常量 */
  enum class InputParamEnum {
    inputFile     =  7,
    debug         =  8,
    help          =  9,
    header        = 10,
    method        = 11,
    uri           = 12,
    body          = 13,
  };

  struct InputParam {
    const string inputFile = "input_file";
    const string debug     = "debug";
    const string help      = "help";
    const string header    = "header";
    const string method    = "method";
    const string uri       = "uri";
    const string body      = "body";


    const map<InputParamEnum, string> inputParamToName = {
      {InputParamEnum::inputFile    ,  inputFile      },
      {InputParamEnum::debug        ,  debug          },
      {InputParamEnum::help         ,  help           },
      {InputParamEnum::header       ,  header         },
      {InputParamEnum::method       ,  method         },
      {InputParamEnum::uri          ,  uri       },
      {InputParamEnum::body         ,  body           },
    };
    
    const map<string, InputParamEnum> inputParamFromName = {
      {inputFile      ,  InputParamEnum::inputFile    },
      {debug          ,  InputParamEnum::debug        },
      {help           ,  InputParamEnum::help         },
      {header         ,  InputParamEnum::header       },
      {method         ,  InputParamEnum::method       },
      {uri            ,  InputParamEnum::uri     },
      {body           ,  InputParamEnum::body         },
    };

    public:
      string getName(InputParamEnum inputEnum) {
        return inputParamToName.at(inputEnum);
      }
      
      
      shared_ptr<InputParamEnum> getEnum(string name) {
        transform(name.begin(), name.end(), name.begin(), ::tolower);
        if(inputParamFromName.count(name) > 0) {
          return make_shared<InputParamEnum>(inputParamFromName.at(name));
        } else {
          return nullptr;
        }
      }


  };
  extern InputParam inputParam;

  /* 是否包含 debug 入参 */
  extern bool contentDebugParam(int argc, char **argv);

  /** 接受参数 , 将参数填充到 httpRequest 
      return true,  HttpRequest 填充有意义 , HttpResponse 的填充有意义(含异常信息)
             false, HttpRequest   无意义  , 可以直接将 HttpResponse 输出 */
  extern bool accept(
    int argc, char **argv,                                  /* 输入 */
    HttpRequest& requestRead, HttpResponse& reponseInflate  /* 输出 */);

  /* 从 filePath 读取文本放到 result 中
     @return true, 读取过程中没有错误 
             false,读取过程中有错误*/
  extern bool readStrFromFile(const string filePath, string& result);
}

