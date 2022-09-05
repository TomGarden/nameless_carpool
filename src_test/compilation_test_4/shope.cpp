#include "shope.h"

double ChinaBaseShop::turnover = {0.0};
void ChinaBaseShop::income(double money) {
  cout << "ChinaBaseShop's Address : " << this << endl;
  unnameStruct.print("Headquarters in China");
  turnover += money;

}