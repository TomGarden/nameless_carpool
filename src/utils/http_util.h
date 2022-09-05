#pragma once

#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <map>
#include <iterator>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <ios>

#include "tom_string_utils.h"
#include "fcgi_util.h"
#include "glog/logging.h"
#include "json/include_json.h"
#include "../model/response_body.h"

extern int flag;

namespace nameless_carpool {


  using namespace std;

  /* 状态码一般信息参考 : https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Status 
     > 信息响应 (100–199)
     > 成功响应 (200–299)
     > 重定向消息 (300–399)
     > 客户端错误响应 (400–499)
     > 服务端错误响应 (500–599)
  */
  enum class HttpStatusEnum {
    success             = 200 ,

    badRequest          = 400 ,
    requestUndefined    = 404 ,
    requestHelp         = 500 ,
    unknowErr           = 501 ,
    parsingFailed       = 502 ,
    wrongFormat         = 503 ,
    argDefineMultiple   = 504 ,
  };

  struct HttpStatus {
    const map<HttpStatusEnum, const string> name = {
      {HttpStatusEnum::success,             "success"            },
      {HttpStatusEnum::badRequest,          "badRequest"         },
      {HttpStatusEnum::requestUndefined,    "requestUndefined"   },
      {HttpStatusEnum::requestHelp,         "requestHelp"        },
      {HttpStatusEnum::unknowErr,           "unknowErr"          },
      {HttpStatusEnum::parsingFailed,       "parsingFailed"      },
      {HttpStatusEnum::wrongFormat,         "wrongFormat"        },
      {HttpStatusEnum::argDefineMultiple,   "argDefineMultiple"  },
    };

    const map<HttpStatusEnum, const string> desc = {
      {HttpStatusEnum::success,             "解析完成"                                                                                                          },
      {HttpStatusEnum::badRequest,          "Bad Request : 被认为是客户端错误（例如，错误的请求语法、无效的请求消息帧或欺骗性的请求路由），服务器无法或不会处理请求。"             },
      {HttpStatusEnum::requestUndefined,    "404 请求未定义"                                                                                                    },
      {HttpStatusEnum::requestHelp,         "查看帮助信息"                                                                                                        },
      {HttpStatusEnum::unknowErr,           "未知错误, 需要进一步跟进"                                                                                             },
      {HttpStatusEnum::parsingFailed,       "服务端异常 , 解析失败"                                                                                                },
      {HttpStatusEnum::wrongFormat,         "入参错误 , 入参格式错误"                                                                                             },
      {HttpStatusEnum::argDefineMultiple,   "入参错误 , 属性多次定义"                                                                                             },
    };

    string getName(HttpStatusEnum statusEnum) {
      return name.at(statusEnum);
    }

    string getDesc(HttpStatusEnum statusEnum) {
      return desc.at(statusEnum);
    }

  } ;
  extern HttpStatus httpStatus;

  enum class HttpMethodEnum {
    /* https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Methods */
    GET,         // GET 方法请求一个指定资源的表示形式，使用 GET 的请求应该只被用于获取数据。
    HEAD,        // HEAD 方法请求一个与 GET 请求的响应相同的响应，但没有响应体。
    POST,        // POST 方法用于将实体提交到指定的资源，通常导致在服务器上的状态变化或副作用。
    PUT,         // PUT 方法用请求有效载荷替换目标资源的所有当前表示。
    DELETE,      // DELETE 方法删除指定的资源。
    CONNECT,     // CONNECT 方法建立一个到由目标资源标识的服务器的隧道。
    OPTIONS,     // OPTIONS 方法用于描述目标资源的通信选项。
    TRACE,       // TRACE 方法沿着到目标资源的路径执行一个消息环回测试。
    PATCH,       // PATCH 方法用于对资源应用部分修改。
  };

  struct HttpMethodUtil {

    const map<string, HttpMethodEnum> httpMethodFromName = {
      { "GET"      ,   HttpMethodEnum::GET     },
      { "HEAD"     ,   HttpMethodEnum::HEAD    },
      { "POST"     ,   HttpMethodEnum::POST    },
      { "PUT"      ,   HttpMethodEnum::PUT     },
      { "DELETE"   ,   HttpMethodEnum::DELETE  },
      { "CONNECT"  ,   HttpMethodEnum::CONNECT },
      { "OPTIONS"  ,   HttpMethodEnum::OPTIONS },
      { "TRACE"    ,   HttpMethodEnum::TRACE   },
      { "PATCH"    ,   HttpMethodEnum::PATCH   },
    };

    const map<HttpMethodEnum, string> httpMethodToName = {
      { HttpMethodEnum::GET      ,    "GET"      },
      { HttpMethodEnum::HEAD     ,    "HEAD"     },
      { HttpMethodEnum::POST     ,    "POST"     },
      { HttpMethodEnum::PUT      ,    "PUT"      },
      { HttpMethodEnum::DELETE   ,    "DELETE"   },
      { HttpMethodEnum::CONNECT  ,    "CONNECT"  },
      { HttpMethodEnum::OPTIONS  ,    "OPTIONS"  },
      { HttpMethodEnum::TRACE    ,    "TRACE"    },
      { HttpMethodEnum::PATCH    ,    "PATCH"    },
    };

    shared_ptr<HttpMethodEnum> getEnum(string str) {
      std::transform(str.begin(), str.end(),str.begin(), ::toupper);
      if(httpMethodFromName.count(str) > 0) {
        return make_shared<HttpMethodEnum>(httpMethodFromName.at(str));
      } else {
        return nullptr;
      }
    }

    string getName(HttpMethodEnum methodEnum) {
      return httpMethodToName.at(methodEnum);
    }

  };
  extern HttpMethodUtil httpMethodUtil;

  struct MediaType {
    /* 常见 media type https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types */

    const string json = "application/json";
    const string txt  = "text/plain";

  };
  extern MediaType mediaType;

  struct HttpContent {
    map<string, string> headers;
    string body = "";

    HttpContent& addHeader(string key, string val) {
      headers.insert({key, val});
      return *this;
    }
    HttpContent& setBody(string str) {
      body = str;
      return *this;
    }
    virtual HttpContent& clear() {
      headers.clear();
      body.clear();
      return *this;
    }
    
    // NLOHMANN_DEFINE_TYPE_INTRUSIVE(HttpContent, headers, body);
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(HttpContent, headers, body)
  };

  struct HttpRequest : public HttpContent {
    string method;
    string function;

    HttpRequest& setMethod(string str) {
      method = str;
      return *this;
    }
    HttpRequest& setFunction(string str) {
      function = str;
      return *this;
    }
    /* 从 FCGX_Request 初始化 HttpRequest 
       只获取我们关心的数据 HttpRequest */
    HttpRequest& initByNet(FCGX_Request &request) {
      char **envp = request.envp;
      for (int index = 0; *envp; index++, envp++) {
        // string env = utfMbsrtowcs(*envp);
        string env = *envp;
        int equalIndex = env.find('=');
        string key = env.substr(0, equalIndex);
        if(key == "REQUEST_METHOD") {
          this->method = env.substr(equalIndex + 1);
        } else if(key == "REQUEST_URI") {
          this->function = env.substr(equalIndex + 1);
        } else {
          string prefix = env.substr(0, 5);
          if(prefix == "HTTP_") {
            string value = env.substr(equalIndex+1);
            this->addHeader(key, value);
          }
        }
      }
      this->body = fcgxStreamRead(request.in);
      return *this;
    }
    virtual HttpRequest& clear() override {
      HttpContent::clear();
      method.clear();
      function.clear();
      return *this;
    }
    /* 获取 method + uri 字符串 */
    string methodUri() const {
      string upperMethod = method;
      std::transform(upperMethod.begin(), upperMethod.end(),upperMethod.begin(), ::toupper);
      return upperMethod + function;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(HttpRequest, headers, body, method, function)
  };

  struct HttpResponse : public HttpContent {
    
    int status = -1;

    HttpResponse() {
      headers = {
        {"Content-type", mediaType.json + "; charset=utf-8"},
        {"PID"         , std::to_string(getpid())          },
      };
    }

    HttpResponse& setStatus(int statusEnum) {
      this->status = statusEnum;
      return *this;
    }

    string toString() {
      stringstream ss;

      /* 状态码 */   {
        if(status >= 0) {
          ss << "Status: " << status << endl;
        }
      }
      /* header */  {
        for (map<string, string>::iterator headerIterator = headers.begin();
            headerIterator != headers.end();
            headerIterator++) {

          string key = headerIterator->first;
          string val = headerIterator->second;
          ss << key << ": " << val << endl;
        }
        ss << endl;
      }
      /* body */    {
        ss << body ;
      }
      string result = ss.str();
      ss.str("");
      ss.clear();

      return result;
    }

    wstring toWString() {
      string str = toString();
      wstring result = utfMbsrtowcs(str.c_str());
      return result;
    }

    virtual HttpResponse& clear() override {
      HttpContent::clear();
      status = -1;
      return *this;
    }
  
    /* 初始化请求错误返回信息 */
    void initByInlegalRequest(string internalMsg = "", string externalMsg = "") {
      HttpStatusEnum statusEnum = HttpStatusEnum::badRequest;
      status = static_cast<int>(statusEnum);
      Json jsonBody = ResponseBody {
        status,
        "",
        httpStatus.getDesc(statusEnum),
        internalMsg,
        externalMsg
      };
      body = jsonBody.dump(2);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(HttpResponse, headers, body, status)
  };

}
