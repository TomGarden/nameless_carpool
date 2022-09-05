#include "beijing_shope.h"

void BeijingShop::income(double money) /* override */ {
  cout << "-北京–" << endl;
  cout << "BeijingShop:" << this << endl;
  unnameStruct.print("北京分店");
  ChinaBaseShop::income(money);
  if(money > 100) {
    /* 北京门店满 100 减 10 */
    turnover -= 10.0;

  }
}

