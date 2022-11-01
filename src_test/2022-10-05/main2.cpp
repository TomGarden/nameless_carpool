
#include <iostream>
#include <string>
#include <climits>

int main(int argc, char** argv) {
  float floatVal = 123.123f;
  int   intVal1  ;      /* 有时候会有歧义 , 程序员可能无心的做了一次类型转换 */
  int   intVal2  ; /* 程序员可以用这种方式来表达: 我知道发生了什么, 我在做类型转换 */
  int   intVal3  ;
  unsigned long long ullVal = ULLONG_MAX;
  intVal1 = static_cast<int>(floatVal);
  intVal2 = (int)floatVal;
  intVal3 = ullVal;


  std::cout << intVal1 << "\t" << intVal2  << "\t" << intVal3 << std::endl << std::endl << std::endl;

  class Father {};
  class Son : public Father{};
  Son son = Son();
  Father father = son;
  Father& father = son;
  Father* father = &son;

}
