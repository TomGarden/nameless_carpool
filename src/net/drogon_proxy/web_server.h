/**
 *  @desc : HttpServer , 后端容器存在
 *  Created by Tom on 2023/3/16.
 */


#pragma once
#include <drogon/HttpSimpleController.h>
#include <drogon/drogon.h>

#include "http_util.h"

namespace nameless_carpool {

  class WebServer;
}  // namespace nameless_carpool

class nameless_carpool::WebServer {
 private:
  void convert(const drogon::HttpRequestPtr& inDrogonHttpRequestPtr, nameless_carpool::HttpRequest& outNamelessHttpRequest);
  void convert(const nameless_carpool::HttpResponse& inNamelessHttpResponse, const drogon::HttpResponsePtr& outDrogonHttpResponsePtr);


 public:
  void withHandler();
  void run();
};