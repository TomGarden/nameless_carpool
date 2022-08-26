
#ifndef nameless_carpool_log_utils_h
#define nameless_carpool_log_utils_h

#include "glog/logging.h"
#include "boost/stacktrace.hpp"

using namespace google;

namespace nameless_carpool {


  /* 如果这个函数 在 main 函数中调用, 这个 progressNmae = argv[0] */
  void initGlog(const char* progressName) {

    /* 将日志信息打印到 stderr , 而不是日志文件 ; 默认值 false */
    //FLAGS_logtostderr = false;

    /* INFO(0), WARNING(1), ERROR(2), FATAL(3) 
       日志信息除了输出到日志文件还拷贝到 stderr , 大于等于这里指定的日志级别就进行拷贝 . 
       默认值 2 */
    FLAGS_stderrthreshold = google::INFO;

    /* 大于等于此日志级别的日志将被输出 , 默认 INFO(0) */
    //FLAGS_minloglevel = google::INFO;

    /* 如果设置了这个值 , 日志文件就会写入这个文件夹下 , 默认在 `/tmp` 文件夹 ;
        此标志必须在 initGoogleLogging 之前设置才能生效 . 
        其他的标志随时可以设置 , 设置后生效 . */
    FLAGS_log_dir = "/mount_point/data/_document/c_cpp_program/nameless_carpool/tmp_dir/run_log";

    /* 剩余两个标志 , 暂不介绍 */
    //FLAGS_v
    //FLAGS_vmodule

    /* 启用异常信息转储 , 要自定义转储信息格式需要使用 InstallFailureWriter() */
    google::InstallFailureSignalHandler();

    google::InitGoogleLogging(progressName);


  }

  void gLogFlushAllLogFiles() {
    FlushLogFiles(google::INFO);
    FlushLogFiles(google::WARNING);
    FlushLogFiles(google::ERROR);
    FlushLogFiles(google::FATAL);
  }

  /* https://github.com/boostorg/stacktrace */
  string getStackTrace() {
    using namespace boost::stacktrace;
    return to_string(stacktrace());
  }

}


#endif