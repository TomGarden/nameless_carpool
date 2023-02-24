#pragma once

#include <regex>
#include <string>
#include <stdexcept>
#include <memory>
#include <stdlib.h>
#include <cstring>
#include <vector>


/** 
 * 在我们的测试中 , 模板函数的定义与声明分离 , 会链接失败 : https://stackoverflow.com/questions/3261093/function-template-in-a-namespace-in-a-separate-file-compiles-fine-but-the-linke
 * 代码拷贝自 : https://gist.github.com/Zitrax/a2e0040d301bf4b8ef8101c0b1e3f1d5
 */
namespace tom_utils {

  
  extern const char *defLocalStr;

  /** UTF8 'char*' convert to 'wstring' */
  extern std::wstring utfMbsrtowcs(const char *input, const char *localeStr = defLocalStr);
  extern std::wstring utfMbsrtowcs(std::string  input,      const char *localeStr = defLocalStr);

  extern std::string utfWcstombs(const wchar_t *input, const char *localeStr = defLocalStr);
  extern std::string utfWcstombs(std::wstring input,        const char *localeStr = defLocalStr);
  


  /** @description: 返回 sepRegex 分隔的子串列表 .
   * @param {string} &str
   * @param {regex} &sepRegex
   * @return {*}
   */
  inline std::vector<std::string> split(const std::string &str, const std::regex &sepRegex = std::regex{"\\s+"}) {
    /* https://zh.cppreference.com/w/cpp/regex/regex_token_iterator/regex_token_iterator
       返回与 setRegex 不匹配的子串
     */
    std::sregex_token_iterator iter(str.begin(), str.end(), sepRegex, -1);
    std::sregex_token_iterator end;
    return {iter, end};
  }

  // trim from start (in place)
  static inline void trimLeft(std::string &str) {
    std::string::iterator position =
        std::find_if(str.begin(), str.end(), [](unsigned char ch) { return !std::isspace(ch); });
    str.erase(str.begin(), position);
  }

  // trim from end (in place)
  static inline void trimRight(std::string &str) {
    std::string::iterator position =
        std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base();
    str.erase(position, str.end());
  }

  // trim from both ends (in place)
  static inline void trim(std::string &str) {
    trimRight(str);
    trimLeft(str);
  }

  // trim from start (copying)
  static inline std::string trimLeftCopy(std::string str) {
    trimLeft(str);
    return str;
  }

  // trim from end (copying)
  static inline std::string trimRightCopy(std::string str) {
    trimRight(str);
    return str;
  }

  // trim from both ends (copying)
  static inline std::string trimCopy(std::string str) {
    trim(str);
    return str;
  }

  template <typename Number>
  static inline std::string numToStr(const Number& inNumber) {
    std::ostringstream oStrStream;
    oStrStream << inNumber;
    return oStrStream.str();
  }

  /** 将 inNum 转换为 十六进制 字符串 ， 输出的十六进制字符串如果宽度不够 inWidth , 前面补零 */
  static std::string dec2hex(int inNum, int inWidth) {
    std::stringstream oStrStream;
    oStrStream << std::hex << inNum;
    std::string strResult = oStrStream.str();

    if(inWidth > strResult.size()) {
      std::string strZero(inWidth - strResult.size(), '0');
      strResult = strZero + strResult;
    }

    return std::string("0x") + strResult;
  }
}
