
#include "date_time.h"
#include "user.h"
#include "mysql_db.h"
#include <string>



using namespace src_test;

int main2() {
  DateTime dateTime;
  dateTime.test_time_t();
  dateTime.test_cpp_time();

  WcUser wcUser = { 123, "wc_uid", "wc_number"};
  Json json = Json(wcUser);
  std::cout << json.dump() << std::endl;

  MysqlDb mysqlDb;
  mysqlDb.tryMysql();


  std::cout << "\n\n\n" <<std::endl;
  return 0;
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

int main() {
  Test test;

  test.printIntVal();
  test.swapParamValue(test.iA, test.iB);
  test.printIntVal();



  return 0;
}