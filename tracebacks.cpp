#include <bits/stdc++.h>
using namespace std;

#define decorate(func) _decorate(string(#func), func)

class exc {
  public:
  string what_arg;  
  explicit exc ( string wht) {
    what_arg = wht;
  }
  string what() {
    return what_arg;
  }
};

template<typename R, typename ... Args> function<R(Args...)> _decorate(string name, R (*f) (Args...) ) {
  return function<R(Args...)>([=](Args ... args) {
    try {
      return f(args...);
    }
    catch (exc e) {
      throw exc("In " + name + ":\n" + e.what());
    }
  });
}

void test(int depth) {
  if(depth > 10) throw exc("Maximum recursion depth exceeded.");
  decorate(test)(depth+1);
}

int main() {
  try {
    decorate(test)(0);
  }
  catch (exc e) {
    cerr << e.what() << endl;
  }
}