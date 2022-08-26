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

namespace nameless_carpool {


  using namespace std;


  enum class HttpStatusEnum {
    success             = 200 ,

    requestHelp         = 500 ,
    unknowErr           = 501 ,
    parsingFailed       = 502 ,
    wrongFormat         = 503 ,
    argDefineMultiple   = 504 ,
  };

  struct {
    const map<HttpStatusEnum, const wstring> name = {
      {HttpStatusEnum::success,             L"success"            },
      {HttpStatusEnum::requestHelp,         L"requestHelp"        },
      {HttpStatusEnum::unknowErr,           L"unknowErr"          },
      {HttpStatusEnum::parsingFailed,       L"parsingFailed"      },
      {HttpStatusEnum::wrongFormat,         L"wrongFormat"        },
      {HttpStatusEnum::argDefineMultiple,   L"argDefineMultiple"  },
    };

    const map<HttpStatusEnum, const wstring> desc = {
      {HttpStatusEnum::success,             L"解析完成"                   },
      {HttpStatusEnum::requestHelp,         L"查看帮助信息"                },
      {HttpStatusEnum::unknowErr,           L"未知错误, 需要进一步跟进"      },
      {HttpStatusEnum::parsingFailed,       L"服务端异常 , 解析失败"        },
      {HttpStatusEnum::wrongFormat,         L"入参错误 , 入参格式错误"      },
      {HttpStatusEnum::argDefineMultiple,   L"入参错误 , 属性多次定义"      },
    };

    wstring getStatusName(HttpStatusEnum statusEnum) {
      return name.at(statusEnum);
    }

    wstring getStatusDesc(HttpStatusEnum statusEnum) {
      return desc.at(statusEnum);
    }

  } httpStatusUtil;


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

  class {

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

  } httpMethodUtil ;


  struct HttpContent {
    map<wstring, wstring> headers;
    wstring body = L"";

    HttpContent& addHeader(wstring key, wstring val) {
      headers.insert({key, val});
      return *this;
    }
    HttpContent& setBody(wstring str) {
      body = str;
      return *this;
    }
    virtual HttpContent& clear() {
      headers.clear();
      body.clear();
      return *this;
    }
  };

  struct HttpRequest : public HttpContent {
    wstring method;
    wstring function;
    HttpRequest& setMethod(wstring str) {
      method = str;
      return *this;
    }
    HttpRequest& setFunction(wstring str) {
      function = str;
      return *this;
    }
    /* 从 FCGX_Request 初始化 HttpRequest 
       只获取我们关心的数据 HttpRequest */
    HttpRequest& initByNet(FCGX_Request &request) {
      char **envp = request.envp;
      for (int index = 0; *envp; index++, envp++) {
        // wstring env = utfMbsrtowcs(*envp);
        wstring env = utfMbsrtowcs(*envp);
        int equalIndex = env.find('=');
        wstring key = env.substr(0, equalIndex);
        if(key == L"REQUEST_METHOD") {
          this->method = env.substr(equalIndex + 1);
        } else if(key == L"REQUEST_URI") {
          this->function = env.substr(equalIndex + 1);
        } else {
          wstring prefix = env.substr(0, 5);
          if(prefix == L"HTTP_") {
            wstring value = env.substr(equalIndex+1);
            this->addHeader(key, value);
          }
        }
      }
      this->body = fcgxStreamReadw(request.in);
      return *this;
    }
    virtual HttpRequest& clear() override {
      HttpContent::clear();
      method.clear();
      function.clear();
      return *this;
    }
  };

  struct HttpResponse : public HttpContent {

    int status = -1;

    HttpResponse() {
      headers = {
        {L"Content-type", L"text/plain; charset=utf-8"},
        {L"PID"         , std::to_wstring(getpid())   },
      };
    }

    HttpResponse& setStatus(int statusEnum) {
      this->status = statusEnum;
      return *this;
    }

    wstring toWstring() {
      wstringstream ss;

      /* 状态码 */   {
        ss << "Status: " << status << endl;
      }
      /* header */  {
        for (map<wstring, wstring>::iterator headerIterator = headers.begin();
            headerIterator != headers.end();
            headerIterator++) {

          wstring key = headerIterator->first;
          wstring val = headerIterator->second;
          ss << key << ": " << val << endl;
        }
        ss << endl;
      }
      /* body */    {
        ss << body ;
      }
      wstring result = ss.str();
      ss.str(L"");
      ss.clear();

      return result;
    }

    string toString() {
      wstring wstr = toWstring();
      string result = utfWcstombs(wstr.c_str());
      return result;
    }

    virtual HttpResponse& clear() override {
      HttpContent::clear();
      status = -1;
      return *this;
    }
  };

}
