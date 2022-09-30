// Type your code here, or load an example.
#include <vector>
#include <iostream>
using namespace std;
int main() {

  cout << "oneVector.size() = " ;
  vector<int> oneVector{1,2,3,4} ;
  oneVector.push_back(5);
  vector<int>::iterator ite = oneVector.begin();
  cout << oneVector.size() << endl;

  cout << "twoVector.size() = " ;
  const vector<int> twoVector{1,2,3,4} ;
  vector<int>::const_iterator ite2 = twoVector.begin();
  //twoVector.push_back(5); /* 常量修改报错 */
  cout << twoVector.size() << endl;
  //pringf("asfasdf");
  return 0;
}