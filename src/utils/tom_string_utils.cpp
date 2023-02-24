#include "tom_string_utils.h"
#include <cctype>
#include <iostream>
#include <cstring>
#include <locale>
#include <codecvt>
#include <clocale>
#include <cwchar>

namespace tom_utils {

  void test() {

    // std::cout  << "loclae:" << std::setlocale(LC_ALL, NULL) << std::endl;
    // std::setlocale(LC_ALL, "en_US.UTF-8");
    std::cout  << "loclae:" << std::setlocale(LC_ALL, NULL) << std::endl;

    
    std::cout  << "cout.getloc().name()-->" << std::cout .getloc().name() << std::endl;
    std::cout  << "wcout.getloc().name()-->" << std::wcout.getloc().name() << std::endl;

    const char32_t cp32[] = U"你好啊";
    std::cout  << "sizeof(cp32)/sizeof(char32_t) : " 
        << sizeof(cp32) << '/' << sizeof(char32_t) << '='
        << sizeof(cp32)/sizeof(char32_t) 
        << std::endl;

    const char16_t cp16[] = u"你好啊";
    std::cout  << "sizeof(cp16)/sizeof(char16_t) : " 
        << sizeof(cp16) << '/' << sizeof(char16_t) << '='
        << sizeof(cp16)/sizeof(char16_t) 
        << std::endl;
    
    const char cp8[] = "你好啊";
    std::cout  << "sizeof(cp8)/sizeof(char) : " 
        << sizeof(cp8) << '/' << sizeof(char) << '='
        << sizeof(cp8)/sizeof(char) 
        << "\t" << cp8
        << std::endl;

    const wchar_t cpW[] = L"你好啊";
    std::cout  << "sizeof(cpW)/sizeof(wchar_t) : " 
        << sizeof(cpW) << '/' << sizeof(wchar_t) << '='
        << sizeof(cpW)/sizeof(wchar_t) 
        << std::endl;


    std::wcout << "****************************::::" << /* cp32 << */ cpW << std::endl;
    //setlocale(LC_ALL, "zh_CN.UTF-8");
    std::ios_base::sync_with_stdio(false);
    std::wcout.imbue(std::locale("zh_CN.UTF-8"));
    std::cout  << "\nloclae:" << std::setlocale(LC_ALL, NULL) << std::endl;
    std::wcout << "****************************" << /* cp32 << */ cpW << std::endl;

    const wchar_t cpW2[] = L"你好啊巴拉巴巴";
    std::wcout << "****************************" << /* cp32 << */ cpW2 << std::endl;


    // const  wchar_t *testCpW = {L"中文按时;对开发商;地方"};
    // wprintf(L"wprintf0");
    // wprintf(L"wprintf1:%s\n", cpW);
    // printf("printf2:%ls\n", cpW);
    // std::setlocale(LC_ALL, "zh_CN.UTF-8");
    // std::cout  << "\nloclae:" << std::setlocale(LC_ALL, NULL) << std::endl;
    // wprintf(L"wprintf3:%ls\n", cpW);
    // printf("printf4:%s\n", cpW);

    // u32string u32Str = U"你好啊 ABC 32 - ";

    // std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    // std::cout  << std::stoi(convert.to_bytes(u32Str))  << std::endl;
    
    //===========================================================

    printf("我是测试代码 \n");
    const char *zhCnP  {"铭嘼"};
    const char *enUsP  {"Z"};

    char zhCn = *zhCnP;
    char enUs = *enUsP;

    std::cout  << static_cast<int>(*zhCnP) << std::endl;
    std::cout  << (long long)(*zhCnP) << std::endl;

    std::cout  << zhCnP << "转换为" << tolower(*zhCnP) << std::endl;
    std::cout  << enUsP << "转换为" << tolower(*enUsP) << std::endl;
    printf("%c 转换为 %c \n", zhCn, tolower(zhCn));
    printf("%c 转换为 %c \n", enUs, tolower(enUs));


    const char *cpZh = {"一共五个字"};
    std::cout  << "cpZh --> \t  " << cpZh << ":" << strlen(cpZh) << "\t" << cpZh << std::endl;
    std::cout  << "cpZh --> \t  " << cpZh << ":" << sizeof(cpZh) << "\t" << cpZh << std::endl;

    std::string strZh = { std::string(cpZh) };
    std::cout  << "strZh --> \t  " << strZh << ":" << strZh.length() << "\t" << strZh << std::endl;
    std::cout  << "strZh --> \t  " << strZh << ":" << strZh.size() << "\t" << strZh << std::endl;


    std::string strZhThree("三个字");
    std::cout  << "strZhThree --> \t  " << strZhThree << ":" << strZhThree.length() << "\t" << strZhThree << std::endl;
    std::cout  << "strZhThree --> \t  " << strZhThree << ":" << strZhThree.size() << "\t" << strZhThree << std::endl;

    const char *cp = {"four"};
    std::cout  << "cp --> \t  " << cp << ":" << strlen(cp) << std::endl;
    std::cout  << "cp --> \t  " << cp << ":" << sizeof(cp) << std::endl;


    char charAryEN[] = {"four"};
    std::cout  << "charAryEN --> \t  " << charAryEN << ":" << strlen(charAryEN) << std::endl;
    std::cout  << "charAryEN --> \t  " << charAryEN << ":" << sizeof(charAryEN) << std::endl;

    char charAry[] = {"一\0共五个字"};
    std::cout  << "charAry --> \t  " << charAry << ":" << strlen(charAry) << std::endl;
    std::cout  << "charAry --> \t  " << charAry << ":" << sizeof(charAry) << std::endl;
    std::cout  << "charAry --> \t  " << charAry << ":" << sizeof(charAry[0]) << ":" << charAry[0] << std::endl;
  }

  const char *defLocalStr = "C.UTF-8";

  std::wstring utfMbsrtowcs(const char *input , const char *localeStr/*  = defLocalStr */) {
    setlocale(LC_CTYPE, localeStr);
    int srcLength = strlen(input);
    wchar_t dest[srcLength];
    memset(dest, 0, srcLength);
    size_t convertCount = mbsrtowcs(dest, &input, srcLength, nullptr);
    return std::wstring(dest).substr(0,convertCount);
  }
  std::wstring utfMbsrtowcs(std::string input,      const char *localeStr/*  = defLocalStr */) {
    return utfMbsrtowcs(input.c_str(), localeStr);
  }

  std::string utfWcstombs(const wchar_t *input, const char *localeStr/*   = defLocalStr */) {
    setlocale(LC_CTYPE, localeStr);
    int rawStrLength = wcslen(input);
    int maxLength = rawStrLength * 4;
    char dest[maxLength];
    memset(dest, 0, maxLength);
    //动态内存还需要处理内存分配失败的异常 , 所以没有使用它
    //char *dest = static_cast<char*>(calloc(maxLength, sizeof(char)));
    //char *dest = static_cast<char*>(malloc(maxLength));
    size_t convertCount = wcstombs(dest, input, maxLength); /* 乘以 4 是因为我们判断宽字符转换成窄字符 , 最多需要四个 窄字符 */
    std::string result = std::string(dest).substr(0, convertCount);
    //free(dest);
    return result;
  }
  std::string utfWcstombs(std::wstring input,        const char *localeStr /* = defLocalStr */) {
    return utfWcstombs(input.c_str(), localeStr);
  }


}  // namespace tom_utils