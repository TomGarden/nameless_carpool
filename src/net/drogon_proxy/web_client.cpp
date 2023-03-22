//
// Created by Tom on 2023/3/16.
//

#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>
// #include "json"

#include <iostream>

#include "include_json.h"
#include "rxcpp/rx-observable.hpp"
#include "src/utils/log_utils.h"
#include "web_client.h"

namespace nameless_carpool {

  int nth_resp = 0;

  void WebClient::test() {

    trantor::Logger::setLogLevel(trantor::Logger::kTrace);
//
//    auto client = drogon::HttpClient::newHttpClient("http://baidu.com");
//    auto req    = drogon::HttpRequest::newHttpRequest();
////    req->setPath("/s");
////    req->setParameter("wd", "wx");
////    req->setParameter("oq", "wx");
//
//    for (int i = 0; i < 10; ++i) {
////      auto client = drogon::HttpClient::newHttpClient("http://baidu.com");
////      auto req    = drogon::HttpRequest::newHttpRequest();
//      req->setMethod(drogon::Get);
//
//      client->sendRequest(
//          req, [](drogon::ReqResult result, const drogon::HttpResponsePtr& response) {
//            if (result != drogon::ReqResult::Ok) {
//              std::cout
//                  << "error while sending request to server! result: "
//                  << result << std::endl;
//              return;
//            }
//
//            std::cout << "receive response!" << std::endl;
//            // auto headers=response.
//            ++nth_resp;
//            std::cout << response->getBody() << std::endl;
//            auto cookies = response->cookies();
//            for (auto const& cookie : cookies) {
//              std::cout << cookie.first << "="
//                        << cookie.second.value()
//                        << ":domain=" << cookie.second.domain()
//                        << std::endl;
//            }
//            std::cout << "count=" << nth_resp << std::endl;
//          });
//      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//    }
//
//
//
//
//    return;


    auto client = drogon::HttpClient::newHttpClient("http://restapi.amap.com");
    auto req    = drogon::HttpRequest::newHttpRequest();
    req->setPath("/v3/place/text");
    req->setParameter("key","f5963d2d0f57b0fe46fc59e422a9dd41");
    req->setParameter("keywords","小直要村委会");


    rxcpp::composite_subscription       lifetime;
    //rxcpp::schedulers::run_loop         runLoop;
    //const rxcpp::observe_on_one_worker& mainThread = rxcpp::observe_on_run_loop(runLoop);
    rxcpp::observable<>::create<std::string>(
        [&](rxcpp::subscriber<std::string> objSubscriber) {
          try {
            const std::pair<drogon::ReqResult, drogon::HttpResponsePtr>& result =
                client->sendRequest(req, 3);
            const drogon::HttpResponse&      httpResponse = *result.second;
            const std::string&                      stateStr     = boost::str(boost::format("response state code : %s") % drogon::to_string_view(result.first));
            const std::string&                      headerStr     = boost::str(boost::format("response header : %s") % nlohmann::json(httpResponse.headers()).dump());
            const std::string&                      bodyStr       = boost::str(boost::format("response body : %s") % httpResponse.getBody());

            objSubscriber.on_next(stateStr);
            objSubscriber.on_next(headerStr);
            objSubscriber.on_next(bodyStr);
          } catch (std::exception& exception) {
            objSubscriber.on_error(std::make_exception_ptr(exception));
          }
          objSubscriber.on_completed();
        })
        .subscribe_on(rxcpp::synchronize_new_thread())
        //.observe_on(mainThread)
        .subscribe(
            lifetime,
            [](const std::string& item) {
              logInfo << item << std::endl;
              logFlush();
            },
            [&lifetime](const std::exception_ptr& objExceptionPtr) {
              try {
                std::rethrow_exception(objExceptionPtr);
              } catch (const std::exception& objException) {
                logError << objException.what() << logEndl;
              }
            },
            []() {
              logInfo << "onComplete" << std::endl;
              logFlush();
            });

    drogon::app().run();
  }

}  // namespace nameless_carpool