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

    // cout << "loclae:" << std::setlocale(LC_ALL, NULL) << endl;
    // std::setlocale(LC_ALL, "en_US.UTF-8");
    cout << "loclae:" << std::setlocale(LC_ALL, NULL) << endl;

    
    cout << "cout.getloc().name()-->" << cout.getloc().name() << endl;
    cout << "wcout.getloc().name()-->" << wcout.getloc().name() << endl;

    const char32_t cp32[] = U"你好啊";
    cout << "sizeof(cp32)/sizeof(char32_t) : " 
        << sizeof(cp32) << '/' << sizeof(char32_t) << '='
        << sizeof(cp32)/sizeof(char32_t) 
        << endl;

    const char16_t cp16[] = u"你好啊";
    cout << "sizeof(cp16)/sizeof(char16_t) : " 
        << sizeof(cp16) << '/' << sizeof(char16_t) << '='
        << sizeof(cp16)/sizeof(char16_t) 
        << endl;
    
    const char cp8[] = "你好啊";
    cout << "sizeof(cp8)/sizeof(char) : " 
        << sizeof(cp8) << '/' << sizeof(char) << '='
        << sizeof(cp8)/sizeof(char) 
        << "\t" << cp8
        << endl;

    const wchar_t cpW[] = L"你好啊";
    cout << "sizeof(cpW)/sizeof(wchar_t) : " 
        << sizeof(cpW) << '/' << sizeof(wchar_t) << '='
        << sizeof(cpW)/sizeof(wchar_t) 
        << endl;


    wcout << "****************************::::" << /* cp32 << */ cpW << endl;
    //setlocale(LC_ALL, "zh_CN.UTF-8");
    std::ios_base::sync_with_stdio(false);
    wcout.imbue(locale("zh_CN.UTF-8"));
    cout << "\nloclae:" << std::setlocale(LC_ALL, NULL) << endl;
    wcout << "****************************" << /* cp32 << */ cpW << endl;

    const wchar_t cpW2[] = L"你好啊巴拉巴巴";
    wcout << "****************************" << /* cp32 << */ cpW2 << endl;


    // const  wchar_t *testCpW = {L"中文按时;对开发商;地方"};
    // wprintf(L"wprintf0");
    // wprintf(L"wprintf1:%s\n", cpW);
    // printf("printf2:%ls\n", cpW);
    // std::setlocale(LC_ALL, "zh_CN.UTF-8");
    // cout << "\nloclae:" << std::setlocale(LC_ALL, NULL) << endl;
    // wprintf(L"wprintf3:%ls\n", cpW);
    // printf("printf4:%s\n", cpW);

    // u32string u32Str = U"你好啊 ABC 32 - ";

    // std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    // cout << std::stoi(convert.to_bytes(u32Str))  << endl;
    
    //===========================================================

    printf("我是测试代码 \n");
    const char *zhCnP  {"铭嘼"};
    const char *enUsP  {"Z"};

    char zhCn = *zhCnP;
    char enUs = *enUsP;

    cout << static_cast<int>(*zhCnP) << endl;
    cout << (long long)(*zhCnP) << endl;

    cout << zhCnP << "转换为" << tolower(*zhCnP) << endl;
    cout << enUsP << "转换为" << tolower(*enUsP) << endl;
    printf("%c 转换为 %c \n", zhCn, tolower(zhCn));
    printf("%c 转换为 %c \n", enUs, tolower(enUs));


    const char *cpZh = {"一共五个字"};
    cout << "cpZh --> \t  " << cpZh << ":" << strlen(cpZh) << "\t" << cpZh << endl;
    cout << "cpZh --> \t  " << cpZh << ":" << sizeof(cpZh) << "\t" << cpZh << endl;

    std::string strZh = { std::string(cpZh) };
    cout << "strZh --> \t  " << strZh << ":" << strZh.length() << "\t" << strZh << endl;
    cout << "strZh --> \t  " << strZh << ":" << strZh.size() << "\t" << strZh << endl;


    std::string strZhThree("三个字");
    cout << "strZhThree --> \t  " << strZhThree << ":" << strZhThree.length() << "\t" << strZhThree << endl;
    cout << "strZhThree --> \t  " << strZhThree << ":" << strZhThree.size() << "\t" << strZhThree << endl;

    const char *cp = {"four"};
    cout << "cp --> \t  " << cp << ":" << strlen(cp) << endl;
    cout << "cp --> \t  " << cp << ":" << sizeof(cp) << endl;


    char charAryEN[] = {"four"};
    cout << "charAryEN --> \t  " << charAryEN << ":" << strlen(charAryEN) << endl;
    cout << "charAryEN --> \t  " << charAryEN << ":" << sizeof(charAryEN) << endl;

    char charAry[] = {"一\0共五个字"};
    cout << "charAry --> \t  " << charAry << ":" << strlen(charAry) << endl;
    cout << "charAry --> \t  " << charAry << ":" << sizeof(charAry) << endl;
    cout << "charAry --> \t  " << charAry << ":" << sizeof(charAry[0]) << ":" << charAry[0] << endl;
  }

  const char *defLocalStr = "C.UTF-8";

  wstring utfMbsrtowcs(const char *input , const char *localeStr/*  = defLocalStr */) {
    setlocale(LC_CTYPE, localeStr);
    int srcLength = strlen(input);
    wchar_t dest[srcLength];
    memset(dest, 0, srcLength);
    size_t convertCount = mbsrtowcs(dest, &input, srcLength, nullptr);
    return wstring(dest).substr(0,convertCount);
  }
  wstring utfMbsrtowcs(string input,      const char *localeStr/*  = defLocalStr */) {
    return utfMbsrtowcs(input.c_str(), localeStr);
  }

  string utfWcstombs(const wchar_t *input, const char *localeStr/*   = defLocalStr */) {
    setlocale(LC_CTYPE, localeStr);
    int rawStrLength = wcslen(input);
    int maxLength = rawStrLength * 4;
    char dest[maxLength];
    memset(dest, 0, maxLength);
    //动态内存还需要处理内存分配失败的异常 , 所以没有使用它
    //char *dest = static_cast<char*>(calloc(maxLength, sizeof(char)));
    //char *dest = static_cast<char*>(malloc(maxLength));
    size_t convertCount = wcstombs(dest, input, maxLength); /* 乘以 4 是因为我们判断宽字符转换成窄字符 , 最多需要四个 窄字符 */
    string result = string(dest).substr(0, convertCount);
    //free(dest);
    return result;
  }
  string utfWcstombs(wstring input,        const char *localeStr /* = defLocalStr */) {
    return utfWcstombs(input.c_str(), localeStr);
  }


}  // namespace tom_utils