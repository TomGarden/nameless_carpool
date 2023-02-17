
#include <unistd.h>
#include <filesystem>
#include <optional>

#include "linux_os.h"

namespace nameless_carpool {

  bool getCurExePath(string& result){
    int length = 1024;
    char buf[length];
    int readCount = readlink("/proc/self/exe", buf, length);

    if(readCount >= 0) {
      result = string(buf, readCount);
    }

    return (readCount >= 0);
  }

  bool getCurExeFd(string& result) {
    string filePath;
    if( getCurExePath(filePath) ) {
      size_t index = filePath.find_last_of(std::filesystem::path::preferred_separator);
      result = filePath.substr(0, index);
      return true;
    } else {
      return false;
    }
  }

  /* #region 根据 参数 获取参数类型字符串 , 是准确的类型字符串 */
  string getTypeName(const std::type_info& typeInfo) {
    int     status;
    char   *realname;
    realname = abi::__cxa_demangle(typeInfo.name(), 0, 0, &status);
    string str(realname);
    free(realname);
    return str;
  }
  string getFiledTypeName(auto filed) {
    return getTypeName(typeid(filed));
  }
  bool isOptionalObj(auto field) {
    /* 这是对 getTypeName / getFiledTypeName 的一个例程 */
    
    using namespace std;

    string objTypeName = getFiledTypeName(field);
    auto objTNIndex = objTypeName.find_first_of('<');
    if(objTNIndex == string::npos) {
      return false;
    }

    string optionalTypeName = getTypeName(typeid(optional<int>));

    return objTypeName.compare(0, objTNIndex, optionalTypeName, 0, objTNIndex) == 0;
  }
  /* #endregion  */

}