//
// Created by Tom on 2023/3/22.
//

#include "a_map.h"

#include <drogon/drogon.h>

namespace nameless_carpool {

  std::string AMap::strHttpRequest(const drogon::HttpClientPtr& clientPtr, const drogon::HttpRequestPtr requestPtr) {
    std::stringstream strStream;

    strStream << requestPtr->getMethodString() << "   "
              << clientPtr->getHost() << requestPtr->path() << "   "
              << requestPtr->versionString() << std::endl;
    for (const auto& header : requestPtr->parameters()) {
      strStream << header.first << " : " << header.second << std::endl;
    }

    strStream << std::endl
              << requestPtr->body() << std::endl;

    return strStream.str();
  }

  std::string AMap::strHttpResponse(const std::string& urlStr,const std::pair<drogon::ReqResult, drogon::HttpResponsePtr>& responsePair) {
    const drogon::HttpResponsePtr& responsePtr = responsePair.second;
    std::stringstream strStream;
    strStream << responsePtr->getStatusCode() << "   " << responsePair.first << "   " << urlStr << "   " << responsePtr->versionString() << std::endl;
    for (const auto& header : responsePtr->headers()) {
      strStream << header.first << " : " << header.second << std::endl;
    }

    strStream << std::endl
              << responsePtr->body() << std::endl;

    return strStream.str();
  }

  std::pair<drogon::ReqResult, drogon::HttpResponsePtr> AMap::sendSyncRequest(const drogon::HttpRequestPtr& inRequestPtr,double timeout) {
    drogon::HttpClientPtr client = drogonNewClient();
    const std::string& requestStr = strHttpRequest(client,inRequestPtr);
    logInfo << requestStr << logEndl;
    const std::pair<drogon::ReqResult, drogon::HttpResponsePtr>& response = client->sendRequest(inRequestPtr, timeout);

    const std::string& responseStr = strHttpResponse(client->getHost() + inRequestPtr->path(), response);
    logInfo << responseStr << logEndl;

    return response;
  }

  void AMap::doPositionInputTip(const nlohmann::json& inInputTip, HttpResponse& outResponse) {

    auto drogonRequest = drogon::HttpRequest::newHttpRequest();
    drogonRequest->setMethod(drogon::HttpMethod::Get);
    drogonRequest->setPath(positonInputTip);
    for (auto item : inInputTip.items()) {
      drogonRequest->setParameter(item.key(), item.value());
    }

    std::pair<drogon::ReqResult, drogon::HttpResponsePtr> httpResult = aMap().sendSyncRequest(drogonRequest, 2);

    if (httpResult.first != drogon::ReqResult::Ok) {
      const std::string& interMsg = boost::str(boost::format("请求失败 : %s") % drogon::to_string_view(httpResult.first));
      InputTipResponse inputTipResponse;
      inputTipResponse.count = "0";
      outResponse.inflateResponse(HttpStatus::Enum::success, interMsg);
      outResponse.inflateBodyData(inputTipResponse);

    } else {
      try{
        const nlohmann::json& jBody = nlohmann::json::parse(httpResult.second->body());
        outResponse.inflateResponse(HttpStatus::Enum::success);
        outResponse.inflateBodyData(jBody);
      } catch (const std::exception& objException) {
        const std::string& internalMsg = boost::str(boost::format("数据获取成功 , 转换失败 , 系服务端异常[%s]") % objException.what());
        outResponse.inflateResponse(HttpStatus::Enum::success, internalMsg);
      }
    }
  }

}  // namespace nameless_carpool