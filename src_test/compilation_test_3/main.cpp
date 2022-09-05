#include "beijing_shope.h"
#include "tianjin_shope.h"

#include <iostream>
using namespace std;

int main() {
  BeijingShop beijingShop;
  TianjinShop tianjinShop;

  cout << "before open any shope : " << ChinaBaseShop::turnover << endl;

  beijingShop.income(20.0);

  cout << "after open BeiJing shope : " << ChinaBaseShop::turnover << endl;

  tianjinShop.income(30.0);

  cout << "after open TianJin shope : " << ChinaBaseShop::turnover << endl;

  return 0 ;
}