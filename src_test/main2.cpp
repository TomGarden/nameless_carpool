
#include <string>
#include <iostream>

using namespace std;
struct Test {

  int iA = 1;
  int iB = 8;

  void swapParamValue(int a , int b ) {

    printf("\t oldValue         :  [a,b]:[%d,%d]  \n", a, b);
    printf("\t oldValueMemPath  :  [a,b]:[%x,%x]  \n", &a, &b);

    int temp = a;
    a = b;
    b = temp;

    printf("\t newValue         :  [a,b]:[%d,%d]  \n", a, b);
    printf("\t newValueMemPath  :  [a,b]:[%x,%x]  \n", &a, &b);
  }

  void swapParamPointer(int *a , int *b) {

    printf("\t oldValue         :  [a,b]:[%d,%d]  \n", *a, *b);
    printf("\t oldValueMemPath  :  [a,b]:[%x,%x]  \n", a, b);

    int temp = *a;
    *a = *b;
    *b = temp;

    printf("\t newValue         :  [a,b]:[%d,%d]  \n", *a, *b);
    printf("\t newValueMemPath  :  [a,b]:[%x,%x]  \n", a, b);
  }

  void swapParamRefrence(int &a, int &b) {
    printf("\t oldValue         :  [a,b]:[%d,%d]  \n", a, b);
    printf("\t oldValueMemPath  :  [a,b]:[%x,%x]  \n", &a, &b);

    int temp = a;
    a = b;
    b = temp;

    printf("\t newValue         :  [a,b]:[%d,%d]  \n", a, b);
    printf("\t newValueMemPath  :  [a,b]:[%x,%x]  \n", &a, &b);
  }

  void printIntVal() {
    printf("\n");
    printf(">>>>>>>>    Value         :  [a,b]:[%d,%d]  \n", iA, iB);
    printf(">>>>>>>>    ValueMemPath  :  [a,b]:[%x,%x]  \n", &iA, &iB);
  }


  /* -------------------------------------------------------------------- */

  string strA = "string AAA 11111";
  string strB = "string BBB 22222";

  void swapParamValue(string a , string b ) {

    cout  << "\t oldValue         :  [a,b]:[" << a << ',' << b << "]  " << endl;
    printf("\t oldValueMemPath  :  [a,b]:[%x,%x]  \n", &a, &b);

    string temp = a;
    a = b;
    b = temp;

    cout  << "\t newValue         :  [a,b]:[" << a << ',' << b << "]  " << endl;
    printf("\t newValueMemPath  :  [a,b]:[%x,%x]  \n", &a, &b);
  }

  void swapParamPointer(string *a , string *b) {

    cout  << "\t oldValue         :  [a,b]:[" << *a << ',' << *b << "]  " << endl;
    printf("\t oldValueMemPath  :  [a,b]:[%x,%x]  \n", a, b);

    string temp = *a;
    *a = *b;
    *b = temp;

    cout  << "\t newValue         :  [a,b]:[" << *a << ',' << *b << "]  " << endl;
    printf("\t newValueMemPath  :  [a,b]:[%x,%x]  \n", a, b);
  }

  void swapParamRefrence(string &a, string &b) {
    cout  << "\t oldValue         :  [a,b]:[" << a << ',' << b << "]  " << endl;
    printf("\t oldValueMemPath  :  [a,b]:[%x,%x]  \n", &a, &b);

    string temp = a;
    a = b;
    b = temp;

    cout  << "\t newValue         :  [a,b]:[" << a << ',' << b << "]  " << endl;
    printf("\t newValueMemPath  :  [a,b]:[%x,%x]  \n", &a, &b);
  }

  void printStrVal() {
    printf("\n");
    cout<< ">>>>>>>>    Value         :  [a,b]:[" << strA << ',' << strB << "]  " << endl;
    printf(">>>>>>>>    ValueMemPath  :  [a,b]:[%x,%x]  \n", &strA, &strB);
  }

  /* -------------------------------------------------------------------- */

  struct CustomStruct {
    int intVal = 111;
    string strVal = "二二二";

    void printSelfInfo() {
      cout << "int value : " << intVal << "\t"
           << "str value : " << strVal 
           << endl;
      printf("structMemValeu : %x      \t      int memValue : %x    \t   strMemValue : %x \n", this, &intVal, &strVal);
    }
  } oneCustomStruct;

  void inputStructType(CustomStruct customStruct) {
    customStruct.printSelfInfo();
  }

  CustomStruct returnStructType() {
    CustomStruct twoCustomStruct;
    twoCustomStruct.intVal = 999;
    twoCustomStruct.strVal = "阿萨德发斯蒂芬";

    twoCustomStruct.printSelfInfo();

    return twoCustomStruct;
  }


  /* -------------------------------------------------------------------- */


  int returnInt() {
    int testInt = 111;
    cout<< ">>>>>>>>    returnValue         :" << testInt << endl;
    printf(">>>>>>>>    returnValueMemPath  :%x  \n", &testInt);
    return testInt;
  }

  void callReturnInt() {
    int result = returnInt();
    cout<< ">>>>>>>>    resultValue         :" << result << endl;
    printf(">>>>>>>>    resultValueMemPath  :%x  \n\n", &result);
  }

  string returnStr() {
    string testStr = "我是测试字符串 , 如果 return 这个字符串看是引用传递还是值传递";
    cout<< ">>>>>>>>    returnValue         :" << testStr << endl;
    printf(">>>>>>>>    returnValueMemPath  :%x  \n", &testStr);
    return testStr;
  }

  void callReturnStr() {
    string result = returnStr();
    cout<< ">>>>>>>>    resultValue         :" << result << endl;
    printf(">>>>>>>>    resultValueMemPath  :%x  \n\n", &result);
  }


};

int main() {
  Test test;

  test.printIntVal();
  test.swapParamValue(test.iA, test.iB);

  test.printIntVal();
  test.swapParamPointer(&test.iA, &test.iB);

  test.printIntVal();
  test.swapParamRefrence(test.iA, test.iB);

  test.printIntVal();

  printf("\n---------------------------------------------n");

  test.printStrVal();
  test.swapParamValue(test.strA, test.strB);

  test.printStrVal();
  test.swapParamPointer(&test.strA, &test.strB);

  test.printStrVal();
  test.swapParamRefrence(test.strA, test.strB);

  test.printStrVal();

  printf("\n---------------------------------------------\n");

  
  test.oneCustomStruct.printSelfInfo();
  test.inputStructType(test.oneCustomStruct);
  Test::CustomStruct result = test.returnStructType();
  result.printSelfInfo();

  printf("\n以上说明 , 直接传参(非指针非引用) , 那就是值传递 . 这里涉及到的类的拷贝控制函数的知识点 , 暂不详究\n");

  printf("\n============================================\n\n");

  test.callReturnInt();
  test.callReturnStr();

  printf("\n以上说明 , 对象返回是引用传递 . 简单类型返回是拷贝传递 \n");
  
  return 0;
}