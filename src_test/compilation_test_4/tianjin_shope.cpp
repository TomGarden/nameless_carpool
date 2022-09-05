#include "tianjin_shope.h"

void TianjinShop::income(double money) /* override */ {
  cout << "-TianJin–" << endl;
  cout << "TianjinShop's Address:" << this << endl;
  unnameStruct.print("TianJin shope ");
  ChinaBaseShop::income(money);
  if(money >= 100) {
      /* over 100 , 10% off */
      turnover -= (money / 10.0);
  }
}