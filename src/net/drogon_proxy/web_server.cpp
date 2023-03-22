//
// Created by Tom on 2023/3/16.
//

#include "web_server.h"

#include <drogon/drogon.h>

#include "api.h"
#include "http_util.h"

namespace nameless_carpool {

  void WebServer::convert(const drogon::HttpRequestPtr& inDrogonHttpRequestPtr, nameless_carpool::HttpRequest& outNamelessHttpRequest) {
    outNamelessHttpRequest.setMethod(inDrogonHttpRequestPtr->getMethodString())
        .setUri(inDrogonHttpRequestPtr->getPath())
        .setHeaders(inDrogonHttpRequestPtr->getHeaders())
        .setBody(inDrogonHttpRequestPtr->bodyData());
  }
  void WebServer::convert(const nameless_carpool::HttpResponse& inNamelessHttpResponse, const drogon::HttpResponsePtr& outDrogonHttpResponsePtr) {
    outDrogonHttpResponsePtr->setCustomStatusCode(inNamelessHttpResponse.getIntStatus(), inNamelessHttpResponse.getStatusNameView());
    for (auto& header :inNamelessHttpResponse.headers.items()) {
      outDrogonHttpResponsePtr->addHeader(header.key(),header.value());
    }
    outDrogonHttpResponsePtr->setBody(inNamelessHttpResponse.body.dump(2));
  }

  void WebServer::withHandler() {

    auto func = [&](const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback) {

      nameless_carpool::HttpRequest  httpRequest;
      nameless_carpool::HttpResponse httpResponse;
      logWarning << "☞☞☞☞  get http request" << logEndl;
      try {
        convert(req, httpRequest);
        httpRequest.printSelf();
      } catch (const std::exception &objException) {
        logError << objException.what() << logEndl;
        httpResponse.inflateResponse(HttpStatus::Enum::badRequest, constantStr.bodyFormatErr);
      }
      logWarning << "☞☞☞☞  http request convert done" << logEndl;

      if (httpResponse.isEmpty()) Api::optRequest(httpRequest, httpResponse);
      logWarning << "☞☞☞☞  httpResponse generate done " << logEndl;
      auto resp = drogon::HttpResponse::newHttpResponse();
      convert(httpResponse, resp);
      logWarning << "☞☞☞☞  httpResponse convert done " << logEndl;
      callback(resp);
      httpResponse.printSelf();
    };

    /*预期效果是所有请求都被转发到 func */
    drogon::app().registerHandlerViaRegex("(.*)", func);

  }

  void WebServer::run(){
    withHandler();

    drogon::app()
        .setLogPath("/mount_point/data/_document/c_cpp_program/nameless_carpool/tmp_dir/drogon_log")
        .addListener("0.0.0.0", 80)
        .run();
  }
}  // namespace nameless_carpool