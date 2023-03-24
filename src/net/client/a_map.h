//
// Created by Tom on 2023/3/22.
//

#pragma once

#include <string>

#include "http_util.h"
#include "application.h"
#include "input_tip.h"

#include <drogon/drogon.h>

namespace nameless_carpool {

  class AMap;

}  // namespace nameless_carpool

/** 管理与 高德地图的 api 交互 */
class nameless_carpool::AMap {
 private:
  const std::string aMapHost        = "restapi.amap.com";
  const std::string positonInputTip = "/v3/assistant/inputtips"; /*位置输入提示 : https://lbs.amap.com/api/webservice/guide/api/inputtips/*/

 public:
  static AMap& instance() {
    static AMap aMap;
    return aMap;
  }
  inline std::string getAMapKey() { return app().localConfig.amap_key.value_or(""); }
  inline drogon::HttpClientPtr drogonNewClient() { return drogon::HttpClient::newHttpClient(getHttpScheme() + aMapHost); }

  std::string strHttpRequest(const drogon::HttpClientPtr& clientPtr, const drogon::HttpRequestPtr requestPtr);
  std::string strHttpResponse(const std::string& urlStr,const std::pair<drogon::ReqResult, drogon::HttpResponsePtr>& responsePair);

  std::pair<drogon::ReqResult, drogon::HttpResponsePtr> sendSyncRequest(const drogon::HttpRequestPtr& inRequestPtr, double timeout = 0);

  void doPositionInputTip(const nlohmann::json& inInputTip,HttpResponse& outResponse);
};

namespace nameless_carpool {

  inline AMap& aMap() { return AMap::instance(); }

}  // namespace nameless_carpool