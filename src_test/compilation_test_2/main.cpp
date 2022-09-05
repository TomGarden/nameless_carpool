#include "beijing_shope.h"
#include "tianjin_shope.h"

#include <iostream>
using namespace std;

/* 查看商店营收 */
int main() {

  BeijingShop beijingShop;
  TianjinShop tianjinShop;

  cout << "开张前流水 : " << ChinaBaseShop::turnover << endl;

  beijingShop.income(20.0);

  cout << "北京开张后 : " << ChinaBaseShop::turnover << endl;

  tianjinShop.income(30.0);

  cout << "天津开张后 : " << ChinaBaseShop::turnover << endl;

  return 0 ;
}