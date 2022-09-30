
#include "date_time.h"
#include "user.h"
#include "mysql_db.h"
#include <string>
#include <iostream>
#include <optional>
#include <typeinfo>
#include <cxxabi.h>
#include <map>




using namespace src_test;
using namespace std;


int main() {

  int     status;
  char   *realname;

  // exception classes not in <stdexcept>, thrown by the implementation
  // instead of the user
  std::bad_exception  e;
  realname = abi::__cxa_demangle(e.what(), 0, 0, &status);
  std::cout << e.what() << "\t=> " << realname << "\t: " << status << '\n';
  free(realname);


  // typeid
  map<int,map<string, double>>          u;
  const std::type_info  &ti = typeid(u);

  realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
  std::cout << ti.name() << "\t=> " << realname << "\t: " << status << '\n';
  free(realname);

  return 0;


  // DateTime dateTime;
  // dateTime.test_time_t();
  // dateTime.test_cpp_time();
  // dateTime.test_cpp_time2();
  // uint64_t flag = 10'000'000'000ull;
  // std::cout << "flag:" << flag << std::endl;


  // WcUser wcUser = { 123, "wc_uid", "wc_number"};
  // Json json = Json(wcUser);
  // std::cout << json.dump() << std::endl;

  // MysqlDb mysqlDb;
  // mysqlDb.tryMysql();


  // std::cout << "\n\n\n" <<std::endl;

  // optional<string> flag;
  // const type_info& typeInfo = typeid(flag);
  // cout << typeid(flag).name() << endl;
  // if(typeInfo == typeid(optional<int>)){
  //   cout << "success" << endl;
  // } else {typeInfo.before()
  //   cout << "failed" << endl;
  // }

  // return 0;
}

using namespace std;
struct Test {

  int iA = 1;
  int iB = 8;

  void swapParamValue(int a , int b ) {
    int temp = a;
    a = b;
    b = temp;

    printf("\t[a,b]:[%d,%d]\n", a, b);
  }

  void swapParamPointer(int *a , int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;

    printf("\t[a,b]:[%d,%d]\n", a, b);
  }

  void swapParamRefrence(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;

    printf("\t[a,b]:[%d,%d]\n", a, b);
  }

  void printIntVal() {
    printf("\n>>>>[a,b]:[%d,%d]\n", iA, iB);
  }

  string strOne = "string one [11111]";
  string strTwo = "string two [22222]";
};

int main2() {
  Test test;

  test.printIntVal();
  test.swapParamValue(test.iA, test.iB);
  test.printIntVal();



  return 0;
}