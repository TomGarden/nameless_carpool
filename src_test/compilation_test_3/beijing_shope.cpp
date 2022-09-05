#include "beijing_shope.h"

void BeijingShop::income(double money) /* override */ {
  cout << "-BeiJing–" << endl;
  cout << "BeijingShop's Address:" << this << endl;
  unnameStruct.print("BeiJing shope");
  ChinaBaseShop::income(money);
  if(money > 100) {
    /* Over 100 minus 10 */
    turnover -= 10.0;
  }
}

