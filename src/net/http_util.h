#pragma once

#include <unistd.h>

#include <any>
#include <boost/algorithm/string.hpp>
#include <cmath>
#include <cstdlib>
#include <ios>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include "glog/logging.h"
#include "src/net/model/response_body.h"
#include "src/utils/constant.h"
#include "src/utils/fcgi_util.h"
#include "src/utils/json/include_json.h"
#include "src/utils/log_utils.h"
#include "src/utils/tom_string_utils.h"
#include "enum_util.h"

extern int flag;

namespace nameless_carpool {

  extern std::string getHttpScheme() ;


  /** 状态码一般信息参考 : https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Status
   *> 信息响应 (100–199)
   *> 成功响应 (200–299)
   *> 重定向消息 (300–399)
   *> 客户端错误响应 (400–499)
   *> 服务端错误响应 (500–599)
   */
  ENUM_UTIL_3_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT(
      HttpStatus /*            */, std::string, str /*    */, std::string_view, str_view, std::string, desc,
      unsetErr /*         */, -01, "unsetErr" /*         */, "unsetErr" /*            */, "未设置状态码",
      success /*          */, 200, "success" /*          */, "success" /*             */, "解析完成",
      badRequest /*       */, 400, "badRequest" /*       */, "badRequest" /*          */, "Bad Request : 被认为是客户端错误（例如，错误的请求语法、无效的请求消息帧或欺骗性的请求路由），服务器无法或不会处理请求。",
      forbidden /*        */, 403, "forbidden" /*        */, "forbidden" /*           */, "Permission Denied",
      requestUndefined /* */, 404, "requestUndefined" /* */, "requestUndefined" /*    */, "404 请求未定义",
      requestHelp /*      */, 500, "requestHelp" /*      */, "requestHelp" /*         */, "查看帮助信息",
      unknowErr /*        */, 501, "unknowErr" /*        */, "unknowErr" /*           */, "未知错误, 需要进一步跟进",
      parsingFailed /*    */, 502, "parsingFailed" /*    */, "parsingFailed" /*       */, "服务端异常 , 解析失败",
      wrongFormat /*      */, 503, "wrongFormat" /*      */, "wrongFormat" /*         */, "入参错误 , 入参格式错误",
      argDefineMultiple /**/, 504, "argDefineMultiple" /**/, "argDefineMultiple" /*   */, "入参错误 , 属性多次定义",
      serverLogicError /* */, 505, "serverLogicError" /* */, "serverLogicError" /*    */, "服务端内部逻辑错误")

  ENUM_UTIL_1_FIELD_DEF_ENUM_GENERATE_IN_STRUCT(
      HttpMethod, std::string, name,
      GET /*    */, "GET" /*    */, /* GET 方法请求一个指定资源的表示形式，使用 GET 的请求应该只被用于获取数据。 */
      HEAD /*   */, "HEAD" /*   */, /* HEAD 方法请求一个与 GET 请求的响应相同的响应，但没有响应体。 */
      POST /*   */, "POST" /*   */, /* POST 方法用于将实体提交到指定的资源，通常导致在服务器上的状态变化或副作用。 */
      PUT /*    */, "PUT" /*    */, /* PUT 方法用请求有效载荷替换目标资源的所有当前表示。 */
      DELETE /* */, "DELETE" /* */, /* DELETE 方法删除指定的资源。 */
      CONNECT /**/, "CONNECT" /**/, /* CONNECT 方法建立一个到由目标资源标识的服务器的隧道。 */
      OPTIONS /**/, "OPTIONS" /**/, /* OPTIONS 方法用于描述目标资源的通信选项。 */
      TRACE /*  */, "TRACE" /*  */, /* TRACE 方法沿着到目标资源的路径执行一个消息环回测试。 */
      PATCH /*  */, "PATCH" /*  */) /* PATCH 方法用于对资源应用部分修改。 */



  struct MediaType {
    /* 常见 media type https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types */

    const std::string json = "application/json";
    const std::string txt  = "text/plain";

    static MediaType& instance() {
      static MediaType instance;
      return instance;
    }
  };

  struct HttpHeaderNames {
    const std::string timeZone    = "time_zone";
    const std::string token       = "token";
    const std::string contentType = "Content-type";
    const std::string pid         = "PID";
    const std::string tid         = "Thread-id";

    static HttpHeaderNames& instance() {
      static HttpHeaderNames instance;
      return instance;
    }
  };

  struct HttpContent {
    //map<string, std::string> headers;
    nlohmann::json headers;
    nlohmann::json body;

    HttpContent& addHeader(const std::string& key, const std::string& val) {
      headers[key] = val;
      return *this;
    }

    HttpContent& setHeaders(const nlohmann::json& headers) {
      this->headers = headers;
      return *this;
    }

    HttpContent& setBody(const nlohmann::json& jsonBody) {
      body = jsonBody;
      return *this;
    }

    HttpContent& setBody(const std::string& str) {
      body = nlohmann::json::parse(str);
      return *this;
    }

    HttpContent& setBody(const char* str) {
      body = nlohmann::json::parse(str);
      return *this;
    }

    virtual HttpContent& clear() {
      headers.clear();
      body.clear();
      return *this;
    }
    virtual bool isEmpty() {
      return headers.empty() && body.empty();
    }
    virtual void printSelf() { /* empty */ }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(HttpContent, headers, body);
    // NLOHMANN_DEFINE_NORMAL_TYPE_INTRUSIVE(HttpContent, headers, body);
  };

  struct HttpRequest : virtual HttpContent {
    std::string method;
    std::string uri;

    HttpRequest& setMethod(string str) {
      method = str;
      return *this;
    }
    HttpRequest& setUri(string str) {
      uri = str;
      return *this;
    }
    /* 从 FCGX_Request 初始化 HttpRequest
       只获取我们关心的数据 HttpRequest */
    HttpRequest& initByNet(FCGX_Request &request) {
      char **envp = request.envp;
      for (int index = 0; *envp; index++, envp++) {
        // std::string env = utfMbsrtowcs(*envp);
        std::string env = *envp;
        int equalIndex = env.find('=');
        std::string key = env.substr(0, equalIndex);
        if(key == "REQUEST_METHOD") {
          this->method = env.substr(equalIndex + 1);
        } else if(key == "REQUEST_URI") {
          this->uri = env.substr(equalIndex + 1);
        } else {
          std::string prefix = env.substr(0, 5);
          if(prefix == "HTTP_") {
            std::string value = env.substr(equalIndex+1);
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
      uri.clear();
      return *this;
    }
    /* 获取 method + uri 字符串 */
    std::string methodUri() const {
      std::string upperMethod = method;
      std::transform(upperMethod.begin(), upperMethod.end(),upperMethod.begin(), ::toupper);
      return upperMethod + uri;
    }
    virtual bool isEmpty() override {
      return HttpContent::isEmpty() && method.empty() && uri.empty();
    }
    virtual void printSelf() override {
      HttpContent::printSelf();
      logWarning << std::endl
              << "****************   Request ↓↓↓  ***************" << std::endl
              << nlohmann::json(*this).dump(2) << std::endl
              << "****************   Request done  ***************" << logEndl;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(HttpRequest,
    // NLOHMANN_DEFINE_NORMAL_TYPE_INTRUSIVE(HttpRequest,
                                          method,
                                          uri,
                                          headers,
                                          body);
  };

  struct HttpResponse : virtual HttpContent {

    HttpStatus::Enum status = HttpStatus::Enum::unsetErr;

    void appendDefaultHeaders() {
      if (!headers.contains(HttpHeaderNames::instance().contentType)) headers[HttpHeaderNames::instance().contentType] = MediaType::instance().json + "; charset=utf-8";
      if (!headers.contains(HttpHeaderNames::instance().pid)) headers[HttpHeaderNames::instance().pid] = std::to_string(getpid());
      if (!headers.contains(HttpHeaderNames::instance().tid)) headers[HttpHeaderNames::instance().tid] = boost::str(boost::format("%1%") % std::this_thread::get_id());
    }

    HttpResponse& setStatus(const HttpStatus::Enum& enumStatus) {
      this->status = enumStatus;
      return *this;
    }
    int getIntStatus() const { return static_cast<int>(status); }
    std::string      getStatusName() const { return HttpStatus::enum_to_str(status); }
    std::string_view getStatusNameView() const { return HttpStatus::enum_to_str_view(status); }

    std::string toString() {
      std::stringstream ss;

      const std::string formatEndLine = "\r\n";

      /* 状态码 */ ss << "Status: " << HttpStatus::enum_to_str(status) << formatEndLine;
      /* header */ for (const auto& objPair : headers.items()) {
        ss << objPair.key() << ":" << objPair.value() << formatEndLine;
      }
      /*空行*/ ss << formatEndLine;
      /* body */ ss << body.dump(2);

      return ss.str();
    }

    wstring toWString() {
      std::string str = toString();
      wstring result = utfMbsrtowcs(str.c_str());
      return result;
    }

    virtual HttpResponse& clear() override {
      HttpContent::clear();
      status = HttpStatus::Enum::unsetErr;
      return *this;
    }

    virtual bool isEmpty() override {
      return HttpContent::isEmpty() && (status == HttpStatus::Enum::unsetErr) ;
    }
    virtual void printSelf() override {
      HttpContent::printSelf();
      logInfo<< std::endl;
      switch(status) {
        case HttpStatus::Enum::success      :
        case HttpStatus::Enum::requestHelp  : {
          logInfo << std::endl
                  << "****************   Response [" << getIntStatus() << "] ↑↑↑  ***************" << std::endl
                  << nlohmann::json(*this).dump(2) << std::endl;
        } break;
        default: {
          logInfo << std::endl
                  << "****************   Response [" << getIntStatus() << "] ×××  ***************" << std::endl
                  << nlohmann::json(*this).dump(2) << logEndl;
        } break;
      }
      logInfo << "****************   Response [" << getIntStatus() << "] done  ***************" << std::endl ;
    }

    void initForRequestBodyFormatError(const std::string& internalMsg = constantStr.bodyFormatErr,
                                       const std::string& externalMsg = "") {
      inflateResponse(HttpStatus::Enum::badRequest, internalMsg, externalMsg);
    }

    /* 初始化请求错误返回信息 */
    HttpResponse& inflateResponse(const HttpStatus::Enum& statusEnum,
                         const std::string&    internalMsg = "",
                         const std::string&    externalMsg = "") {
      status = statusEnum;
      body   = ResponseBody{
          getIntStatus(),
          "",
          HttpStatus::enum_to_desc(statusEnum),
          internalMsg,
          externalMsg};

      return *this;
    }
    HttpResponse& inflateBodyData(const std::string& bodyData) {
      body[ResponseBody::Names::data()] = bodyData;
      return *this;
    }
    HttpResponse& inflateBodyData(const nlohmann::json& bodyData) {
      body[ResponseBody::Names::data()] = bodyData;
      return *this;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(HttpResponse,
                                                status,
                                                headers,
                                                body);
  };

}
