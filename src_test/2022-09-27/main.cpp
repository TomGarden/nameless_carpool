

// #include <optional> 
// #include <string>
#include <iostream>
#include <optional>
#include  <typeinfo>
#include <cxxabi.h>
#include <map>
#include <exception>

using namespace std;

#define FROM_NLOHMANN_JSON__TO(val2) std::cout << val2 << std::endl;
#define NLOHMANN_JSON__TO(val2) std::cout << val2 << std::endl;
#define TEST_DEFINE(strVal, val2) FROM_## strVal ##_TO(val2)
#define TEST_DEFINE_2(strVal) FROM_## strVal



inline string getTypeName(const std::type_info& typeInfo) {
  int     status;
  char   *realname;
  realname = abi::__cxa_demangle(typeInfo.name(), 0, 0, &status);
  string str(realname);
  free(realname);
  return str;
}

inline string getFiledTypeName(auto filed) {
  return getTypeName(typeid(filed));
}


void test() {
  int     status;
  char   *realname;

  // exception classes not in <stdexcept>, thrown by the implementation
  // instead of the user
  // std::bad_exception  e;
  // realname = abi::__cxa_demangle(e.what(), 0, 0, &status);
  // std::cout << e.what() << "\t=> " << realname << "\t: " << status << '\n';
  // free(realname);


  // typeid
  //map<int,map<char, double>>          u;

  const auto& typeInfo = typeid(optional<int>);
  cout << getTypeName(typeid(optional<int>)) << endl << endl ;

  optional<map<int,map<char, double>>>          u;
  const std::type_info  &ti = typeid(u);

  realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
  std::cout << ti.name() << "\t=> " << realname << "\t: " << status << '\n';

  string str(realname);
  free(realname);

  cout << endl << "string str(realname); >> " << str << endl<< endl;

  std::cout << ti.name() << "\t=> " << realname << "\t: " << status << '\n';
}

int main() {
  optional<string> flag = "";

  if(!flag.has_value()) {
    cout << "!flag.has_value()" << endl;
  }
  if(flag->empty()) {
    cout << "flag->empty()" << endl;
  } else {
    cout << "!flag->empty() -> " << flag.value() << endl;
  }


  // const type_info& typeInfo = typeid(flag);
  // cout << typeid(flag).name() << endl;
  // if(typeInfo == typeid(optional<int>)){
  //   cout << "success" << endl;
  // } else {
  //   cout << "failed" << endl;
  // }
  // int flag = 123;
  // TEST_DEFINE(NLOHMANN_JSON_, flag)
  // TEST_DEFINE_2(NLOHMANN_JSON__TO(flag))

  // optional<string> createTime;

  // //cout << "createTime:" << createTime.value() << endl;
  // cout << "createTime:" << createTime.value_or("控制控制") << endl;


  // createTime = "asdfasd";
  
  // createTime.has_value();
  // cout << "createTime:" << createTime.value() << endl;

  // //createTime.reset();
  // //createTime = nullopt;
  // createTime.value().append("附加值");

  // cout << "createTime:" << createTime.value_or("控制控制") << endl;

  // createTime->empty();
  test();
}

