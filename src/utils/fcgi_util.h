
#pragma once

#include "fcgio.h"
#include "src/utils/tom_string_utils.h"

#include <string>
#include <sstream>


namespace nameless_carpool {

  using namespace std;  
  using namespace tom_utils;  

  inline string fcgxStreamRead(FCGX_Stream *stream) {
    const int length = 1024;
    char fcgiStrTmp[length];
    std::stringstream strStream;

    int readCount = 0;
    do {
      memset(fcgiStrTmp, 0, length);
      readCount = FCGX_GetStr(fcgiStrTmp, length, stream);
      if(readCount > 0) {
        strStream << fcgiStrTmp;
      }
    } while ( readCount > 0 );


    string requestInStr = strStream.str();
    strStream.str("");
    strStream.clear();
    return requestInStr;
  }

  inline wstring fcgxStreamReadw(FCGX_Stream *stream) {
    string str = fcgxStreamRead(stream);
    return utfMbsrtowcs(str);
  }

  

}
