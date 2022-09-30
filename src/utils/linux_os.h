#pragma once

#include <string>
#include <typeinfo>
#include <cxxabi.h>

namespace nameless_carpool {

  using namespace std;

  /* 获取当前程序的路径 , 
     @return true , 获取成功
             false, 获取失败 */
  extern bool getCurExePath(string& result);

  /* 获取当前程序所在文件夹 
     @return true , 获取成功
             false, 获取失败 */
  extern bool getCurExeFd(string& result);

  /* 获取 typeInfo 对应的类的真实名称 */
  string getTypeName(const std::type_info& typeInfo) ;
  string getFiledTypeName(auto filed);
}