#include <memory>
#include <iostream>
#include <mutex>
#include <thread>


using namespace std;

class Singleton {

  private:
    Singleton() {
      cout << this_thread::get_id() << "  Create ~~~~" << endl;
    };
  public:
    Singleton(Singleton&) = delete;
    Singleton& operator=(Singleton&) = delete;
    Singleton(const Singleton&&) = delete;
    Singleton& operator=(const Singleton&&) = delete;

  public:
    static Singleton& getInstance() {
      static Singleton instance;
      return instance;
    }

    void printThreadName() {
      cout << "printThreadName:" << this_thread::get_id() << endl;
    }

};


void test() {
  //Singleton& instance_0 { Singleton::getInstance() };
  Singleton& instance_1 = Singleton::getInstance() ;
  //instance_0.printThreadName();
  instance_1.printThreadName();
}


int main() {
  cout << "---------------START : " << this_thread::get_id() << endl;
  
  thread th1(test);
  thread th2(test);
  thread th3(test);
  thread th4(test);
  thread th6(test);

  th1.join();
  th2.join();
  th3.join();
  th4.join();
  th6.join();

  cout << "---------------" << "DONE" << endl;

  return 0;
}