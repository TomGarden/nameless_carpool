#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <stdlib.h>
#include <cstring>

/** 
 * 在我们的测试中 , 模板函数的定义与声明分离 , 会链接失败 : https://stackoverflow.com/questions/3261093/function-template-in-a-namespace-in-a-separate-file-compiles-fine-but-the-linke
 * 代码拷贝自 : https://gist.github.com/Zitrax/a2e0040d301bf4b8ef8101c0b1e3f1d5
 */
namespace tom_utils {

  using namespace std;

  // template <typename Arg>
  // auto convert_string_to_charp(Arg&& arg) {
  //   if constexpr (std::is_same_v<std::remove_cv_t< std::remove_reference_t<Arg> >, std::string>) {
  //     return arg.c_str();
  //   } else {
  //     return std::forward<Arg>(arg);
  //   }
  // }

  // /** 定义一个格式化字符串的能力 
  //  * 
  //  * call example : 
  //  * 
  //  * std::string result_str = {
  //  *   tom_utils::string_format (
  //  *     "我是一个字符串:\n\t 整数:%d\n\t 字符串:%s\n\t 浮点数:%f\n", 
  //  *     333, 
  //  *     "子串",
  //  *     12.12f)
  //  * };
  //  * printf(result_str.c_str());
  //  * 
  // */
  // template <typename... Args>
  // std::string string_format(const std::string& format, Args&&... args) {
  //   return tom_utils::string_format(format.c_str(), std::forward<Args>(args)...);
  // }

  // /** GCC(g++ (Debian 10.2.1-6) 10.2.1 20210110) 还没有支持 C++20 的特性
  //  * `std::format` 支持列表:
  //  *   1. [cpp reference说明](https://en.cppreference.com/w/cpp/compiler_support)
  //  *   2. [GUN 官方说明 需要全文检索 `std::format`](https://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html#status.iso.2020)
  //  *  string_format 拷贝自 
  //  *   1. https://stackoverflow.com/a/26221725/7707781
  //  *   2. https://gist.github.com/Zitrax/a2e0040d301bf4b8ef8101c0b1e3f1d5
  //  */
  // template <typename... Args>
  // std::string string_format(const char* format, Args&&... args) {
  //   /** 有必要对 snprintf 做出说明
  //       @param buf_size 写入至多 buf_size - 1 个字符，再加上空终止符
  //       @return int 若成功则为会写入充分大缓冲区的字符数（不包含空终止字符），若发生错误则为负值。 

  //       +1 的动作是为了把 空字符也计数
  //    */
  //   int str_length = { std::snprintf(nullptr, 0, format, convert_string_to_charp(std::forward<Args>(args))...) + 1 };
  //   //int str_length = {std::snprintf(nullptr, 0, format, std::forward<Args>(args)...) + 1};
  //   if(str_length < 0) {
  //     throw std::runtime_error("格式化字符串失败① errnum = " + errno);
  //   }

  //   std::size_t str_size = {static_cast<std::size_t>(str_length)};
  //   std::unique_ptr<char[]> buffer = {std::make_unique<char[]>(str_size)};
  //   char *buf_point = buffer.get();
  //   /* buf_point 最后是包含一个空字符的 */
  //   int result_b{
  //       std::snprintf(buf_point, str_size, format,
  //                     convert_string_to_charp(std::forward<Args>(args))...)};

  //   if(result_b < 0) {
  //     throw std::runtime_error("格式化字符串失败② errnum = " + errno);
  //   }

  //   std::string result = std::string(buf_point, str_size - 1 );
  //   return result;

  //   // int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;  // Extra space for '\0'
  //   // if (size_s <= 0) {
  //   //   throw std::runtime_error("Error during formatting.");
  //   // }
  //   // auto size = static_cast<size_t>(size_s);
  //   // auto buf = std::make_unique<char[]>(size);
  //   // std::snprintf(buf.get(), size, format.c_str(), args...);
  //   // return std::string(buf.get(), buf.get() + size - 1);  // We don't want the '\0' inside
  // }

  // char* convertCharCase(bool isUpper, char *input) {
  //   if(isUpper) {
      
  //   } else {

  //   }
  //   return "";
  // }

  // char* toLowerCase(char* input) {
    
  //   std::string(input);

  //   return "";
  // }

  // char* toUpperCase(char* input) {
  //   return "";
  // }

  
  extern const char *defLocalStr;

  /** UTF8 'char*' convert to 'wstring' */
  extern wstring utfMbsrtowcs(const char *input, const char *localeStr = defLocalStr);
  extern wstring utfMbsrtowcs(string input,      const char *localeStr = defLocalStr);

  extern string utfWcstombs(const wchar_t *input, const char *localeStr = defLocalStr);
  extern string utfWcstombs(wstring input,        const char *localeStr = defLocalStr);
  
  /** 测试函数 */
  void test();
}
