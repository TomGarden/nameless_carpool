#pragma once

#include <unistd.h>

#include <boost/format/format_fwd.hpp>
#include <chrono>
#include <exception>
#include <iostream>
#include <rxcpp/operators/rx-observe_on.hpp>
#include <rxcpp/rx-coordination.hpp>
#include <rxcpp/rx-includes.hpp>
#include <rxcpp/schedulers/rx-runloop.hpp>
#include <rxcpp/schedulers/rx-sameworker.hpp>
#include <rxcpp/subjects/rx-synchronize.hpp>
#include <thread>

#include "src/utils/log_utils.h"

namespace nameless_carpool {

  void testRxCxx0() {
    auto ints = rxcpp::observable<>::create<int>(
        [](rxcpp::subscriber<int> s) {
          s.on_next(1);
          s.on_next(2);
          s.on_completed();
        });
    ints.subscribe(
        [](int v) -> void { printf("OnNext: %d\n", v); },
        [](std::exception_ptr& exceptionPtr) {},
        []() { printf("OnCompleted\n"); });
  }

  void testRxCxx3() {
    auto flagAuto = rxcpp::observable<>::create<int>(
        [](rxcpp::subscriber<int> objSubscriber) {
          try {
            objSubscriber.on_next(1);
            objSubscriber.on_next(2);
            objSubscriber.on_next(3);
            logInfo << "try code" << std::endl;
          } catch (std::exception& exception) {
            objSubscriber.on_error(std::make_exception_ptr(exception));
            return;
          }

          objSubscriber.on_completed();
        });
    auto objDestory =
        flagAuto
            .observe_on(rxcpp::synchronize_event_loop())
            // .subscribe_on(rxcpp::synchronize_new_thread())
            .subscribe(
                [](int item) { logInfo << item << std::endl; },
                [](const std::exception_ptr& objExceptionPtr) { logInfo << "objExceptionPtr" << std::endl; },
                []() { logInfo << "onComplete" << std::endl; });

    // objDestory.
  }

  std::string get_pid() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
  }

  void testRxCxx4() {
    logInfo << boost::format("[thread %s] Start task \n") % get_pid().c_str();


    rxcpp::composite_subscription lifetime;

    rxcpp::schedulers::run_loop runLoop;
    auto                        mainThread = rxcpp::observe_on_run_loop(runLoop);

    auto objObservable =
        rxcpp::observable<>::create<int>(
            [](rxcpp::subscriber<int> objSubscriber) {
              try {
                objSubscriber.on_next(1);
                objSubscriber.on_next(2);
                objSubscriber.on_next(3);
                logInfo << "try code" << std::endl;
              } catch (std::exception& exception) {
                objSubscriber.on_error(std::make_exception_ptr(exception));
                return;
              }

              objSubscriber.on_completed();
            });
    //            .map([](int v) {
    //              logInfo << boost::format("[thread %s] Emit value %d\n") % get_pid().c_str() % v;
    //              std::this_thread::sleep_for(1000ms);
    //              return v;
    //            });

    //    auto testResult = values
    //        .subscribe_on(rxcpp::synchronize_new_thread())
    //        .observe_on(mainThread)
    //        .as_dynamic();

    objObservable
        .subscribe_on(rxcpp::synchronize_new_thread())
        .observe_on(mainThread)
        //                .as_blocking()
        //                .as_dynamic()
        .subscribe(
            lifetime,
            [](int v) {
              std::cout << boost::format("[thread %s] onNext %d\n") % get_pid().c_str() % v;
              std::cout.flush();
              logFlush();
            },
            [&lifetime](const std::exception_ptr& objExceptionPtr) {
              logInfo << "objExceptionPtr" << std::endl;
              logInfo << "lambda:" << reinterpret_cast<std::int64_t>(&lifetime) << std::endl;
              lifetime.unsubscribe();
              logFlush();
            },
            [&]() {
              logInfo << boost::format("[thread %s] onComplete\n") % get_pid().c_str();
              logInfo << "lambda:" << reinterpret_cast<std::int64_t>(&lifetime) << std::endl;
              lifetime.unsubscribe();
              logFlush();
            });

    logInfo << boost::format("[thread %s] Finish task \n") % get_pid().c_str();


    //    lifetime.unsubscribe();
    logInfo << "main_tread:" << reinterpret_cast<std::int64_t>(&lifetime) << std::endl;
    logInfo << std::boolalpha << lifetime.is_subscribed() << std::endl;
    while (true) {
      logInfo << std::boolalpha << lifetime.is_subscribed() << std::endl;
      if (lifetime.is_subscribed()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
      } else {
        break;
      }
    }

    logFlush();
    // std::this_thread::
    // std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    // disposable.add()
    // disposable.clear();
    // disposable.is_subscribed();
    // disposable.unsubscribe();
  }

  void testRxCxx() {
    logInfo << boost::format("[thread %s] Start task \n") % get_pid().c_str();


    rxcpp::composite_subscription lifetime;

    rxcpp::schedulers::run_loop  runLoop;
    rxcpp::observe_on_one_worker mainThread = rxcpp::observe_on_run_loop(runLoop);

    rxcpp::observable<>::create<int>(
        [](rxcpp::subscriber<int> objSubscriber) {
          try {
            objSubscriber.on_next(1);
            objSubscriber.on_next(2);
            objSubscriber.on_next(3);
            logInfo << "try code" << std::endl;
          } catch (std::exception& exception) {
            objSubscriber.on_error(std::make_exception_ptr(exception));
            return;
          }

          objSubscriber.on_completed();
        })
        .subscribe_on(rxcpp::synchronize_new_thread())
        .observe_on(mainThread)
        .subscribe(
            lifetime,
            [](int v) {
              logInfo << boost::format("[thread %s] onNext %d\n") % get_pid().c_str() % v;
              logFlush();
            },
            [&lifetime](const std::exception_ptr& objExceptionPtr) {
              logInfo << "objExceptionPtr" << std::endl;
              logInfo << "lambda:" << reinterpret_cast<std::int64_t>(&lifetime) << std::endl;
              lifetime.unsubscribe();
              logFlush();
            },
            [&lifetime]() {
              logInfo << boost::format("[thread %s] onComplete\n") % get_pid().c_str();
              logInfo << "lambda:" << reinterpret_cast<std::int64_t>(&lifetime) << std::endl;
              lifetime.unsubscribe();
              logFlush();
            });

    logInfo << boost::format("[thread %s] Finish task \n") % get_pid().c_str();

    logInfo << "main_tread:" << reinterpret_cast<std::int64_t>(&lifetime) << std::endl;
    while (lifetime.is_subscribed()) {
      while (!runLoop.empty() && runLoop.peek().when <= runLoop.now()) runLoop.dispatch(); /* 手动事件分发 */
      logInfo << std::boolalpha << lifetime.is_subscribed() << std::endl;
      if (!runLoop.empty()) runLoop.dispatch();
      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
    logInfo << std::boolalpha << lifetime.is_subscribed() << std::endl;

    logFlush();
  }
}  // namespace nameless_carpool
