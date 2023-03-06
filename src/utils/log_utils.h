#pragma once

#include <boost/format.hpp>
#include <cmath>
#include <string>

#include "glog/logging.h"
#include "boost/stacktrace.hpp"

namespace nameless_carpool {

  /* https://stackoverflow.com/a/58556021/7707781 */
  #define GET_LINE_INFO()  boost::str(boost::format("%1%:%2%") % __FILE__ % __LINE__)
  #define WITH_LINE_INFO(msg) boost::str(boost::format("(%1%):%2%") % GET_LINE_INFO() % msg)
  // #define WITH_LINE_INFO(msg) boost::str(boost::format("(%1%):%2%") % GET_LINE_INFO() % msg);

  /* 标示符 : 如果定义了这个宏 , 那就说明在 debug 模式下 */
  #define DEBUG_MODEL
  
  #define DIVIDE  "↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓\n"
 
  #define LOG_COLOR_RESET              "\033[0m"
  #define LOG_COLOR_DEFAULT            "\033[39m"
  #define LOG_COLOR_FG_BLACK           "\033[30m"    
  #define LOG_COLOR_FG_DARK_RED        "\033[31m"    
  #define LOG_COLOR_FG_DARK_GREEN      "\033[32m"    
  #define LOG_COLOR_FG_DARK_YELLOW     "\033[33m"    
  #define LOG_COLOR_FG_DARK_BLUE       "\033[34m"    
  #define LOG_COLOR_FG_DARK_MAGENTA    "\033[35m"    
  #define LOG_COLOR_FG_DARK_CYAN       "\033[36m"    
  #define LOG_COLOR_FG_LIGHT_GRAY      "\033[37m"    
  #define LOG_COLOR_FG_DARK_GRAY       "\033[90m"    
  #define LOG_COLOR_FG_LIGHT_RED       "\033[91m"    
  #define LOG_COLOR_FG_LIGHT_GREEN     "\033[92m"    
  #define LOG_COLOR_FG_LIGHT_ORANGE    "\033[93m"    
  #define LOG_COLOR_FG_LIGHT_BLUE      "\033[94m"    
  #define LOG_COLOR_FG_LIGHT_MAGENTA   "\033[95m"    
  #define LOG_COLOR_FG_LIGHT_CYAN      "\033[96m"    
  #define LOG_COLOR_FG_LIGHT_WHITE     "\033[97m"    
  #define LOG_COLOR_BG_BLACK           "\033[40m"
  #define LOG_COLOR_BG_DARK_RED        "\033[41m"
  #define LOG_COLOR_BG_DARK_GREEN      "\033[42m"
  #define LOG_COLOR_BG_DARK_YELLOW     "\033[43m"
  #define LOG_COLOR_BG_DARK_BLUE       "\033[44m"
  #define LOG_COLOR_BG_DARK_MAGENTA    "\033[45m"
  #define LOG_COLOR_BG_DARK_CYAN       "\033[46m"
  #define LOG_COLOR_BG_LIGHT_GRAY      "\033[47m"
  #define LOG_COLOR_BG_DARK_GRAY       "\033[100m"
  #define LOG_COLOR_BG_LIGHT_RED       "\033[101m"
  #define LOG_COLOR_BG_LIGHT_GREEN     "\033[101m"
  #define LOG_COLOR_BG_LIGHT_ORANGE    "\033[103m"
  #define LOG_COLOR_BG_LIGHT_BLUE      "\033[104m"
  #define LOG_COLOR_BG_LIGHT_MAGENTA   "\033[105m"
  #define LOG_COLOR_BG_LIGHT_CYAN      "\033[106m"
  #define LOG_COLOR_BG_LIGHT_WHITE     "\033[107m"

  // #define logMsg(payload) google::LogMessage(__FILE__, __LINE__, payload).stream()
  // #define logReset      "\033[0m"
  // #define default       "\033[39m"
  // #define fgBlack(payload) logMsg(payload) << "\033[30m"
  // #define fgBlack()        

  // #define logInfo     LogStreamForward(google::INFO    , LOG_COLOR_DEFAULT)               << "INFO  : "
  // #define logError    LogStreamForward(google::ERROR   , LOG_COLOR_FG_LIGHT_RED)          << "ERROR : "
  // #define logWarning  LogStreamForward(google::WARNING , LOG_COLOR_FG_LIGHT_ORANGE)       << "WARN  : "
  // #define logFatal    LogStreamForward(google::FATAL   , LOG_COLOR_FG_LIGHT_RED BG_BLACK) << "FATAL : "
  
  #define logEndl                                                                         LOG_COLOR_RESET                    << std::endl
  #define logInfo     google::LogMessage(__FILE__, __LINE__, google::INFO).stream()    << LOG_COLOR_DEFAULT                  << " | INFO  : "
  #define logError    google::LogMessage(__FILE__, __LINE__, google::ERROR).stream()   << LOG_COLOR_FG_LIGHT_RED             << " | ERROR : "
  #define logWarning  google::LogMessage(__FILE__, __LINE__, google::WARNING).stream() << LOG_COLOR_FG_LIGHT_ORANGE          << " | WARN  : "
  #define logFatal    google::LogMessage(__FILE__, __LINE__, google::FATAL).stream()   << LOG_COLOR_FG_LIGHT_RED << BG_BLACK << " | FATAL : "
  #ifdef DEBUG_MODEL
    #define logDebug    logFatal
  #else
    #define logDebug    logError
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
  extern inline std::string getStackTrace() { return to_string(boost::stacktrace::stacktrace()); }


  /* Log 装饰 并 流转发 
  // [[deprecated("输出 std::endl 有异常 , 不明白什么原因")]]
  */
  struct LogStreamForward {
    /* 
      1. https://man.archlinux.org/man/terminal-colors.d.5.en
          - https://man7.org/linux/man-pages/man5/terminal-colors.d.5.html
      2. https://billc.io/2019/03/stylish-output/
          - https://www.codeproject.com/Articles/5329247/How-to-change-text-color-in-a-Linux-terminal
          - 31-37：设置前景色 ; 41-47：设置背景色
          - 90-106 和 31-37(41-47) 一样，也是用来设置颜色的，不过会更加明亮一些。
          - 也有控制命令行光标移动的语义
              - https://student.cs.uwaterloo.ca/~cs452/terminal.html 

      ┌──────────────────────────────────────────────────────────────────┐
      | Color                      |   Foreground     |    Background    | 
      ├──────────────────────────────────────────────────────────────────┤          
      | Reset                      |   \033[0m        |    \033[0m       |      
      | Default                    |   \033[39m       |    \033[49m      |      
      ├──────────────────────────────────────────────────────────────────┤ 
      | Black                      |   \033[30m       |    \033[40m      |      
      | Dark red                   |   \033[31m       |    \033[41m      |      
      | Dark green                 |   \033[32m       |    \033[42m      |      
      | Dark yellow (Orange-ish)   |   \033[33m       |    \033[43m      |      
      | Dark blue                  |   \033[34m       |    \033[44m      |      
      | Dark magenta               |   \033[35m       |    \033[45m      |      
      | Dark cyan                  |   \033[36m       |    \033[46m      |      
      | Light gray                 |   \033[37m       |    \033[47m      | 
      ├──────────────────────────────────────────────────────────────────┤      
      | Dark gray                  |   \033[90m       |    \033[100m     |       
      | Red                        |   \033[91m       |    \033[101m     |       
      | Green                      |   \033[92m       |    \033[101m     |       
      | Orange                     |   \033[93m       |    \033[103m     |       
      | Blue                       |   \033[94m       |    \033[104m     |       
      | Magenta                    |   \033[95m       |    \033[105m     |       
      | Cyan                       |   \033[96m       |    \033[106m     |       
      | White                      |   \033[97m       |    \033[107m     | 
      └──────────────────────────────────────────────────────────────────┘      

     */



    private:
     int         flag        = google::INFO;
     std::string colorScheme = LOG_COLOR_DEFAULT;

    public:
     LogStreamForward(int inFlag, const std::string inColorScheme) : flag(inFlag), colorScheme(inColorScheme){};

    //  inline std::string reset()          {return "\033[0m";}
    //  inline std::string fgDefault()      {return "\033[39m";}       
    //  inline std::string fgBlack()        {return "\033[30m";}    inline std::string bgBlack()        {return "\033[40m";}          
    //  inline std::string fgDarkRed()      {return "\033[31m";}    inline std::string bgDarkRed()      {return "\033[41m";}          
    //  inline std::string fgDarkGreen()    {return "\033[32m";}    inline std::string bgDarkGreen()    {return "\033[42m";}          
    //  inline std::string fgDarkYellow()   {return "\033[33m";}    inline std::string bgDarkYellow()   {return "\033[43m";}          
    //  inline std::string fgDarkBlue()     {return "\033[34m";}    inline std::string bgDarkBlue()     {return "\033[44m";}          
    //  inline std::string fgDarkMagenta()  {return "\033[35m";}    inline std::string bgDarkMagenta()  {return "\033[45m";}          
    //  inline std::string fgDarkCyan()     {return "\033[36m";}    inline std::string bgDarkCyan()     {return "\033[46m";}          
    //  inline std::string fgLightGray()    {return "\033[37m";}    inline std::string bgLightGray()    {return "\033[47m";}          
    //  inline std::string fgDarkGray()     {return "\033[90m";}    inline std::string bgDarkGray()     {return "\033[100m";}         
    //  inline std::string fgLightRed()     {return "\033[91m";}    inline std::string bgLightRed()     {return "\033[101m";}         
    //  inline std::string fgLightGreen()   {return "\033[92m";}    inline std::string bgLightGreen()   {return "\033[101m";}         
    //  inline std::string fgLightOrange()  {return "\033[93m";}    inline std::string bgLightOrange()  {return "\033[103m";}         
    //  inline std::string fgLightBlue()    {return "\033[94m";}    inline std::string bgLightBlue()    {return "\033[104m";}         
    //  inline std::string fgLightMagenta() {return "\033[95m";}    inline std::string bgLightMagenta() {return "\033[105m";}         
    //  inline std::string fgLightCyan()    {return "\033[96m";}    inline std::string bgLightCyan()    {return "\033[106m";}         
    //  inline std::string fgLightWhite()   {return "\033[97m";}    inline std::string bgLightWhite()   {return "\033[107m";}


    //  template<typename T>
    //  inline LogStreamForward operator<<(T value) {
    //    google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
    //    return *this;
    //  }

    //  basic_ostream& operator<<( short value );
    //  basic_ostream& operator<<( unsigned short value );
    //  (1)
     inline LogStreamForward operator<<(short value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
     inline LogStreamForward operator<<(unsigned short value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
    //  basic_ostream& operator<<( int value );
    //  basic_ostream& operator<<( unsigned int value );
    //  (2)	
     inline LogStreamForward operator<<(int value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
     inline LogStreamForward operator<<(unsigned int value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
    //  basic_ostream& operator<<( long value );
    //  basic_ostream& operator<<( unsigned long value );
    //  (3)	
     inline LogStreamForward operator<<(long value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
     inline LogStreamForward operator<<(unsigned long value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
    //  basic_ostream& operator<<( long long value );
    //  basic_ostream& operator<<( unsigned long long value );
    //  (4)	(C++11 起)
     inline LogStreamForward operator<<(long long value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
     inline LogStreamForward operator<<(unsigned long long value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
    //  basic_ostream& operator<<( float value );
    //  basic_ostream& operator<<( double value );
    //  basic_ostream& operator<<( long double value );
    //  (5)	
     inline LogStreamForward operator<<(float value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
     inline LogStreamForward operator<<(double value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
     inline LogStreamForward operator<<(long double value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
    //  basic_ostream& operator<<( bool value );
    //  (6)	
     inline LogStreamForward operator<<(bool value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
    //  basic_ostream& operator<<( const void* value );
    //  (7)	
     inline LogStreamForward operator<<(const void* value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
    //  basic_ostream& operator<<( const volatile void* value );
    //  (8)	(C++23 起)
     inline LogStreamForward operator<<(const volatile void* value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
    //  basic_ostream& operator<<( std::nullptr_t );
    //  (9)	(C++17 起)
      inline LogStreamForward operator<<(std::nullptr_t) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << "nullptr" << LOG_COLOR_RESET;
       return *this;
     }
    //  basic_ostream& operator<<( std::basic_streambuf<CharT, Traits>* sb );
    //  (10)
     template <typename CharT, typename Traits>
     inline LogStreamForward operator<<(std::basic_streambuf<CharT, Traits>* value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
    //  basic_ostream& operator<<( std::ios_base& (*func)(std::ios_base&) );
    //  (11)
     inline LogStreamForward operator<<(std::ios_base& (*func)(std::ios_base&)) {
       std::ostream& oStream = google::LogMessage(__FILE__, __LINE__, flag).stream();
       oStream << colorScheme;  //<< value << LOG_COLOR_RESET;
       func(oStream);
       oStream << LOG_COLOR_RESET;
       return *this;
     }
    //  basic_ostream& operator<<( std::basic_ios<CharT, Traits>& (*func)(std::basic_ios<CharT, Traits>&) );
    //  (12)
     template <typename CharT, typename Traits>
     inline LogStreamForward operator<<(std::basic_ios<CharT, Traits>& (*func)(std::basic_ios<CharT, Traits>&)) {
       std::ostream& oStream = google::LogMessage(__FILE__, __LINE__, flag).stream();
       oStream << colorScheme;  //<< value << LOG_COLOR_RESET;
       func(oStream);
       oStream << LOG_COLOR_RESET;
       return *this;
     }
     //  basic_ostream& operator<<( std::basic_ostream<CharT, Traits>& (*func)  (std::basic_ostream<CharT, Traits>&) );
     //  (13)
     template <typename CharT, typename Traits>
     inline LogStreamForward operator<<(std::basic_ostream<CharT, Traits>& (*func)  (std::basic_ostream<CharT, Traits>&)) {
       std::ostream& oStream = google::LogMessage(__FILE__, __LINE__, flag).stream();
       oStream << colorScheme;  //<< value << LOG_COLOR_RESET;
       func(oStream);
       oStream << LOG_COLOR_RESET;
       return *this;
     }


     // 字符串输出
     template <class CharT>
     inline LogStreamForward operator<<(const std::basic_string<CharT>& value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
     template <class CharT, class Traits>
     inline LogStreamForward operator<<(const std::basic_ostream<CharT, Traits>& value) {
       google::LogMessage(__FILE__, __LINE__, flag).stream() << colorScheme << value << LOG_COLOR_RESET;
       return *this;
     }
  };
}
