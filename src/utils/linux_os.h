#pragma once

#include <string>
#include <typeinfo>
#include <cxxabi.h>

namespace nameless_carpool {

  /* 获取当前程序的路径 ,
     @return true , 获取成功
             false, 获取失败 */
  extern bool getCurExePath(std::string& result);
  extern std::string getCurExePathOrThrow();

  /* 获取当前程序所在文件夹 
     @return true , 获取成功
             false, 获取失败 */
  extern bool getCurExeFileDir(std::string& result);
  extern std::string getCurExeFileDirOrThrow();

  /* 获取 typeInfo 对应的类的真实名称 */
  std::string getTypeName(const std::type_info& typeInfo) ;
  std::string getFiledTypeName(auto filed);
}