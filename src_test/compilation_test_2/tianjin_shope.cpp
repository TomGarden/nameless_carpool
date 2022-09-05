
#include "tianjin_shope.h"


void TianjinShop::income(double money) /* override */ {
  cout << "-天津–" << endl;
  cout << "TianjinShop:" << this << endl;
  unnameStruct.print("天津分店");
  ChinaBaseShop::income(money);
  if(money >= 100) {
      /* 天津门店满消费满一百 打 9 折*/
      turnover -= (money / 10.0);

  }
}

// struct TianjinShop : ChinaBaseShop {
//   void income(double money) override {
//     ChinaBaseShop::income(money);
//     if(money >= 30) {
//         /* 天津门店满消费满一百 打 9 折*/
//         turnover -= 10;
//     }
//   }
// };
