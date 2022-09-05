#pragma once

#include <iostream>
#include <string>
using namespace std;

struct {
  string flag = "init value";

  void print(string newFlag) {
    cout << "unnameStruct's adderss:" << this << '\t';
    cout << "flag's address:" << &flag << '\t';
    cout << "rawFlag:" << flag << '\t';
    flag = newFlag;
    cout << "newFlag:" << flag << '\t' << endl;
  }
} unnameStruct;


/*  中国大区连锁店总部
    我们做简单模型 , 店面只有收入 , 无需找零 */
struct ChinaBaseShop {
  static double turnover; /* 收入 , 流水 , 静态变量 默认 初始化为 0 */

  /* 收入 , 虚函数 , 子类(连锁店分布)或许有折扣或者满减活动 */
  virtual void income(double money);
};
