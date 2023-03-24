
#include <unistd.h>
#include <filesystem>
#include <optional>

#include "linux_os.h"

namespace nameless_carpool {

  bool getCurExePath(std::string& result){
    int length = 1024;
    char buf[length];
    int readCount = readlink("/proc/self/exe", buf, length);

    if(readCount >= 0) {
      result = std::string(buf, readCount);
    }

    return (readCount >= 0);
  }

  std::string getCurExePathOrThrow() {
    std::string exePath;
    if(getCurExePath(exePath)) return exePath;
    else throw std::runtime_error("获取可执行文件路径失败");
  }

  bool getCurExeFileDir(std::string& result) {
    std::string filePath;
    if( getCurExePath(filePath) ) {
      size_t index = filePath.find_last_of(std::filesystem::path::preferred_separator);
      result = filePath.substr(0, index);
      return true;
    } else {
      return false;
    }
  }

  std::string getCurExeFileDirOrThrow() {
    std::string fileDir;
    if(getCurExeFileDir(fileDir)) return fileDir;
    else throw std::runtime_error("获取可执行文件上级目录失败");
  }

  /* #region 根据 参数 获取参数类型字符串 , 是准确的类型字符串 */
  std::string getTypeName(const std::type_info& typeInfo) {
    int     status;
    char   *realname;
    realname = abi::__cxa_demangle(typeInfo.name(), 0, 0, &status);
    std::string str(realname);
    free(realname);
    return str;
  }
  std::string getFiledTypeName(auto filed) {
    return getTypeName(typeid(filed));
  }
  bool isOptionalObj(auto field) {
    /* 这是对 getTypeName / getFiledTypeName 的一个例程 */
    
    using namespace std;

    std::string objTypeName = getFiledTypeName(field);
    auto objTNIndex = objTypeName.find_first_of('<');
    if(objTNIndex == std::string::npos) {
      return false;
    }

    std::string optionalTypeName = getTypeName(typeid(optional<int>));

    return objTypeName.compare(0, objTNIndex, optionalTypeName, 0, objTNIndex) == 0;
  }
  /* #endregion  */

}