#pragma once

#include <string>

#include "glog/logging.h"
#include "boost/stacktrace.hpp"

using namespace google;

namespace nameless_carpool {

  /* 标示符 : 如果定义了这个宏 , 那就说明在 debug 模式下 */
  #define DEBUG_MODEL
  #define logInfo     google::LogMessage(__FILE__, __LINE__, INFO).stream()
  #define logError    google::LogMessage(__FILE__, __LINE__, ERROR).stream()
  #define logWarning  google::LogMessage(__FILE__, __LINE__, WARNING).stream()
  #define logFatal    google::LogMessage(__FILE__, __LINE__, FATAL).stream()
  #ifdef DEBUG_MODEL
    #define logDebug    google::LogMessage(__FILE__, __LINE__, ERROR).stream()
  #else
    #define logDebug    google::LogMessage(__FILE__, __LINE__, FATAL).stream()
  #endif

  /* trace 就携带了调用栈信息 */
  #define logInfoTrace     logInfo     << getStackTrace() << "\n" 
  #define logErrorTrace    logError    << getStackTrace() << "\n"  
  #define logWarningTrace  logWarning  << getStackTrace() << "\n"    
  #define logFatalTrace    logFatal    << getStackTrace() << "\n"  
  #define logDebugTrace    logDebug    << getStackTrace() << "\n"  

  /* 如果这个函数 在 main 函数中调用, 这个 progressNmae = argv[0] */
  extern void initGlog(const char* progressName) ;
  
  /* 清空日志区域缓存, 避免某个日志动作无法完全输出日志内容 */
  extern void logFlush() ;
  

  /* https://github.com/boostorg/stacktrace */
  extern std::string getStackTrace() ;

}
